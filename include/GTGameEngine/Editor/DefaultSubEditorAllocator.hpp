// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultSubEditorAllocator
#define GT_DefaultSubEditorAllocator

#include "SubEditorAllocator.hpp"

namespace GT
{
    /// Sub-editor allocator for known asset classes and text files.
    class DefaultSubEditorAllocator : public SubEditorAllocator
    {
    public:

        /// Constructor.
        DefaultSubEditorAllocator();

        /// Destructor.
        ~DefaultSubEditorAllocator();


        /// @copydoc SubEditorAllocator::CreateSubEditor()
        EditorSubEditor* CreateSubEditor(Editor &editor, const char* absolutePath, Asset* pAsset);

        /// @copydoc SubEditorAllocator::DeleteSubEditor()
        void DeleteSubEditor(EditorSubEditor* pSubEditor);


        /// Creates a text file sub-editor.
        EditorSubEditor* CreateTextFileSubEditor(Editor &editor, const char* absolutePath);
    };
}

#endif
