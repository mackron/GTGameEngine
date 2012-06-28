
#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTGUI/Server.hpp>


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor::Editor(Game &game)
        : game(game), GUI(),
          modelEditor(*this), sandbox(*this),
          currentMode(nullptr), previousMode(nullptr),
          isStarted(false), isOpen(false)
    {
    }

    Editor::~Editor()
    {
    }

    bool Editor::Startup(GTGUI::Server &guiServer)
    {
        if (!this->isStarted)
        {
            auto &script = guiServer.GetScriptServer().GetScript();

            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("engine/editor/main.xml"))
            {
                this->GUI.EditorMain = guiServer.GetElementByID("EditorMain");
                
                // Here is where we startup our editor modes.
                this->modelEditor.Startup(guiServer);
                this->sandbox.Startup(guiServer);
                

                // Here we enable the default mode.
                this->SwitchToSandboxMode();
                this->isStarted = true;
            }
            else
            {
                GTEngine::PostError("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists. %s.", script.ToString(-1));
                return false;
            }
        }

        return true;
    }

    void Editor::Open()
    {
        if (!this->isOpen && this->GUI.EditorMain != nullptr)
        {
            this->game.ShowCursor();
            this->GUI.EditorMain->Show();

            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            this->isOpen = false;
        }
    }


    void Editor::SwitchToModelEditorMode()
    {
        this->SetEditorMode(&this->modelEditor);
    }

    void Editor::SwitchToSandboxMode()
    {
        this->SetEditorMode(&this->sandbox);
    }

    void Editor::SwitchToPreviousMode()
    {
        this->SetEditorMode(this->previousMode);
    }


    void Editor::Update(double deltaTimeInSeconds)
    {
        if (this->currentMode != nullptr)
        {
            this->currentMode->OnUpdate(deltaTimeInSeconds);
        }
    }

    void Editor::SwapRCQueues()
    {
        if (this->currentMode != nullptr)
        {
            this->currentMode->OnSwapRCQueues();
        }
    }



    void Editor::SetEditorMode(EditorMode* newMode)
    {
        if (this->currentMode != newMode)
        {
            this->previousMode = this->currentMode;
            if (this->previousMode != nullptr)
            {
                this->previousMode->OnDeactivate();
            }

            this->currentMode = newMode;
            if (this->currentMode != nullptr)
            {
                this->currentMode->OnActivate();
            }
        }
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
