
#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    Editor::Editor()
        : GUI(), isStarted(false), isOpen(false)
    {
    }

    Editor::~Editor()
    {
    }

    bool Editor::Startup(GTGUI::Server &guiServer)
    {
        if (!this->isStarted)
        {
            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("editor/main.xml"))
            {
                this->GUI.main = guiServer.GetElementByID("EditorMain");


                this->isStarted = true;
            }
            else
            {
                GTEngine::PostError("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists.");
                return false;
            }
        }

        return true;
    }

    void Editor::Open()
    {
        if (!this->isOpen)
        {
            this->GUI.main->Show();

            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.main->Hide();

            this->isOpen = false;
        }
    }
}
