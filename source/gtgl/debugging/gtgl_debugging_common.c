/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "../gtgl_context.h"
#include "gtgl_debugging_common.h"

/**
*   \brief  The default debug message callback to use with ARB_debug_output. This will call the appropriate debugging routines.
*/
void APIENTRY gtglDefaultDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
    /* The user param here is a pointer to the context that set the debug message callback. This make asynchronous output possible. */
    GTGLcontext context = (GTGLcontext)userParam;
    if (context != NULL)
    {
        char formattedMessage[1024];
        gtglFormatDebugMessage(formattedMessage, 1024, source, type, id, severity, message);

        if (context->debugMessageCallback != NULL)
        {
            context->debugMessageCallback(formattedMessage);
        }
    }

    (void)length;
}


void gtglSetDebugMessageCallback(PFNGTGLDEBUGMESSAGECALLBACKPROC callback)
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        if (glDebugMessageCallbackARB != NULL)
        {
            context->debugMessageCallback = callback;

            glDebugMessageCallbackARB(gtglDefaultDebugMessageCallback, context);
        }
    }
}

PFNGTGLDEBUGMESSAGECALLBACKPROC gtglGetDebugMessageCallback()
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        return context->debugMessageCallback;
    }

    return NULL;
}



void gtglSetGLCallCallback(PFNGTGLOPENGLCALLCALLBACKPROC callback)
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        context->openGLCallCallback = callback;
    }
}

PFNGTGLOPENGLCALLCALLBACKPROC gtglGetGLCallCallback()
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        return context->openGLCallCallback;
    }

    return NULL;
}
