/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

/* Platform-specific functions are in win32/gtgl_window_win32.c and x11/gtgl_window_x11.c. */

#include "gtgl_window.h"
#include <stdlib.h>


GTGLwindow GTGLwindow_new(const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
{
    GTGLwindow self = (GTGLwindow)malloc(sizeof(struct _GTGLwindow));
    GTGLwindow_ctor(self, title, xPos, yPos, width, height);

    return self;
}

void GTGLwindow_delete(GTGLwindow self)
{
    GTGLwindow_dtor(self);
    free(self);
}

void GTGLwindow_OnClose(GTGLwindow self, OnCloseWindowProc proc)
{
    self->OnClose = proc;
}

void GTGLwindow_OnSize(GTGLwindow self, OnSizeWindowProc proc)
{
    self->OnSize = proc;
}

void GTGLwindow_OnMove(GTGLwindow self, OnMoveWindowProc proc)
{
    self->OnMove = proc;
}

void GTGLwindow_SetProp(GTGLwindow self, const char* name, void* value)
{
    GTGLdictionary_Add(self->props, name, value);
}

void* GTGLwindow_GetProp(GTGLwindow self, const char* name)
{
    return GTGLdictionary_Find(self->props, name);
}



/**** Public GTGL API functions. These are just wrappers. ****/
GTGLwindow gtglCreateWindow(const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
{
    return GTGLwindow_new(title, xPos, yPos, width, height);
}

void gtglDeleteWindow(GTGLwindow window)
{
    GTGLwindow_delete(window);
}

void gtglShowWindow(GTGLwindow window)
{
    GTGLwindow_Show(window);
}

void gtglHideWindow(GTGLwindow window)
{
    GTGLwindow_Hide(window);
}

void gtglWindowOnClose(GTGLwindow window, OnCloseWindowProc proc)
{
    GTGLwindow_OnClose(window, proc);
}

void gtglWindowOnSize(GTGLwindow window, OnSizeWindowProc proc)
{
    GTGLwindow_OnSize(window, proc);
}

void gtglWindowOnMove(GTGLwindow window, OnMoveWindowProc proc)
{
    GTGLwindow_OnMove(window, proc);
}

void gtglWindowSetProp(GTGLwindow window, const char* name, void* value)
{
    GTGLwindow_SetProp(window, name, value);
}

void* gtglWindowGetProp(GTGLwindow window, const char* name)
{
    return GTGLwindow_GetProp(window, name);
}
