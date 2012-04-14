
#include <GTEngine/GUIEventHandler.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GUIEventHandler::GUIEventHandler(Game &game)
        : game(game)
    {
    }

    GUIEventHandler::~GUIEventHandler()
    {
    }

    void GUIEventHandler::OnError(const char* msg)
    {
        GTEngine::PostError("%s", msg);
    }

    void GUIEventHandler::OnWarning(const char* msg)
    {
        GTEngine::Log("%s", msg);
    }

    void GUIEventHandler::OnLog(const char* msg)
    {
        GTEngine::Log("%s", msg);
    }

    void GUIEventHandler::OnChangeCursor(GTGUI::Cursor cursor)
    {
        GTEngine::Log("Setting cursor: %d", (int)cursor);

        switch (cursor)
        {
        case GTGUI::Cursor_Arrow:    game.GetWindow()->SetCursor(GTCore::SystemCursor_Arrow);       break;
        case GTGUI::Cursor_Beam:     game.GetWindow()->SetCursor(GTCore::SystemCursor_Beam);        break;
        case GTGUI::Cursor_Hand:     game.GetWindow()->SetCursor(GTCore::SystemCursor_Hand);        break;
        case GTGUI::Cursor_Cross:    game.GetWindow()->SetCursor(GTCore::SystemCursor_Cross);       break;

        case GTGUI::Cursor_SizeNS:   game.GetWindow()->SetCursor(GTCore::SystemCursor_SizeNS);      break;
        case GTGUI::Cursor_SizeWE:   game.GetWindow()->SetCursor(GTCore::SystemCursor_SizeWE);      break;
        case GTGUI::Cursor_SizeNESW: game.GetWindow()->SetCursor(GTCore::SystemCursor_SizeNESW);    break;
        case GTGUI::Cursor_SizeNWSE: game.GetWindow()->SetCursor(GTCore::SystemCursor_SizeNWSE);    break;

        default: break;
        }
    }
}