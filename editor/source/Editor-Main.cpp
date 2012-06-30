
/**
The stand-alone editor is actually implemented as a game. All it does is opens the editor at startup.

This editor works with the game's data files in exactly the same way as the in-game editor. Thus, you can use the stand-alone
editor in your development pipeline in addition to the in-game editor.

You will notice that there isn't much code here. This is because the editor is actually implemented directly in-engine. Doing
things this way allows any game to access the editting tools in-game, which is very useful for development. In addition, it
allows games to include the editor as part of the game itself, like Age of Empires :-).
*/

#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

class EditorGame : public GTEngine::Game
{
public:

    EditorGame(int argc, char** argv)
        : Game(argc, argv)
    {
        // First we'll configure the window...
        this->GetWindow()->SetTitle("GTGameEngine Editor");
        this->GetWindow()->SetSize(1280, 720);

        // Disable v-sync for now.
        //GTEngine::Renderer::SetSwapInterval(0);

        // We open the editor straight away.
        this->OpenEditor();
    }

    ~EditorGame()
    {
    }



// Event handlers.
public:

    // All we need to do here is step the GUI.
    void OnUpdate()
    {
        this->StepGUI(this->GetDeltaTimeInSeconds());
    }


private:
};


int main(int argc, char** argv)
{
#if defined(LICK_PLATFORM_LINUX)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    int retValue = 1;

    // The first thing to do is startup the engine.
    if (GTEngine::Startup(argc, argv))
    {
        // Now we need to instantiate a game object and run it.
        EditorGame game(argc, argv);
        if (game)
        {
            // Running the game will return a value that can be returned from main(). 0 will be returned if the
            // game ran and closed normally.
            retValue = game.Run();
        }
    }

    // When we get here it means the application wants to shutdown. The engine should be shutdown here for
    // a clean termination. Note how we're shutting down AFTER destructing the game object. It's best to
    // do cleaning up before shutting down.
    GTEngine::Shutdown();


    return retValue;
}

