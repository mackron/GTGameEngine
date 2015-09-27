// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_WorkbenchEditor
#define GT_WorkbenchEditor

#include "../EditorSubEditor.hpp"

namespace GT
{
    class WorkbenchEditor : public EditorSubEditor
    {
    public:

        /// Constructor
        WorkbenchEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier);

        /// Destructor.
        ~WorkbenchEditor();



    private:


    };
}

#endif
