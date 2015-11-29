// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_Event
#define GT_GUI_Event

#include "Element.hpp"
#include "EventCodes.hpp"
#include <GTEngine/Core/Window.hpp>
#include <cstdint>

namespace GTGUI
{
    struct Event
    {
        Element*  element;
        EventCode code;

        union
        {
            struct
            {
                unsigned int width;
                unsigned int height;
            }size;

            struct
            {
                int x;
                int y;
            }move;

            struct
            {
                GT::Key key;
            }keypressed;

            struct
            {
                GT::Key  key;
            }keydown, keyup;

            struct
            {
                int x;
                int y;
            }mousemove;

            struct
            {
                int delta;
                int x;
                int y;
            }mousewheel;

            struct
            {
                GT::MouseButton button;
                int x;
                int y;
            }mousedown, mouseup, mousedoubleclick;

            struct
            {
                const char *text;
            }settext;
        };
    };
}

#endif