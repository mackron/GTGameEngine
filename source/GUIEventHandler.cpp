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

    void GUIEventHandler::OnChangeCursor(GTLib::Cursor cursor)
    {
        if (cursor != GTLib::Cursor_None)
        {
            game.GetWindow()->SetCursor(cursor);
        }

#if 0
        switch (cursor)
        {
        case GTLib::Cursor_Arrow:    game.GetWindow()->SetCursor(GTLib::Cursor_Arrow);       break;
        case GTLib::Cursor_Beam:     game.GetWindow()->SetCursor(GTLib::Cursor_Beam);        break;
        case GTLib::Cursor_Hand:     game.GetWindow()->SetCursor(GTLib::Cursor_Hand);        break;
        case GTLib::Cursor_Cross:    game.GetWindow()->SetCursor(GTLib::Cursor_Cross);       break;

        case GTLib::Cursor_VertDoubleArrow: game.GetWindow()->SetCursor(GTLib::Cursor_VertDoubleArrow); break;
        case GTLib::Cursor_HorzDoubleArrow: game.GetWindow()->SetCursor(GTLib::Cursor_HorzDoubleArrow); break;

        case GTLib::Cursor_SizeArrowTop:         game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowTop);         break;
        case GTLib::Cursor_SizeArrowBottom:      game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowBottom);      break;
        case GTLib::Cursor_SizeArrowLeft:        game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowLeft);        break;
        case GTLib::Cursor_SizeArrowRight:       game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowRight);       break;
        case GTLib::Cursor_SizeArrowTopLeft:     game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowTopLeft);     break;
        case GTLib::Cursor_SizeArrowTopRight:    game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowTopRight);    break;
        case GTLib::Cursor_SizeArrowBottomLeft:  game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowBottomLeft);  break;
        case GTLib::Cursor_SizeArrowBottomRight: game.GetWindow()->SetCursor(GTLib::Cursor_SizeArrowBottomRight); break;

        case GTLib::Cursor_None:
        default: break;
        }
#endif
    }
}
