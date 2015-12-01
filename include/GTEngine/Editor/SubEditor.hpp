// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SubEditor
#define GT_SubEditor

#include <GTEngine/Core/String.hpp>
#include <GTEngine/GUI/Server.hpp>

#include "SubEditorTabEventHandler.hpp"
#include "../GameScript.hpp"
#include "../DataFilesWatcher.hpp"

namespace GT
{
    class Editor;

    /// Base class for sub-editors.
    ///
    /// A sub-editor is the scene editor, model editor, text editor, etc. There is one sub-editor associated with every open file in the editor.
    ///
    /// If an open file does not have an associated sub-editor, then it will use an instantiation of this class which will just be an empty editor.
    class SubEditor
    {
    public:

        /// Constructor.
        SubEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        virtual ~SubEditor();

        
        /// Retrieves a reference to the owner editor.
              Editor & GetOwnerEditor()       { return m_ownerEditor; }
        const Editor & GetOwnerEditor() const { return m_ownerEditor; }


        /// Retrieves the absolute path of this editor's file.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path of this editor's file.
        const char* GetRelativePath() const;


        /// Retrieves the tab GUI element associated with this editor.
              GTGUI::GUIElement* GetTabElement()       { return m_tabElement; }
        const GTGUI::GUIElement* GetTabElement() const { return m_tabElement; }


        /// Marks the file as modified.
        void MarkAsModified();

        /// Unmarks the file as modified.
        void UnmarkAsModified();

        /// Determines whether or not the file is marked as modified.
        bool IsMarkedAsModified() const;


        /// Enables marking as modified.
        void EnableMarkingAsModified();

        /// Disables marking as modified.
        void DisableMarkingAsModified();

        /// Determines whether or not marking as modified is enabled.
        bool IsMarkingAsModifiedEnabled() const;


        /// Closes the sub-editor.
        ///
        /// @remarks
        ///     This will tell the owner editor that this sub-editor should be removed.
        void Close();

        /// Deletes the sub-editor's toolbar. Should not be called directly.
        void DeleteToolbar();


        /// A helper function for retrieving the game script object.
              GameScript & GetScript();
        const GameScript & GetScript() const;

        /// A helper function for retreiving the game GUI object.
        GTGUI::GUIServer & GetGUI();
        
        
        
        /// A helper method for retrieving a reference to the game.
              Game & GetGame();
        const Game & GetGame() const;



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// Retrieves the main GUI element of the editor.
        virtual       GTGUI::GUIElement* GetMainElement()       { return nullptr; }
        virtual const GTGUI::GUIElement* GetMainElement() const { return nullptr; }

        /// Called when the editor needs to be shown.
        virtual void Show();

        /// Called when the editor needs to be hidden.
        virtual void Hide();

        /// Called when the file needs to be saved.
        virtual bool Save();

        /// Updates the editor.
        ///
        /// @param deltaTimeInSeconds [in] The time since the last frame, in seconds.
        virtual void OnUpdate(double deltaTimeInSeconds);

        /// Called when a key is pressed.
        virtual void OnKeyPressed(Key key);

        /// Called when a key is released.
        virtual void OnKeyReleased(Key key);

        /// Called when a mouse button is pressed.
        virtual void OnMouseButtonDown(MouseButton button, int x, int y);

        /// Called when a mouse button is released.
        virtual void OnMouseButtonUp(MouseButton button, int x, int y);

        /// Called when the main window received focus.
        virtual void OnMainWindowReceiveFocus();

        /// Called when the main window loses focus.
        virtual void OnMainWindowLoseFocus();


        /// Called when a file is added to a data directory.
        ///
        /// @param item [in] The object containing information about the file.
        virtual void OnFileInsert(const DataFilesWatcher::Item &item);

        /// Called when a file is remove from a data directory.
        ///
        /// @param item [in] The object containing information about the file.
        virtual void OnFileRemove(const DataFilesWatcher::Item &item);

        /// Called when a file in a data directory is updated.
        ///
        /// @param item [in] The object containing information about the file.
        virtual void OnFileUpdate(const DataFilesWatcher::Item &item);


    private:

        /// Helper method for setting the text of the tab.
        ///
        /// @param text [in] The new text.
        void SetTabText(const char* text);


    private:

        /// A reference to the Editor object that owns this sub-editor.
        Editor &m_ownerEditor;

        /// The absolute path of the file this editor is editting.
        String m_absolutePath;

        /// The relative path of the file this editor is editting.
        String m_relativePath;

        /// A pointer to the tab element associated with this editor.
        GTGUI::GUIElement* m_tabElement;

        /// The event handler to attack to the tab.
        SubEditorTabEventHandler m_tabEventHandler;

        /// Keeps track of whether or not the file is marked as modified.
        bool m_isMarkedAsModified;

        /// Keeps track of whether or not marking as modified is enabled. When this is set to false, MarkAsModified() will do nothing.
        bool m_isMarkingAsModifiedEnabled;


    private:    // No copying.
        SubEditor(const SubEditor &);
        SubEditor & operator=(const SubEditor &);
    };
}

#endif
