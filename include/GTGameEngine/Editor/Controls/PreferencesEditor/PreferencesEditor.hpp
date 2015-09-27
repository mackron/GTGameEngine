// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_PreferencesEditor
#define GT_PreferencesEditor

#include "../EditorSubEditor.hpp"

namespace GT
{
    class PreferencesEditor : public EditorSubEditor
    {
    public:

        /// Constructor
        PreferencesEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier);

        /// Destructor.
        ~PreferencesEditor();



    private:


    };
}

#endif
