
/**
*   \file   gtgl.h
*/

/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */


#ifndef __gtgl_h_
#define __gtgl_h_

#include <stddef.h>

/* Platform. */
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
    #define GTGL_WGL
#elif defined(__linux__)
    #define GTGL_GLX
#elif defined(__APPLE__) && defined(__MACH__)
    #define GTGL_AGL
#endif


#include "gtgl_gl.h"

/* Generic platform-specific context handle */
#if   defined(GTGL_WGL)
typedef HGLRC       GTGLplatformcontext;
#elif defined(GTGL_GLX)
typedef GLXContext  GTGLplatformcontext;
#endif


/* Context attributes. */
#define GTGL_CONTEXT_MAJOR_VERSION                   0x01        /* The major OpenGL version. */
#define GTGL_CONTEXT_MINOR_VERSION                   0x02        /* The minor OpenGL version. */
#define GTGL_CONTEXT_SINGLE_BUFFER                   0x03        /* 0 or 1. Whether or not double buffering should be enabled. This is false by default, meaning double-buffering is enabled by default. */
#define GTGL_CONTEXT_DEBUG                           0x04        /* 0 or 1. Whether or not debugging should be enabled. */
#define GTGL_CONTEXT_FORWARD_COMPATIBLE              0x05        /* 0 or 1. For OpenGL 3.0+. Controls whether or not a forward-compatible context should be created. */
#define GTGL_CONTEXT_ES2_COMPATIBLE                  0x06        /* 0 or 1. Whether or not to create an ES2 compatible context. Can not be used with GTGL_CONTEXT_FORWARD_COMPATIBLE. Version must be either unspecified or 2.0. */
#define GTGL_CONTEXT_EMULATE_DEBUG_OUTPUT            0x07        /* 1 or GTGL_FORCE. Controls whether or not ARB_debug_output should be emulated. ARB_debug_output is used for GTGL error handling, so setting this to 0 is not valid. Defaults to 1. */
/*#define GTGL_CONTEXT_EMULATE_VERTEX_ARRAY_OBJECT     0x08*/        /* 0, 1 or GTGL_FORCE. Controls whether or not VAO's should be emulated */

#ifdef GTGL_GLX
#define GTGL_CONTEXT_DISPLAY                         0x08        /* Value should be a pointer to the display to use with the context. */
#endif

#define GTGL_FALSE                                   0
#define GTGL_TRUE                                    1
#define GTGL_FORCE                                   2

/* Error codes */
#define GTGL_ERROR_CONTEXT                           0x1         /* Error creating a context. */
#define GTGL_ERROR_OPENGL                            0x2         /* OpenGL error. */

typedef struct _GTGLcontext* GTGLcontext;

#ifndef __gtgl_window_h_
typedef struct _GTGLwindow*  GTGLwindow;
typedef void(* OnCloseWindowProc) (GTGLwindow window);
typedef void(* OnSizeWindowProc)  (GTGLwindow window, unsigned int newWidth, unsigned int newHeight);
typedef void(* OnMoveWindowProc)  (GTGLwindow window, int newXPos, int newYPos);
#endif


struct _GTGLerror
{
    char* function;
    char* message;
    int   code;
};
typedef struct _GTGLerror* GTGLerror;

typedef void(APIENTRYP GTGLPROC)();
typedef void(* PFNGTGLERRORCALLBACKPROC)        (GTGLerror error);
typedef void(* PFNGTGLDEBUGMESSAGECALLBACKPROC) (const char* message);
typedef void(* PFNGTGLOPENGLCALLCALLBACKPROC)   (const char* function);


/* This group of functions are platform-specific. They will be implemented in gtgl_wgl.c or gtgl_glx.c, depending on the platform. */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
    *   \brief               Initialises GTGL and returns the main context.
    *   \param  attribs [in] The properties of the main context. See remarks.
    *   \return              A handle to the main context, or null if the context could not be created.
    *
    *   \remarks
    *       This should always be called before doing any OpenGL rendering operations. It is OK to call gtglSetErrorCallback() before
    *       calling this function, which is useful if you prefer doing callback-based error handling.
    *       \par
    *       The return value is a handle to the main context. There can only be one main context per application. This is the only function
    *       that can generate the main context, and it will fail if it is called more than once. Additional worker contexts can be created
    *       with gtglCreateContext(). Each context can be made current on it's own thread with gtglMakeCurrent().
    *       \par
    *       The \c attribs argument controls the various properties to apply to the main context. It is a key/value array just like the arrays
    *       used by glXChooseFBConfig() and ARB_create_context. Each key must have an associated value, even booleans. In the case of booleans,
    *       it is not OK to simply have the key with no value. All boolean properties default to false. This array must be double null-terminated.
    *       Below is an example array for creating an OpenGL 2.1, double-buffered debug context:
    *       \code
    *       size_t attribs[] =
    *       {
    *           GTGL_CONTEXT_MAJOR_VERSION, 2,
    *           GTGL_CONTEXT_MINOR_VERSION, 1,
    *           GTGL_CONTEXT_DEBUG,         GL_TRUE,
    *           0, 0
    *       };
    *       \endcode
    *       Note how every property has a value associated with it.
    *       \section Context Attributes
    *       GTGL_CONTEXT_MAJOR_VERSION        (Integer)  - The major component of the OpenGL version to use.
    *       GTGL_CONTEXT_MINOR_VERSION        (Integer)  - The minor component of the OpenGL version to use.
    *       GTGL_CONTEXT_SINGLE_BUFFER        (Boolean)  - Whether or not single buffering should be used. If this is omitted double-buffering will be used.
    *       GTGL_CONTEXT_DEBUG                (Boolean)  - Whether or not to support debugging. Required for ARB_debug_output.
    *       GTGL_CONTEXT_FORWARD_COMPATIBLE   (Boolean)  - Whether or not to enable backwards compatibility.
    *       GTGL_CONTEXT_ES2_COMPATIBLE       (Boolean)  - Whether or not to enable OpenGL|ES 2.0 compatibility. Can not be combined with GTGL_CONTEXT_FORWARD_COMPATIBLE (GTGL_CONTEXT_FORWARD_COMPATIBLE will take precedence).
    *       GTGL_CONTEXT_DISPLAY              (Display*) - X11 Specific. A pointer to the display to use with the context. Use this if the application's display has already been opened.
    *       \endsection
    *       Keep in mind that a debug context has fairly significant CPU overhead. Do not enable debugging in release builds. If you want to
    *       use ARB_debug_output, you must use GTGL_CONTEXT_DEBUG. This will also enable the use of GTGL error detection.
    *       \par
    *       There are a few rules to follow when using OpenGL|ES 2.0 compatibility (GTGL_CONTEXT_ES2_COMPATIBLE). First, it can not be combined
    *       with GTGL_CONTEXT_FORWARD_COMPATIBLE. If it is, GTGL_CONTEXT_FORWARD_COMPATIBLE will take precendence. Second, the version must be left
    *       unspecified, or set as GTGL_CONTEXT_MAJOR_VERSION = 2 and GTGL_CONTEXT_MINOR_VERSION = 0. OpenGL 2.0, in other words. Lastly,
    *       WGL/GLX_EXT_create_context_es2_profile must be supported.
    *       \par
    *       gtglStartup() will return null if the version requested in \c attribs is no supported by the OpenGL implementation.
    *       \par
    *       Use gtglShutdown() to uninitialise GTGL.
    */
    GTGLcontext gtglStartup(size_t attribs[]);

    /**
    *   \brief  Uninitialises GTGL.
    *
    *   \remarks
    *       X11 Note. Remember to consider that if you omitted GTGL_CONTEXT_DISPLAY in gtglStartup(), gtglShutdown() will close the internal X11 Display. Make
    *       sure you delete any windows and other X11 objects before shutting down. If, however, you created the display yourself and passed it to GTGL with
    *       the GTGL_CONTEXT_DISPLAY option, be sure to shutdown GTGL AFTER closing the display.
    */
    void gtglShutdown();

    /**
    *   \brief  Creates a worker/slave context. See remarks.
    *   \return A handle to the new context.
    *
    *   \remarks
    *       A slave context uses the same settings as the master context and also shares it's resources. It can be used as a way
    *       to do multi-threading.
    *       \par
    *       Note that the returned context will not be made current on the calling thread. It must be explicitly made current.
    *       \par
    *       Destroy the returned context with gtglDeleteContext() when it is no longer needed.
    *       \par
    *       gtglCreateContext() and gtglDeleteContext() are NOT thread safe. Ensure no OpenGL calls are currently being executed during
    *       creation and deletion of context's. In practice, this should not be an issue for most applications, but it's something to
    *       be aware of.
    */
    GTGLcontext gtglCreateContext();

    /**
    *   \brief               Deletes a context that was previously created with gtglCreateContext().
    *   \param  context [in] The context to delete.
    *
    *   \remarks
    *       Don't call this function on the main context returned by gtglStartup(). Use gtglShutdown() for that.
    *       \par
    *       gtglCreateContext() and gtglDeleteContext() are NOT thread safe. Ensure no OpenGL calls are currently being executed during
    *       creation and deletion of context's. In practice, this should not be an issue for most applications, but it's something to
    *       be aware of.
    */
    void gtglDeleteContext(GTGLcontext context);

    /**
    *   \brief               Sets the current context for the calling thread.
    *   \param  context [in] The context to make current on the calling thread.
    *
    *   \remarks
    *       A single context can be current on each thread. When calling this function, the given context is made current on the calling thread.
    *       \par
    *       You can use this function for any context, including the main context returned by gtglStartup().
    */
    void gtglMakeCurrent(GTGLcontext context);

    /**
    *   \brief  Retrieves the current context on the calling thread.
    */
    GTGLcontext gtglGetCurrentContext();

    /**
    *   \brief  Retrieves a handle to the current platform-specific context (HGLRC/GLXContext).
    *   \return A handle to the platform-specific context.
    *
    *   \remarks
    *       GTGLplatformcontext can be cast to a HGLRC or GLXContext.
    */
    GTGLplatformcontext gtglGetCurrentPlatformContext();


#if defined(GTGL_WGL)
    /**
    *   \brief           Windows-specific: Sets the device context where drawing should take place on.
    *   \param  hDC [in] The handle to the device context that rendering should take place on.
    */
    void gtglSetCurrentDC(HDC hDC);

    /**
    *   \brief  Windows-specific: Retrieves the pixel format of the main context.
    *   \return The pixel format to use on the device contexts that need to support OpenGL rendering via GTGL.
    *
    *   \remarks
    *       This function is required for the Win32 SetPixelFormat() function. You will also need a pointer to the pixel format descriptor,
    *       which can be retrieved with gtglGetPFD().
    */
    int gtglGetPixelFormat();

    /**
    *   \brief  Windows-specific: Retrieves a pointer to the pixel format descriptor of the main context.
    *
    *   \remarks
    *       Use this with Win32's SetPixelFormat().
    */
    const struct tagPIXELFORMATDESCRIPTOR* gtglGetPFD();

#elif defined(GTGL_GLX)
    /**
    *   \brief  Sets the window where drawing should take place on.
    */
    void gtglSetCurrentWindow(Window window);

    /**
    *   \brief  Retrieves the display that can be used when creating windows.
    */
    Display* gtglGetDisplay();

    /**
    *   \brief  Retrieves the visual info that can be used when creating windows.
    */
    XVisualInfo* gtglGetVisualInfo();

    /**
    *   \brief  Retrieves the colormap that can be used when creating windows.
    */
    Colormap gtglGetColormap();
#elif defined(GTGL_AGL)
#endif


    /**
    *   \brief  A helper function for making an GTGLwindow the current target where rendering operations should take place on.
    */
    void gtglSetCurrentGTGLWindow(GTGLwindow window);


    /**
    *   \brief  Sets the swap interval for framebuffer swaps. This controls vsync. To disable vsync, pass 0. Otherwise, pass 1.
    *
    *   \remarks
    *       <interval> can also be -1, in which case GTGL will use the GLX/WGL_EXT_swap_control_tear extension. This will tear
    *       on missed frames, but keep all the others synchronized. If the extension is unavailable, it will simply use the
    *       absolute value of <interval> instead.
    */
    void gtglSwapInterval(int interval);

    /**
    *   \brief            Swaps the drawing buffers. Only valid with double-buffering enabled.
    *   \param  sync [in] Whether or not to use v-sync.
    *
    *   \remarks
    *       This must be called on the thread that has the master context.
    */
    void gtglSwapBuffers();

    /**
    *   \brief  Retrieves the address of an OpenGL procedure.
    *
    *   \remarks
    *       This is just a wrapper around wglGetProcAddress or glXGetProcAddressARB, depending on the platform.
    */
    GTGLPROC gtglGetProcAddress(const char* procName);




#ifdef __cplusplus
}
#endif

/* This group of functions are cross-platform functions. */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
    *   \brief  Initialises the flat OpenGL API, including WGL and GLX where applicable.
    *
    *   \remarks
    *       This will also initialize extensions. Everything is initialized based on the current OpenGL context.
    *       \par
    *       OpenGL 3+ forward/core contexts do not include extension strings for features that are now part of core. Take note of this,
    *       because gtglIsExtensionSupported() will only work for extensions that were retrieved with glGetStringi(GL_EXTENSIONS) for
    *       OpenGL 3+.
    */
    int gtglInitGLAPI(GLboolean debugMode);

    /**
    *   \brief             Checks if the given OpenGL version is supported.
    *   \param  major [in] The major OpenGL version.
    *   \param  minor [in] The minor OpenGL version.
    *
    *   \remarks
    *       An OpenGL context needs to be current before calling this function. It is not required that gtglInitFlatAPI() has been called
    *       beforehand.
    */
    int gtglIsVersionSupported(unsigned int major, unsigned int minor);

    /**
    *   \brief  Helper function for retrieving the OpenGL version.
    *
    *   \remarks
    *       An OpenGL context needs to be current before calling this function, but it's not required that gtglInitFlatAPI() has been called
    *       beforehand.
    */
    void gtglGetOpenGLVersion(unsigned int* major, unsigned int* minor);


    /**
    *   \brief                 Checks if an extension is supported based on the current context.
    *   \param  extension [in] The extension string to check.
    *   \return                True if the extension is supported, false otherwise.
    *
    *   \remarks
    *       It is highly recommended that applications use the GTGL_* variables for extension detection (GTGL_ARB_fragment_program, etc).
    *       \par
    *       This function is intentionally not thread safe. It has been optimized for calling on a single thread from the same context, which
    *       shouldn't be too restrictive for most applications. Calling this function across different contexts causes the internal cache to be
    *       regenerated, thus slowing down extension retrieval.
    */
    int gtglIsExtensionSupported(const char* extension);


    /**
    *   \brief              Returns a stringified framebuffer status code.
    *   \param  status [in] The framebuffer status code as returned by glCheckFramebufferStatus/EXT().
    *   \return             <status> as a string.
    */
    const char* gtglGetFramebufferStatusString(GLenum status);



#ifdef __cplusplus
}
#endif


/* Error handling. These are implemented in gtgl_errors.c */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
    *   \brief  Retrieves a pointer to the last error.
    *
    *   \remarks
    *       The returned pointer is to a GTGLerror struct. You retrieve information about the error from the members of that struct. Remember, if there are
    *       no errors, this function will return null. Always check the return value.
    *       \par
    *       The returned pointer will be invalidated when the next error is posted, so ensure that you get everything done before calling the next GTGL function.
    */
    GTGLerror gtglGetLastError();

    /**
    *   \brief  Retrieves the message of the last error, or NULL if there are no errors.
    *
    *   \remarks
    *       This will also clear the last error. If you need additional information about the error, such as the error code, use gtglGetLastError() instead.
    */
    const char* gtglGetLastErrorMessage();


    /**
    *   \brief  Sets the error callback for GTGL errors. This is NOT the callback for OpenGL errors.
    */
    void gtglSetErrorCallback(PFNGTGLERRORCALLBACKPROC callback);



    /**
    *   \brief  Retrieves the stringified version of the given GTGL error code.
    */
    const char* gtglGetErrorCodeString(int error);

    /**
    *   \brief             Converts an OpenGL error code to a string.
    *   \param  error [in] The OpenGL error to stringify.
    *
    *   \remarks
    *       This function will return values such as GL_INVALID_ENUM.
    */
    const char* gtglGetGLErrorString(GLenum error);


#ifdef __cplusplus
}
#endif


/* Error handling. These are implemented in gtgl_debugging_common.c */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
    *   \brief                Sets the debug message callback routine for the current context on the calling thread.
    *   \param  callback [in] The callback routine to call when a debug message is received.
    *
    *   \remarks
    *       Setting <callback> to anything other than null will override the current callback used in ARB_debug_output. This is due
    *       to the fact that GTGL uses ARB_debug_output for it's error submission.
    *       \par
    *       The <message> argument of <callback> will use the same formatting as generated by gtglFormatDebugMessage().
    *       \par
    *       This function operates on a per-context basis meaning you must call this on every context that wants debug message callbacks.
    */
    void gtglSetDebugMessageCallback(PFNGTGLDEBUGMESSAGECALLBACKPROC callback);

    /**
    *   \brief  Retrieves the debug message callback routing for the current context on the calling thread.
    */
    PFNGTGLDEBUGMESSAGECALLBACKPROC gtglGetDebugMessageCallback();


    /**
    *   \brief  Retrieves the callback routine for OpenGL call logging.
    */
    PFNGTGLOPENGLCALLCALLBACKPROC gtglGetGLCallCallback();

    /**
    *   \brief                Sets the callback routine for OpenGL call logging.
    *   \param  callback [in] The callback routine to call when an OpenGL call is made.
    *
    *   \remarks
    *       If \c callback is NULL, OpenGL call logging will be disabled.
    *       \par
    *       This callback will be called just before the actual call. This allows the application to log the call before a crash, for example.
    *       Use gtglSetGLPostCallCallback() to set a callback that will be called after the call.
    */
    void gtglSetGLCallCallback(PFNGTGLOPENGLCALLCALLBACKPROC callback);


    /**
    *   \brief                  Formats a message string from the data used by ARB_debug_output.
    *   \param  buffer     [in] A pointer to the buffer that will receive the formatted message.
    *   \param  bufferSize [in] The size of <buffer>.
    *   \param  source     [in] The <source> argument used in ARB_debug_output.
    *   \param  type       [in] The <type> argument used in ARB_debug_output.
    *   \param  id         [in] The ID of the message as used in ARB_debug_output.
    *   \param  severity   [in] The <severity> argument used in ARB_debug_output.
    *   \param  message    [in] The message outputted from ARB_debug_output.
    *   \return                 The number of characters written to the buffer, or if <buffer> is null and <bufferSize> is 0, returns the size of the buffer required to store the formatted string.
    */
    int gtglFormatDebugMessage(char* buffer, size_t bufferSize, GLenum source, GLenum type, GLuint id, GLenum severity, const GLchar* message);


    /**
    *   \brief  Enables synchronous debug output.
    *
    *   \remarks
    *       This is just a wrapper around glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB).
    */
    void gtglEnableSynchronousDebugOutput();

    /**
    *   \brief  Disables synchronous debug output.
    *
    *   \remarks
    *       This is just a wrapper around glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB).
    */
    void gtglDisableSynchronousDebugOutput();


#ifdef __cplusplus
}
#endif



/* Window management */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
    *   \brief  Creates an application window, but does not show it.
    */
    GTGLwindow gtglCreateWindow(const char* title, int xPos, int yPos, unsigned int width, unsigned int height);

    /**
    *   \brief  Destroys a window created with gtglCreateWindow().
    */
    void gtglDeleteWindow(GTGLwindow window);

    /**
    *   \brief  Shows a window.
    */
    void gtglShowWindow(GTGLwindow window);

    /**
    *   \brief  Hides a window.
    */
    void gtglHideWindow(GTGLwindow window);

    /**
    *   \brief  Sets the function that will be called when the window is closed, such as when the 'X' button is hit.
    */
    void gtglWindowOnClose(GTGLwindow window, OnCloseWindowProc proc);

    /**
    *   \brief  Sets the function that will be called after the window has changed size.
    */
    void gtglWindowOnSize(GTGLwindow window, OnSizeWindowProc proc);

    /**
    *   \brief  Sets the function that will be called after the window has moved.
    */
    void gtglWindowOnMove(GTGLwindow window, OnMoveWindowProc proc);


    /**
    *   \brief              Sets a property on a window.
    *   \param  window [in] The window whose property is being set.
    *   \param  name   [in] The name of the property.
    *   \param  value  [in] The value of the property.
    */
    void gtglWindowSetProp(GTGLwindow window, const char* name, void* value);

    /**
    *   \brief              Retrieves a property on a window.
    *   \param  window [in] The window whose property is being retrieved.
    *   \param  name   [in] The name of the property to retrieve.
    *
    *   \return The value of the given property, or null if the property does not exist.
    */
    void* gtglWindowGetProp(GTGLwindow window, const char* name);



#if defined(GTGL_WGL)

    /**
    *   \brief  Retrieves the Win32 HWND object of the given window.
    */
    HWND gtglGetWindowHWND(GTGLwindow window);

    /**
    *   \brief  Retrieves the Win32 HDC object of the given window.
    */
    HDC gtglGetWindowHDC(GTGLwindow window);

#elif defined(GTGL_GLX)

    /**
    *   \brief  Retrieves the X11 Window object of the given window.
    */
    Window gtglGetWindowX11Window(GTGLwindow window);

#endif


    /** Processes any pending window messages. This is non-blocking. */
    void gtglPumpAllPendingWindowMessages();


#ifdef __cplusplus
}
#endif


#endif
