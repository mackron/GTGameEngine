/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>

/* We only continue if we're on a GLX platform. */
#ifdef GTGL_GLX

#include "../gtgl_context.h"
#include "../gtgl_contextmanager.h"
#include "../gtgl_extensions.h"
#include "../gtgl_window.h"
#include "../gtgl_errors.h"
#include "../gtgl_utils.h"
#include "../emulators/gtgl_arb_debug_output.h"
#include <stdlib.h>

/**
*   \brief  Checks if GTGL_CONTEXT_DISPLAY is specified in the given attribute array and returns the value. Returns null if it is not specified.
*/
Display* gtglCheckAttribsForDisplay(size_t attribs[])
{
    if (attribs != NULL)
    {
        while (!(attribs[0] == 0 && attribs[1] == 0))
        {
            size_t key   = attribs[0];
            size_t value = attribs[1];

            if (key == GTGL_CONTEXT_DISPLAY)
            {
                return (Display *)value;
            }

            attribs += 2;
        }
    }

    return NULL;
}

void GTGLcontext_CreateDummyWindow(GTGLcontext self);
void GTGLcontext_DeleteDummyWindow(GTGLcontext self);
GLboolean GTGLcontext_InitLegacyContext(GTGLcontext self);
GLboolean GTGLcontext_EnableExtendedContext(GTGLcontext self);

void GTGLcontext_ctor(GTGLcontext self, size_t attribs[], GTGLcontext master)
{
    GTGLcontext_ctor_base(self);

    self->DummyWindow   = 0;
    self->CurrentWindow = 0;
    self->RC            = NULL;

    if (master != NULL)
    {
        self->master = master;
        GTGLcontext_CopyAttributes(self, master);

        self->XDisplay             = master->XDisplay;
        self->SharedColormap       = master->SharedColormap;
        self->FBConfig             = master->FBConfig;
        self->FBVisualInfo         = master->FBVisualInfo;
        self->UsingExternalDisplay = master->UsingExternalDisplay;

        self->ARB_debug_output.emulating = master->ARB_debug_output.emulating;
    }
    else
    {
        self->master = NULL;
        GTGLcontext_SetAttributes(self, attribs);

        /* Here we need to check if an external Display was specified in the attributes. If so, we need to use that one. */
        self->XDisplay = gtglCheckAttribsForDisplay(attribs);
        if (self->XDisplay != NULL)
        {
            self->UsingExternalDisplay = GL_TRUE;
        }
        else
        {
            self->XDisplay             = XOpenDisplay(NULL);
            self->UsingExternalDisplay = GL_FALSE;
        }

        self->SharedColormap = 0;
        self->FBConfig       = NULL;
        self->FBVisualInfo   = NULL;
    }


    /*
    If we don't yet have any visual info we'll need to grab one. Make sue we check agains FBVisualInfo and not FBConfig because FBConfig
    will always be NULL when a legacy context is being used (see below).
    */
    if (self->FBVisualInfo == NULL)
    {
        const GLboolean singleBuffered = (const GLboolean)self->singleBuffered;

        int attribs[] =
        {
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_X_RENDERABLE,  True,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RENDER_TYPE,   GLX_RGBA_BIT,
            GLX_RED_SIZE,      8,
            GLX_GREEN_SIZE,    8,
            GLX_BLUE_SIZE,     8,
            GLX_ALPHA_SIZE,    8,
            GLX_DEPTH_SIZE,    24,
            GLX_STENCIL_SIZE,  8,
            GLX_DOUBLEBUFFER,  (int)!singleBuffered,
            None
        };

        GLXFBConfig* fbConfigs = NULL;
        int          fbCount   = 0;

        fbConfigs = gtglXChooseFBConfig(self->XDisplay, DefaultScreen(self->XDisplay), attribs, &fbCount);
        if (fbConfigs != NULL && fbCount > 0)
        {
            self->FBConfig = fbConfigs[0];

            self->FBVisualInfo = gtglXGetVisualFromFBConfig(self->XDisplay, self->FBConfig);
            if (self->FBVisualInfo != NULL)
            {
                self->SharedColormap = XCreateColormap(self->XDisplay, RootWindow(self->XDisplay, self->FBVisualInfo->screen), self->FBVisualInfo->visual, AllocNone);
            }
            else
            {
                /*
                *   As of 30/11/2011, glXGetVisualFromFBConfig() does not work properly with VirtualBox/Chromium. To fix, we need to fall back
                *   to glXChooseVisual(). The problem with this is that it does not use the notion of a GLXFBConfig object, which means we can't
                *   create an extended context with ARB_create_context. We're going to call this a legacy context. We'll need to periodically
                *   check if it's been fixed as VirtualBox is updated.
                *
                *   When using a legacy context, self->FBConfig will be NULL while self->FBVisualInfo is non-NULL. This is how we check if a context
                *   is a legacy context.
                */
                if (GTGLcontext_InitLegacyContext(self))
                {
                    self->SharedColormap = XCreateColormap(self->XDisplay, RootWindow(self->XDisplay, self->FBVisualInfo->screen), self->FBVisualInfo->visual, AllocNone);
                }
                else
                {
                    gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Could not construct context. Attempted to fall back to a legacy context (glXChooseVisual), but it failed.");
                    GTGLcontext_dtor(self);
                }
            }
        }
        else
        {
            gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Could not construct context. Failed to choose framebuffer configuration.");
            GTGLcontext_dtor(self);
        }
    }

    /* We can only continue if we have visual info. */
    if (self->FBVisualInfo != NULL)
    {
        self->RC = gtglXCreateContext(self->XDisplay, self->FBVisualInfo, (master ? master->RC : NULL), GL_TRUE);
        if (self->RC != NULL)
        {
            /* Now we need to make a dummy window and make everything current so we can call some OpenGL functions for initialisation. */
            GTGLcontext_CreateDummyWindow(self);

            if (self->DummyWindow == 0)
            {
                gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Could not construct context. Failed to create temporary window.");
                GTGLcontext_dtor(self);
            }
        }
        else
        {
            gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Could not construct context. Failed to create context.");
            GTGLcontext_dtor(self);
        }
    }
}

void GTGLcontext_dtor(GTGLcontext self)
{
    if (self != NULL)
    {
        gtglXDestroyContext(self->XDisplay, self->RC);
        GTGLcontext_DeleteDummyWindow(self);

        if (self->master == NULL)
        {
            /* It's the master context, so this stuff can be deleted. */
            XFreeColormap(self->XDisplay, self->SharedColormap);
            XFree(self->FBVisualInfo);
        }

        self->DummyWindow    = 0;
        self->CurrentWindow  = 0;
        self->SharedColormap = 0;
        self->FBConfig       = NULL;
        self->FBVisualInfo   = NULL;
        self->RC             = NULL;

        GTGLcontext_dtor_base(self);
    }
}

void GTGLcontext_CreateDummyWindow(GTGLcontext self)
{
    XSetWindowAttributes wa;
    wa.colormap     = self->SharedColormap;
    wa.border_pixel = 0;

    self->DummyWindow = XCreateWindow(self->XDisplay, RootWindow(self->XDisplay, self->FBVisualInfo->screen), 0, 0, 1, 1,     /* <-- Window's can not have dimensions of 0 in X11. We stick with dimensions of 1. */
                                      0, self->FBVisualInfo->depth, InputOutput, self->FBVisualInfo->visual,
                                      CWBorderPixel | CWColormap, &wa);

    self->CurrentWindow = self->DummyWindow;
}

void GTGLcontext_DeleteDummyWindow(GTGLcontext self)
{
    XDestroyWindow(self->XDisplay, self->DummyWindow);
}

GLboolean GTGLcontext_InitLegacyContext(GTGLcontext self)
{
    int attribs[] =
    {
        GLX_RGBA,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        GLX_STENCIL_SIZE,  8,
        None,                   /* Reserved for GLX_DOUBLEBUFFER */
        None, None
    };


    /* It's important that we set FBConfig to NULL here because we'll use it to determine whether or not we're using a legacy context. */
    self->FBConfig = NULL;


    if (!self->singleBuffered)
    {
        attribs[13] = GLX_DOUBLEBUFFER;
    }

    self->FBVisualInfo = gtglXChooseVisual(self->XDisplay, DefaultScreen(self->XDisplay), attribs);

    return self->FBVisualInfo != NULL;
}

GLboolean GTGLcontext_EnableExtendedContext(GTGLcontext self)
{
    if (self->FBConfig != NULL)
    {
        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;

        GTGLcontext master = self->master;

        glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)gtglGetProcAddress("glXCreateContextAttribsARB");
        if (glXCreateContextAttribsARB != NULL)
        {
            GLXContext oldRC = self->RC;
            GLXContext newRC;

            int attribList[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                GLX_CONTEXT_MINOR_VERSION_ARB, 1,
                GLX_CONTEXT_FLAGS_ARB,         0,
                0, 0,
                0, 0
            };

            attribList[1] = (int)self->versionMajor;
            attribList[3] = (int)self->versionMinor;

            if (self->debug)
            {
                attribList[5] |= GLX_CONTEXT_DEBUG_BIT_ARB;
            }

            if (self->forwardCompatible)
            {
                /* If we're not backwards compatible, we create a forward-compatible core profile context. */
                if (self->versionMajor >= 3)
                {
                    attribList[5] |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

                    /* Setting GLX_CONTEXT_CORE_PROFILE_BIT_ARB wants to fail on nVidia with anything below OpenGL 3.2... */
                    if (self->versionMajor > 3 || (self->versionMajor == 3 && self->versionMinor >= 2))
                    {
                        attribList[6]  = GLX_CONTEXT_PROFILE_MASK_ARB; attribList[7] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
                    }
                }
            }
            else
            {
                /* Check for ES2 compatibility before explicately setting the compatibility profile. We can not combine the two. */
                if (self->es2Compatible)
                {
                    attribList[6] = GLX_CONTEXT_PROFILE_MASK_ARB; attribList[7] = GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
                }
                else
                {
                    /*
                    If our OpenGL version is 3.2 or above, we need to use a compatibility profile via WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB. If we don't,
                    we'll still get a valid context, but it won't be fully backwards compatible.
                    */
                    if (self->versionMajor > 3 || (self->versionMajor == 3 && self->versionMinor >= 2))
                    {
                        attribList[6] = GLX_CONTEXT_PROFILE_MASK_ARB; attribList[7] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                    }
                }
            }

            newRC = glXCreateContextAttribsARB(self->XDisplay, self->FBConfig, master ? master->RC : 0, True, attribList);
            if (newRC != 0)
            {
                self->RC = newRC;

                /*
                Only if the context is current do we want to make the new one current. This will ensure we don't have to make the context
                current again after enabling the extended context.
                */
                if (gtglGetCurrentContext() == self)
                {
                    gtglXMakeCurrent(self->XDisplay, self->CurrentWindow, self->RC);
                }

                /* The old RC needs to be deleted... */
                gtglXDestroyContext(self->XDisplay, oldRC);

                return GL_TRUE;
            }
            else
            {
                gtglPostError("GTGLcontext_EnabledExtendedContext", GTGL_ERROR_CONTEXT, "Failed to create OpenGL 3+ context.");
            }
        }
        else
        {
            gtglPostError("GTGLcontext_EnabledExtendedContext", GTGL_ERROR_CONTEXT, "Failed to retrieve pointer to wglCreateContextAttribsARB().");
        }
    }
    else
    {
        gtglPostError("GTGLcontext_EnabledExtendedContext", GTGL_ERROR_CONTEXT, "Attempting to enable an extended context on a legacy context.");
    }

    return GL_FALSE;
}



/**** Public GTGL API Functions ****/
GTGLcontext gtglStartup(size_t attribs[])
{
    /* We'll grab some variables for ease of use. */
    unsigned int versionMajor;
    unsigned int versionMinor;
    GLboolean    debug;

    if (GTGL_MasterContext == NULL)
    {
        if (!gtglLoadGLXSymbols())
        {
            printf("Failed to load libGL.so symbols.");
            return NULL;
        }


        /* Before creating the main context, we need to initialize the context manager. */
        GTGLcontextmanager_Init();

        GTGL_MasterContext = GTGLcontext_new(attribs, NULL);
        if (GTGL_MasterContext->RC != NULL)
        {
            /* It's important that the context is made current because we want to make some OpenGL calls. */
            gtglMakeCurrent(GTGL_MasterContext);


            /*
            Right from the start we'll check if we're trying for an ES2 profile. If we are, GLX_EXT_create_context_es2_profile
            must be supported. If it's not, we need to return NULL.
            */
            if (GTGL_MasterContext->es2Compatible && !gtglIsExtensionSupported("GLX_EXT_create_context_es2_profile"))
            {
                gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Can not create ES2 compatible profile. GLX_EXT_create_context_es2_profile is not supported.");

                GTGLcontext_delete(GTGL_MasterContext);
                GTGL_MasterContext = NULL;
                return NULL;
            }


            /* With the context created, it needs to be added to the context manager. */
            GTGLcontextmanager_Add(GTGL_MasterContext);


            versionMajor = (unsigned int)GTGL_MasterContext->versionMajor;
            versionMinor = (unsigned int)GTGL_MasterContext->versionMinor;
            debug        = (GLboolean)GTGL_MasterContext->debug;


            /*
            If the major and minor versions are both 0, we set them to the highest available version. Is ES2 compatibility is requested, the version
            needs to be set to 2.0. Otherwise we use gtglGetOpenGLVersion().
            */
            if (versionMajor == 0 && versionMinor == 0)
            {
                if (GTGL_MasterContext->es2Compatible)
                {
                    versionMajor = 2;
                    versionMinor = 0;
                }
                else
                {
                    gtglGetOpenGLVersion(&versionMajor, &versionMinor);
                }

                /* Can't forget to update the version on the context, too. */
                GTGL_MasterContext->versionMajor = versionMajor;
                GTGL_MasterContext->versionMinor = versionMinor;
            }

            /* Before we go any further, we want to check that the versions are supported. If not, we need to delete the context and return false. */
            if (gtglIsVersionSupported(versionMajor, versionMinor))
            {
                /*
                With the base context created, we now move on to create an extended context. The reason we make the basic context first is that we
                assume we need a current context before doing any OpenGL operations. Using ARB_create_context means we can take advantage of newer
                features, in particular OpenGL 3.0+ and ARB_debug_output.

                Also, we can't try an extended context if we using a legacy context.
                */
                if (gtglIsExtensionSupported("GLX_ARB_create_context") && GTGL_MasterContext->FBConfig != NULL)
                {
                    if (GTGLcontext_EnableExtendedContext(GTGL_MasterContext))
                    {
                        gtglInitGLAPI(debug);

                        GTGLcontextmanager_Add(GTGL_MasterContext);
                        return GTGL_MasterContext;
                    }
                    else
                    {
                        gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Failed to enable ARB_create_context context.");
                    }
                }
                else
                {
                    /*
                    If our requested version is 3.0+ and we don't have this extension, something is seriously wrong. We should never get into this
                    situation since we've a) already checked the version is supported and b) ARB_create_context should always be available when 3.0+
                    is available. We do an explicit check for the sake of sanity.
                    */
                    if (versionMajor >= 3)
                    {
                        gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "ARB_create_context is unavailable. Can not create an OpenGL %d.%d context.", versionMajor, versionMinor);
                    }
                    else
                    {
                        gtglInitGLAPI(debug);

                        GTGLcontextmanager_Add(GTGL_MasterContext);
                        return GTGL_MasterContext;
                    }
                }
            }
            else
            {
                unsigned int actualVersionMajor;
                unsigned int actualVersionMinor;
                gtglGetOpenGLVersion(&actualVersionMajor, &actualVersionMinor);

                gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Can not create OpenGL context. Requesting OpenGL version %d.%d, but only support version %d.%d.", versionMajor, versionMinor, actualVersionMajor, actualVersionMinor);
            }
        }
        else
        {
            gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Failed to create basic OpenGL context.");
        }

        /* If we've made it here it means we have failed to initialise. We need to delete the context and return false. */
        GTGLcontextmanager_Remove(GTGL_MasterContext);
        GTGLcontext_delete(GTGL_MasterContext);
        GTGL_MasterContext = NULL;

        return NULL;
    }
    else
    {
        gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "GTGL has already been initialised.");
    }

    return NULL;
}


void gtglShutdown()
{
    Display* displayToClose = NULL;
    if (!GTGL_MasterContext->UsingExternalDisplay)
    {
        displayToClose = GTGL_MasterContext->XDisplay;
    }

    gtglMakeCurrent(NULL);
    gtglDeleteContext(GTGL_MasterContext);

    /* We do not want to close the display if it is external. */
    if (displayToClose != NULL)
    {
        XCloseDisplay(displayToClose);
    }

    GTGLcontextmanager_Uninit();

    gtglShutdownErrors();
    gtglShutdownExtensions();

    gtglUnloadGLXSymbols();
}

GTGLcontext gtglCreateContext()
{
    if (GTGL_MasterContext != NULL)
    {
        GTGLcontext context = GTGLcontext_new(NULL, GTGL_MasterContext);

        if (gtglIsExtensionSupported("GLX_ARB_create_context"))
        {
            if (!GTGLcontext_EnableExtendedContext(context))
            {
                gtglPostError("gtglCreateContext", GTGL_ERROR_CONTEXT, "Failed to enable ARB_create_context context.");
                GTGLcontext_delete(context);

                return NULL;
            }
        }

        /* If we've made it here it means we've got a valid rendering context. */

        /* The new context needs to be managed by the context manager. */
        GTGLcontextmanager_Add(context);

        return context;
    }
    else
    {
        gtglPostError("gtglCreateContext", GTGL_ERROR_CONTEXT, "GTGL is not initialized. Ensure you have successfully initialized GTGL with gtglStartup().");
    }

    return NULL;
}

void gtglDeleteContext(GTGLcontext context)
{
    /* The context needs to be removed from the manager. */
    GTGLcontextmanager_Remove(context);

    if (context == GTGL_MasterContext)
    {
        GTGL_MasterContext = NULL;
    }

    GTGLcontext_delete(context);
}

void gtglMakeCurrent(GTGLcontext contextIn)
{
    GTGLcontext context  = contextIn;
    int        threadID = gtglGetCurrentThreadID();

    /*
    If there is already a context current on this thread, it needs to be made uncurrent. The OS function will do
    their part, so all we need to do is clear the threadID property of the applicable context, if any.
    */
    GTGLcontext currentContext = GTGLcontextmanager_FindByThreadID(threadID);
    if (currentContext != NULL)
    {
        currentContext->threadID = 0;
    }

    /* We need to ensure the context's thread ID is consistent. */
    if (contextIn != NULL)
    {
        contextIn->threadID = threadID;
        gtglXMakeCurrent(context->XDisplay, context->CurrentWindow, context->RC);
    }
    else
    {
        if (GTGL_MasterContext != NULL)
        {
            gtglXMakeCurrent(GTGL_MasterContext->XDisplay, 0, NULL);
        }
    }
}

GTGLcontext gtglGetCurrentContext()
{
    return GTGLcontextmanager_FindByThreadID(gtglGetCurrentThreadID());
}

GTGLplatformcontext gtglGetCurrentPlatformContext()
{
    return gtglXGetCurrentContext();
}

void gtglSetCurrentWindow(Window window)
{
    if (GTGL_MasterContext != NULL)
    {
        GTGL_MasterContext->CurrentWindow = window;

        if (window != 0)
        {
            gtglXMakeCurrent(GTGL_MasterContext->XDisplay, GTGL_MasterContext->CurrentWindow, GTGL_MasterContext->RC);
        }
        else
        {
            gtglXMakeCurrent(GTGL_MasterContext->XDisplay, 0, 0);
        }
    }
}

void gtglSetCurrentGTGLWindow(GTGLwindow window)
{
    gtglSetCurrentWindow(window->x11Window);
}

Display* gtglGetDisplay()
{
    if (GTGL_MasterContext != NULL)
    {
        return GTGL_MasterContext->XDisplay;
    }

    return 0;
}

XVisualInfo* gtglGetVisualInfo()
{
    if (GTGL_MasterContext != NULL)
    {
        return GTGL_MasterContext->FBVisualInfo;
    }

    return NULL;
}

Colormap gtglGetColormap()
{
    if (GTGL_MasterContext != NULL)
    {
        return GTGL_MasterContext->SharedColormap;
    }

    return 0;
}

void gtglSwapInterval(int interval)
{
    if (GTGL_GLX_EXT_swap_control)
    {
        if (interval < 0 && !GTGL_GLX_EXT_swap_control_tear)
        {
            interval = -interval;
        }

        glXSwapIntervalEXT(GTGL_MasterContext->XDisplay, GTGL_MasterContext->CurrentWindow, interval);
    }
    else if (GTGL_GLX_SGI_swap_control)
    {
        if (interval < 0)
        {
            interval = -1;
        }

        glXSwapIntervalSGI(interval);
    }
}

void gtglSwapBuffers()
{
    gtglXSwapBuffers(GTGL_MasterContext->XDisplay, GTGL_MasterContext->CurrentWindow);
}

GTGLPROC gtglGetProcAddress(const char* procName)
{
    return (GTGLPROC)gtglXGetProcAddress((const GLubyte *)procName);
}


#endif
