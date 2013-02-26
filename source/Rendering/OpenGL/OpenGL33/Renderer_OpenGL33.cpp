// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Renderer2.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Rendering/RCQueue.hpp>

#include <gtgl/gtgl.h>

#if defined(GTCORE_PLATFORM_WINDOWS)
#include <WinGDI.h>
#else
#include <GTCore/Windowing/X11/X11.hpp>
#endif


#include "../RendererCaps.hpp"
#include "../Debugging_OpenGL.hpp"


namespace GTEngine
{
    /////////////////////////////////////////////////////////////
    // Direct Calls

    /// Keeps track of whether or not the renderer is initialised.
    static bool IsInitialised = false;

    /// The OpenGL context.
    static GTGLcontext OpenGLContext = nullptr;

    /// The hardware capabilities. This is initialised at startup.
    static RendererCaps_OpenGL RendererCaps;


    /// The two call caches. The back call back is identified with BackCallCacheIndex.
    static RCQueue CallCaches[2];

    /// The index identifying the back call cache.
    static unsigned int BackCallCacheIndex = 0;



    bool Renderer2::Startup()
    {
        assert(!IsInitialised);
        {
            size_t contextAttribs[] =
            {
                GTGL_CONTEXT_MAJOR_VERSION,      3,
                GTGL_CONTEXT_MINOR_VERSION,      3,
                GTGL_CONTEXT_FORWARD_COMPATIBLE, 1,

            #if !defined(NDEBUG)
                GTGL_CONTEXT_DEBUG,         GL_TRUE,
            #endif

                0, 0
            };

            OpenGLContext = gtglStartup(contextAttribs);
            if (OpenGLContext != nullptr)
            {
                // We'll log a few capabilities.
                unsigned int versionMajor;
                unsigned int versionMinor;
                gtglGetOpenGLVersion(&versionMajor, &versionMinor);
                Log("OpenGL Version:  %d.%d", versionMajor, versionMinor);
                Log("OpenGL Renderer: %s",    glGetString(GL_RENDERER));
                Log("OpenGL Vendor:   %s",    glGetString(GL_VENDOR));

                Log("OpenGL Extensions:");
                Log("    ARB_ES2_compatibility:              %s", GTGL_ARB_ES2_compatibility              ? "yes" : "no");
                Log("    EXT_texture_compression_s3tc:       %s", GTGL_EXT_texture_compression_s3tc       ? "yes" : "no");
                Log("    EXT_texture_filter_anisotropic:     %s", GTGL_EXT_texture_filter_anisotropic     ? "yes" : "no");
                Log("    NV_bindless_texture:                %s", GTGL_NV_bindless_texture                ? "yes" : "no");


            #if !defined(NDEBUG)
                gtglEnableSynchronousDebugOutput();
                gtglSetDebugMessageCallback(OpenGLDebugMessageHandler);
            #endif


                // Here we'll grab the renderer's capabilities.
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,   &RendererCaps.MaxColourAttachments);
                glGetIntegerv(GL_MAX_DRAW_BUFFERS,        &RendererCaps.MaxDrawBuffers);
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &RendererCaps.MaxTextureUnits);


                // Now we'll set some defaults.
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_CULL_FACE);

                // TODO: Check if we need these texture enables by default. Should the shader control these when they are bound?
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_TEXTURE_CUBE_MAP);
                

                // We're going to initialise the X11 sub-system from here.
            #ifdef GTGL_GLX
                GTCore::X11::Initialize(gtglGetDisplay());
            #endif

                IsInitialised = true;
            }
            else
            {
                GTEngine::PostError("Error creating OpenGL context. %s", gtglGetLastError()->message);
            }
        }

        return IsInitialised;
    }

    void Renderer2::Shutdown()
    {
        assert(IsInitialised);
        {
            // TODO: Delete renderer-created objects.


            // Both caches should be cleared.
            CallCaches[0].Clear();
            CallCaches[1].Clear();

            // GTGL needs to be shutdown.
            gtglShutdown();
            OpenGLContext = nullptr;


            // Finally, we need to mark the renderer as uninitialised.
            IsInitialised = false;
        }
    }


    GTCore::Window* Renderer2::CreateWindow()
    {
        assert(IsInitialised);
        {
            // When creating a window, we use GTCore to create the main window. Then we do platform specific stuff to get it working with the GTGL context.
        #ifdef GTCORE_PLATFORM_WINDOWS
            GTCore::Window *window = new GTCore::Window();
            const GTCore::InternalWindowObjects &iwo = window->GetInternalObjects();

            ::SetPixelFormat(iwo.hDC, gtglGetPixelFormat(), gtglGetPFD());

            // We now need to make the window current.
            gtglSetCurrentDC(iwo.hDC);
        #else
            GTCore::InternalWindowObjects iwo;
            iwo.vi       = gtglGetVisualInfo();
            iwo.colormap = gtglGetColormap();

            GTCore::Window *window = new GTCore::Window(iwo);

            // The window needs to be made current...
            gtglSetCurrentWindow(window->GetInternalObjects().window);
        #endif

            return window;
        }
    }

    void Renderer2::SetCurrentWindow(GTCore::Window *window)
    {
    #ifdef GTCORE_PLATFORM_WINDOWS
        if (window != nullptr)
        {
            gtglSetCurrentDC(window->GetInternalObjects().hDC);
        }
        else
        {
            gtglSetCurrentDC(0);
        }
    #else   // X
        if (window != nullptr)
        {
            gtglSetCurrentWindow(window->GetInternalObjects().window);
        }
        else
        {
            gtglSetCurrentWindow(0);
        }
    #endif
    }

    void Renderer2::SwapCallCaches()
    {
        // 1) Swap the back call cache index.
        BackCallCacheIndex = !BackCallCacheIndex;

        // 2) Clear the new back cache in preparation for filling by another thread.
        CallCaches[BackCallCacheIndex].Clear();
    }

    void Renderer2::ExecuteCallCache()
    {
        CallCaches[!BackCallCacheIndex].Execute();
    }

    void Renderer2::SwapBuffers()
    {
        gtglSwapBuffers();
    }




    /////////////////////////////////////////////////////////////
    // Cached Calls






    /////////////////////////////////////////////////////////////
    // Support Queries

    bool Renderer2::HasFlippedTextures()
    {
        return true;
    }

    unsigned int Renderer2::GetMaxColourAttachments()
    {
        return static_cast<unsigned int>(RendererCaps.MaxColourAttachments);
    }

    unsigned int Renderer2::GetMaxDrawBuffers()
    {
        return static_cast<unsigned int>(RendererCaps.MaxDrawBuffers);
    }

    unsigned int Renderer2::GetMaxTextureUnits()
    {
        return static_cast<unsigned int>(RendererCaps.MaxTextureUnits);
    }
}