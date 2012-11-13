
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
        auto &gui = this->editor.GetGame().GetGUI();

        this->GUI.TextBox = gui.GetElementByID(this->editor.GetGame().GetScript().GetString("Editor_TextEditor_TextBox.TextArea:GetID()"));
    }

    bool Editor_TextEditor::LoadTextFile(const char* fileName)
    {
        // TODO: This is super inefficient. We should simply create a separate GUI text box for this.


        GTCore::String fileContent;
        if (GTCore::IO::OpenAndReadTextFile(fileName, fileContent))
        {
            // Now we need to setup some state for the new text file. If the text file has already had it's state recorded, we can just restore it from the map.
            auto iState = this->loadedStates.Find(fileName);
            if (iState != nullptr)
            {
                this->currentState = iState->value;
            }
            else
            {
                this->currentState = new State;
                this->loadedStates.Add(fileName, this->currentState);
            }

            this->GUI.TextBox->SetText(fileContent.c_str());

            return true;
        }

        return false;
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
