/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_emulation_common.h"
#include "gtgl_arb_debug_output.h"
#include "../gtgl_context.h"
#include "../gtgl_utils.h"

/*
All functions in this file have a server implementation of the same function. This implementation must be called. Erroneous input
will be handled by the server implementation.
*/

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4054)   /* from function pointer 'GLDEBUGPROCARB' to data pointer 'GLvoid *' */
#endif

#define GTGL_CALL_SERVER_IMPL(functype, func, params) \
    if (__server_##func == NULL) \
    { \
        __server_##func = (functype)gtglGetProcAddress(#func); \
    } \
    if (__server_##func != NULL) \
    { \
        __server_##func params; \
    }


#define GTGL_CALL_SERVER_IMPL_RETURN(functype, func, params) \
    if (__server_##func == NULL) \
    { \
        __server_##func = (functype)gtglGetProcAddress(#func); \
    } \
    if (__server_##func != NULL) \
    { \
        retValue = __server_##func params; \
    }



PFNGLENABLEPROC      __server_glEnable      = NULL;
PFNGLDISABLEPROC     __server_glDisable     = NULL;
PFNGLISENABLEDPROC   __server_glIsEnabled   = NULL;
PFNGLGETBOOLEANVPROC __server_glGetBooleanv = NULL;
PFNGLGETDOUBLEVPROC  __server_glGetDoublev  = NULL;
PFNGLGETFLOATVPROC   __server_glGetFloatv   = NULL;
PFNGLGETINTEGERVPROC __server_glGetIntegerv = NULL;
PFNGLGETPOINTERVPROC __server_glGetPointerv = NULL;

void APIENTRY gtglEnable(GLenum cap)
{
    if (GTGL_MasterContext->ARB_debug_output.emulating)
    {
        GTGLcontext context = gtglGetCurrentContext();
        if (context != NULL)
        {
            switch (cap)
            {
            case GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB:
                {
                    context->ARB_debug_output.DEBUG_OUTPUT_SYNCHRONOUS_ARB = GL_TRUE;
                    return;
                }

            default: break;
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLENABLEPROC, glEnable, (cap));
}

void APIENTRY gtglDisable(GLenum cap)
{
    if (GTGL_MasterContext->ARB_debug_output.emulating)
    {
        GTGLcontext context = gtglGetCurrentContext();
        if (context != NULL)
        {
            switch (cap)
            {
            case GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB:
                {
                    context->ARB_debug_output.DEBUG_OUTPUT_SYNCHRONOUS_ARB = GL_FALSE;
                    return;
                }

            default: break;
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLDISABLEPROC, glDisable, (cap));
}

GLboolean APIENTRY gtglIsEnabled(GLenum cap)
{
    GLboolean retValue = GL_FALSE;

    if (GTGL_MasterContext->ARB_debug_output.emulating)
    {
        GTGLcontext context = gtglGetCurrentContext();
        if (context != NULL)
        {
            switch (cap)
            {
            case GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB:
                {
                    return context->ARB_debug_output.DEBUG_OUTPUT_SYNCHRONOUS_ARB;
                }

            default: break;
            }
        }
    }


    GTGL_CALL_SERVER_IMPL_RETURN(PFNGLISENABLEDPROC, glIsEnabled, (cap));
    return retValue;
}


void APIENTRY gtglGetBooleanv(GLenum pname, GLboolean* params)
{
    if (params != NULL)
    {
        if (GTGL_MasterContext->ARB_debug_output.emulating)
        {
            GTGLcontext context = gtglGetCurrentContext();
            if (context != NULL)
            {
                switch (pname)
                {
                case GL_MAX_DEBUG_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLboolean)context->ARB_debug_output.MAX_DEBUG_MESSAGE_LENGTH_ARB;
                        return;
                    }

                case GL_MAX_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLboolean)context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLboolean)context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLboolean)context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB;
                        return;
                    }

                default: break;
                }
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLGETBOOLEANVPROC, glGetBooleanv, (pname, params));
}

void APIENTRY gtglGetDoublev(GLenum pname, GLdouble* params)
{
    if (params != NULL)
    {
        if (GTGL_MasterContext->ARB_debug_output.emulating)
        {
            GTGLcontext context = gtglGetCurrentContext();
            if (context != NULL)
            {
                switch (pname)
                {
                case GL_MAX_DEBUG_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLdouble)context->ARB_debug_output.MAX_DEBUG_MESSAGE_LENGTH_ARB;
                        return;
                    }

                case GL_MAX_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLdouble)context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLdouble)context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLdouble)context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB;
                        return;
                    }

                default: break;
                }
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLGETDOUBLEVPROC, glGetDoublev, (pname, params));
}

void APIENTRY gtglGetFloatv(GLenum pname, GLfloat* params)
{
    if (params != NULL)
    {
        if (GTGL_MasterContext->ARB_debug_output.emulating)
        {
            GTGLcontext context = gtglGetCurrentContext();
            if (context != NULL)
            {
                switch (pname)
                {
                case GL_MAX_DEBUG_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLfloat)context->ARB_debug_output.MAX_DEBUG_MESSAGE_LENGTH_ARB;
                        return;
                    }

                case GL_MAX_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLfloat)context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLfloat)context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLfloat)context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB;
                        return;
                    }

                default: break;
                }
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLGETFLOATVPROC, glGetFloatv, (pname, params));
}

void APIENTRY gtglGetIntegerv(GLenum pname, GLint* params)
{
    if (params != NULL)
    {
        if (GTGL_MasterContext->ARB_debug_output.emulating)
        {
            GTGLcontext context = gtglGetCurrentContext();
            if (context != NULL)
            {
                switch (pname)
                {
                case GL_MAX_DEBUG_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLint)context->ARB_debug_output.MAX_DEBUG_MESSAGE_LENGTH_ARB;
                        return;
                    }

                case GL_MAX_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLint)context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_LOGGED_MESSAGES_ARB:
                    {
                        *params = (GLint)context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
                        return;
                    }

                case GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB:
                    {
                        *params = (GLint)context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB;
                        return;
                    }

                default: break;
                }
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLGETINTEGERVPROC, glGetIntegerv, (pname, params));
}

void APIENTRY gtglGetPointerv(GLenum pname, GLvoid** params)
{
    if (params != NULL)
    {
        if (GTGL_MasterContext->ARB_debug_output.emulating)
        {
            GTGLcontext context = gtglGetCurrentContext();
            if (context != NULL)
            {
                switch (pname)
                {
                case GL_DEBUG_CALLBACK_FUNCTION_ARB:
                    {
                        *params = (GLvoid*)function_to_voidp((voidproc)context->ARB_debug_output.DEBUG_CALLBACK_FUNCTION_ARB);
                        return;
                    }

                case GL_DEBUG_CALLBACK_USER_PARAM_ARB:
                    {
                        *params = (GLvoid*)context->ARB_debug_output.DEBUG_CALLBACK_USER_PARAM_ARB;
                        return;
                    }

                default: break;
                }
            }
        }
    }

    GTGL_CALL_SERVER_IMPL(PFNGLGETPOINTERVPROC, glGetPointerv, (pname, params));
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
