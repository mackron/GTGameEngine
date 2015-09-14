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


        /// Saves the contents to the given file.
        virtual bool SaveFile(const char* absolutePath) = 0;
    
    };
}

#endif
