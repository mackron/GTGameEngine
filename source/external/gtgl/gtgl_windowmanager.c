/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

/* Platform-specific functions are implemented in win32/gtgl_windowmanager_win32.c and x11/gtgl_windowmanager_x11.c */

#include "gtgl_windowmanager.h"



/**** GTGL public API functions. These are just wrappers. ****/
void gtglPumpAllPendingWindowMessages()
{
    GTGLwindowmanager_PumpAllPendingMessages();
}
