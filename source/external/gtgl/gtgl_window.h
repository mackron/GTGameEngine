/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_window_h_
#define __gtgl_window_h_

#include "gtgl_utils.h"

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#include <gtgl/GL/gtwgl_windef.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#elif defined(__APPLE__) && defined(__MACH__)
#endif

#ifndef __gtgl_h_
typedef struct _GTGLwindow* GTGLwindow;

typedef void(* OnCloseWindowProc)(GTGLwindow window);
typedef void(* OnSizeWindowProc) (GTGLwindow window, unsigned int newWidth, unsigned int newHeight);
typedef void(* OnMoveWindowProc) (GTGLwindow window, int newXPos, int newYPos);
#endif

struct _GTGLwindow
{
    OnCloseWindowProc OnClose;      /**< The function to call when the window receives a Close event (such as when the 'X' is hit on the window). */
    OnSizeWindowProc  OnSize;       /**< The function to call after the window has changed size. */
    OnMoveWindowProc  OnMove;       /**< The function to call after the window has moved. */

    GTGLdictionary props;

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
    HWND hWnd;
    HDC  hDC;
#elif defined(__linux__)
    Window x11Window;

    /*
    X11 sends the ConfigureNotify for both movement and sizing. We need to keep track of the position and
    size so that we can selectively call the appropriate handlers.
    */
    int xPos;
    int yPos;
    unsigned int width;
    unsigned int height;
#elif defined(__APPLE__) && defined(__MACH__)
#endif
};


/** GTGLwindow constructor. */
void GTGLwindow_ctor(GTGLwindow self, const char* title, int xPos, int yPos, unsigned int width, unsigned int height);

/** GTGLwindow destructor. */
void GTGLwindow_dtor(GTGLwindow self);

/** Creates a new GTGLwindow object on the heap. */
GTGLwindow GTGLwindow_new(const char* title, int xPos, int yPos, unsigned int width, unsigned int height);

/** Deletes a window created with GTGLwindow_delete. */
void GTGLwindow_delete(GTGLwindow self);


/** Shows a window. */
void GTGLwindow_Show(GTGLwindow self);

/** Hides a window. */
void GTGLwindow_Hide(GTGLwindow self);


/** Sets the OnClose event handler. */
void GTGLwindow_OnClose(GTGLwindow self, OnCloseWindowProc proc);

/** Sets the OnSize event handler. */
void GTGLwindow_OnSize(GTGLwindow self, OnSizeWindowProc proc);

/** Sets the OnMove event handler. */
void GTGLwindow_OnMove(GTGLwindow self, OnMoveWindowProc proc);


/** Sets a property on the given window. */
void GTGLwindow_SetProp(GTGLwindow self, const char* name, void* value);

/** Retrieves a property on the given window. */
void* GTGLwindow_GetProp(GTGLwindow self, const char* name);


#endif