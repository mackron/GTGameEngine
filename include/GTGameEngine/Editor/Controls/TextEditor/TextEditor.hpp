// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_TextEditor
#define GT_TextEditor

#include "../EditorSubEditor.hpp"
#include "../EditorScrollableControl.hpp"

namespace GT
{
    class TextEditor : public EditorSubEditor
    {
    public:

        /// Constructor.
        TextEditor(Editor &editor, const char* absolutePath);

        /// Destructor.
        ~TextEditor();


        /// Loads the given file.
        bool LoadFile(const char* absolutePath);

        /// @copydoc EditorSubEditor::SaveFile()
        bool SaveFile(const char* absolutePath);


    private:

        /// The scrollable control that contains the text.
        EditorScrollableControl m_textBox;
    };
}

#endif
