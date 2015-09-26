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
        TextEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath);

        /// Destructor.
        ~TextEditor();


        /// Loads the file.
        bool Load();


        /// @copydoc EditorSubEditor::Save()
        bool Save();

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

        /// Called when the text changes.
        void OnTextChanged();

        /// Refreshes the scrollbars.
        void RefreshScrollbars();

        /// Counts the number of lines making up the text.
        unsigned int GetLineCount() const;

        /// Calculates the number of lines that fit on a single page.
        unsigned int GetLinesPerPage() const;

        /// Retrieves the line height in pixels.
        unsigned int GetLineHeight() const;

        /// Attempts to vertically scroll.
        void TryScrollY(int delta);


    private:

        /// The scrollable control that contains the text.
        EditorScrollableControl m_textBox;

        /// The index of the undo/redo point at the time the file was last loaded or saved. This is used to control when OnUnchanged() should
        /// be called in response to undo/redo operations.
        unsigned int m_undoRedoPointAtTimeOfLastSave;
    };
}

#endif
