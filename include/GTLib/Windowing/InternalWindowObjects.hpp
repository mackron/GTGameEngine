// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_InternalWindowObjects_hpp_
#define __GTLib_InternalWindowObjects_hpp_

#include <cstddef>

#if defined(_WIN32)
#include "Win32/windows_types.h"
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace GTLib
{
    /**
    *   \brief  Structure containing the internal objects used by the Window class.
    */
    struct InternalWindowObjects
    {
    #if defined(_WIN32)
        InternalWindowObjects()
            : hWnd(0), hDC(0)
        {
        }

        InternalWindowObjects(const InternalWindowObjects &other)
            : hWnd(other.hWnd), hDC(other.hDC)
        {
        }

        /// The handle to the window. On Windows this is a HWND. With X11 it is a Window pointer.
        HWND hWnd;

        /// The device context of the window.
        HDC hDC;
    #elif defined(__linux__)
        InternalWindowObjects()
            : window(0), display(0), vi(0), colormap(0)
        {
        }

        InternalWindowObjects(const InternalWindowObjects &other)
            : window(other.window), display(other.display), vi(other.vi), colormap(other.colormap)
        {
        }

        /// The handle to the window.
        ::Window window;

        /// The display the window is on. This is a Display pointer.
        Display *display;

        /// A pointer to the XVisualInfo structure that can be used when creating the window.
        XVisualInfo *vi;

        /// The colour map to use with the window.
        Colormap colormap;
    #endif
    };
}

#endif