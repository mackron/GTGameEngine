
#ifndef __GTEngine_SubEditor_hpp_
#define __GTEngine_SubEditor_hpp_

#include <GTCore/String.hpp>
#include <GTGUI/Server.hpp>

#include "SubEditorTabEventHandler.hpp"
#include "../GameScript.hpp"

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



        /// A helper function for retrieving the game script object.
        GTEngine::GameScript & GetScript();

        /// A helper function for retreiving the game GUI object.
        GTGUI::Server & GetGUI();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// Called when the editor needs to be shown.
        virtual void Show();

        /// Called when the editor needs to be hidden.
        virtual void Hide();

        /// Called when the file needs to be saved.
        virtual void Save();



        ///////////////////////////////////////////////////
        // Events.

        /// Called when the sub-editor wants to be closed.
        ///
        /// @remarks
        ///     This will tell the owner editor that this sub-editor should be removed.
        void Close();



    private:

        /// A reference to the Editor object that owns this sub-editor.
        Editor &ownerEditor;

        /// The absolute path of the file this editor is editting.
        GTCore::String absolutePath;

        /// The relative path of the file this editor is editting.
        GTCore::String relativePath;

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