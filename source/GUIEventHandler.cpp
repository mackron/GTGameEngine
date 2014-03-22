// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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
        switch (cursor)
        {
        case GTGUI::Cursor_Arrow:    game.GetWindow()->SetCursor(GTLib::SystemCursor_Arrow);       break;
        case GTGUI::Cursor_Beam:     game.GetWindow()->SetCursor(GTLib::SystemCursor_Beam);        break;
        case GTGUI::Cursor_Hand:     game.GetWindow()->SetCursor(GTLib::SystemCursor_Hand);        break;
        case GTGUI::Cursor_Cross:    game.GetWindow()->SetCursor(GTLib::SystemCursor_Cross);       break;

        case GTGUI::Cursor_VertDoubleArrow: game.GetWindow()->SetCursor(GTLib::SystemCursor_VertDoubleArrow); break;
        case GTGUI::Cursor_HorzDoubleArrow: game.GetWindow()->SetCursor(GTLib::SystemCursor_HorzDoubleArrow); break;

        case GTGUI::Cursor_SizeArrowTop:         game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowTop);         break;
        case GTGUI::Cursor_SizeArrowBottom:      game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowBottom);      break;
        case GTGUI::Cursor_SizeArrowLeft:        game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowLeft);        break;
        case GTGUI::Cursor_SizeArrowRight:       game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowRight);       break;
        case GTGUI::Cursor_SizeArrowTopLeft:     game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowTopLeft);     break;
        case GTGUI::Cursor_SizeArrowTopRight:    game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowTopRight);    break;
        case GTGUI::Cursor_SizeArrowBottomLeft:  game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowBottomLeft);  break;
        case GTGUI::Cursor_SizeArrowBottomRight: game.GetWindow()->SetCursor(GTLib::SystemCursor_SizeArrowBottomRight); break;

        case GTGUI::Cursor_None:
        default: break;
        }
    }
}
