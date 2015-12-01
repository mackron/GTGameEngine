// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIGameEventCodes
#define GT_GUIGameEventCodes

namespace GTGUI
{
    enum GUIEventCode
    {
        GUIEventCode_Unknown = 0,
        GUIEventCode_Null    = 0,

        GUIEventCode_OnSize,
        GUIEventCode_OnMove,
        GUIEventCode_OnKeyPressed,
        GUIEventCode_OnKeyDown,
        GUIEventCode_OnKeyUp,
        GUIEventCode_OnMouseMove,
        GUIEventCode_OnMouseWheel,
        GUIEventCode_OnMouseButtonDown,
        GUIEventCode_OnMouseButtonUp,
        GUIEventCode_OnMouseButtonDoubleClick,
    };
}

#endif