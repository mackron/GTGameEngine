// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GUIEventHandler.hpp>
#include <GTGE/GTEngine.hpp>

namespace GT
{
    GUIEventHandler::GUIEventHandler(Context &context)
        : context(context)
    {
    }

    GUIEventHandler::~GUIEventHandler()
    {
    }

    void GUIEventHandler::OnError(const char* msg)
    {
        g_Context->LogErrorf("%s", msg);
    }

    void GUIEventHandler::OnWarning(const char* msg)
    {
        g_Context->Logf("%s", msg);
    }

    void GUIEventHandler::OnLog(const char* msg)
    {
        g_Context->Logf("%s", msg);
    }

    void GUIEventHandler::OnChangeCursor(Cursor cursor)
    {
        if (cursor != Cursor_None)
        {
            context.GetWindow()->SetCursor(cursor);
        }
    }
}
