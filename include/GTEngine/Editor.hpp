
#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

#include "Editor/Editor_ModelEditor.hpp"
#include "Editor/Editor_ImageEditor.hpp"
//#include "Editor/Editor_SoundEditor.hpp"
#include "Editor/Editor_TextEditor.hpp"
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

        /// Retrieves a reference to the image sub-editor.
              Editor_ImageEditor & GetImageEditor()       { return this->imageEditor; }
        const Editor_ImageEditor & GetImageEditor() const { return this->imageEditor; }

        /// Retrieves a reference to the text sub-editor.
              Editor_TextEditor & GetTextEditor()       { return this->textEditor; }
        const Editor_TextEditor & GetTextEditor() const { return this->textEditor; }



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

        /// Called when the tab for a text file is activated.
        ///
        /// @param fileName [in] The full, absolute path of the text file being shown.
        void OnTextFileActivated(const char* fileName);


        /// Called when the tab for a model is closed.
        ///
        /// @param fileName [in] The full, absolute path of the file being closed.
        void OnModelClosed(const char* fileName);

        /// Called when the tab for an image is closed.
        ///
        /// @param fileName [in] The full, absolute path of the file being closed.
        void OnImageClosed(const char* fileName);

        /// Called when the tab for a sound is closed.
        ///
        /// @param fileName [in] The full, absolute path of the file being closed.
        void OnSoundClosed(const char* fileName);

        /// Called when the tab for a text file is closed.
        ///
        /// @param fileName [in] The full, absolute path of the file being closed.
        void OnTextFileClosed(const char* fileName);


        /// Called when a file is activated.
        ///
        /// @param fileName [in] The full, absolute path of the file being activated.
        void OnFileActivated(const char* fileName);

        /// Called when a file is closed.
        ///
        /// @param fileName [in] The full, absolute path of the file being activated.
        void OnFileClosed(const char* fileName);



        ///////////////////////////////////////////////
        // Events from Files Watcher.

        /// Called when a file is added.
        void OnFileInsert(const DataFilesWatcher::Item &item);

        /// Called when a file is removed.
        void OnFileRemove(const DataFilesWatcher::Item &item);

        /// Called when a file is updated.
        void OnFileUpdate(const DataFilesWatcher::Item &item);

        

        /// Called when the definition of a model has changed.
        void OnModelDefinitionChanged(const char* absolutePath) { this->modelEditor.OnModelDefinitionChanged(absolutePath); }



    private:

        /// Starts up the scripting environment for the editor.
        void StartupScripting();


    private:

        /// The game that owns this editor object.
        Game &game;

        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : EditorMain(nullptr),
                  Editor_Delta(nullptr), Editor_FPS(nullptr)
            {
            }

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* EditorMain;

            // GUI elements for the profiling information.
            GTGUI::Element* Editor_Delta;
            GTGUI::Element* Editor_FPS;

        }GUI;


        /// The model editor.
        Editor_ModelEditor modelEditor;

        /// The image editor.
        Editor_ImageEditor imageEditor;

        /// The text editor.
        Editor_TextEditor textEditor;


        /// The last time the profiling elements were updated.
        double lastProfilingUpdateTime;


        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;

        /// Whether or not file watching should be disabled when the editor is closed.
        bool disableFileWatchingAfterClose;

        /// Whether or not auto-repeat should be disabled when the editor is closed.
        bool disableKeyboardAutoRepeatAfterClose;



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
            static int OnTextFileActivated(GTCore::Script &script);


            static int OnFileActivated(GTCore::Script &script);
            static int OnFileClosed(GTCore::Script &script);

            struct ModelEditorFFI
            {
                static int SaveFile(GTCore::Script &script);

                static int SetMaterial(GTCore::Script &script);

                static int ShowConvexDecomposition(GTCore::Script &script);
                static int HideConvexDecomposition(GTCore::Script &script);
                static int BuildConvexDecomposition(GTCore::Script &script);

                static int PlayAnimation(GTCore::Script &script);
                static int StopAnimation(GTCore::Script &script);
            };

            struct TextEditorFFI
            {
                static int SaveFile(GTCore::Script &script);
            };
        };
    };
}

#endif