
#if defined(_MSC_VER)
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameState.hpp>
#include <GTGameEngine/Graphics/DefaultGraphicsWorld.hpp>
#include <GTGameEngine/Graphics/GraphicsAssetResourceManager.hpp>
#include <GTGameEngine/Graphics/GraphicsWorldGUIRenderer.hpp>
#include <GTGameEngine/Assets/ModelAsset.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>
#include <GTGameEngine/Assets/MaterialAsset.hpp>
#include <GTGameEngine/Scene/Scene.hpp>
#include <GTGameEngine/Scene/SceneCallback.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneNodeComponent_Graphics.hpp>
#include <GTGameEngine/ConfigFile.hpp>
#include <GTLib/IO.hpp>

class SandboxSceneCallback : public GT::SceneCallback
{
public:

    SandboxSceneCallback()
        : m_pGraphicsWorld(nullptr)
    {
    }


    void SetGraphicsWorld(GT::GraphicsWorld* pGraphicsWorld)
    {
        m_pGraphicsWorld = pGraphicsWorld;
    }


    void OnSceneNodeInserted(GT::SceneNode* pSceneNode, const GT::vec4 &worldPosition, const GT::quat &worldRotation, const GT::vec4 &worldScale)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->AddModelToGraphicsWorld(worldPosition, worldRotation, worldScale);
        }
#else
        (void)pSceneNode;
#endif
    }

    void OnSceneNodeRemoved(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->RemoveModelFromGraphicsWorld();
        }
#else
        (void)pSceneNode;
#endif
    }

    void OnSceneNodeTransformed(GT::SceneNode* pSceneNode, const GT::vec4 &worldPosition, const GT::quat &worldRotation, const GT::vec4 &worldScale)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_GRAPHICS)
        auto pGraphicsComponent = pSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
        if (pGraphicsComponent != nullptr)
        {
            pGraphicsComponent->SetModelTransform(worldPosition, worldRotation, worldScale);
        }
#else
        (void)pSceneNode;
#endif
    }


    void UpdateSceneNode(GT::SceneNode* pSceneNode, double deltaTimeInSeconds)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        (void)deltaTimeInSeconds;
    }

    void PostUpdateSceneNode(GT::SceneNode* pSceneNode, double deltaTimeInSeconds)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        (void)deltaTimeInSeconds;
    }

    void UpdateSceneNodePhysicsTransform(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
    }


    bool DoesSceneNodeRequireUpdate(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        return true;
    }

    bool DoesSceneNodeRequirePostUpdate(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        return true;
    }

    bool IsSceneNodeStatic(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        (void)pSceneNode;
        return false;
    }

    bool IsPhysicsObject(GT::SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

#if defined(GT_BUILD_COMPONENT_DYNAMICS)
        (void)pSceneNode;
        return true;
#else
        (void)pSceneNode;
        return false;
#endif
    }


private:

    /// The graphics world for rendering.
    GT::GraphicsWorld* m_pGraphicsWorld;
};

class SandboxGameState : public GT::GameState
{
public:

    /// Constructor.
    SandboxGameState()
        : m_pGraphicsAPI(nullptr), 
          m_pGraphicsWorld(nullptr), m_pGraphicsAssetResourceManager(nullptr),
          m_guiRenderer(), m_guiFontManager(GT::FontOption_NoClearType), m_gui(&m_guiRenderer, &m_guiFontManager),
          m_hMainWindow(0),
          m_isShiftKeyDown(false),
          m_sceneCallback(),
          m_scene(m_sceneCallback)
    {
    }

    /// Destructor.
    ~SandboxGameState()
    {
    }


    unsigned int GetWindowCount(GT::GameContext &gameContext) const
    {
        return 1;
    }

    GT::HWindow GetWindowByIndex(GT::GameContext &gameContext, unsigned int index)
    {
        assert(index == 0);
        return m_hMainWindow;
    }


    void Step(GT::GameContext &gameContext, double deltaTimeInSeconds)
    {
        (void)gameContext;
        (void)deltaTimeInSeconds;

        // TODO: Add support for multi-threaded rendering.
        m_pGraphicsWorld->ExecuteRenderingCommands();
    }



    bool OnStarting(GT::GameContext &gameContext, const GTLib::CommandLine &commandLine)
    {
        auto &engine = gameContext.GetEngineContext();

        // The index to insert base directory, from the back.
        unsigned int baseDirectoryInsertOffset = static_cast<unsigned int>(engine.GetFileSystem().GetBaseDirectoryCount());

        // Load the config file.
        GTLib::String configAbsolutePath;
        if (engine.GetFileSystem().FindAbsolutePath("config.lua", configAbsolutePath))
        {
            if (m_config.Load(configAbsolutePath.c_str()))
            {
                // Retrieve the base directories.
                if (m_config.IsArray("BaseDirectories"))
                {
                    auto &fileSystem = engine.GetFileSystem();

                    size_t baseDirectoriesCount = m_config.GetArrayCount("BaseDirectories");
                    for (size_t i = 0; i < baseDirectoriesCount; ++i)
                    {
                        const char* relativeBaseDirectory = m_config.GetStringFromArray("BaseDirectories", i);
                        if (relativeBaseDirectory != nullptr)
                        {
                            //char absoluteBaseDirectory[EASYPATH_MAX_PATH];
                            //easypath_makeabsolute(engine.GetCommandLine().GetApplicationDirectory(), relativeBaseDirectory, absoluteBaseDirectory, EASYPATH_MAX_PATH);

                            // The directory needs to be added at to one position from the back. That is because we want the application directory
                            GTLib::String absoluteBaseDirectory = GTLib::IO::ToAbsolutePath(relativeBaseDirectory, engine.GetCommandLine().GetApplicationDirectory());
                            fileSystem.InsertBaseDirectory(absoluteBaseDirectory.c_str(), fileSystem.GetBaseDirectoryCount() - baseDirectoryInsertOffset);
                        }
                    }
                }
            }
        }



        // Create the game windows.
        m_hMainWindow = gameContext.CreateMainWindow(GT::WindowType::PrimaryWindow, "GTGameEngine Sandbox", 0, 0, 1280, 720);
        if (m_hMainWindow == 0)
        {
            return false;
        }

        // Show the window.
        gameContext.ShowWindow(m_hMainWindow);



        m_pGraphicsAPI = gameContext.GetEngineContext().GetBestGraphicsAPI();
        assert(m_pGraphicsAPI != nullptr);
        {
            m_pGraphicsWorld = new GT::DefaultGraphicsWorld(m_gui, *m_pGraphicsAPI);
            if (m_pGraphicsWorld->Startup())
            {
                m_hWindowRT = m_pGraphicsWorld->CreateRenderTargetFromWindow(reinterpret_cast<HWND>(m_hMainWindow), 0);
                m_pGraphicsWorld->SetRenderTargetProjectionAndView(m_hWindowRT, GT::mat4::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f), GT::mat4::translate(GT::mat4::identity, GT::vec4(0, 0, -5, 0.0f)));

                m_pGraphicsAssetResourceManager = new GT::GraphicsAssetResourceManager(*m_pGraphicsWorld, gameContext.GetEngineContext().GetAssetLibrary());


                // The GUI renderer needs to know about the graphics world so it knows where to route rendering commands for GUIs.
                m_guiRenderer.SetGraphicsWorld(m_pGraphicsWorld);


                // TESTING
                m_hWindowSurface = m_gui.CreateSurface();
                GT::HGUIElement hTestElement1 = m_gui.CreateElement();
                m_gui.AttachElementToSurface(hTestElement1, m_hWindowSurface);
                //m_gui.SetElementSize(hTestElement1, 100U, 100U);
                //m_gui.SetElementPosition(hTestElement1, 100, 100);
                m_gui.SetElementSizeToChildren(hTestElement1);
                //m_gui.SetElementBackgroundColor(hTestElement1, GTLib::Colour(0.5f, 0.5f, 0.5f, 0.0f));
                //m_gui.SetElementBorder(hTestElement1, 2, GTLib::Colour::Black);
                m_gui.SetElementFont(hTestElement1, "Courier New", GT::FontWeight_Normal, GT::FontSlant_None, 72);
                m_gui.SetElementText(hTestElement1, "int main()\n{\n    return 0;\n}");
                m_gui.SetElementTextColor(hTestElement1, GTLib::Colour::White);
                m_pGraphicsWorld->SetRenderTargetGUISurface(m_hWindowRT, m_hWindowSurface);

                unsigned int windowWidth;
                unsigned int windowHeight;
                gameContext.GetWindowSize(m_hMainWindow, windowWidth, windowHeight);
                m_gui.SetSurfaceSize(m_hWindowSurface, windowWidth, windowHeight);
            }
        }


        m_sceneCallback.SetGraphicsWorld(m_pGraphicsWorld);

        // Test scene node.
        auto pGraphicsComponent = new GT::SceneNodeComponent_Graphics;
        pGraphicsComponent->SetModel("data/cube.obj", m_pGraphicsAssetResourceManager);

        m_pTestSceneNode = m_scene.CreateSceneNode();
        m_pTestSceneNode->AttachComponent(pGraphicsComponent);
        m_pTestSceneNode->SetPosition(GT::vec4(0, 0, 0, 0));
        m_scene.InsertSceneNode(m_pTestSceneNode);


        return true;
    }

    void OnClosing(GT::GameContext &gameContext)
    {
        if (m_pTestSceneNode != nullptr)
        {
            auto pComponent = m_pTestSceneNode->GetComponent<GT::SceneNodeComponent_Graphics>();
            m_pTestSceneNode->DetachComponent(pComponent);
            delete pComponent;

            m_scene.DeleteSceneNode(m_pTestSceneNode);
            m_pTestSceneNode = nullptr;
        }

        delete m_pGraphicsAssetResourceManager;
        m_pGraphicsAssetResourceManager = nullptr;

        m_pGraphicsWorld->DeleteRenderTarget(m_hWindowRT);
        m_pGraphicsWorld->Shutdown();
        delete m_pGraphicsWorld;
        m_pGraphicsWorld = nullptr;


        gameContext.DeleteWindow(m_hMainWindow);
        m_hMainWindow = 0;
    }
    

    void OnKeyPressed(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key)
    {
        if (key == GTLib::Keys::Shift)
        {
            m_isShiftKeyDown = true;
        }

        if (key == GTLib::Keys::Tab && m_isShiftKeyDown)
        {
            if (gameContext.IsEditorOpen())
            {
                gameContext.CloseEditor();
            }
            else
            {
                gameContext.OpenEditor();
            }
        }
    }

    void OnKeyReleased(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key)
    {
        if (key == GTLib::Keys::Shift)
        {
            m_isShiftKeyDown = false;
        }
    }


    void OnWindowResized(GT::GameContext &gameContext, GT::HWindow hWindow, unsigned int width, unsigned int height)
    {
        if (m_pGraphicsWorld != NULL)
        {
            m_pGraphicsWorld->SetRenderTargetViewport(m_pGraphicsWorld->GetRenderTargetByWindow(reinterpret_cast<HWND>(hWindow)), 0, 0, width, height);
            
            if (hWindow == m_hMainWindow)
            {
                m_gui.SetSurfaceSize(m_hWindowSurface, width, height);
            }
        }
    }



private:

    /// The config file.
    GT::ConfigFile m_config;


    /// The graphics API.
    GT::GraphicsAPI* m_pGraphicsAPI;

    /// The default graphics world.
    GT::DefaultGraphicsWorld* m_pGraphicsWorld;

    /// The graphics resource manager.
    GT::GraphicsAssetResourceManager* m_pGraphicsAssetResourceManager; 

    /// The GUI renderer.
    GT::GraphicsWorldGUIRenderer m_guiRenderer;

    /// The GUI font manager. For now we're using GDI, but later on we'll make once specific to GraphicsWorld which will be better optimized.
    GT::GUIFontManager_GDI m_guiFontManager;

    /// The game's GUI.
    GT::GUIContext m_gui;


    /// The main window.
    GT::HWindow m_hMainWindow;

    /// The render target for the main window.
    GT::HGraphicsRenderTarget m_hWindowRT;

    /// The GUI surface associated with the main window.
    GT::HGUISurface m_hWindowSurface;


    /// Keeps track of whether or not the shift key is down.
    bool m_isShiftKeyDown;


    /// The scene callback.
    SandboxSceneCallback m_sceneCallback;

    /// The scene.
    GT::Scene m_scene;


    /// Test scene node.
    GT::SceneNode* m_pTestSceneNode;
};


int main(int argc, char** argv)
{
#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    int result = 1;
    {
        GT::EngineContext engine;
        if (GT::Succeeded(engine.Startup(argc, argv)))
        {
            SandboxGameState gameState;    
            GT::GameContext game(engine, gameState);
            if (GT::Succeeded(game.Startup()))
            {
                int exitCode = game.Run();

                game.Shutdown();
                engine.Shutdown();

                result = exitCode;
            }
            else
            {
                // Error initializing the game.
                engine.Shutdown();
                result = 2;
            }
        }
        else
        {
            // Error initializing the engine.
            result = 1;
        }
    }


    return result;
}
