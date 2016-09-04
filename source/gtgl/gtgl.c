/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>
#include <stdlib.h>
#include <string.h>

static PFNGLGETSTRINGPROC __local_glGetString_ = NULL;

int gtglInitGLAPI(GLboolean debugMode)
{
    if (gtglGetCurrentPlatformContext() != NULL)
    {
        gtglInitGLAPI_GL(debugMode);

    #ifdef GTGL_WGL
        gtglInitGLAPI_WGL(debugMode);
    #else
        gtglInitGLAPI_GLX(debugMode);
    #endif

        return 1;
    }

    return 0;
}



int gtglIsVersionSupported(unsigned int major, unsigned int minor)
{
    unsigned int actualMajor = 0;
    unsigned int actualMinor = 0;
    gtglGetOpenGLVersion(&actualMajor, &actualMinor);

    if (major < actualMajor)
    {
        return 1;
    }
    if (major > actualMajor)
    {
        return 0;
    }

    /* If we have made it here, major == actualMajor. */
    return minor <= actualMinor;
}

void gtglGetOpenGLVersion(unsigned int *major, unsigned int *minor)
{
    if (__local_glGetString_ == NULL)
    {
        __local_glGetString_ = (PFNGLGETSTRINGPROC)gtglGetProcAddress("glGetString");
    }

    {
        const char * majorStart = (const char *)__local_glGetString_(GL_VERSION);
              char * minorStart;

        *major = strtoul(majorStart, &minorStart, 0);
        *minor = strtoul(minorStart + 1, NULL, 0);
    }
}


const char* gtglGetFramebufferStatusString(GLenum status)
{
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:                      return "GL_FRAMEBUFFER_COMPLETE";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";

    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:     return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT";
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:        return "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT";

    default: break;
    }

    return "Unknown";
}
