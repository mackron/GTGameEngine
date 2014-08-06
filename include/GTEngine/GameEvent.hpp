// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_GameEvent_hpp_
#define __GTEngine_GameEvent_hpp_

#include "GameEventCodes.hpp"
#include <GTLib/Windowing/Keys.hpp>
#include <GTLib/Windowing/MouseButtons.hpp>

namespace GTEngine
{
    struct GameEvent
    {
        EventCode code;

        union
        {
            struct
            {
            }close;

            struct
            {
                unsigned int width;
                unsigned int height;
            }size;

            struct
            {
                GTLib::Key key;
            }keypressed, keyreleased;

            struct
            {
                GTLib::Key key;
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
                GTLib::MouseButton button;
                int x;
                int y;
            }mousedown, mouseup, mousedoubleclick;

            struct
            {
            }receivefocus, losefocus;

            struct
            {
                void* data0;
                void* data1;
                void* data2;
                void* data3;
            }custom;
        };
    };
}

#endif