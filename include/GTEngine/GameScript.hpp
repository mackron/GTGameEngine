
#ifndef __GTEngine_GameScript_hpp_
#define __GTEngine_GameScript_hpp_

#include <GTCore/Script.hpp>
#include <GTCore/String.hpp>
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    class Game;

    /// Class representing the scripting environment of a game.
    ///
    /// This class is actually a GTCore::Script object, but with additional stuff to make it suitable for use by a game.
    ///
    /// Anything involving a script is done through this, including configuration.
    ///
    /// The commands sent to the engine are actually just Lua scripts which are executed through the scripting environment.
    class GameScript : public GTCore::Script
    {
    public:

        /// Constructor.
        GameScript(Game &game);

        /// Destructor.
        ~GameScript();

        /// Starts up the game script.
        ///
        /// @return True if the script is started up properly.
        bool Startup();


        /// GTCore::Script::LoadFile().
        ///
        /// This is a special implementation that keeps track of the file so we can do auto-reloading.
        bool LoadFile(const char* fileName);


        /// Retrieves a reference to the game object that owns this environment.
              Game & GetGame()       { return this->game; }
        const Game & GetGame() const { return this->game; }


        /// Retrieves a string containing the last error message for a failed script.
        const char* GetLastError() { return this->lastError.c_str(); }

        /// Sets the last error message.
        ///
        /// @remarks
        ///     This should only really be used internally. Mainly used for the error handler.
        void SetLastError(const char* newLastError) { this->lastError = newLastError; }


        /// Determines whether or not the file has been loaded.
        ///
        /// @param fileName [in] The name of the file to check.
        ///
        /// @return True if the file has already been loaded, false otherwise.
        bool HasFileBeenLoaded(const char* fileName) const;



        /////////////////////////////////////////////
        // Public FFI functions.

        struct FFI
        {
            /// A helper for retrieving the Game instance from the script.
            static Game & GetGameObject(GTCore::Script &script);
        };


    private:

        /// Private function for registering the foreign-function interface.
        bool RegisterFFI();


    private:

        /// A reference to the game that owns this environment.
        Game &game;

        /// The last error message generated by a call to execute.
        GTCore::String lastError;

        /// Keeps track of the loaded files.
        GTCore::Vector<GTCore::String> loadedFiles;


        /// The error handler of the game script.
        struct ErrorHandler : GTCore::ScriptErrorHandler
        {
            /// Constructor.
            ErrorHandler(GameScript &script);

            /// GTCore::ScriptErrorHandler::OnError().
            void OnError(GTCore::Script &, const char* message);



            /// A reference to the game script that owns this error handler.
            GameScript &script;


        private:    // No copying.
            ErrorHandler(const ErrorHandler &);
            ErrorHandler & operator=(const ErrorHandler &);

        }errorHandler;
    };
}

#endif