// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/TextEditor/TextEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

#include <GTLib/Windowing/Clipboard.hpp>        // For cut/copy/paste

namespace GT
{
    TextEditor::TextEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath)
        : EditorSubEditor(editor, allocator, absolutePath),
          m_textBox(editor),
          m_undoRedoPointAtTimeOfLastSave(0)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        gui.SetElementSizeRatio(this->GetRootGUIElement(), 1, 1);
        gui.SetElementBorder(this->GetRootGUIElement(), 1, theme.borderDim);


        // The text box.
        gui.SetElementParent(m_textBox.GetRootGUIElement(), this->GetRootGUIElement());
        gui.SetElementSizeRatio(m_textBox.GetRootGUIElement(), 1, 1);
        gui.SetElementPadding(m_textBox.GetContentElement(), 2);
        gui.SetElementCursor(m_textBox.GetContentElement(), GUISystemCursor::IBeam);
        gui.EnableEditableText(m_textBox.GetContentElement());
        gui.EnableMultiLineText(m_textBox.GetContentElement());
        gui.EnableFocusOnMouseClick(m_textBox.GetContentElement());
        gui.SetElementFont(m_textBox.GetContentElement(), "Courier New", FontWeight_Medium, FontSlant_None, 15);
        gui.SetElementTextColor(m_textBox.GetContentElement(), theme.defaultTextColor);
        gui.GiveElementKeyboardFocus(m_textBox.GetContentElement());
        gui.OnElementTextChanged(m_textBox.GetContentElement(), [&]() {
            this->OnChanged();
        });

        gui.SetElementVerticalAlign(m_textBox.GetContentElement(), VerticalAlign_Top);
        gui.SetElementHorizontalAlign(m_textBox.GetContentElement(), HorizontalAlign_Left);


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
}
