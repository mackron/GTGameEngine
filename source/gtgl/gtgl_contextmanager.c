/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_contextmanager.h"
#include <stdlib.h>
#include <string.h>

struct _GTGLcontextmanager GTGL_ContextManager;

#define GTGL_MAX(x, y) (((x) > (y)) ? (x) : (y))

void GTGLcontextmanager_Init()
{
    GTGL_ContextManager.count      = 0;
    GTGL_ContextManager.bufferSize = 16;
    GTGL_ContextManager.buffer     = (GTGLcontext *)malloc(sizeof(GTGLcontext) * GTGL_ContextManager.bufferSize);
}

void GTGLcontextmanager_Uninit()
{
    free(GTGL_ContextManager.buffer);

    GTGL_ContextManager.count      = 0;
    GTGL_ContextManager.bufferSize = 0;
    GTGL_ContextManager.buffer     = NULL;
}

void GTGLcontextmanager_ResizeBuffer(size_t newSize)
{
    GTGLcontext * oldBuffer = GTGL_ContextManager.buffer;

    GTGL_ContextManager.bufferSize = GTGL_MAX(newSize, 16U);
    GTGL_ContextManager.buffer     = (GTGLcontext *)malloc(sizeof(GTGLcontext) * GTGL_ContextManager.bufferSize);

    if (oldBuffer != NULL)
    {
        memcpy(GTGL_ContextManager.buffer, oldBuffer, sizeof(GTGLcontext) * GTGL_ContextManager.count);
        free(oldBuffer);
    }
}

void GTGLcontextmanager_Add(GTGLcontext context)
{
    /* Resize if we have to... */
    if (GTGL_ContextManager.count == GTGL_ContextManager.bufferSize)
    {
        GTGLcontextmanager_ResizeBuffer(GTGL_ContextManager.count * 2);
    }

    GTGL_ContextManager.buffer[GTGL_ContextManager.count] = context;
    ++GTGL_ContextManager.count;
}

void GTGLcontextmanager_Remove(GTGLcontext context)
{
    /* All we do is find the item and them move everything after down a position. */
    size_t i;
    for (i = 0; i < GTGL_ContextManager.count; ++i)
    {
        if (GTGL_ContextManager.buffer[i] == context)
        {
            /* We found the context, so we just move everything after it down a position and then return. */
            for (i += 1; i < GTGL_ContextManager.count; ++i)
            {
                GTGL_ContextManager.buffer[i - 1] = GTGL_ContextManager.buffer[i];
            }

            --GTGL_ContextManager.count;

            /*
            We're going to shrink the buffer in order to avoid potential wastage. This function should rarely, if ever, be called in
            time-critical code, so the memory savings outweigh the speed penalty, IMO (not that it would really matter either way...).
            */
            if (GTGL_ContextManager.bufferSize > 16 && (GTGL_ContextManager.count << 1) < GTGL_ContextManager.bufferSize)
            {
                GTGLcontextmanager_ResizeBuffer(GTGL_ContextManager.count);
            }

            return;
        }
    }
}

GTGLcontext GTGLcontextmanager_FindByThreadID(int threadID)
{
    size_t i;
    for (i = 0; i < GTGL_ContextManager.count; ++i)
    {
        if (GTGL_ContextManager.buffer[i]->threadID == threadID)
        {
            return GTGL_ContextManager.buffer[i];
        }
    }

    return NULL;
}