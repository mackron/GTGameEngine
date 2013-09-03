// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_GameEventCodes_hpp_
#define __GTEngine_GameEventCodes_hpp_

#include <cstddef>

namespace GTEngine
{
    typedef size_t EventCode;

    namespace EventCodes
    {
        static const EventCode Unknown                  = 0;
        static const EventCode OnClose                  = 1;
        static const EventCode OnSize                   = 2;
        static const EventCode OnKeyPressed             = 3;
        static const EventCode OnKeyReleased            = 4;
        static const EventCode OnKeyDown                = 5;
        static const EventCode OnKeyUp                  = 6;
        static const EventCode OnMouseMove              = 7;
        static const EventCode OnMouseWheel             = 8;
        static const EventCode OnMouseButtonDown        = 9;
        static const EventCode OnMouseButtonUp          = 10;
        static const EventCode OnMouseButtonDoubleClick = 11;
        static const EventCode OnReceiveFocus           = 12;
        static const EventCode OnLoseFocus              = 13;

        static const EventCode Custom                   = 1024;
    }
}

#endif
