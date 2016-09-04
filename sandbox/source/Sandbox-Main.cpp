
//#include <GTGE/GTEngine.hpp>
#include "../source/GTGE.hpp"

#if defined(_WIN32)
#include <windows.h>
#endif


int main(int argc, char** argv)
{
#if defined(__linux__)
    XInitThreads();
#endif

#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


#if defined(_WIN32)
    //FreeConsole();
#endif


    int retValue = -1;

    // First we start up the engine, specifying the Game class we want to instantiate. This uses the default constructor.
    GT::DefaultGameStateManager gameStateManager;

#if 0
    GT::Context context(gameStateManager);
    if (context.Startup(argc, argv))
    {
        retValue = context.Run();
        context.Shutdown();
    }
#endif

    auto pContext = GT::Startup<GT::Context>(argc, argv, gameStateManager);
    if (pContext != nullptr)
    {
        // Now we run the game, keeping track of the return value.
        retValue = pContext->Run();
    }

    // And now we shut down the engine, passing the game object returned by Startup().
    GT::Shutdown(pContext);


    return retValue;
}

