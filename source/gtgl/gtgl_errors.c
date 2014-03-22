/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_errors.h"
#include "gtgl_utils.h"
#include "gtgl_context.h"
#include <gtgl/gtgl.h>
#include <stdlib.h>

/**
*   \brief                        Constructs a GTGLerror object.
*   \param  function         [in] The name of the function that posted the error.
*   \param  errorcode        [in] The error code.
*   \param  formattedMessage [in] The formatted error message.
*
*   \remarks
*       \c formattedMessage should have already been pre-allocated before calling this function. It will not be copied. It will be freed
*       in GTGLerror_delete.
*/
GTGLerror GTGLerror_new(const char *function, int errorcode, char *formattedMessage)
{
    GTGLerror self = (GTGLerror)malloc(sizeof(struct _GTGLerror));

    self->function = (char *)malloc(strlen(function) + 1);
    gtgl_strcpy(self->function, function);

    self->code    = errorcode;
    self->message = formattedMessage;

    return self;
}

/**
*   \brief            Deletes a GTGLerror object that was previously created with GTGLerror_new().
*   \param  self [in] The error to delete.
*/
void GTGLerror_delete(GTGLerror self)
{
    if (self != NULL)
    {
        free(self->function);
        free(self->message);

        free(self);
    }
}


/** The last error message. This will be returned by gtglGetLastError() and set with gtglPostError(). */
GTGLerror GTGL_LastError = NULL;

/**
*   gtglGetLastError() uses the notion of "clearing" the last error. However, it needs to return a pointer to that last error, so actually
*   freeing the error in gtglGetLastError() is not an option. To resolve, we keep track of whether or not the error is "valid".
*/
GLboolean GTGL_IsLastErrorValid = GL_FALSE;

/** A pointer to the error callback function. */
PFNGTGLERRORCALLBACKPROC GTGL_ErrorCallback = NULL;


void gtglPostError(const char *function, int errorcode, const char *format, ...)
{
    size_t bufferSize;
    char * buffer;

    va_list argList1;
    va_list argList2;
    va_start(argList1, format);
    va_start(argList2, format);
    {
        bufferSize = (size_t)gtgl_vsnprintf(NULL, 0, format, argList1) + 1;
        buffer     = (char *)malloc(bufferSize);

        gtgl_vsnprintf(buffer, bufferSize, format, argList2);

        GTGLerror_delete(GTGL_LastError);
        GTGL_LastError = GTGLerror_new(function, errorcode, buffer);

        if (GTGL_ErrorCallback != NULL)
        {
            GTGL_ErrorCallback(GTGL_LastError);
        }

        GTGL_IsLastErrorValid = GL_TRUE;
    }
    va_end(argList1);
    va_end(argList2);
}

void gtglShutdownErrors()
{
    GTGLerror_delete(GTGL_LastError);
    GTGL_LastError = NULL;
}

GTGLerror gtglGetLastError()
{
    if (GTGL_IsLastErrorValid)
    {
        GTGL_IsLastErrorValid = GL_FALSE;
        return GTGL_LastError;
    }

    return NULL;
}

const char * gtglGetLastErrorMessage()
{
    GTGLerror error = gtglGetLastError();
    if (error != NULL)
    {
        return error->message;
    }

    return NULL;
}

void gtglSetErrorCallback(PFNGTGLERRORCALLBACKPROC callback)
{
    GTGL_ErrorCallback = callback;
}



const char * gtglGetErrorCodeString(int error)
{
    switch (error)
    {
    case GTGL_ERROR_CONTEXT: return "GTGL_ERROR_CONTEXT";
    case GTGL_ERROR_OPENGL:  return "GTGL_ERROR_OPENGL";

    default: return "Unknown Error";
    }
}

const char * gtglGetGLErrorString(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:           return "GL_NO_ERROR";
        case GL_INVALID_ENUM:       return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:      return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:  return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:      return "GL_OUT_OF_MEMORY";

        default: return "Unknown Error";
    }
}

int gtglFormatDebugMessage(char* buffer, size_t bufferSize, GLenum source, GLenum type, GLuint id, GLenum severity, const GLchar* message)
{
    const char* sourceStr;
    const char* typeStr;
    const char* severityStr;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API_ARB:               sourceStr = "OpenGL";           break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     sourceStr = "Window System";    break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   sourceStr = "Shader Compiler";  break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:       sourceStr = "Application";      break;
    case GL_DEBUG_SOURCE_OTHER_ARB:             sourceStr = "Other";            break;

    default: sourceStr = "Unknown"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR_ARB:               typeStr = "Error";                  break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeStr = "Deprecated Behaviour";   break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  typeStr = "Undefined Behavious";    break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:         typeStr = "Portability";            break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:         typeStr = "Performance";            break;
    case GL_DEBUG_TYPE_OTHER_ARB:               typeStr = "Message";                break;

    default: typeStr = "Unknown"; break;
    }
 
	switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_ARB:   severityStr = "High";   break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityStr = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW_ARB:    severityStr = "Low";    break;

    default: severityStr = "Unknown"; break;
    }

    return gtgl_snprintf(buffer, bufferSize,
        "%s\n"
        "    ID:       %u\n"
        "    Source:   %s\n"
        "    Type:     %s\n"
        "    Severity: %s",
        message, id, sourceStr, typeStr, severityStr);
}

void gtglEnableSynchronousDebugOutput()
{
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
}

void gtglDisableSynchronousDebugOutput()
{
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
}
