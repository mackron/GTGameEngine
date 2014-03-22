/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#if defined(_MSC_VER)
    #pragma warning(disable:4206)   /* nonstandard extension used : translation unit is empty */
#endif

#if defined(__linux__)      /* This guard is important in order to keep the build system simple. Causes warnings in MSVC, though :( */

#include "../gtgl_windowmanager.h"
#include <gtgl/gtgl.h>
#include <X11/Xlib.h>

/* This is defined in gtgl_window_x11.c. */
void GTGLHandleHelperWindowX11Event(XEvent *e);

XEvent g_event;
void GTGLwindowmanager_PumpAllPendingMessages()
{
    Display* display = gtglGetDisplay();

    /* We just keep looping while we still having messages pending. Note how we use gtglGetDisplay() to retrieve the X11 display. */
    while (XPending(display) > 0)
    {
        XNextEvent(display, &g_event);
        GTGLHandleHelperWindowX11Event(&g_event);
    }
}

#endif