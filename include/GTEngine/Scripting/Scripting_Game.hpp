// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Scripting_Game_hpp_
#define __GTEngine_Scripting_Game_hpp_

#include "../GameScript.hpp"

namespace GT
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
    bool LoadGameLibrary(GT::Script &script, GTEngine::Game &game);


    namespace GameFFI
    {
        /// Retrieves a reference to the global game object.
        GTEngine::Game & GetGame(GT::Script &script);


        /// Retrieves the absolute path of the directory containing the game executable.
        int GetExecutableDirectoryAbsolutePath(GT::Script &script);

        /// Retrieves the absolute path of the game executable.
        int GetExecutableAbsolutePath(GT::Script &script);


        /// Closes the game.
        int Close(GT::Script &script);

        /// Pauses the game.
        int Pause(GT::Script &script);

        /// Resumes the game.
        int Resume(GT::Script &script);

        /// Determines whether or not the game is paused.
        int IsPaused(GT::Script &script);

        /// Enables fullscreen mode.
        int EnableFullscreen(GT::Script &script);

        /// Disable fullscreen mode.
        int DisableFullscreen(GT::Script &script);


        /// Opens the editor.
        int OpenEditor(GT::Script &script);

        /// Closes the editor.
        int CloseEditor(GT::Script &script);


        /// Shows the debugging information.
        int ShowDebug(GT::Script &script);

        /// Hides the debugging information.
        int HideDebug(GT::Script &script);

        /// Determines if the given key is down.
        ///
        /// @remarks
        ///     Argument 1: The key code of the key to check.
        int IsKeyDown(GT::Script &script);

        /// Determines if the given mouse button is down.
        ///
        /// @remarks
        ///     Argument 1: the mouse button to check.
        int IsMouseButtonDown(GT::Script &script);

        /// Captures the mouse.
        int CaptureMouse(GT::Script &script);

        /// Releases the mouse.
        int ReleaseMouse(GT::Script &script);

        /// Determines whether or not the mouse is captured.
        int IsMouseCaptured(GT::Script &script);

        /// Retrieves the mouse offset for when it is captured as two floats.
        ///
        /// @remarks
        ///     This returns nil if the mouse is not captured.
        int GetMouseOffset(GT::Script &script);

        /// Enables mouse smoothing.
        int EnableMouseSmoothing(GT::Script &script);

        /// Disables mouse smoothing.
        int DisableMouseSmoothing(GT::Script &script);

        /// Determines whether or not mouse smoothing is enabled.
        int IsMouseSmoothingEnabled(GT::Script &script);


        /// Scans the data files for changes and posts the relevant events.
        ///
        /// @remarks
        ///     This happens immediately and synchronously.
        int ScanDataFilesForChanges(GT::Script &script);


        /// Retrieves the GUI element of the main game window.
        int GetGameWindowGUIElement(GT::Script &script);


        /// Saves the game state to the given file.
        ///
        /// @remarks
        ///     Use this for doing save games.
        ///     @par
        ///     Argument 1: A string containing the destination file path for the save game. Can be relative or absolute.
        ///     Return:     True if the file is saved successfully; false otherwise.
        int SaveGameState(GT::Script &script);

        /// Restores the game state from the given file.
        ///
        /// @remarks
        ///     Use this for doing load games.
        ///     @par
        ///     Argument 1: A string containing the source file path for the save game. Can be relative or absolute.
        ///     Return:     True if the game is loaded successfully; false otherwise.
        int LoadGameState(GT::Script &script);

        /// Loads the given scene.
        ///
        /// @remarks
        ///     Argument 1: A string containing the path of the scene to load.
        int LoadScene(GT::Script &script);


        /// Packages the game for distribution.
        ///
        /// @remarks
        ///     This should only be called by editting tools.
        ///     @par
        ///     Argument 1: The path of the destination directory, relative to the running executable.
        ///     Argument 2: The new name of the executable, not including the path.
        ///     Return:     True if successful.
        int PackageForDistribution(GT::Script &script);
    }
}

#endif