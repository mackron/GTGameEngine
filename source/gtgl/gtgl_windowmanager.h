/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_windowmanager_h_
#define __gtgl_windowmanager_h_

/* Uncomment when we actually have attributes...
struct _GTGLwindowmanager
{
};
extern struct _GTGLwindowmanager GTGL_WindowManager;
*/

/** Processes any pending window messages. Non-blocking. */
void GTGLwindowmanager_PumpAllPendingMessages();


#endif