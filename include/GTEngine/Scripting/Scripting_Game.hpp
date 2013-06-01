// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Game_hpp_
#define __GTEngine_Scripting_Game_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the Game script library.
        ///
        /// @param script [in] The script to load the Game library into.
        /// @param game   [in] A reference to the game object.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This will create the global Game object.
        bool LoadGameLibrary(GTCore::Script &script, Game &game);


        namespace GameFFI
        {
            /// Retrieves a reference to the global game object.
            Game & GetGame(GTCore::Script &script);


            /// Closes the game.
            int Close(GTCore::Script &script);

            /// Pauses the game.
            int Pause(GTCore::Script &script);

            /// Resumes the game.
            int Resume(GTCore::Script &script);

            /// Enables fullscreen mode.
            int EnableFullscreen(GTCore::Script &script);

            /// Disable fullscreen mode.
            int DisableFullscreen(GTCore::Script &script);


            /// Shows the debugging information.
            int ShowDebug(GTCore::Script &script);

            /// Hides the debugging information.
            int HideDebug(GTCore::Script &script);

            /// Determines if the given key is down.
            ///
            /// @remarks
            ///     Argument 1: The key code of the key to check.
            int IsKeyDown(GTCore::Script &script);

            /// Determines if the given mouse button is down.
            ///
            /// @remarks
            ///     Argument 1: the mouse button to check.
            int IsMouseButtonDown(GTCore::Script &script);

            /// Captures the mouse.
            int CaptureMouse(GTCore::Script &script);

            /// Releases the mouse.
            int ReleaseMouse(GTCore::Script &script);

            /// Determines whether or not the mouse is captured.
            int IsMouseCaptured(GTCore::Script &script);

            /// Retrieves the mouse offset for when it is captured as two floats.
            ///
            /// @remarks
            ///     This returns nil if the mouse is not captured.
            int GetMouseOffset(GTCore::Script &script);


            /// Scans the data files for changes and posts the relevant events.
            ///
            /// @remarks
            ///     This happens immediately and synchronously.
            int ScanDataFilesForChanges(GTCore::Script &script);


            /// Retrieves the GUI element of the main game window.
            int GetGameWindowGUIElement(GTCore::Script &script);

            /// Loads the given scene.
            ///
            /// @remarks
            ///     Argument 1: A string containing the path of the scene to load.
            int LoadScene(GTCore::Script &script);



            // TODO: These are now in GTEngine, but are still used in the editor. These references need to be deleted at some point.

            /// Executes the script defined in the given file.
            ///
            /// @remarks
            ///     Argument 1: The name of the script file to load and execute.
            int ExecuteFile(GTCore::Script &script);

            /// Executes the given script text.
            ///
            /// @remarks
            ///     Argument 1: The script text to execute.
            int ExecuteScript(GTCore::Script &script);

            /// Retrieves the last script error.
            int GetLastScriptError(GTCore::Script &script);

        }
    }
}

#endif