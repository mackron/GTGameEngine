// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/ServerEventHandler.hpp>
#include <GTEngine/Errors.hpp>

namespace GTGUI
{
    ServerEventHandler ServerEventHandler::Default;
    
    void ServerEventHandler::OnError(const char *msg)
    {
        GT::PostError("%s", msg);
    }
    
    void ServerEventHandler::OnWarning(const char *msg)
    {
        GT::PostError("%s", msg);
    }
    
    void ServerEventHandler::OnLog(const char *)
    {
        // Application must define this so they can use their own logs without having to attach them to the server.
    }

    void ServerEventHandler::OnChangeCursor(GT::Cursor)
    {
    }

    void ServerEventHandler::OnLoadFont(GT::Font &)
    {
    }

    void ServerEventHandler::OnUnloadFont(GT::Font &)
    {
    }
}
