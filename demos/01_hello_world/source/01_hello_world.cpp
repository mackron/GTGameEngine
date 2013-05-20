
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ShaderLibrary.hpp>


class ExampleGame : public GTEngine::Game
{
public:

    ExampleGame()
        : Game(), scene(), viewport(),
          cameraNode(), modelNode()
    {
    }

    ~ExampleGame()
    {
    }



// Event handlers.
public:


    // This is called during the initialisation step of the game.
    bool OnStartup(int argc, char** argv)
    {
        (void)argc;
        (void)argv;

        // First we'll configure the window...
        this->GetWindow()->SetTitle("Example 01 - Hello, World");
        this->GetWindow()->SetSize(1280, 720);

        // We'll clear the background to a grey colour so everything looks a bit nicer. Also, this makes HDR play a little bit better.
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.25f, 0.25f, 0.25f);

        // Now we'll setup the viewport. First we'll need to setup the camera. Scene nodes are component based. To
        // create a camera node, all we need to do is add a camera component. In this case, we'll create a 3D/perspective
        // camera component.
        this->cameraNode.AddComponent<GTEngine::CameraComponent>();
        this->cameraNode.SetPosition(0.0f, 0.0f, 10.0f);
        this->cameraNode.AddComponent<GTEngine::AmbientLightComponent    >()->SetColour(0.1f,  0.1f,  0.1f);
        this->cameraNode.AddComponent<GTEngine::DirectionalLightComponent>()->SetColour(0.25f, 0.25f, 0.25f);
        

        // Now we set the add the viewport to the scene.
        this->viewport.SetCameraNode(this->cameraNode);
        this->scene.AddViewport(this->viewport);


        // We create a model by adding a model component and calling SetModel() on the component.
        this->modelNode.AddComponent<GTEngine::ModelComponent>()->SetModel("engine/models/default.dae");
        
        
        // Now we need to add the scene nodes to the scene. Camera's do not need to be added to the scene, but since we've got
        // a light attached to it, we need to.
        this->scene.AddSceneNode(this->cameraNode);
        this->scene.AddSceneNode(this->modelNode);


        return true;
    }

    // This is called during the shutdown phase.
    void OnShutdown()
    {
    }

    // This is called on the update thread at the start of every frame. This is a good place to update the scene. Updating a scene
    // will also render it. You should not update more than one scene at a time.
    void OnUpdate(double deltaTimeInSeconds)
    {
        // We will rotate the model node a bit.
        this->modelNode.RotateY(static_cast<float>(20.0 * deltaTimeInSeconds));

        // The scene needs to be updated in order to be rendered.
        this->scene.Update(deltaTimeInSeconds);
    }

    // This is called when the frame has finished. No other threads will be executing when this is called. It is
    // completely thread-safe.
    //
    // The rendering commands for the next frame will be cached in preparation for execution in the next frame. This
    // is a good time to tell the renderer to draw the final fullscreen triangle. Of course, we don't want to do
    // this if the editor is open, otherwise it will be drawn over the top of it.
    void OnEndFrame()
    {
        if (!this->IsEditorOpen())
        {
            GTEngine::Renderer::SetCurrentFramebuffer(nullptr);
            GTEngine::Renderer::Utils::DrawFullscreenQuad(this->viewport.GetColourBuffer());
        }
    }

    // This is called when the game window is sized.
    void OnSize(unsigned int newWidth, unsigned int newHeight)
    {
        this->viewport.Resize(newWidth, newHeight);

        auto camera = this->cameraNode.GetComponent<GTEngine::CameraComponent>();
        camera->Set3DProjection(90.0f, static_cast<float>(newWidth) / static_cast<float>(newHeight), 0.1f, 1000.0f);
    }



private:

    // The scene where all of the game scene nodes will be located.
    GTEngine::Scene scene;

    // In order to display the scene, we are going to need a viewport. You can attach multiple viewports
    // to a scene. This could be useful for things like split-screen or editors.
    GTEngine::SceneViewport viewport;


    // The camera node where everything will be drawn from.
    GTEngine::SceneNode cameraNode;

    // A model node for this example.
    GTEngine::SceneNode modelNode;
};


int main(int argc, char** argv)
{
#if defined(GTCORE_PLATFORM_LINUX)
    XInitThreads();
#endif

    int retValue = 1;

    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    auto game = GTEngine::Startup<ExampleGame>(argc, argv);
    if (game != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = game->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GTEngine::Shutdown(game);


    return retValue;
}

