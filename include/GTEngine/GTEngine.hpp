
#ifndef __GTEngine_hpp_
#define __GTEngine_hpp_

#include "Game.hpp"

namespace GTEngine
{
    extern Game* GlobalGame;


    /// Starts up the engine.
    ///
    /// @param argc [in] The argument count.
    /// @param argv [in] The argument strings.
    ///
    /// @return True if the engine is initialised successfully; false otherwise.
    bool _PreStartup(int argc, char** argv);


    /// Starts up the engine, returning an instance of the given game class (T).
    ///
    /// @param argc [in] The argument count from main().
    /// @param argv [in] The argument strings from main().
    ///
    /// @return A pointer to the main game object.
    ///
    /// @remarks
    ///     The 'T' template argument is the type of the game object to create. This will use the default constructor. Thus, the game
    ///     object MUST use a default constructor.
    template <typename T>
    T* Startup(int argc, char** argv)
    {
        if (GlobalGame == nullptr)
        {
            if (_PreStartup(argc, argv))
            {
                GlobalGame = new T;
                if (!GlobalGame->Startup(argc, argv))
                {
                    delete GlobalGame;
                    GlobalGame = nullptr;
                }
            }
        }

        return static_cast<T*>(GlobalGame);
    }

    /// Shuts down the engine, deleting the given game object returned by Startup().
    ///
    /// @param game [in] The game object returned by Startup().
    void Shutdown(Game* game);

    


    /// Retrieves the directory containing the executable.
    ///
    /// @return The directory containing the executable.
    const char* GetExecutableDirectory();
}

#endif
