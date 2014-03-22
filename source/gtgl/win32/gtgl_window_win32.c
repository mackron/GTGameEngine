/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */


#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)    /* This guard is important in order to keep the build system simple. */

#include "../gtgl_window.h"
#include <gtgl/gtgl.h>
#include <windows.h>
#include <assert.h>

/**
*   The number of windows that have been created. We need to keep track of this so that we know when to
*   register and unregister the window class.
*/
static int GTGL_WindowCount = 0;
    
/** The class name of the window. */
static const char *GTGL_HelperWindowClass = "GTGL_HelperWindow";

/** The window procedure for every window. */
LRESULT GTGLHelperWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GTGLwindow self = (GTGLwindow)GetPropA(hWnd, "__self");
    if (self != NULL)
    {
        switch (msg)
        {
        case WM_CLOSE:
            {
                if (self->OnClose != NULL)
                {
                    self->OnClose(self);
                }

                return 0;
            }

        case WM_SIZE:
            {
                if (self->OnSize != NULL)
                {
                    self->OnSize(self, (unsigned int)LOWORD(lParam), (unsigned int)HIWORD(lParam));
                }

                break;
            }

        case WM_MOVE:
            {
                if (self->OnMove != NULL)
                {
                    self->OnMove(self, LOWORD(lParam), HIWORD(lParam));
                }

                break;
            }

        default: break;
        }
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void GTGLwindow_ctor(GTGLwindow self, const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
{
    assert(self != NULL);

    self->OnClose = NULL;
    self->OnSize  = NULL;
    self->OnMove  = NULL;

    self->props = GTGLdictionary_new();

    /* If the window count is zero we need to register the window class. */
    if (GTGL_WindowCount == 0)
    {
        WNDCLASSEXA wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = sizeof(void *);
        wc.lpfnWndProc   = (WNDPROC)GTGLHelperWindowProc;
        wc.lpszClassName = GTGL_HelperWindowClass;
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        RegisterClassExA(&wc);
    }
 
    /* We can create the window when we have a class. */
    self->hWnd = CreateWindowExA(0, GTGL_HelperWindowClass, title, 
                                 WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
                                 xPos, yPos, (int)width, (int)height, NULL, NULL, GetModuleHandle(NULL), NULL);

    /*
    Once we have the window created, we want to setup it's device context so that it's usable by OpenGL. Take note of how we
    retrieve platform-specific data with gtglGetPixelFormat() and gtglGetPFD(). These are required for setting up a window in
    Windows. On X systems, we have equivalent functions.
    */
    self->hDC = GetDC(self->hWnd);
    SetPixelFormat(self->hDC, gtglGetPixelFormat(), gtglGetPFD());

    /* Set need to set a property which will simply be a pointer to 'self'. This will be used in the window procedure. */
    SetPropA(self->hWnd, "__self", (HANDLE)self);

    ++GTGL_WindowCount;
}

void GTGLwindow_dtor(GTGLwindow self)
{
    DestroyWindow(self->hWnd);

    --GTGL_WindowCount;
    if (GTGL_WindowCount <= 0)
    {
        UnregisterClassA(GTGL_HelperWindowClass, GetModuleHandle(NULL));
    }

    GTGLdictionary_delete(self->props);
    self->props = NULL;
}


void GTGLwindow_Show(GTGLwindow self)
{
    ShowWindow(self->hWnd, SW_SHOWNORMAL);
}

void GTGLwindow_Hide(GTGLwindow self)
{
    ShowWindow(self->hWnd, SW_HIDE);
}

#endif
