// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_GUIEventHandler_hpp_
#define __GTEngine_GUIEventHandler_hpp_

#include <GTGUI/ServerEventHandler.hpp>

namespace GTEngine
{
    class Game;

    class GUIEventHandler : public GTGUI::ServerEventHandler
    {
    public:

        GUIEventHandler(Game &game);
        ~GUIEventHandler();

        void OnError(const char *msg);
        void OnWarning(const char *msg);
        void OnLog(const char *msg);
        void OnChangeCursor(GTLib::Cursor cursor);

    public:

        /// A reference to the Game object that owns the GUI that this event handler is attached to.
        Game &game;


    private:    // No copying.
        GUIEventHandler(const GUIEventHandler &);
        GUIEventHandler & operator=(const GUIEventHandler &);
    };
}

#endif
