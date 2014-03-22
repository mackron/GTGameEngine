// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SubEditor_hpp_
#define __GTEngine_SubEditor_hpp_

#include <GTLib/String.hpp>
#include <GTGUI/Server.hpp>

#include "SubEditorTabEventHandler.hpp"
#include "../GameScript.hpp"
#include "../DataFilesWatcher.hpp"

namespace GTEngine
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
              Editor & GetOwnerEditor()       { return this->ownerEditor; }
        const Editor & GetOwnerEditor() const { return this->ownerEditor; }


        /// Retrieves the absolute path of this editor's file.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path of this editor's file.
        const char* GetRelativePath() const;


        /// Retrieves the tab GUI element associated with this editor.
              GTGUI::Element* GetTabElement()       { return this->tabElement; }
        const GTGUI::Element* GetTabElement() const { return this->tabElement; }


        /// Marks the file as modified.
        void MarkAsModified();

        /// Unmarks the file as modified.
        void UnmarkAsModified();

        /// Determines whether or not the file is marked as modified.
        bool IsMarkedAsModified() const;


        /// Closes the sub-editor.
        ///
        /// @remarks
        ///     This will tell the owner editor that this sub-editor should be removed.
        void Close();

        /// Deletes the sub-editor's toolbar. Should not be called directly.
        void DeleteToolbar();


        /// A helper function for retrieving the game script object.
              GTEngine::GameScript & GetScript();
        const GTEngine::GameScript & GetScript() const;

        /// A helper function for retreiving the game GUI object.
        GTGUI::Server & GetGUI();
        
        
        
        /// A helper method for retrieving a reference to the game.
              Game & GetGame();
        const Game & GetGame() const;



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// Retrieves the main GUI element of the editor.
        virtual       GTGUI::Element* GetMainElement()       { return nullptr; }
        virtual const GTGUI::Element* GetMainElement() const { return nullptr; }

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
        virtual void OnKeyPressed(GTLib::Key key);

        /// Called when a key is released.
        virtual void OnKeyReleased(GTLib::Key key);

        /// Called when a mouse button is pressed.
        virtual void OnMouseButtonDown(GTLib::MouseButton button, int x, int y);

        /// Called when a mouse button is released.
        virtual void OnMouseButtonUp(GTLib::MouseButton button, int x, int y);

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
        Editor &ownerEditor;

        /// The absolute path of the file this editor is editting.
        GTLib::String absolutePath;

        /// The relative path of the file this editor is editting.
        GTLib::String relativePath;

        /// A pointer to the tab element associated with this editor.
        GTGUI::Element* tabElement;

        /// The event handler to attack to the tab.
        SubEditorTabEventHandler tabEventHandler;

        /// Keeps track of whether or not the file is marked as modified.
        bool isMarkedAsModified;


    private:    // No copying.
        SubEditor(const SubEditor &);
        SubEditor & operator=(const SubEditor &);
    };
}

#endif
