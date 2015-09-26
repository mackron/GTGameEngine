// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/TextEditor/TextEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

#include <GTLib/Windowing/Clipboard.hpp>        // For cut/copy/paste
#include <GTLib/Strings/LineIterator.hpp>       // For GetLineCount()

namespace GT
{
    TextEditor::TextEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath)
        : EditorSubEditor(editor, allocator, absolutePath),
          m_textBox(editor),
          m_undoRedoPointAtTimeOfLastSave(0)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        gui.SetElementSizeRatio(this->GetRootElement(), 1, 1);
        gui.SetElementBorder(this->GetRootElement(), 1, theme.borderDefault);


        // The text box.
        gui.SetElementParent(m_textBox.GetRootElement(), this->GetRootElement());
        gui.SetElementSizeRatio(m_textBox.GetRootElement(), 1, 1);

        gui.SetElementPadding(m_textBox.GetContentElement(), 2);
        gui.SetElementCursor(m_textBox.GetContentElement(), GUISystemCursor::IBeam);
        gui.EnableEditableText(m_textBox.GetContentElement());
        gui.EnableMultiLineText(m_textBox.GetContentElement());
        gui.EnableFocusOnMouseClick(m_textBox.GetContentElement());
        gui.SetElementFont(m_textBox.GetContentElement(), "Courier New", FontWeight_Medium, FontSlant_None, 15);
        gui.SetElementTextColor(m_textBox.GetContentElement(), theme.defaultTextColor);
        gui.GiveElementKeyboardFocus(m_textBox.GetContentElement());
        gui.OnElementTextChanged(m_textBox.GetContentElement(), [&]() {
            this->OnTextChanged();
        });
        gui.OnElementSize(m_textBox.GetContentElement(), [&](unsigned int, unsigned int) {
            this->RefreshScrollbars();
        });
        gui.OnElementMouseWheel(m_textBox.GetContentElement(), [&](int delta, int, int) {
            this->TryScrollY(delta);
        });


        // Scrollbars
        m_textBox.GetVerticalScrollbar().OnScroll([&](int scrollPos) {
            gui.SetElementInnerOffsetY(m_textBox.GetContentElement(), float(-scrollPos * static_cast<int>(this->GetLineHeight())));
        });


        this->Load();
    }

    TextEditor::~TextEditor()
    {
    }


    bool TextEditor::Load()
    {
        FileSystem &fileSystem = this->GetEditor().GetEngineContext().GetFileSystem();
        HFile hFile = fileSystem.OpenFile(this->GetAbsolutePathOrIdentifier(), FileAccessMode::Read);
        if (hFile != NULL)
        {
            bool result = false;

            size_t fileSize = size_t(fileSystem.GetFileSize(hFile));
            if (fileSize > 0)
            {
                char* fileData = reinterpret_cast<char*>(malloc(fileSize + 1));
                if (fileData != nullptr)
                {
                    fileSystem.ReadFile(hFile, fileSize, fileData);
                    fileData[fileSize] = '\0';

                    this->GetGUI().SetElementText(m_textBox.GetContentElement(), fileData);

                    result = true;
                    free(fileData);
                }
            }
            


            fileSystem.CloseFile(hFile);
            return result;
        }

        return false;
    }

    bool TextEditor::Save()
    {
        const char* fileData = this->GetGUI().GetElementText(m_textBox.GetContentElement());
        if (fileData != nullptr)
        {
            FileSystem &fileSystem = this->GetEditor().GetEngineContext().GetFileSystem();
            HFile hFile = fileSystem.OpenFile(this->GetAbsolutePathOrIdentifier(), FileAccessMode::Write);
            if (hFile != NULL)
            {
                fileSystem.WriteFile(hFile, strlen(fileData), fileData);
                fileSystem.CloseFile(hFile);


                // We need to update the undo/redo point at the time of saving so that we can correctly call OnUnchanged() in response to undo/redo operations.
                m_undoRedoPointAtTimeOfLastSave = this->GetGUI().GetUndoPointsRemainingCount(m_textBox.GetContentElement());

                // We just saved which means we are now unchanged - post the event.
                this->OnUnchanged();

                return true;
            }
        }

        return false;
    }

    void TextEditor::OnActivate()
    {
        this->GetGUI().GiveElementKeyboardFocus(m_textBox.GetContentElement());
    }

    void TextEditor::OnDeactivate()
    {
        if (this->GetGUI().GetTextCursorOwnerElement() == m_textBox.GetContentElement()) {
            this->GetGUI().ReleaseKeyboardFocus();
        }
    }


    void TextEditor::Cut()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            GTLib::String selectedText = this->GetGUI().GetSelectedText(m_textBox.GetContentElement());
            GTLib::Clipboard::SetText(selectedText.c_str(), selectedText.GetLengthInTs());

            this->GetGUI().PrepareUndoRedoPoint(m_textBox.GetContentElement());
            {
                this->GetGUI().DeleteSelectedText(m_textBox.GetContentElement());
            }
            this->GetGUI().CreateUndoRedoPoint(m_textBox.GetContentElement());
        }
    }

    void TextEditor::Copy()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            GTLib::String selectedText = this->GetGUI().GetSelectedText(m_textBox.GetContentElement());
            GTLib::Clipboard::SetText(selectedText.c_str(), selectedText.GetLengthInTs());
        }
    }

    void TextEditor::Paste()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            GTLib::String clipboard = GTLib::Clipboard::GetText();
            if (clipboard.GetLengthInTs() > 0) {
                this->GetGUI().PrepareUndoRedoPoint(m_textBox.GetContentElement());
                {
                    this->GetGUI().DeleteSelectedText(m_textBox.GetContentElement());
                    this->GetGUI().InsertTextAtCursor(m_textBox.GetContentElement(), clipboard.c_str());
                }
                this->GetGUI().CreateUndoRedoPoint(m_textBox.GetContentElement());
            }
        }
    }

    void TextEditor::Undo()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            if (this->GetGUI().UndoTextEdit(m_textBox.GetContentElement())) {
                if (this->GetGUI().GetUndoPointsRemainingCount(m_textBox.GetContentElement()) == m_undoRedoPointAtTimeOfLastSave) {
                    this->OnUnchanged();
                }
            }
        }
    }

    void TextEditor::Redo()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            if (this->GetGUI().RedoTextEdit(m_textBox.GetContentElement())) {
                if (this->GetGUI().GetUndoPointsRemainingCount(m_textBox.GetContentElement()) == m_undoRedoPointAtTimeOfLastSave) {
                    this->OnUnchanged();
                }
            }
        }
    }

    void TextEditor::SelectAll()
    {
        if (this->GetGUI().GetElementWithKeyboardFocus() == m_textBox.GetContentElement())
        {
            this->GetGUI().SelectAllText(m_textBox.GetContentElement());
        }
    }


    /////////////////////////////////////////
    // Private

    void TextEditor::OnTextChanged()
    {
        this->RefreshScrollbars();
        this->OnChanged();              // <-- Let's the main editor know that this sub-editor should be marked as modified.
    }

    void TextEditor::RefreshScrollbars()
    {
        int textLines = this->GetLineCount();
        int pageLines = this->GetLinesPerPage();
        int scrollingLines = textLines + pageLines - 1 - 1;     // -1 because the range is zero based and -1 because want to see the last line in the text box when scrolled to the bottom.
        if (scrollingLines < 0) {
            scrollingLines = 0;
        }

        m_textBox.GetVerticalScrollbar().SetRange(0, scrollingLines);
        m_textBox.GetVerticalScrollbar().SetPageSize(pageLines);
    }

    unsigned int TextEditor::GetLineCount() const
    {
        const char* pText = this->GetGUI().GetElementText(m_textBox.GetContentElement());
        if (pText != nullptr)
        {
            unsigned int lineCount = 0;
            GTLib::Strings::LineIterator iLine(pText);
            while (iLine)
            {
                ++iLine;
                ++lineCount;
            }

            return lineCount;
        }

        return 0;
    }

    unsigned int TextEditor::GetLinesPerPage() const
    {
        return static_cast<unsigned int>(this->GetGUI().GetElementHeight(m_textBox.GetContentElement()) / this->GetLineHeight());
    }

    unsigned int TextEditor::GetLineHeight() const
    {
        return this->GetGUI().GetElementLineHeight(m_textBox.GetContentElement());
    }

    void TextEditor::TryScrollY(int delta)
    {
        if (this->GetGUI().IsElementUnderMouse(m_textBox.GetRootElement()))
        {
            m_textBox.GetVerticalScrollbar().Scroll(-delta);
        }
    }
}
