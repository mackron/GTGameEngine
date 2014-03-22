/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_contextmanager_h_
#define __gtgl_contextmanager_h_

#include "gtgl_context.h"

struct _GTGLcontextmanager
{
    /** The number of contexts. This is not the size of the buffer. */
    size_t count;

    /** The size of the buffer. */
    size_t bufferSize;

    /** The buffer containing pointers to all of the existing contexts. */
    GTGLcontext *buffer;
};

void GTGLcontextmanager_Init();
void GTGLcontextmanager_Uninit();
void GTGLcontextmanager_Add(GTGLcontext context);
void GTGLcontextmanager_Remove(GTGLcontext context);
GTGLcontext GTGLcontextmanager_FindByThreadID(int threadID);

/** The global context manager. */
extern struct _GTGLcontextmanager GTGL_ContextManager;

#endif