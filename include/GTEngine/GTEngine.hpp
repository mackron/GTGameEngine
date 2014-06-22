/*
GTGameEngine - https://github.com/mackron/GTGameEngine

Copyright (C) 2012 David Reid

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#ifndef __GTEngine_hpp_
#define __GTEngine_hpp_

#include "Game.hpp"

#define GTENGINE_VERSION_STRING "0.0.4"

namespace GTEngine
{
    extern Game* GlobalGame;


    /// Starts up the engine.
    ///
    /// @param commandLine [in] A reference to the command line.
    ///
    /// @return True if the engine is initialised successfully; false otherwise.
    bool _PreStartup(const GTLib::CommandLine &commandLine);


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
    T* Startup(int argc, char** argv, GameStateManager &gameStateManager)
    {
        if (GlobalGame == nullptr)
        {
            GTLib::CommandLine commandLine(argc, argv);
            if (_PreStartup(commandLine))
            {
                GlobalGame = new T(gameStateManager);
                if (!GlobalGame->Startup(commandLine))
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

    
    /// Retrieves the version string.
    inline const char* GetVersionString() { return GTENGINE_VERSION_STRING; }
}

#endif
