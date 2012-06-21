
#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

#include "Editor/EditorMode_ModelEditor.hpp"
#include "Editor/EditorMode_Sandbox.hpp"

namespace GTGUI
{
    class Element;
    class Server;
}

namespace GTEngine
{
    class Game;
    
    /// Class representing the editor.
    ///
    /// An editor object with every Game object.
    class Editor
    {
    public:

        /// Constructor.
        Editor(Game &game);

        /// Destructor.
        ~Editor();


        /// Initialises the editor.
        bool Startup(GTGUI::Server &guiServer);

        /// Whether or not the editor is initialised.
        bool IsStarted() const { return this->isStarted; }


        /// Retrieves a reference to the game object that owns this editor.
              Game & GetGame()       { return this->game; }
        const Game & GetGame() const { return this->game; }


        /// Opens the editor.
        void Open();

        /// Closes the editor.
        void Close();

        /// Determines whether or not the editor is open.
        bool IsOpen() const { return this->isOpen; }


        /// Switches editor modes.
        void SwitchToModelEditorMode();

        /// Switches to the sandbox mode.
        void SwitchToSandboxMode();

        /// Switches to the previous mode.
        void SwitchToPreviousMode();



        //////////////////////////////////////////////////////////////////////////////
        // Events.

        /// Updates the editor so that it can do rendering and whatnot.
        void Update(double deltaTimeInSeconds);

        /// Swaps the RC queues.
        void SwapRCQueues();



    private:

        /// Helper function for setting the editor mode.
        ///
        /// @param mode [in] A pointer to the new editor mode. Can be nullptr.
        void SetEditorMode(EditorMode* mode);



    private:

        /// The game that owns this editor object.
        Game &game;

        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : EditorMain(nullptr)
            {
            }

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* EditorMain;

        }GUI;


        /// The model editor.
        EditorMode_ModelEditor modelEditor;

        /// The sandbox mode.
        EditorMode_Sandbox sandbox;

        /// The current editor mode.
        EditorMode* currentMode;

        /// The previously current mode. Good for toggling.
        EditorMode* previousMode;


        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;
    };
}

#endif