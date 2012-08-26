
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
          isStarted(false), isOpen(false),
          dataFilesWatcherEventHandler(*this)
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

                // Here we need to attach our files watcher event handler.
                this->game.GetDataFilesWatcher().AddEventHandler(this->dataFilesWatcherEventHandler);
                
                // Here is where we startup our editor modes.
                this->modelEditor.Startup(guiServer);
                this->sandbox.Startup(guiServer);

                // Here we enable the default mode.
                this->SwitchToModelEditorMode();
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

            // We want to watch the data files.
            this->game.EnableDataFilesWatching();

            // We also want to get an update on the data files immediately.
            this->game.GetDataFilesWatcher().CheckForChanges(false);
            this->game.GetDataFilesWatcher().DispatchEvents();

            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            // We don't want to be watching data files anymore.
            this->game.DisableDataFilesWatching();

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


    void Editor::OnFileInsert(const DataFilesWatcher::Item &item)
    {
        printf("File Insert: %s\n", item.info.path.c_str());
    }

    void Editor::OnFileRemove(const DataFilesWatcher::Item &item)
    {
        printf("File Remove: %s\n", item.info.path.c_str());
    }

    void Editor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        printf("File Update: %s\n", item.info.path.c_str());
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
