// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_GameEvent_hpp_
#define __GTEngine_GameEvent_hpp_

#include "GameEventCodes.hpp"
#include <GTCore/Windowing/Keys.hpp>
#include <GTCore/Windowing/MouseButtons.hpp>

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
                GTCore::Key key;
            }keypressed, keyreleased;

            struct
            {
                GTCore::Key key;
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
                GTCore::MouseButton button;
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