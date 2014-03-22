/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_context_h_
#define __gtgl_context_h_

#include <gtgl/gtgl.h>     /* For OpenGL types. */

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#include <gtgl/GL/gtwgl_windef.h>
#include <wingdi.h>
#elif defined(__linux__)
#include <gtgl/GL/glx.h>
#elif defined(__APPLE__) && defined(__MACH__)
#endif

struct _ARB_debug_output_Message
{
    GLenum  source;
    GLenum  type;
    GLenum  severity;
    GLuint  id;
    GLsizei length;
    GLchar* msg;
};

struct _ARB_debug_output
{
    GLboolean emulating;

    GLDEBUGPROCARB DEBUG_CALLBACK_FUNCTION_ARB;
    const GLvoid*  DEBUG_CALLBACK_USER_PARAM_ARB;
    GLushort       DEBUG_LOGGED_MESSAGES_ARB;
    GLushort       DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB;
    GLboolean      DEBUG_OUTPUT_SYNCHRONOUS_ARB;
    GLushort       MAX_DEBUG_MESSAGE_LENGTH_ARB;
    GLushort       MAX_DEBUG_LOGGED_MESSAGES_ARB;

    GLboolean messageStates[6][6][3];

    struct _ARB_debug_output_Message messageLog[64];

    GLsizei nextMessageIndex;
};


struct _GTGLcontext
{
    unsigned int versionMajor;
    unsigned int versionMinor;

    GLboolean singleBuffered;
    GLboolean debug;
    GLboolean forwardCompatible;
    GLboolean es2Compatible;

    int emulateDebugOutput;

    /** The master context. */
    struct _GTGLcontext *master;

    /** The ID of the thread this context is current on. This will be 0 if the context is not current on anything. */
    int threadID;

    /** The debug message callback. */
    PFNGTGLDEBUGMESSAGECALLBACKPROC debugMessageCallback;
    PFNGTGLOPENGLCALLCALLBACKPROC   openGLCallCallback;


    /** Structure containing the ARB_debug_output state for the emulator. */
    struct _ARB_debug_output ARB_debug_output;



#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
    /** The HWND that was used to create the context. */
    HWND DummyHWND;

    /** The device context of the window that is currently being used as the render target. This is not necessarily the device context of DummyHWND. */
    HDC CurrentDC;

    /** The OpenGL rendering context. This is constant. */
    HGLRC RC;

    /** The pixel format to use with windows that will use this rendering context. */
    int PixelFormat;

    /** The pixel format descriptor associated with PixelFormat. */
    PIXELFORMATDESCRIPTOR PFD;
#elif defined(__linux__)
    /** The X display being used by this context. */
    Display* XDisplay;

    /** The window that was used to create the context. */
    Window DummyWindow;

    /** The window that rendering operations are currently taking place on. */
    Window CurrentWindow;

    /** The OpenGL rendering context. */
    GLXContext RC;

    /** The colour map to use when creating windows. */
    Colormap SharedColormap;

    /** The GLX framebuffer configuration. We derivce the visual info from this object. */
    GLXFBConfig FBConfig;

    /** The visual to use when creating windows. */
    XVisualInfo *FBVisualInfo;

    /** Keeps track of whether or not the Display was created externally. When this is true, we don't want to delete the display. */
    GLboolean UsingExternalDisplay;
#endif

};

#ifndef __gtgl_h_
typedef struct _GTGLcontext* GTGLcontext;
#endif

extern GTGLcontext GTGL_MasterContext;       /* Will be declared properly in gtgl_context.c */


/** GTGLcontext constructor. Platform dependant. */
void GTGLcontext_ctor(GTGLcontext self, size_t attribs[], GTGLcontext master);

/** GTGLcontext destructor. Platform dependant. */
void GTGLcontext_dtor(GTGLcontext self);




/**** Platform Independant GTGLcontext Methods ****/

/** Creates a new GTGLcontext on the heap. */
GTGLcontext GTGLcontext_new(size_t attribs[], GTGLcontext master);

/** Deletes an GTGLcontext created with GTGLcontext_new(). */
void GTGLcontext_delete(GTGLcontext self);


/**
*   \brief  GTGLcontext constructor for the common attributes.
*/
void GTGLcontext_ctor_base(GTGLcontext self);

/**
*   \brief  GTGLcontext destructor for shared attributes. Called by platform-dependant GTGLcontext_dtor().
*/
void GTGLcontext_dtor_base(GTGLcontext self);

/**
*   \brief  Sets the attributes of the context based on the given attributes.
*/
void GTGLcontext_SetAttributes(GTGLcontext self, size_t attribs[]);

/**
*   \brief  Sets the attributes of the context based on the attributes of the given context.
*/
void GTGLcontext_CopyAttributes(GTGLcontext self, GTGLcontext other);



#endif
