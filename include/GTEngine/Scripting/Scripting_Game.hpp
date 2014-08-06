// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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
        bool LoadGameLibrary(GTLib::Script &script, Game &game);


        namespace GameFFI
        {
            /// Retrieves a reference to the global game object.
            Game & GetGame(GTLib::Script &script);


            /// Retrieves the absolute path of the directory containing the game executable.
            int GetExecutableDirectoryAbsolutePath(GTLib::Script &script);

            /// Retrieves the absolute path of the game executable.
            int GetExecutableAbsolutePath(GTLib::Script &script);


            /// Closes the game.
            int Close(GTLib::Script &script);

            /// Pauses the game.
            int Pause(GTLib::Script &script);

            /// Resumes the game.
            int Resume(GTLib::Script &script);

            /// Determines whether or not the game is paused.
            int IsPaused(GTLib::Script &script);

            /// Enables fullscreen mode.
            int EnableFullscreen(GTLib::Script &script);

            /// Disable fullscreen mode.
            int DisableFullscreen(GTLib::Script &script);


            /// Opens the editor.
            int OpenEditor(GTLib::Script &script);

            /// Closes the editor.
            int CloseEditor(GTLib::Script &script);


            /// Shows the debugging information.
            int ShowDebug(GTLib::Script &script);

            /// Hides the debugging information.
            int HideDebug(GTLib::Script &script);

            /// Determines if the given key is down.
            ///
            /// @remarks
            ///     Argument 1: The key code of the key to check.
            int IsKeyDown(GTLib::Script &script);

            /// Determines if the given mouse button is down.
            ///
            /// @remarks
            ///     Argument 1: the mouse button to check.
            int IsMouseButtonDown(GTLib::Script &script);

            /// Captures the mouse.
            int CaptureMouse(GTLib::Script &script);

            /// Releases the mouse.
            int ReleaseMouse(GTLib::Script &script);

            /// Determines whether or not the mouse is captured.
            int IsMouseCaptured(GTLib::Script &script);

            /// Retrieves the mouse offset for when it is captured as two floats.
            ///
            /// @remarks
            ///     This returns nil if the mouse is not captured.
            int GetMouseOffset(GTLib::Script &script);

            /// Enables mouse smoothing.
            int EnableMouseSmoothing(GTLib::Script &script);

            /// Disables mouse smoothing.
            int DisableMouseSmoothing(GTLib::Script &script);

            /// Determines whether or not mouse smoothing is enabled.
            int IsMouseSmoothingEnabled(GTLib::Script &script);


            /// Scans the data files for changes and posts the relevant events.
            ///
            /// @remarks
            ///     This happens immediately and synchronously.
            int ScanDataFilesForChanges(GTLib::Script &script);


            /// Retrieves the GUI element of the main game window.
            int GetGameWindowGUIElement(GTLib::Script &script);


            /// Saves the game state to the given file.
            ///
            /// @remarks
            ///     Use this for doing save games.
            ///     @par
            ///     Argument 1: A string containing the destination file path for the save game. Can be relative or absolute.
            ///     Return:     True if the file is saved successfully; false otherwise.
            int SaveGameState(GTLib::Script &script);

            /// Restores the game state from the given file.
            ///
            /// @remarks
            ///     Use this for doing load games.
            ///     @par
            ///     Argument 1: A string containing the source file path for the save game. Can be relative or absolute.
            ///     Return:     True if the game is loaded successfully; false otherwise.
            int LoadGameState(GTLib::Script &script);

            /// Loads the given scene.
            ///
            /// @remarks
            ///     Argument 1: A string containing the path of the scene to load.
            int LoadScene(GTLib::Script &script);


            /// Packages the game for distribution.
            ///
            /// @remarks
            ///     This should only be called by editting tools.
            ///     @par
            ///     Argument 1: The path of the destination directory, relative to the running executable.
            ///     Argument 2: The new name of the executable, not including the path.
            ///     Return:     True if successful.
            int PackageForDistribution(GTLib::Script &script);
        }
    }
}

#endif