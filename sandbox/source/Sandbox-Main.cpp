
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>


int main(int argc, char** argv)
{
#if defined(GT_PLATFORM_LINUX)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    int retValue = 1;


    // The first thing we need is an engine context.
    GT::Engine::EngineContext engineContext(argc, argv);
    if (engineContext.Startup())
    {
        GTEngine::DefaultGameStateManager gameStateManager;
        GTEngine::Game game(engineContext, gameStateManager);
        if (game.Startup())
        {
            game.Run();
        }

        // Shutdown the game for cleanup.
        game.Shutdown();
    }

    // Shutdown the engine for cleanup.
    engineContext.Shutdown();


#if 0
    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    GTEngine::DefaultGameStateManager gameStateManager;

    auto game = GTEngine::Startup<GTEngine::Game>(argc, argv, gameStateManager);
    if (game != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = game->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GTEngine::Shutdown(game);
#endif


    return retValue;
}

