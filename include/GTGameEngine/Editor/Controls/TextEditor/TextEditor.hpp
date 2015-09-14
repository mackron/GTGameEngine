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

        /// @copydoc EditorSubEditor::OnActivate()
        void OnActivate();

        /// @copydoc EditorSubEditor::OnDeactivate()
        void OnDeactivate();

        /// @copydoc EditorSubEditor::Cut()
        void Cut();

        /// @copydoc EditorSubEditor::Copy()
        void Copy();

        /// @copydoc EditorSubEditor::Paste()
        void Paste();

        /// @copydoc EditorSubEditor::Undo()
        void Undo();

        /// @copydoc EditorSubEditor::Redo()
        void Redo();

        /// @copydoc EditorSubEditor::SelectAll()
        void SelectAll();


    private:

        /// The scrollable control that contains the text.
        EditorScrollableControl m_textBox;
    };
}

#endif
