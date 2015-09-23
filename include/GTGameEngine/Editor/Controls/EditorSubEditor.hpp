// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorSubEditor
#define GT_EditorSubEditor

#include "EditorControl.hpp"
#include "../../FileSystem.hpp" // For GT_MAX_PATH
#include <functional>

namespace GT
{
    class EditorTab;
    class SubEditorAllocator;

    class EditorSubEditor : public EditorControl
    {
    public:

        typedef std::function<void ()> OnChangedProc;
        typedef std::function<void ()> OnUnchangedProc;


        /// Constructor.
        EditorSubEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePathOrIdentifier);

        /// Destructor.
        virtual ~EditorSubEditor();


        /// Retrieves a pointer to the allocator that created the sub-editor. This is used to delete the sub-editor.
        SubEditorAllocator & GetAllocator() const;

        /// Retrieves the absolute path or identifier of the resource the sub-editor is associated with.
        const char* GetAbsolutePathOrIdentifier() const;


        /// Sets the tab to associate with the sub editor.
        void SetTab(EditorTab* pTab);

        /// Retrieves a pointer to the tab associated with the sub-editor.
        EditorTab* GetTab() const;


        /// Sets the function to call when a change is made to the resource.
        void OnChanged(OnChangedProc onChangedProc);

        /// Calls the OnChanged function.
        void OnChanged();


        /// Sets the function to call when a resource goes from a changed state to an unchanged state.
        ///
        /// @remarks
        ///     This callback will be called in response to the resource being saved, or if a change is undone with an undo command or whatnot.
        void OnUnchanged(OnUnchangedProc onUnchangedProc);

        /// Calls the OnUnchanged function().
        void OnUnchanged();


        /// Saves the contents to the given file.
        virtual bool Save() = 0;

        /// Called when the sub editor is activated.
        ///
        /// @remarks
        ///     This allows the sub-editor to do things like give an element keyboard focus or whatnot.
        virtual void OnActivate() = 0;

        /// Called when the sub-editor is deactivated.
        virtual void OnDeactivate() = 0;


        /// Performs a cut operation
        virtual void Cut() = 0;

        /// Performs a copy operation
        virtual void Copy() = 0;

        /// Performs a paste operation
        virtual void Paste() = 0;

        /// Performs a undo operation
        virtual void Undo() = 0;

        /// Performs a redo operation
        virtual void Redo() = 0;

        /// Performs a select-all operation.
        virtual void SelectAll() = 0;


    private:

        /// A pointer to the allocator that created the sub-editor.
        SubEditorAllocator &m_allocator;

        /// The absolute path of the resource being editted, or a unique identifier in cases when it is not an asset being editted.
        char m_absolutePathOrIdentifier[GT_MAX_PATH];

        /// A pointer to the tab associated with the sub editor.
        EditorTab* m_pTab;


        /// The function to call when the resource is modified. This is set by OnChanged()
        OnChangedProc m_onChangedProc;

        /// The function to call when the resource goes from a modified to an unmodified state. An example would be when a change is
        /// made to a text file, and then that changed is undone with an undo command or whatnot. This is set via OnUnchanged().
        OnUnchangedProc m_onUnchangedProc;
    };
}

#endif
