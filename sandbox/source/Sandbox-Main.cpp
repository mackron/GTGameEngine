
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTCore/Profiling/valgrind/callgrind.h>

class SandboxGame : public GTEngine::Game
{
public:

    SandboxGame()
        : currentScene(nullptr), aspectRatio(0.0f)
    {
    }

    ~SandboxGame()
    {
    }


    ////////////////////////////////////////////
    // Event handlers.

    bool OnStartup(int, char**)
    {
        auto &script = this->GetScript();
        script.ExecuteFile("sandbox-config.lua");


        // First we'll configure the window...
        auto title = script.GetString("Game.Title");
        if (title != nullptr)
        {
            this->GetWindow()->SetTitle(title);
        }
        else
        {
            this->GetWindow()->SetTitle("GTGameEngine");
        }

        int width  = script.GetInteger("Game.Width");
        int height = script.GetInteger("Game.Height");

        if (width  == 0) width  = 1280;
        if (height == 0) height = 720;
        this->GetWindow()->SetSize(width, height);

        if (script.GetBoolean("Game.Fullscreen"))
        {
            this->EnableFullscreen();
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
        if (!this->OnLoadScene(script.GetString("Game.StartupScene")))
        {
            this->OpenEditor();
        }


        return true;
    }

    void OnShutdown()
    {
        delete this->currentScene;
    }

    void OnUpdate(double deltaTimeInSeconds)
    {
        if (!this->IsEditorOpen())      // <-- Don't bother updating the scene if the editor is open.
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
        
        CALLGRIND_STOP_INSTRUMENTATION;
    }

    void OnSize(unsigned int newWidth, unsigned int newHeight)
    {
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

    bool OnLoadScene(const char* sceneRelativePath)
    {
        auto newScene = new GTEngine::Scene;
        if (newScene->LoadFromFile(sceneRelativePath))
        {
            // The scene loaded successfully. The old scene needs to be replaced.
            delete this->currentScene;
            this->currentScene = newScene;

            // The default viewport needs to have it's size updated. We're going to simulate an OnSize event to handle this.
            unsigned int viewportWidth;
            unsigned int viewportHeight;
            this->GetWindow()->GetSize(viewportWidth, viewportHeight);
            this->OnSize(viewportWidth, viewportHeight);

            // In order for scripting to work we need to register it.
            this->currentScene->RegisterToScript(this->GetScript());
            newScene->PostSceneNodeScriptEvent_OnStartup();

            return true;
        }
        else
        {
            // The scene failed to load. Simply return false.
            delete newScene;
            return false;
        }
    }

    // OnEditorClosing() can be used to let the engine know whether or not the editor is allowed to close. If we don't
    // have a scene to go back to, we don't want to close the editor.
    bool OnEditorClosing()
    {
        return this->currentScene != nullptr;
    }


private:

    /// A pointer to the current scene. We use a pointer because we'll be dynamically creating and deleting scenes.
    GTEngine::Scene* currentScene;

    /// The aspect ratio to use. If this is 0, the aspect ratio will be re-calculated dynamically when the window is resized.
    float aspectRatio;
    
    
private:    // No copying.
    SandboxGame(const SandboxGame &);
    SandboxGame & operator=(const SandboxGame &);
};


int main(int argc, char** argv)
{
    CALLGRIND_ZERO_STATS;
    
#if defined(GTCORE_PLATFORM_LINUX)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    int retValue = 1;

    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    auto game = GTEngine::Startup<SandboxGame>(argc, argv);
    if (game != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = game->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GTEngine::Shutdown(game);


    return retValue;
}

