/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#if defined(_MSC_VER)
    #pragma warning(disable:4206)   /* nonstandard extension used : translation unit is empty */
#endif

#if defined(__linux__)      /* This guard is important in order to keep the build system simple. Causes warnings in MSVC, though :( */

#include "../gtgl_window.h"
#include <gtgl/gtgl.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xresource.h>
#include <string.h>
#include <assert.h>

/*
The number of windows that have been created. We need to keep track of this so that we know when to
allocate and deallocate various resources.
*/
static int GTGL_WindowCount = 0;

/** The X11 Atom we need to use for handling when the close button is hit. */
static Atom GTGL_Atom_WM_DELETE_WINDOW = 0;

/** The X11 context we'll be using to associate the GTGLwindow with the X11 window object. */
static XContext GTGL_X11Context_self = 0;

void GTGLHandleHelperWindowX11Event(XEvent *e)
{
    GTGLwindow self;
    assert(e != NULL);

    XFindContext(e->xany.display, e->xany.window, GTGL_X11Context_self, (XPointer*)&self);
    if (self != NULL)
    {
        switch (e->type)
        {
        case ClientMessage:
            {
                if ((Atom)e->xclient.data.l[0] == GTGL_Atom_WM_DELETE_WINDOW)
                {
                    if (self->OnClose != NULL)
                    {
                        self->OnClose(self);
                    }
                }

                break;
            }

        case ConfigureNotify:
            {
                /*
                This message is sent for both positions and sizing. We need to check which it was and send the
                appropriate event.
                */
                unsigned int newWidth  = (unsigned int)e->xconfigure.width;
                unsigned int newHeight = (unsigned int)e->xconfigure.height;
                int newXPos = (int)e->xconfigure.x;
                int newYPos = (int)e->xconfigure.y;

                if (self->OnSize != NULL && (self->width != newWidth || self->height != newHeight))
                {
                    self->OnSize(self, newWidth, newHeight);
                }

                if (self->OnMove != NULL && (self->xPos != newXPos || self->yPos != newYPos))
                {
                    self->OnMove(self, newXPos, newYPos);
                }

                self->xPos   = newXPos;
                self->yPos   = newYPos;
                self->width  = newWidth;
                self->height = newHeight;

                break;
            }

        default: break;
        }
    }
}

void GTGLwindow_ctor(GTGLwindow self, const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
{
    XSetWindowAttributes wa;
    XTextProperty windowName;

    /* These properties here are what we need to create a window suitable for rendering. */
    Display*     display   = gtglGetDisplay();
    XVisualInfo* vi        = gtglGetVisualInfo();
    Colormap     colourmap = gtglGetColormap();

    assert(self != NULL);

    self->OnClose = NULL;
    self->OnSize  = NULL;
    self->OnMove  = NULL;

    self->props = GTGLdictionary_new();

    self->xPos   = xPos;
    self->yPos   = yPos;
    self->width  = width;
    self->height = height;

    /* Need to create a few things if required. */
    if (GTGL_WindowCount == 0)
    {
        /* We need to create the context for associating 'self' with the window. */
        GTGL_X11Context_self = XUniqueContext();

        /*
        We use this atom for detecting when the close button is pressed. When a window is created, we use XSetWMProtocols() to "attach"
        the atom to the window. We can then check it when we receive a ClientMessage event.
        */
        GTGL_Atom_WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", True);
    }


    wa.colormap     = colourmap;
    wa.border_pixel = 0;
    wa.event_mask   = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonMotionMask | FocusChangeMask;

    self->x11Window = XCreateWindow(display, RootWindow(display, vi->screen), xPos, yPos, width, height,
                                    0, vi->depth, InputOutput, vi->visual,
                                    CWBorderPixel | CWColormap | CWEventMask, &wa);

    /* "Attach" the close event atom. If we don't do this we won't be able to detect when the window's close button was pressed. */
    XSetWMProtocols(display, self->x11Window, &GTGL_Atom_WM_DELETE_WINDOW, 1);

    /* Here we set the title of the window. This is pretty ridiculous... */
    windowName.value    = (unsigned char *)title;
    windowName.encoding = XA_STRING;
    windowName.format   = 8;
    windowName.nitems   = (unsigned int)strlen(title);
    XSetWMName(display, self->x11Window, &windowName);

    /** We need to associate the 'self' pointer with the window. */
    XSaveContext(display, self->x11Window, GTGL_X11Context_self, (XPointer)self);

    ++GTGL_WindowCount;
}

void GTGLwindow_dtor(GTGLwindow self)
{
    XDestroyWindow(gtglGetDisplay(), self->x11Window);

    --GTGL_WindowCount;
    if (GTGL_WindowCount == 0)
    {
        XDeleteContext(gtglGetDisplay(), self->x11Window, GTGL_X11Context_self);
    }

    GTGLdictionary_delete(self->props);
    self->props = NULL;
}

void GTGLwindow_Show(GTGLwindow self)
{
    XMapWindow(gtglGetDisplay(), self->x11Window);
}

void GTGLwindow_Hide(GTGLwindow self)
{
    XUnmapWindow(gtglGetDisplay(), self->x11Window);
}

#endif
