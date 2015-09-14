// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorSubEditor
#define GT_EditorSubEditor

#include "EditorControl.hpp"
#include <functional>

namespace GT
{
    class EditorSubEditor : public EditorControl
    {
    public:

        typedef std::function<void ()> OnChangedProc;


        /// Constructor.
        EditorSubEditor(Editor &editor);

        /// Destructor.
        virtual ~EditorSubEditor();


        /// Sets the function to call when a change is made to the resource.
        void OnChanged(OnChangedProc onChangedProc);

        /// Calls the OnChanged function.
        void OnChanged();


        /// Saves the contents to the given file.
        virtual bool SaveFile(const char* absolutePath) = 0;

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

        /// The function to call when the resource is modified. This is set by OnChanged()
        OnChangedProc m_onChangedProc;
    
    };
}

#endif
