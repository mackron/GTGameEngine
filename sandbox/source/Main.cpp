
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

        m_pTestMaterialAsset = reinterpret_cast<GT::MaterialAsset*>(gameContext.GetEngineContext().LoadAsset("data/default.mtl/default"));
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
                m_hTestTexture0 = m_pGraphicsWorld->CreateTextureResource(m_pTestImageAsset->GetImageWidth(), m_pTestImageAsset->GetImageHeight(), 1, m_pTestImageAsset->GetImageFormat(), m_pTestImageAsset->GetImageData());



                // Materials.
                GT::vec4 materialRData(1.0f, 0.0f, 0.0f, 1.0f);
                GT::vec4 materialGData(0.0f, 1.0f, 0.0f, 1.0f);
                GT::vec4 materialBData(0.0f, 0.0f, 1.0f, 1.0f);

                /*GT::GraphicsMaterialResourceDesc materialR;
                materialR.channelDataSizeInBytes = sizeof(materialRData);
                materialR.pChannelData = &materialRData[0];
                m_hTestMaterialR = m_pGraphicsWorld->CreateMaterialResource(materialR);

                GT::GraphicsMaterialResourceDesc materialG;
                materialG.channelDataSizeInBytes = sizeof(materialGData);
                materialG.pChannelData = &materialGData[0];
                m_hTestMaterialG = m_pGraphicsWorld->CreateMaterialResource(materialG);

                GT::GraphicsMaterialResourceDesc materialB;
                materialB.channelDataSizeInBytes = sizeof(materialBData);
                materialB.pChannelData = &materialBData[0];
                m_hTestMaterialB = m_pGraphicsWorld->CreateMaterialResource(materialB);*/


                GT::GraphicsMaterialResourceDesc material0;
                material0.channelDataSizeInBytes = sizeof(materialRData);
                material0.pChannelData = &materialRData[0];

                GTLib::Vector<uint32_t> inputVariableData;
                inputVariableData.PushBack(static_cast<uint32_t>((8 << spv::WordCountShift) | static_cast<uint32_t>(GT::SpirVOp::DeclareMaterialVariable)));
                inputVariableData.PushBack(static_cast<uint32_t>(GT::SpirVCommonTypeID::Float4));
                inputVariableData.PushBack(reinterpret_cast<uint32_t*>(&materialGData[0])[0]);
                inputVariableData.PushBack(reinterpret_cast<uint32_t*>(&materialGData[0])[1]);
                inputVariableData.PushBack(reinterpret_cast<uint32_t*>(&materialGData[0])[2]);
                inputVariableData.PushBack(reinterpret_cast<uint32_t*>(&materialGData[0])[3]);
                inputVariableData.PushBack(static_cast<uint32_t>('tset')); inputVariableData.PushBack(static_cast<uint32_t>('\0gni'));

                inputVariableData.PushBack(static_cast<uint32_t>((3 << spv::WordCountShift) | static_cast<uint32_t>(GT::SpirVOp::DeclareMaterialVariable)));
                inputVariableData.PushBack(static_cast<uint32_t>(GT::SpirVCommonTypeID::Texture2D));
                inputVariableData.PushBack(static_cast<uint32_t>('\0pam'));

                material0.pInputVariableData           = inputVariableData.buffer;
                material0.inputVariableDataSizeInBytes = inputVariableData.count * sizeof(uint32_t);

                m_hTestMaterial0 = m_pGraphicsWorld->CreateMaterialResource(material0);

                


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
                m_pGraphicsWorld->SetMeshObjectMaterialInputVariable(m_hTestMeshObject0, 0, "testing", 1.0f, 1.0f, 1.0f, 1.0f);
                m_pGraphicsWorld->SetMeshObjectMaterialInputVariable(m_hTestMeshObject0, 0, "map", m_hTestTexture0);
                m_pGraphicsWorld->SetObjectPosition(m_hTestMeshObject0, GT::vec4(0, 0, 0, 0));

                //m_hTestMeshObject1 = m_pGraphicsWorld->CreateMeshObject(m_hTestMeshResource);
                //m_pGraphicsWorld->SetMeshObjectMaterial(m_hTestMeshObject1, 0, m_hTestMaterialG);
                //m_pGraphicsWorld->SetObjectPosition(m_hTestMeshObject1, GT::vec4(-2, 0, 0, 0));

                //m_hTestMeshObject2 = m_pGraphicsWorld->CreateMeshObject(m_hTestMeshResource);
                //m_pGraphicsWorld->SetMeshObjectMaterial(m_hTestMeshObject2, 0, m_hTestMaterialB);
                //m_pGraphicsWorld->SetObjectPosition(m_hTestMeshObject2, GT::vec4(2, 0, 0, 0));
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

    GT::HGraphicsResource m_hTestMaterialR;
    GT::HGraphicsResource m_hTestMaterialG;
    GT::HGraphicsResource m_hTestMaterialB;

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
