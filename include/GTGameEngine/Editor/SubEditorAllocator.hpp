// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SubEditorAllocator
#define GT_SubEditorAllocator

#include "Controls/EditorSubEditor.hpp"

namespace GT
{
    class Asset;

    /// Base class for sub-editor allocators.
    class SubEditorAllocator
    {
    public:

        /// Constructor.
        SubEditorAllocator();

        /// Destructor.
        virtual ~SubEditorAllocator();


        /// Creates a sub-editor based on the given absolute path and an optional asset object.
        virtual EditorSubEditor* CreateSubEditor(Editor &editor, const char* absolutePath) = 0;

        /// Deletes the given sub-editor.
        virtual void DeleteSubEditor(EditorSubEditor* pSubEditor) = 0;
    };
}

#endif
