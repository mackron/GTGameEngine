
#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

#include "Editor/ModelEditor.hpp"

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


        /// Opens the editor.
        void Open();

        /// Closes the editor.
        void Close();

        /// Determines whether or not the editor is open.
        bool IsOpen() const { return this->isOpen; }


        /// Updates the editor so that it can do rendering and whatnot.
        void Update(double deltaTimeInSeconds);

        /// Swaps the RC queues.
        void SwapRCQueues();


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
        ModelEditor modelEditor;

        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;

    };
}

#endif