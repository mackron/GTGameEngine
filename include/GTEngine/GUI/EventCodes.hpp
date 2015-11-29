// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_GameEventCodes
#define GT_GUI_GameEventCodes

namespace GTGUI
{
    enum EventCode
    {
        EventCode_Unknown = 0,
        EventCode_Null    = 0,

        EventCode_OnSize,
        EventCode_OnMove,
        EventCode_OnKeyPressed,
        EventCode_OnKeyDown,
        EventCode_OnKeyUp,
        EventCode_OnMouseMove,
        EventCode_OnMouseWheel,
        EventCode_OnMouseButtonDown,
        EventCode_OnMouseButtonUp,
        EventCode_OnMouseButtonDoubleClick,
    };
}

#endif