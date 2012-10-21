
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

    EditorGame()
    {
    }

    ~EditorGame()
    {
    }



// Event handlers.
public:

    bool OnStartup(int argc, char** argv)
    {
        (void)argc;
        (void)argv;


        // First we'll configure the window...
        this->GetWindow()->SetTitle("GTGameEngine Editor");
        this->GetWindow()->SetSize(1280, 720);

        // Disable v-sync for now.
        //GTEngine::Renderer::SetSwapInterval(0);

        // We open the editor straight away.
        this->OpenEditor();

        return true;
    }

    // We don't want to let the editor close, so we just implement OnEditorClosing() and return false.
    bool OnEditorClosing()
    {
        return false;
    }


private:
};


int main(int argc, char** argv)
{
#if defined(GTCORE_PLATFORM_LINUX)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    int retValue = 1;

    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    auto game = GTEngine::Startup<EditorGame>(argc, argv);
    if (game != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = game->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GTEngine::Shutdown(game);


    return retValue;
}

