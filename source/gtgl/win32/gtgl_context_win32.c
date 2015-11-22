/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>

/* We only continue if we're on a WGL platform. */
#ifdef GTGL_WGL

#include "../gtgl_context.h"
#include "../gtgl_contextmanager.h"
#include "../gtgl_extensions.h"
#include "../gtgl_window.h"
#include "../gtgl_errors.h"
#include "../gtgl_utils.h"
#include "../emulators/gtgl_arb_debug_output.h"

/* We'll need windows.h here for various Windows functions. Remember, gtgl_wgl.h does not include windows.h; only windef.h. */
#include <windows.h>

static size_t         GTGL_WindowCount = 0;
static const wchar_t *GTGL_WindowClass = L"GTGLWindow";
LRESULT               GTGL_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return DefWindowProcW(hWnd, msg, wParam, lParam); }

void GTGLcontext_CreateDummyHWND(GTGLcontext self);
void GTGLcontext_DeleteDummyHWND(GTGLcontext self);
GLboolean GTGLcontext_EnableExtendedContext(GTGLcontext self);

void GTGLcontext_ctor(GTGLcontext self, size_t attribs[], GTGLcontext master)
{
    GTGLcontext_ctor_base(self);

    self->DummyHWND = NULL;
    self->CurrentDC = NULL;
    self->RC        = NULL;

    if (master != NULL)
    {
        self->master = master;
        GTGLcontext_CopyAttributes(self, master);

        self->PixelFormat = master->PixelFormat;
        self->PFD         = master->PFD;

        self->ARB_debug_output.emulating = master->ARB_debug_output.emulating;
    }
    else
    {
        self->master = NULL;
        GTGLcontext_SetAttributes(self, attribs);

        self->PixelFormat = 0;
    }

    /* We need a window before we can create a context... */
    GTGLcontext_CreateDummyHWND(self);

    /* With the dummy window created, we can continue. */
    self->CurrentDC = GetDC(self->DummyHWND);

    /* If we haven't yet got a pixel format, we'll need one. */
    if (self->PixelFormat == 0)
    {
        memset(&self->PFD, 0, sizeof(self->PFD));
        self->PFD.nSize        = sizeof(self->PFD);
        self->PFD.nVersion     = 1;
        self->PFD.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | (self->singleBuffered ? 0 : PFD_DOUBLEBUFFER);
        self->PFD.iPixelType   = PFD_TYPE_RGBA;
        self->PFD.cStencilBits = 8;
        self->PFD.cDepthBits   = 24;
        self->PFD.cColorBits   = 32;

        self->PixelFormat = ChoosePixelFormat(self->CurrentDC, &self->PFD);
    }

    if (self->PixelFormat != 0)
    {
        if (SetPixelFormat(self->CurrentDC, self->PixelFormat, &self->PFD))
        {
            self->RC = gtwglCreateContext(self->CurrentDC);
            if (self->RC != NULL)
            {
            }
            else
            {
                gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Error constructing context. Failed to create rendering context from device context.");
                GTGLcontext_dtor(self);
            }
        }
        else
        {
            gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Error constructing context. Failed to set pixel format.");
            GTGLcontext_dtor(self);
        }
    }
    else
    {
        gtglPostError("GTGLcontext_ctor", GTGL_ERROR_CONTEXT, "Error constructing context. Failed to find suitable pixel format.");
        GTGLcontext_dtor(self);
    }
}

void GTGLcontext_dtor(GTGLcontext self)
{
    if (self != NULL)
    {
        gtwglDeleteContext(self->RC);
        GTGLcontext_DeleteDummyHWND(self);

        self->DummyHWND   = NULL;
        self->CurrentDC   = NULL;
        self->RC          = NULL;
        self->PixelFormat = 0;

        GTGLcontext_dtor_base(self);
    }
}

void GTGLcontext_CreateDummyHWND(GTGLcontext self)
{
#if 0
    /* If the window count is zero we need to register the window class. */
    if (GTGL_WindowCount == 0)
    {
        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = sizeof(void *);
        wc.lpfnWndProc   = (WNDPROC)GTGL_WindowProc;
        wc.lpszClassName = GTGL_WindowClass;
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

        RegisterClassExW(&wc);
    }

    ++GTGL_WindowCount;
        
    /* We can create the window when we have a class. */
    self->DummyHWND = CreateWindowExW(0, GTGL_WindowClass, L"GTGL Dummy Window", 
                                      WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
                                      0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
#endif

    self->DummyHWND = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
}

void GTGLcontext_DeleteDummyHWND(GTGLcontext self)
{
#if 0
    if (GTGL_WindowCount > 0)
    {
        --GTGL_WindowCount;
        DestroyWindow(self->DummyHWND);
            
        if (GTGL_WindowCount == 0)
        {
            /* If we don't have any more windows we will unregister the class. */
            UnregisterClassW(GTGL_WindowClass, GetModuleHandle(NULL));
        }
    }
#endif

    DestroyWindow(self->DummyHWND);
}

GLboolean GTGLcontext_EnableExtendedContext(GTGLcontext self)
{
    PFNWGLCREATECONTEXTATTRIBSARBPROC _wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)gtglGetProcAddress("wglCreateContextAttribsARB");
    if (_wglCreateContextAttribsARB != NULL)
    {
        HGLRC oldRC = self->RC;
        HGLRC newRC;

        int attribList[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
            WGL_CONTEXT_MINOR_VERSION_ARB, 1,
            WGL_CONTEXT_FLAGS_ARB,         0,
            0, 0,                               /* WGL_CONTEXT_PROFILE_MASK_ARB */
            0, 0
        };

        attribList[1] = (int)self->versionMajor;
        attribList[3] = (int)self->versionMinor;

        if (self->debug)
        {
            attribList[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
        }

        if (self->forwardCompatible)
        {
            /* If we're not backwards compatible, we create a forward-compatible core profile context. */
            if (self->versionMajor >= 3)
            {
                attribList[5] |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

                /* Setting WGL_CONTEXT_CORE_PROFILE_BIT_ARB wants to fail on nVidia with anything below OpenGL 3.2... */
                if (self->versionMajor > 3 || (self->versionMajor == 3 && self->versionMinor >= 2))
                {
                    attribList[6] = WGL_CONTEXT_PROFILE_MASK_ARB; attribList[7] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
                }
            }
        }
        else
        {
            /* Check for ES2 compatibility before explicately setting the compatibility profile. We can not combine the two. */
            if (self->es2Compatible)
            {
                attribList[6] = WGL_CONTEXT_PROFILE_MASK_ARB; attribList[7] = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
            }
            else
            {
                /*
                If our OpenGL version is 3.2 or above, we need to use a compatibility profile via WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB. If we don't,
                we'll still get a valid context, but it won't be fully backwards compatible.
                */
                if (self->versionMajor > 3 || (self->versionMajor == 3 && self->versionMinor >= 2))
                {
                    attribList[6] = WGL_CONTEXT_PROFILE_MASK_ARB; attribList[7] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                }
            }
        }


        newRC = _wglCreateContextAttribsARB(self->CurrentDC, 0, attribList);
        if (newRC != 0)
        {
            self->RC = newRC;

            /*
            Only if the context is current do we want to make the new one current. This will ensure we don't have to make the context
            current again after enabling the extended context.
            */
            if (gtglGetCurrentContext() == self)
            {
                gtwglMakeCurrent(self->CurrentDC, self->RC);
            }

            /* The old RC needs to be deleted... */
            gtwglDeleteContext(oldRC);

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

    return GL_FALSE;
}



/**** Public GTGL API Functions. ****/
GTGLcontext gtglStartup(size_t attribs[])
{
    /* We'll grab some variables for ease of use. */
    unsigned int versionMajor;
    unsigned int versionMinor;
    GLboolean    debug;

    if (GTGL_MasterContext == NULL)
    {
        /* We need to load the appropriate symbols from OpenGL32.dll. If this fails, we can't continue. */
        if (gtglLoadWGLSymbols())
        {
            /* Before creating the main context, we need to initialize the context manager. */
            GTGLcontextmanager_Init();

            GTGL_MasterContext = GTGLcontext_new(attribs, NULL);
            if (GTGL_MasterContext->RC != NULL)
            {
                /* It's important that the context is made current because we want to make some OpenGL calls. */
                gtglMakeCurrent(GTGL_MasterContext);


                /*
                Right from the start we'll check if we're trying for an ES2 profile. If we are, WGL_EXT_create_context_es2_profile
                must be supported. If it's not, we need to return NULL.
                */
                if (GTGL_MasterContext->es2Compatible && !gtglIsExtensionSupported("WGL_EXT_create_context_es2_profile"))
                {
                    gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Can not create ES2 compatible profile. WGL_EXT_create_context_es2_profile is not supported.");

                    GTGLcontext_delete(GTGL_MasterContext);
                    GTGL_MasterContext = NULL;
                    return NULL;
                }


                /* With the context created, it needs to be added to the context manager. */
                GTGLcontextmanager_Add(GTGL_MasterContext);

                versionMajor = GTGL_MasterContext->versionMajor;
                versionMinor = GTGL_MasterContext->versionMinor;
                debug        = GTGL_MasterContext->debug;

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
                    */
                    if (gtglIsExtensionSupported("WGL_ARB_create_context"))
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
            gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "Failed to load symbols from OpenGL32.dll.");
        }
    }
    else
    {
        gtglPostError("gtglStartup", GTGL_ERROR_CONTEXT, "GTGL has already been initialised.");
    }

    return NULL;
}

void gtglShutdown()
{
    gtglDeleteContext(GTGL_MasterContext);
    GTGLcontextmanager_Uninit();

    gtglShutdownErrors();
    gtglShutdownExtensions();
}

GTGLcontext gtglCreateContext()
{
    if (GTGL_MasterContext != NULL)
    {
        GTGLcontext context = GTGLcontext_new(NULL, GTGL_MasterContext);

        if (gtglIsExtensionSupported("WGL_ARB_create_context"))
        {
            if (!GTGLcontext_EnableExtendedContext(context))
            {
                gtglPostError("gtglCreateContext", GTGL_ERROR_CONTEXT, "Failed to enable ARB_create_context context.");
                GTGLcontext_delete(context);

                return NULL;
            }
        }

        /* If we've made it here it means we've got a valid rendering context. We need to try sharing with the master. */
        if (gtwglShareLists(GTGL_MasterContext->RC, context->RC))
        {
            /* The new context needs to be managed by the context manager. */
            GTGLcontextmanager_Add(context);

            return (GTGLcontext)context;
        }
        else
        {
            gtglPostError("gtglCreateContext", GTGL_ERROR_CONTEXT, "Failed to share resources with master context.");
            GTGLcontext_delete(context);

            return NULL;
        }
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
    int         threadID = gtglGetCurrentThreadID();

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
        gtwglMakeCurrent(context->CurrentDC, context->RC);
    }
    else
    {
        gtwglMakeCurrent(NULL, NULL);
    }
}

GTGLcontext gtglGetCurrentContext()
{
    return GTGLcontextmanager_FindByThreadID(gtglGetCurrentThreadID());
}

GTGLplatformcontext gtglGetCurrentPlatformContext()
{
    return gtwglGetCurrentContext();
}

void gtglSetCurrentDC(HDC hDC)
{
    if (GTGL_MasterContext != NULL)
    {
        GTGL_MasterContext->CurrentDC = hDC;
        gtwglMakeCurrent(GTGL_MasterContext->CurrentDC, GTGL_MasterContext->RC);
    }
}

void gtglSetCurrentGTGLWindow(GTGLwindow window)
{
    gtglSetCurrentDC(window->hDC);
}

int gtglGetPixelFormat()
{
    if (GTGL_MasterContext != NULL)
    {
        return GTGL_MasterContext->PixelFormat;
    }

    return 0;
}

const struct tagPIXELFORMATDESCRIPTOR * gtglGetPFD()
{
    if (GTGL_MasterContext != NULL)
    {
        return &GTGL_MasterContext->PFD;
    }

    return NULL;
}

void gtglSwapInterval(int interval)
{
    if (GTGL_WGL_EXT_swap_control)
    {
        if (interval < 0 && !GTGL_WGL_EXT_swap_control_tear)
        {
            interval = -interval;
        }

        wglSwapIntervalEXT(interval);
    }
}

void gtglSwapBuffers()
{     
    SwapBuffers(GTGL_MasterContext->CurrentDC);
}

GTGLPROC gtglGetProcAddress(const char *procName)
{
    /* We're first going to try loading from OpenGL32.dll. If that fails, we try gtwglGetProcAddress(). */
    GTGLPROC result = (GTGLPROC)GetProcAddress(GTGL_OpenGL32_DLL, procName);
    if (result == NULL)
    {
        result = (GTGLPROC)gtwglGetProcAddress(procName);
    }

    return result;
}

#endif
