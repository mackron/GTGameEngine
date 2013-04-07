// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

#include "Editor/ImageEditor/ImageEditor.hpp"
#include "Editor/ModelEditor/ModelEditor.hpp"
#include "Editor/MaterialEditor/MaterialEditor.hpp"
#include "Editor/SceneEditor/SceneEditor.hpp"
#include "Editor/ParticleEditor/ParticleEditor.hpp"
#include "Editor/TextEditor/TextEditor.hpp"
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


        /// Opens a file.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is absolute, defines the base path that will be used to retrieve the relative path.
        ///
        /// @return True if the file is opened successfully; false otherwise.
        ///
        /// @remarks
        ///     The editor must be aware of the files absolute AND relative paths. If 'path' is absolute, it needs to have it's relative path
        ///     extracted from that. In order to do that, we need the base part of the path that is used to make it relative. If is an error
        ///     for 'path' to be absolute while 'relativeTo' is null.
        bool OpenFile(const char* path, const char* relativeTo = nullptr);

        /// Closes a file.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        ///
        ///     This function does not save or prompt to save. It simply closes the file and discards any changes. Saving and save prompting
        ///     should be done at a higher level.
        void CloseFile(const char* path, const char* relativeTo = nullptr);

        /// Closes every open file.
        ///
        /// @remarks
        ///     Currently, this will save any modified files.
        void CloseAllOpenFiles();

        /// Closes the currently shown file.
        void CloseCurrentlyShownFile();

        /// Shows the given file.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @return True if the file is shown successfully; false otherwise.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        bool ShowFile(const char* path, const char* relativeTo = nullptr);

        /// Hides the file and it's editor that is current being shown.
        void HideCurrentlyShownFile();

        /// Saves the given file.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @return True if the file is shown successfully; false otherwise.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        bool SaveFile(const char* path, const char* relativeTo = nullptr);

        /// Saves every open file marked as modified.
        void SaveAllOpenModifiedFiles();

        /// Saves the currently shown file.
        bool SaveCurrentlyShownFile();

        /// Marks the given file as modified.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        void MarkFileAsModified(const char* path, const char* relativeTo = nullptr);

        /// Marks the given file as modified.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        void UnmarkFileAsModified(const char* path, const char* relativeTo = nullptr);

        /// Determines if the given file is marked as modified.
        ///
        /// @param path       [in] The path of the file. Can be relative or absolute; see remarks.
        /// @param relativeTo [in] If 'path' is relative, defines the base path to will be used to make it absolute.
        ///
        /// @return True if the file is marked as modified; false otherwise.
        ///
        /// @remarks
        ///     Internally, the editor uses absolute paths to associate editors with files. If 'path' is relative, it will need to be converted
        ///     to an absolute path in order for the editor to do correct identification. To do this, 'relativeTo' must be set when 'path' is
        ///     relative.
        bool IsFileMarkedAsModified(const char* path, const char* relativeTo = nullptr);

        /// Determines if any open file is marked as modified.
        bool IsAnyOpenFileMarkedAsModified();



        /// Updates the menu buttons by enabling and disabling the buttons based on the modification state.
        void UpdateMenuButtonEnableStates();



        ///////////////////////////////////////////////
        // Events.

        /// Updates the editor so that it can do rendering and whatnot.
        void Update(double deltaTimeInSeconds);

        /// Called when a mouse button is pusehd.
        ///
        /// @param button [in] The button that was pushed.
        /// @param x      [in] The x position of the mouse at the time it was pushed.
        /// @param y      [in] The y position of the mouse at the time it was pushed.
        void OnMouseButtonDown(GTCore::MouseButton button, int x, int y);

        /// Called when a mouse button is released.
        ///
        /// @param button [in] The button that was released.
        /// @param x      [in] The x position of the mouse at the time it was released.
        /// @param y      [in] The y position of the mouse at the time it was released.
        void OnMouseButtonUp(GTCore::MouseButton button, int x, int y);



        ///////////////////////////////////////////////
        // Events from Files Watcher.

        /// Called when a file is added.
        void OnFileInsert(const DataFilesWatcher::Item &item);

        /// Called when a file is removed.
        void OnFileRemove(const DataFilesWatcher::Item &item);

        /// Called when a file is updated.
        void OnFileUpdate(const DataFilesWatcher::Item &item);


    private:

        /// Shows the save file dialog for the given file.
        ///
        /// @param absolutePath [in] The absolute path of the file being closed and saved.
        ///
        /// @remarks
        ///     This is only shown when the file is being closed while being marked as modified. Thus, when the 'Yes' or 'No' buttons
        ///     are hit, the file will be closed. If the 'cancel' button is hit, it will not be saved and the file will remain open.
        void ShowSaveFileDialog(const char* absolutePath);

        /// Starts up the scripting environment for the editor.
        void StartupScripting();


    private:

        /// The game that owns this editor object.
        Game &game;


        /// The list of open files. This is indexed by the full, absolute file path. We need to index like this since we need to support
        /// multiple data directories, which may result in something having the same relative path.
        GTCore::Dictionary<SubEditor*> openedFiles;

        /// A pointer to the sub-editor that is currently being shown.
        SubEditor* currentlyShownEditor;



        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : EditorMain(nullptr), EditorCenterCenterPanel(nullptr),
                  File_Save(nullptr), File_SaveAll(nullptr), File_Close(nullptr), File_CloseAll(nullptr),
                  Editor_Delta(nullptr), Editor_FPS(nullptr)
            {
            }

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* EditorMain;

            /// The center, center panel element. This will be dynamically shown and hidden as tabs are added or removed.
            GTGUI::Element* EditorCenterCenterPanel;

            // Menu buttons.
            GTGUI::Element* File_Save;
            GTGUI::Element* File_SaveAll;
            GTGUI::Element* File_Close;
            GTGUI::Element* File_CloseAll;


            // GUI elements for the profiling information.
            GTGUI::Element* Editor_Delta;
            GTGUI::Element* Editor_FPS;

        }GUI;



        /// The last time the profiling elements were updated.
        double lastProfilingUpdateTime;


        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;

        /// Whether or not file watching should be disabled when the editor is closed.
        bool disableFileWatchingAfterClose;


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
            static Game   & GetGame(GTCore::Script &script);
            static Editor & GetEditor(GTCore::Script &script);

            static int Open(GTCore::Script &script);
            static int Close(GTCore::Script &script);

            static int OpenFile(GTCore::Script &script);
            static int CloseFile(GTCore::Script &script);
            static int CloseAllOpenFiles(GTCore::Script &script);
            static int CloseCurrentlyShownFile(GTCore::Script &script);
            static int ShowFile(GTCore::Script &script);
            static int HideCurrentlyShownFile(GTCore::Script &script);
            static int SaveFile(GTCore::Script &script);
            static int SaveAllOpenModifiedFiles(GTCore::Script &script);
            static int SaveCurrentlyShownFile(GTCore::Script &script);
            static int MarkFileAsModified(GTCore::Script &script);
            static int UnmarkFileAsModified(GTCore::Script &script);
            static int IsFileMarkedAsModified(GTCore::Script &script);
        };



    private:    // No copying.
        Editor(const Editor &);
        Editor & operator=(const Editor &);
    };
}

#endif
