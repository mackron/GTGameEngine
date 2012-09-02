
#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

#include "Editor/EditorMode_ModelEditor.hpp"
#include "Editor/EditorMode_Sandbox.hpp"
#include "Editor/Editor_ModelEditor.hpp"
#include "Editor/Editor_ImageEditor.hpp"
//#include "Editor/Editor_SoundEditor.hpp"
#include "DataFilesWatcher.hpp"

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


        /// Retrieves a reference to the model sub-editor.
              Editor_ModelEditor & GetModelEditor()       { return this->modelEditor; }
        const Editor_ModelEditor & GetModelEditor() const { return this->modelEditor; }

        /// Retrieves a reference to the sandbox sub-editor.s
        //      EditorMode_Sandbox & GetSandbox()       { return this->sandbox; }
        //const EditorMode_Sandbox & GetSandbox() const { return this->sandbox; }


        /// Switches editor modes.
        //void SwitchToModelEditorMode();

        /// Switches to the sandbox mode.
        //void SwitchToSandboxMode();

        /// Switches to the previous mode.
        //void SwitchToPreviousMode();



        ///////////////////////////////////////////////
        // Events.

        /// Updates the editor so that it can do rendering and whatnot.
        void Update(double deltaTimeInSeconds);


        /// Called when the tab for a model is activated.
        ///
        /// @param fileName [in] The full, absolute path of the model being shown.
        void OnModelActivated(const char* fileName);

        /// Called when the tab for an image is activated.
        ///
        /// @param fileName [in] The full, absolute path of the image being shown.
        void OnImageActivated(const char* fileName);

        /// Called when the tab for a sound is activated.
        ///
        /// @param fileName [in] The full, absolute path of the sound being shown.
        void OnSoundActivated(const char* fileName);



        ///////////////////////////////////////////////
        // Events from Files Watcher.

        /// Called when a files is added.
        void OnFileInsert(const DataFilesWatcher::Item &item);

        /// Called when a file is removed.
        void OnFileRemove(const DataFilesWatcher::Item &item);

        /// Called when a file is updated.
        void OnFileUpdate(const DataFilesWatcher::Item &item);

        



    private:

        /// Starts up the scripting environment for the editor.
        void StartupScripting();


        /// Helper function for setting the editor mode.
        ///
        /// @param mode [in] A pointer to the new editor mode. Can be nullptr.
        //void SetEditorMode(EditorMode* mode);



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
        //EditorMode_ModelEditor modelEditorMode;

        /// The sandbox mode.
        //EditorMode_Sandbox sandbox;

        /// The current editor mode.
        //EditorMode* currentMode;

        /// The previously current mode. Good for toggling.
        //EditorMode* previousMode;


        /// The model editor.
        Editor_ModelEditor modelEditor;

        /// The image editor.
        Editor_ImageEditor imageEditor;


        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;



        /// The event handler for the data files watcher. This will just dispatch the events to the main Editor object.
        class DataFilesWatcherEventHandler : public DataFilesWatcher::EventHandler
        {
        public:

            /// Constructor.
            DataFilesWatcherEventHandler(Editor &editor)
                : editor(editor)
            {
            }


            void OnInsert(const DataFilesWatcher::Item &item) { this->editor.OnFileInsert(item); }
            void OnRemove(const DataFilesWatcher::Item &item) { this->editor.OnFileRemove(item); }
            void OnUpdate(const DataFilesWatcher::Item &item) { this->editor.OnFileUpdate(item); }

        private:

            /// The editor object that owns this event handler.
            Editor &editor;

        private:    // No copying.
            DataFilesWatcherEventHandler(const DataFilesWatcherEventHandler &);
            DataFilesWatcherEventHandler & operator=(const DataFilesWatcherEventHandler &);

        }dataFilesWatcherEventHandler;




        /// The scripting FFI.
        struct FFI
        {
            static Editor & GetEditor(GTCore::Script &script);

            static int Open(GTCore::Script &script);
            static int Close(GTCore::Script &script);

            static int OnModelActivated(GTCore::Script &script);
            static int OnImageActivated(GTCore::Script &script);
            static int OnSoundActivated(GTCore::Script &script);

            struct ModelEditorFFI
            {
                static int SetMaterial(GTCore::Script &script);

                static int ShowConvexDecomposition(GTCore::Script &script);
                static int HideConvexDecomposition(GTCore::Script &script);
                static int BuildConvexDecomposition(GTCore::Script &script);
            };
        };
    };
}

#endif