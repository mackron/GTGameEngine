
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTLib/Profiling/valgrind/callgrind.h>

class SandboxGameStateManager : public GTEngine::GenericGameStateManager
{
public:

    /// Constructor.
    SandboxGameStateManager()
        : currentScene(nullptr), currentSceneRelativePath(), nextScene(nullptr), nextSceneRelativePath(), aspectRatio(0.0f)
    {
    }

    /// Destructor.
    ~SandboxGameStateManager()
    {
    }


    ////////////////////////////////////////
    // Events

    /// GameStateManager::OnStartup().
    bool OnStartup(GTEngine::Game &game, const GTLib::CommandLine &commandLine)
    {
        (void)commandLine;

        auto &script = game.GetScript();
        script.ExecuteFile("sandbox-config.lua");


        // First we'll configure the window...
        auto window = game.GetWindow();
        if (window != nullptr)
        {
            auto title = script.GetString("Game.Title");
            if (title != nullptr)
            {
                window->SetTitle(title);
            }
            else
            {
                window->SetTitle("GTGameEngine");
            }

            int width  = script.GetInteger("Game.Width");
            int height = script.GetInteger("Game.Height");

            if (width  == 0) width  = 1280;
            if (height == 0) height = 720;
            window->SetSize(width, height);
        }

        

        if (script.GetBoolean("Game.Fullscreen"))
        {
            game.EnableFullscreen();
        }


        if (script.GetBoolean("Game.EnableVSync"))
        {
            GTEngine::Renderer::SetSwapInterval(1);
        }
        else
        {
            GTEngine::Renderer::SetSwapInterval(0);
        }


        this->aspectRatio = script.GetFloat("Game.AspectRatio");


        // Load the default scene, if any. To do this, we're just going to call the OnLoadScene() event handler which
        // will handle everything for us. Typically this event handler will get called from the scripting environment
        // to give the game a chance to change scenes.
        //
        // If this fails, we want to just start up the editor.
        if (!this->LoadScene(game, script.GetString("Game.StartupScene")))
        {
            game.OpenEditor();
        }


        return true;
    }


    void OnShutdown(GTEngine::Game &game)
    {
        (void)game;

        delete this->currentScene;
    }

    void OnUpdate(GTEngine::Game &game, double deltaTimeInSeconds)
    {
        if (!game.IsEditorOpen())      // <-- Don't bother updating the scene if the editor is open.
        {
            if (this->currentScene != nullptr)
            {
                this->currentScene->Update(deltaTimeInSeconds);
            }


            // With the scene drawn, we should draw a fullscreen triangle over the whole window to show the viewport contents.
            if (this->currentScene != nullptr)
            {
                GTEngine::Renderer::SetCurrentFramebuffer(nullptr);
                GTEngine::Renderer::Utils::DrawFullscreenQuad(this->currentScene->GetDefaultViewport().GetColourBuffer());
            }
        }
        
        //CALLGRIND_STOP_INSTRUMENTATION;
    }

    void OnStartFrame(GTEngine::Game &game)
    {
        // If a scene needs to be switched, it needs to be done so now.
        if (this->nextScene != nullptr)
        {
            delete this->currentScene;
            this->currentScene             = this->nextScene;
            this->currentSceneRelativePath = this->nextSceneRelativePath;

            // The default viewport needs to have it's size updated. We're going to simulate an OnSize event to handle this.
            unsigned int viewportWidth;
            unsigned int viewportHeight;
            game.GetWindow()->GetSize(viewportWidth, viewportHeight);
            this->OnSize(game, viewportWidth, viewportHeight);

            // In order for scripting to work we need to register it.
            this->currentScene->PostSceneNodeScriptEvent_OnStartup();


            this->nextScene             = nullptr;
            this->nextSceneRelativePath.Clear();
        }
    }


    void OnSize(GTEngine::Game &game, unsigned int newWidth, unsigned int newHeight)
    {
        (void)game;

        if (this->currentScene != nullptr)
        {
            this->currentScene->GetDefaultViewport().Resize(newWidth, newHeight);

            auto cameraNode = this->currentScene->GetDefaultViewport().GetCameraNode();
            if (cameraNode != nullptr)
            {
                auto cameraComponent = cameraNode->GetComponent<GTEngine::CameraComponent>();
                if (cameraComponent != nullptr)
                {
                    float fov    = cameraComponent->perspective.fov;
                    float aspect = this->aspectRatio;
                    float znear  = cameraComponent->zNear;
                    float zfar   = cameraComponent->zFar;

                    if (aspect == 0.0f)
                    {
                        aspect = static_cast<float>(newWidth) / static_cast<float>(newHeight);
                    }

                    cameraComponent->Set3DProjection(fov, aspect, znear, zfar);
                }
            }
        }
    }

    // OnEditorClosing() can be used to let the engine know whether or not the editor is allowed to close. If we don't
    // have a scene to go back to, we don't want to close the editor.
    bool OnEditorClosing(GTEngine::Game &game)
    {
        (void)game;

        return this->currentScene != nullptr;
    }




    bool LoadScene(GTEngine::Game &game, const char* sceneRelativePath)
    {
        // Here we'll load the scene, but we don't actually do the switch until the next frame. The reason for this is that the current scene may be in
        // the middle of updating, so we don't want any mixups there. The actual switch will be done in OnStartFrame(), which is a synchronized call.

        auto newScene = new GTEngine::Scene;

        // The scene should be registered to the script immediately. No real reason it shouldn't be. Later on, scenes will take an EngineContext object in their
        // constructors which will then allow us to register it with the script in the constructor.
        newScene->RegisterToScript(game.GetScript());
        
        // Only after registering the scene with the script should we do the load.
        if (newScene->LoadFromFile(sceneRelativePath))
        {
            // The scene state needs to be restored appropriately.
            auto sceneRestoreCommands = this->GetSceneData(sceneRelativePath);
            if (sceneRestoreCommands != nullptr)
            {
                sceneRestoreCommands->Execute(*newScene);
            }


            // The state of the previous scene needs to be updated in the game state manager.
            if (this->currentScene != nullptr)
            {
                this->UpdateSceneState(this->currentSceneRelativePath.c_str(), *this->currentScene);
            }


            // The scene loaded successfully. The old scene needs to be replaced.
            this->nextScene             = newScene;
            this->nextSceneRelativePath = sceneRelativePath;

            return true;
        }
        else
        {
            // The scene failed to load. Simply return false.
            delete newScene;
            return false;
        }
    }



    ///////////////////////////////////////
    // Serialization

    /// GenericGameStateManager::Serialize()
    virtual bool Serialize(GTEngine::Game &game, GTLib::Serializer &serializer)
    {
        // We need to update the current scene state so that it can be restored properly later.
        if (this->currentScene != nullptr)
        {
            this->UpdateSceneState(this->currentSceneRelativePath.c_str(), *this->currentScene);
        }

        // Now we just call the base implementation of Serialize() like normal.
        return GenericGameStateManager::Serialize(game, serializer);
    }

    /// GenericGameStateManager::SerializeHeaderData()
    virtual uint32_t SerializeHeaderData(GTEngine::Game &game, GTLib::Serializer &serializer)
    {
        // The name of the current scene - we're going to load this when we deserialize.
        serializer.WriteString(this->currentSceneRelativePath);

        return 1;
    }

    /// GenericGameStateManager::SerializeGlobalData()
    virtual uint32_t SerializeGlobalData(GTEngine::Game &game, GTLib::Serializer &serializer)
    {
        return 1;
    }



    ///////////////////////////////////////
    // Deserialization

    /// GenericGameStateManager::Deserialize()
    virtual bool Deserialize(GTEngine::Game &game, GTLib::Deserializer &deserializer)
    {
        // Call the base implementation of Deserialize(). This will call DeserializeHeaderData() and DeserializeGlobalData().
        if (GenericGameStateManager::Deserialize(game, deserializer))
        {
            // We want to now load the current scene - the path will be stored as this->nextSceneRelativePath.
            this->LoadScene(game, this->nextSceneRelativePath.c_str());

            return true;
        }

        return false;
    }

    /// GenericGameStateManager::DeserializeHeaderData()
    virtual bool DeserializeHeaderData(GTEngine::Game &game, GTLib::Deserializer &deserializer, uint32_t version)
    {
        (void)game;

        if (version == 1)
        {
            deserializer.ReadString(this->nextSceneRelativePath);
        }
        
        return true;
    }

    /// GenericGameStateManager::DeserializeGlobalData()
    virtual bool DeserializeGlobalData(GTEngine::Game &game, GTLib::Deserializer &deserializer, uint32_t version)
    {
        (void)game;
        (void)deserializer;

        if (version == 1)
        {
        }

        return true;
    }


private:

    /// A pointer to the current scene. We use a pointer because we'll be dynamically creating and deleting scenes.
    GTEngine::Scene* currentScene;

    /// The relative path of the current scene. We need this so we can update the game state manager when a scene changes.
    GTLib::String currentSceneRelativePath;

    /// A pointer to the next scene that's just about to be loaded. When a scene is loaded, we don't want to switch scenes
    /// immediately because there is a chance it will be in the middle of updating. Instead, we want to delay the loading
    /// until the next frame, in which case we'll do the switch.
    GTEngine::Scene* nextScene;

    /// The relative path of the next scene that's just about to be loaded.
    GTLib::String nextSceneRelativePath;

    /// The aspect ratio to use. If this is 0, the aspect ratio will be re-calculated dynamically when the window is resized.
    float aspectRatio;
};



int main(int argc, char** argv)
{
    //CALLGRIND_ZERO_STATS;
    
#if defined(GTCORE_PLATFORM_LINUX)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    int retValue = 1;

    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    SandboxGameStateManager gameStateManager;

    auto game = GTEngine::Startup<GTEngine::Game>(argc, argv, gameStateManager);
    if (game != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = game->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GTEngine::Shutdown(game);


    return retValue;
}

