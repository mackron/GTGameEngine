
#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameState.hpp>
#include <GTGameEngine/Graphics/DefaultGraphicsWorld.hpp>
#include <GTGameEngine/Assets/ModelAsset.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>
#include <GTGameEngine/Assets/MaterialAsset.hpp>
#include <GTGameEngine/ConfigFile.hpp>
#include <GTGameEngine/external/spirv.h>
#include <GTLib/IO.hpp>

#include <GTGameEngine/../../source/external/easy_mtl/easy_mtl.h>

class SandboxGameState : public GT::GameState
{
public:

    /// Constructor.
    SandboxGameState()
        : m_pGraphicsAPI(nullptr), 
          m_pGraphicsWorld(nullptr),
          m_hMainWindow(0),
          m_isShiftKeyDown(false)
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


        m_pTestMeshAsset = reinterpret_cast<GT::ModelAsset*>(gameContext.GetEngineContext().LoadAsset("data/cube.obj"));
        if (m_pTestMeshAsset == nullptr)
        {
            return false;
        }

        m_pTestImageAsset = reinterpret_cast<GT::ImageAsset*>(gameContext.GetEngineContext().LoadAsset("data/default.png"));
        if (m_pTestImageAsset == nullptr)
        {
            return false;
        }

        m_pTestMaterialAsset = reinterpret_cast<GT::MaterialAsset*>(gameContext.GetEngineContext().LoadAsset("data/defaults.mtl/default"));
        if (m_pTestMaterialAsset == nullptr)
        {
            return false;
        }


        m_pGraphicsAPI = gameContext.GetEngineContext().GetBestGraphicsAPI();
        assert(m_pGraphicsAPI != nullptr);
        {
            m_pGraphicsWorld = new GT::DefaultGraphicsWorld(gameContext.GetGUI(), *m_pGraphicsAPI);
            if (m_pGraphicsWorld->Startup())
            {
                m_hWindowRT = m_pGraphicsWorld->CreateRenderTargetFromWindow(reinterpret_cast<HWND>(m_hMainWindow), 0);
                m_pGraphicsWorld->SetRenderTargetProjectionAndView(m_hWindowRT, GT::mat4::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f), GT::mat4::translate(GT::mat4::identity, GT::vec4(0, 0, -5, 0.0f)));



                // Textures.
                GT::GraphicsTextureResourceDesc testTextureDesc;
                testTextureDesc.width = m_pTestImageAsset->GetImageWidth();
                testTextureDesc.height = m_pTestImageAsset->GetImageHeight();
                testTextureDesc.depth  = 1;
                testTextureDesc.format = m_pTestImageAsset->GetImageFormat();
                testTextureDesc.pData  = m_pTestImageAsset->GetImageData();
                m_hTestTexture0 = m_pGraphicsWorld->CreateTextureResource(testTextureDesc);

                // Materials.
                GT::GraphicsMaterialResourceDesc material0;
                material0.pData           = m_pTestMaterialAsset->GetData();
                material0.dataSizeInBytes = m_pTestMaterialAsset->GetDataSizeInBytes();
                m_hTestMaterial0 = m_pGraphicsWorld->CreateMaterialResource(material0);

                // Meshes.
                GT::GraphicsMeshResourceDesc testMeshDesc;
                testMeshDesc.topology                      = GT::PrimitiveTopologyType_Triangle;
                testMeshDesc.pVertexData                   = m_pTestMeshAsset->GetMeshVertexData(0);
                testMeshDesc.vertexDataSize                = m_pTestMeshAsset->GetMeshVertexDataSize(0);
                testMeshDesc.vertexStride                  = m_pTestMeshAsset->GetMeshVertexStride(0);
                testMeshDesc.pVertexLayout                 = m_pTestMeshAsset->GetMeshVertexLayout(0, testMeshDesc.vertexAttribCount);
                testMeshDesc.pIndexData                    = m_pTestMeshAsset->GetMeshIndexData(0);
                testMeshDesc.indexDataSize                 = m_pTestMeshAsset->GetMeshIndexDataSize(0);
                testMeshDesc.indexCount                    = m_pTestMeshAsset->GetMeshIndexCount(0);
                testMeshDesc.indexFormat                   = m_pTestMeshAsset->GetMeshIndexFormat(0);
                testMeshDesc.materialIndexOffsetCountPairs = m_pTestMeshAsset->GetMeshMaterialIndexOffsetCountPairs(0);
                testMeshDesc.materialCount                 = m_pTestMeshAsset->GetMeshMaterialCount(0);
                m_hTestMeshResource = m_pGraphicsWorld->CreateMeshResource(testMeshDesc);
                m_pGraphicsWorld->SetMeshResourceMaterial(m_hTestMeshResource, 0, m_hTestMaterial0);

                m_hTestMeshObject0 = m_pGraphicsWorld->CreateMeshObject(m_hTestMeshResource);
                m_pGraphicsWorld->SetMeshObjectMaterial(m_hTestMeshObject0, 0, m_hTestMaterial0);
                m_pGraphicsWorld->SetMeshObjectMaterialInputVariable(m_hTestMeshObject0, 0, "DiffuseColor", 0.75f, 0.75f, 1.0f, 1.0f);
                m_pGraphicsWorld->SetMeshObjectMaterialInputVariable(m_hTestMeshObject0, 0, "DiffuseMap", m_hTestTexture0);
                m_pGraphicsWorld->SetObjectPosition(m_hTestMeshObject0, GT::vec4(0, 0, 0, 0));
            }
        }


        return true;
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
        }
    }



private:

    /// The config file.
    GT::ConfigFile m_config;


    /// The graphics API.
    GT::GraphicsAPI* m_pGraphicsAPI;

    /// The default graphics world.
    GT::DefaultGraphicsWorld* m_pGraphicsWorld;


    /// The main window.
    GT::HWindow m_hMainWindow;

    /// The render target for the main window.
    GT::HGraphicsRenderTarget m_hWindowRT;

    /// Keeps track of whether or not the shift key is down.
    bool m_isShiftKeyDown;


    GT::ModelAsset* m_pTestMeshAsset;
    GT::MaterialAsset* m_pTestMaterialAsset;
    GT::ImageAsset* m_pTestImageAsset;

    GT::HGraphicsResource m_hTestTexture0;
    GT::HGraphicsResource m_hTestMaterial0;

    GT::HGraphicsResource m_hTestMeshResource;
    GT::HGraphicsObject   m_hTestMeshObject0;
    GT::HGraphicsObject   m_hTestMeshObject1;
    GT::HGraphicsObject   m_hTestMeshObject2;
};


int main(int argc, char** argv)
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

            return exitCode;
        }
        else
        {
            // Error initializing the game.
            engine.Shutdown();
            return 2;
        }
    }
    else
    {
        // Error initializing the engine.
        return 1;
    }    
}
