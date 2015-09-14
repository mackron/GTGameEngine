// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/TextEditor/TextEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    TextEditor::TextEditor(Editor &editor, const char* absolutePath)
        : EditorSubEditor(editor),
          m_textBox(editor)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        gui.SetElementSizeRatio(this->GetRootGUIElement(), 1, 1);


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



        this->LoadFile(absolutePath);
    }

    TextEditor::~TextEditor()
    {
    }


    bool TextEditor::LoadFile(const char* absolutePath)
    {
        FileSystem &fileSystem = this->GetEditor().GetEngineContext().GetFileSystem();
        HFile hFile = fileSystem.OpenFile(absolutePath, FileAccessMode::Read);
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
}