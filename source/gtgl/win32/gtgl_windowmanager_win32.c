/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)    /* This guard is important in order to keep the build system simple. */

#include "../gtgl_windowmanager.h"
#include <windows.h>

MSG g_MSG;
void GTGLwindowmanager_PumpAllPendingMessages()
{
    while (PeekMessageA(&g_MSG, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&g_MSG);
        DispatchMessageA(&g_MSG);
    }
}

#endif