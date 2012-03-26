
#include <GTEngine/GUIEventHandler.hpp>
#include <GTEngine/Game.hpp>
#include <GTCore/Errors.hpp>
#include <GTCore/Log.hpp>

namespace GTEngine
{
    GUIEventHandler::GUIEventHandler(Game &game)
        : game(game)
    {
    }

    GUIEventHandler::~GUIEventHandler()
    {
    }

    void GUIEventHandler::OnError(const char *msg)
    {
        GTCore::PostError("%s", msg);
    }

    void GUIEventHandler::OnWarning(const char *msg)
    {
        GTCore::GlobalLog.Write("%s", msg);
    }

    void GUIEventHandler::OnLog(const char *msg)
    {
        GTCore::GlobalLog.Write("%s", msg);
    }

    void GUIEventHandler::OnChangeCursor(GTGUI::Cursor cursor)
    {
        switch (cursor)
        {
        case GTGUI::Cursor_Arrow: game.GetWindow()->SetCursor(GTWindow::SystemCursor_Arrow);    break;
        case GTGUI::Cursor_Beam:  game.GetWindow()->SetCursor(GTWindow::SystemCursor_IBeam);    break;
        case GTGUI::Cursor_Hand:  game.GetWindow()->SetCursor(GTWindow::SystemCursor_Hand);     break;

        default: break;
        }
    }
}