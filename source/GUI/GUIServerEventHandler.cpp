// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GUIServerEventHandler.hpp>
#include <GTEngine/Errors.hpp>

namespace GTGUI
{
    GUIServerEventHandler GUIServerEventHandler::Default;
    
    void GUIServerEventHandler::OnError(const char *msg)
    {
        GT::PostError("%s", msg);
    }
    
    void GUIServerEventHandler::OnWarning(const char *msg)
    {
        GT::PostError("%s", msg);
    }
    
    void GUIServerEventHandler::OnLog(const char *)
    {
        // Application must define this so they can use their own logs without having to attach them to the server.
    }

    void GUIServerEventHandler::OnChangeCursor(GT::Cursor)
    {
    }

    void GUIServerEventHandler::OnLoadFont(GT::Font &)
    {
    }

    void GUIServerEventHandler::OnUnloadFont(GT::Font &)
    {
    }
}
