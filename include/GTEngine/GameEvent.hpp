
#ifndef __GTEngine_GameEvent_hpp_
#define __GTEngine_GameEvent_hpp_

#include "GameEventCodes.hpp"
#include <GTWindow/Keys.hpp>
#include <GTWindow/MouseButtons.hpp>
#include <cstdint>

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
                GTWindow::Key key;
            }keypressed, keyreleased;

            struct
            {
                GTWindow::Key key;
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
                GTWindow::MouseButton button;
                int x;
                int y;
            }mousedown, mouseup, mousedoubleclick;

            struct
            {
                void * data0;
                void * data1;
                void * data2;
                void * data3;
            }custom;
        };
    };
}

#endif