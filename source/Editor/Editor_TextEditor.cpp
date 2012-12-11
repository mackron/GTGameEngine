
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor_TextEditor::Editor_TextEditor(Editor &editor)
        : editor(editor), GUI(),
          currentState(nullptr), loadedStates()
    {
    }

    Editor_TextEditor::~Editor_TextEditor()
    {
    }

    void Editor_TextEditor::Startup()
    {
    }

    bool Editor_TextEditor::LoadFile(const char* fileName)
    {
        // What we do here is first determine whether or not the file has already been loaded. If it has, we just show the existing text box. Otherwise, we need
        // to create a new one.
        auto iState = this->loadedStates.Find(fileName);
        if (iState != nullptr)
        {
            if (this->currentState != nullptr)
            {
                this->currentState->textBox->Hide();
            }

            this->currentState = iState->value;
            this->currentState->textBox->Show();
        }
        else
        {
            // The file is not currently open, so we will need to open it now.
            GTCore::String fileContent;
            if (GTCore::IO::OpenAndReadTextFile(fileName, fileContent))
            {
                auto &gui    = this->editor.GetGame().GetGUI();
                auto &script = this->editor.GetGame().GetScript();


                // The file contents have been loaded, so now we need to create the text-box element that will display it.
                auto textBoxElement = gui.CreateElement("<div parentid='Editor_TextEditor' styleclass='multiline-textbox' style='width:100%; height:100%; border:1px #222; vertical-align:top; font-family:liberation mono; font-size:10pt;' />");
                if (textBoxElement != nullptr)
                {
                    // The element has been created, but we need to execute a script to have it turn into a proper multi-line text box.
                    script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):MultiLineTextBox();", textBoxElement->id).c_str());


                    // Now what we need to do is actually set the text. This will be much quicker if done on the C++ side so that the script parser doesn't need to
                    // parse potentially very large files.
                    auto textArea = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextArea:GetID();", textBoxElement->id).c_str()));
                    if (textArea != nullptr)
                    {
                        textArea->SetText(fileContent.c_str());

                        // Now we just need to create a new state object and make it the current one.
                        if (this->currentState != nullptr)
                        {
                            this->currentState->textBox->Hide();
                        }

                        this->currentState = new State;
                        this->currentState->textBox = textBoxElement;
                        this->loadedStates.Add(fileName, this->currentState);
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    void Editor_TextEditor::CloseFile(const char* fileName)
    {
        auto iState = this->loadedStates.Find(fileName);
        if (iState != nullptr)
        {
            this->GetGame().GetGUI().DeleteElement(iState->value->textBox);
            

            if (this->currentState == iState->value)
            {
                this->currentState = nullptr;
            }


            delete iState->value;
            this->loadedStates.Remove(fileName);
        }
    }


    Game & Editor_TextEditor::GetGame()
    {
        return this->editor.GetGame();
    }

    const Game & Editor_TextEditor::GetGame() const
    {
        return this->editor.GetGame();
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
