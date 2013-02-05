// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/TextEditor/TextEditor.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    TextEditor::TextEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          textBox(nullptr), textArea(nullptr), textAreaEventHandler(new TextAreaEventHandler(this))
    {
        GTCore::String fileContent;
        if (GTCore::IO::OpenAndReadTextFile(absolutePath, fileContent))
        {
            auto &gui    = this->GetGUI();
            auto &script = this->GetScript();


            // The file contents have been loaded, so now we need to create the text-box element that will display it.
            this->textBox = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='text-editor-text-area' />");
            assert(this->textBox != nullptr);
            {
                // The element has been created, but we need to execute a script to have it turn into a proper multi-line text box.
                script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):MultiLineTextBox();", this->textBox->id).c_str());


                // Now what we need to do is actually set the text. This will be much quicker if done on the C++ side so that the script parser doesn't need to
                // parse potentially very large files.
                this->textArea = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextArea:GetID();", this->textBox->id).c_str()));
                assert(this->textArea != nullptr);
                {
                    this->textArea->SetText(fileContent.c_str());
                    this->textArea->SetStyleAttribute("background-color", "inherit");
                    this->textArea->SetStyleAttribute("text-color",       "inherit");
                     
                    this->textArea->textManager.MoveCursorToStart();
                    gui.FocusElement(this->textArea);


                    // Now we need to attach an event handler to the text area so we can detect changes to the text. We propagate this to the scripting
                    // environment so that the owner tab can be marked as modified.
                    this->textArea->AttachEventHandler(*this->textAreaEventHandler);
                }
            }
        }
    }

    TextEditor::~TextEditor()
    {
        this->GetGUI().DeleteElement(this->textBox);
        delete this->textAreaEventHandler;
    }




    ///////////////////////////////////////////////////
    // Virtual Methods.

    void TextEditor::Show()
    {
        this->textBox->Show();
        this->GetGUI().FocusElement(this->textArea);
    }

    void TextEditor::Hide()
    {
        this->textBox->Hide();
    }

    bool TextEditor::Save()
    {
        bool wasSaved = false;

        auto text = this->textArea->GetText();
        if (text != nullptr)
        {
            wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), text);
        }
        else
        {
            wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), "");
        }

        if (wasSaved)
        {
            this->UnmarkAsModified();
        }

        return wasSaved;
    }







    ///////////////////////////////////////////////////
    // TextAreaEventHandler.

    TextEditor::TextAreaEventHandler::TextAreaEventHandler(TextEditor* ownerTextEditorIn)
        : ownerTextEditor(ownerTextEditorIn)
    {
    }

    void TextEditor::TextAreaEventHandler::OnTextChanged(GTGUI::Element &)
    {
        assert(this->ownerTextEditor != nullptr);
        {
            if (!this->ownerTextEditor->IsMarkedAsModified())
            {
                this->ownerTextEditor->MarkAsModified();
            }
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
