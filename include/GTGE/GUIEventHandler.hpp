// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GUIEventHandler
#define GT_GUIEventHandler

#include <GTGE/GUI/GUIServerEventHandler.hpp>

namespace GT
{
    class Game;

    class GUIEventHandler : public GUIServerEventHandler
    {
    public:

        GUIEventHandler(Game &game);
        ~GUIEventHandler();

        void OnError(const char *msg);
        void OnWarning(const char *msg);
        void OnLog(const char *msg);
        void OnChangeCursor(Cursor cursor);

    public:

        /// A reference to the Game object that owns the GUI that this event handler is attached to.
        Game &game;


    private:    // No copying.
        GUIEventHandler(const GUIEventHandler &);
        GUIEventHandler & operator=(const GUIEventHandler &);
    };
}

#endif
