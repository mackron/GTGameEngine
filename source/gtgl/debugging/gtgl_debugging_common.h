/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

/*
Always include this file after gtgl.h. It depends on some error routines that are delcared there.
*/

#ifndef __gtgl_debugging_generic_h_
#define __gtgl_debugging_generic_h_

#include "../gtgl_errors.h"
#include "../gtgl_utils.h"
#include "../gtgl_context.h"
#include "../emulators/gtgl_arb_debug_output.h"

extern PFNGLGETERRORPROC              __native_glGetError;
extern PFNGLDEBUGMESSAGEINSERTARBPROC __native_glDebugMessageInsertARB;

/* Always use __native_glGetError() for error checking! */
#define GTGL_DEBUG_CHECK_ERROR(function) \
    { \
        GLenum error = __native_glGetError(); \
        if (error != GL_NO_ERROR) \
        { \
            char buf[1024]; \
            GLsizei length = gtgl_snprintf(buf, 1024, "%s in %s.", gtglGetGLErrorString(error), #function); \
            \
            __native_glDebugMessageInsertARB(GL_DEBUG_SOURCE_API_ARB, GL_DEBUG_TYPE_ERROR_ARB, 0, GL_DEBUG_SEVERITY_HIGH_ARB, length, buf); \
        } \
    }

#define GTGL_DEBUG_BEGIN_CALL(function) \
    PFNGTGLOPENGLCALLCALLBACKPROC GLCallCallback = gtglGetGLCallCallback(); \
    if (GLCallCallback != NULL) \
    { \
        GLCallCallback(#function); \
    }

#define GTGL_DEBUG_END_CALL(function) \
    GTGL_DEBUG_CHECK_ERROR(function);


#define GTGL_DEBUG_END_CALL_NO_EMU(function)


#define GTGL_DEBUG_IMPL(function, args) \
    if (GTGL_MasterContext->ARB_debug_output.emulating) \
    { \
        GTGL_DEBUG_BEGIN_CALL(function); \
        __native_glGetError(); \
        __native_##function args; \
        GTGL_DEBUG_END_CALL(function); \
    } \
    else \
    { \
        GTGL_DEBUG_BEGIN_CALL(function); \
        __native_##function args; \
        GTGL_DEBUG_END_CALL_NO_EMU(function); \
    } \

#define GTGL_DEBUG_IMPL_RETURN(returnType, function, args) \
    returnType result; \
    if (GTGL_MasterContext->ARB_debug_output.emulating) \
    { \
        GTGL_DEBUG_BEGIN_CALL(function); \
        __native_glGetError(); \
        result = __native_##function args; \
        GTGL_DEBUG_END_CALL(function); \
    } \
    else \
    { \
        GTGL_DEBUG_BEGIN_CALL(function); \
        result = __native_##function args; \
        GTGL_DEBUG_END_CALL_NO_EMU(function); \
    } \
    return result;

#endif