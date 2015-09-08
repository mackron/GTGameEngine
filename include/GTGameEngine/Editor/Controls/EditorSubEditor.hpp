// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorSubEditor
#define GT_EditorSubEditor

#include "EditorControl.hpp"

namespace GT
{
    class EditorSubEditor : public EditorControl
    {
    public:

        /// Constructor.
        EditorSubEditor(Editor &editor);

        /// Destructor.
        virtual ~EditorSubEditor();
    
    };
}

#endif
