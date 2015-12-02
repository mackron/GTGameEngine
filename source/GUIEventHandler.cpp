// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GUIEventHandler.hpp>
#include <GTGE/Game.hpp>
#include <GTGE/GTEngine.hpp>

namespace GT
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
        g_EngineContext->LogErrorf("%s", msg);
    }

    void GUIEventHandler::OnWarning(const char* msg)
    {
        g_EngineContext->Logf("%s", msg);
    }

    void GUIEventHandler::OnLog(const char* msg)
    {
        g_EngineContext->Logf("%s", msg);
    }

    void GUIEventHandler::OnChangeCursor(Cursor cursor)
    {
        if (cursor != Cursor_None)
        {
            game.GetWindow()->SetCursor(cursor);
        }

#if 0
        switch (cursor)
        {
        case Cursor_Arrow:    game.GetWindow()->SetCursor(Cursor_Arrow);       break;
        case Cursor_Beam:     game.GetWindow()->SetCursor(Cursor_Beam);        break;
        case Cursor_Hand:     game.GetWindow()->SetCursor(Cursor_Hand);        break;
        case Cursor_Cross:    game.GetWindow()->SetCursor(Cursor_Cross);       break;

        case Cursor_VertDoubleArrow: game.GetWindow()->SetCursor(Cursor_VertDoubleArrow); break;
        case Cursor_HorzDoubleArrow: game.GetWindow()->SetCursor(Cursor_HorzDoubleArrow); break;

        case Cursor_SizeArrowTop:         game.GetWindow()->SetCursor(Cursor_SizeArrowTop);         break;
        case Cursor_SizeArrowBottom:      game.GetWindow()->SetCursor(Cursor_SizeArrowBottom);      break;
        case Cursor_SizeArrowLeft:        game.GetWindow()->SetCursor(Cursor_SizeArrowLeft);        break;
        case Cursor_SizeArrowRight:       game.GetWindow()->SetCursor(Cursor_SizeArrowRight);       break;
        case Cursor_SizeArrowTopLeft:     game.GetWindow()->SetCursor(Cursor_SizeArrowTopLeft);     break;
        case Cursor_SizeArrowTopRight:    game.GetWindow()->SetCursor(Cursor_SizeArrowTopRight);    break;
        case Cursor_SizeArrowBottomLeft:  game.GetWindow()->SetCursor(Cursor_SizeArrowBottomLeft);  break;
        case Cursor_SizeArrowBottomRight: game.GetWindow()->SetCursor(Cursor_SizeArrowBottomRight); break;

        case Cursor_None:
        default: break;
        }
#endif
    }
}
