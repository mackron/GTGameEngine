
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

    bool Editor_TextEditor::LoadFile(const char* absolutePath)
    {
        // What we do here is first determine whether or not the file has already been loaded. If it has, we just show the existing text box. Otherwise, we need
        // to create a new one.
        auto iState = this->loadedStates.Find(absolutePath);
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
            if (GTCore::IO::OpenAndReadTextFile(absolutePath, fileContent))
            {
                auto &gui    = this->editor.GetGame().GetGUI();
                auto &script = this->editor.GetGame().GetScript();


                // The file contents have been loaded, so now we need to create the text-box element that will display it.
                auto textBoxElement = gui.CreateElement("<div parentid='Editor_TextEditor' styleclass='text-editor-text-area' />");
                if (textBoxElement != nullptr)
                {
                    // The element has been created, but we need to execute a script to have it turn into a proper multi-line text box.
                    script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):MultiLineTextBox();", textBoxElement->id).c_str());


                    // Now what we need to do is actually set the text. This will be much quicker if done on the C++ side so that the script parser doesn't need to
                    // parse potentially very large files.
                    auto textAreaElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextArea:GetID();", textBoxElement->id).c_str()));
                    if (textAreaElement != nullptr)
                    {
                        textAreaElement->SetText(fileContent.c_str());
                        textAreaElement->SetStyleAttribute("background-color", "inherit");
                        textAreaElement->SetStyleAttribute("text-color",       "inherit");

                        // Now we just need to create a new state object and make it the current one.
                        if (this->currentState != nullptr)
                        {
                            this->currentState->textBox->Hide();
                        }

                        this->currentState = new State(textBoxElement, textAreaElement, absolutePath);
                        this->loadedStates.Add(absolutePath, this->currentState);


                        // Now we need to attach an event handler to the text area so we can detect changes to the text. We propagate this to the scripting
                        // environment so that the owner tab can be marked as modified.
                        textAreaElement->AttachEventHandler(*this->currentState->textAreaEventHandler);
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

    bool Editor_TextEditor::SaveFile(const char* absolutePath)
    {
        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            iState->value->isMarkedAsModified = false;

            auto textArea = iState->value->textArea;
            assert(textArea != nullptr);

            auto text = textArea->GetText();
            if (text != nullptr)
            {
                return GTCore::IO::OpenAndWriteTextFile(absolutePath, text);
            }
        }

        return false;
    }

    void Editor_TextEditor::CloseFile(const char* absolutePath)
    {
        auto iState = this->loadedStates.Find(absolutePath);
        if (iState != nullptr)
        {
            this->GetGame().GetGUI().DeleteElement(iState->value->textBox);
            

            if (this->currentState == iState->value)
            {
                this->currentState = nullptr;
            }


            delete iState->value;
            this->loadedStates.Remove(absolutePath);
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



    //////////////////////////////////////////////////
    // State

    Editor_TextEditor::State::State(GTGUI::Element* textBoxIn, GTGUI::Element* textAreaIn, const char* absolutePathIn)
        : textBox(textBoxIn), textArea(textAreaIn), textAreaEventHandler(new TextAreaEventHandler(this)), absolutePath(absolutePathIn), isMarkedAsModified(false)
    {
    }

    Editor_TextEditor::State::~State()
    {
        delete this->textAreaEventHandler;
    }


    //////////////////////////////////////////////////
    // TextAreaEventHandler

    Editor_TextEditor::TextAreaEventHandler::TextAreaEventHandler(State* stateIn)
        : GTGUI::ElementEventHandler(), state(stateIn)
    {
    }

    void Editor_TextEditor::TextAreaEventHandler::OnTextChanged(GTGUI::Element &element)
    {
        assert(this->state != nullptr);

        // The scripting environment needs to know that the element has been modified.
        auto &script = element.GetServer().GetScriptServer().GetScript();

        if (!this->state->isMarkedAsModified)
        {
            script.Execute(GTCore::String::CreateFormatted("Editor.MarkFileAsModified('%s')", this->state->absolutePath.c_str()).c_str());
            this->state->isMarkedAsModified = true;
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
