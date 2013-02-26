// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/GUIRenderer.hpp>
#include <GTEngine/Rendering/VertexAttributes.hpp>
#include <GTEngine/Rendering/OpenGL/OpenGL20.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Math.hpp>
#include <GTCore/Strings/Tokenizer.hpp>
#include <GTCore/ToString.hpp>
#include <GTGUI/Server.hpp>

#include <gtgl/gtgl.h>

#if defined(GTCORE_PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <GTCore/Windowing/X11/X11.hpp>
#endif

#include "OpenGL/TypeConversion.hpp"
#include "OpenGL/Debugging_OpenGL.hpp"


// OpenGL Utilities
namespace GTEngine
{
    enum OpenGLRenderer
    {
        OpenGLRenderer_GeForce,
        OpenGLRenderer_Radeon,
        OpenGLRenderer_Intel,
        OpenGLRenderer_Chomium,
        OpenGLRenderer_Gallium,
    };

    struct _TempOpenGLObjects
    {
        _TempOpenGLObjects()
            : drawBuffers(nullptr)
        {
        }

        ~_TempOpenGLObjects()
        {
            delete [] this->drawBuffers;
        }

        /// This stores an array required for glDrawBuffers(). It is used by ToOpenGLDrawBuffers().
        GLenum *drawBuffers;

    private:    // No copying.
        _TempOpenGLObjects(const _TempOpenGLObjects &);
        _TempOpenGLObjects & operator=(const _TempOpenGLObjects &);

    }TempOpenGLObjects;


    GLenum* ToOpenGLDrawBuffers(size_t count, int *buffers)
    {
        if (TempOpenGLObjects.drawBuffers == nullptr)
        {
            TempOpenGLObjects.drawBuffers = new GLenum[Renderer::GetMaxDrawBuffers()];
        }

        for (size_t i = 0; i < count; ++i)
        {
            TempOpenGLObjects.drawBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + buffers[i];
        }

        return TempOpenGLObjects.drawBuffers;
    }
}

// Renderer Globals. Needs to be before the renderer support utils.
namespace GTEngine
{
    /// The main OpenGL context.
    GTGLcontext OpenGLContext = nullptr;

    bool IsRendererInitialised = false;
    bool IsSRGBEnabled         = false;


    // This is a bitfield containing bits representing which vertex attributes are currently enabled on the OpenGL side.
    uint32_t VertexAttribEnableBits = 0x0;

    // A pointer to the vertex array that was last drawn.
    const VertexArray* LastDrawnVertexArray = nullptr;


    // Hardware capabilities.
    struct _RendererCaps
    {
        GLint MaxColourAttachments;         ///< The maximum number of framebuffer colour attachments.
        GLint MaxDrawBuffers;               ///< The maximum number of draw buffers for MRT.
        GLint MaxTextureUnits;              ///< The maximum number of active texture units.

    }RendererCaps;


    /// Structure containing the current state of the renderer.
    struct _RendererState
    {
        _RendererState()
            : CurrentShader(nullptr),
              ViewportX(0), ViewportY(0), ViewportWidth(0), ViewportHeight(0),
              ScissorX(0), ScissorY(0), ScissorWidth(0), ScissorHeight(0), IsScissorEnabled(false),
              IsColourWritesEnabled(true),
              IsDepthTestEnabled(false), IsDepthWritesEnabled(true), CurrentDepthFunc(RendererFunction_Less),
              IsStencilTestEnabled(false),
              IsBlendingEnabled(false), CurrentBlendSourceFactor(BlendFunc_One), CurrentBlendDestFactor(BlendFunc_Zero), CurrentBlendEquation(BlendEquation_Add),
              IsAlphaTestEnabled(false), CurrentAlphaTestFunc(RendererFunction_Always), CurrentAlphaTestRef(0.0f),
              SwapInterval(1), SwapIntervalChanged(false),
              CurrentFramebuffer(nullptr)
        {
        }

        /// The current shader.
        Shader* CurrentShader;

        /// The current viewport.
        int ViewportX;
        int ViewportY;
        unsigned int ViewportWidth;
        unsigned int ViewportHeight;

        /// The current scissor rectangle.
        int ScissorX;
        int ScissorY;
        unsigned int ScissorWidth;
        unsigned int ScissorHeight;
        bool IsScissorEnabled;

        /// The current colour writing state.
        bool IsColourWritesEnabled;

        /// The current depth testing state.
        bool IsDepthTestEnabled;
        bool IsDepthWritesEnabled;
        RendererFunction CurrentDepthFunc;

        /// The current stencil testing state.
        bool IsStencilTestEnabled;


        /// The current blending state.
        bool IsBlendingEnabled;
        BlendFunc     CurrentBlendSourceFactor;
        BlendFunc     CurrentBlendDestFactor;
        BlendEquation CurrentBlendEquation;

        /// The current alpha testing state.
        bool IsAlphaTestEnabled;
        RendererFunction CurrentAlphaTestFunc;
        float            CurrentAlphaTestRef;

        /// Swap interval.
        int  SwapInterval;
        bool SwapIntervalChanged;


        /// The current framebuffer.
        Framebuffer* CurrentFramebuffer;


    private:    // No copying.
        _RendererState(const _RendererState &);
        _RendererState & operator=(const _RendererState &);

    }RendererState;
}


namespace GTEngine
{
    RCQueue* Renderer::BackRCQueue  = nullptr;
    RCQueue* Renderer::FrontRCQueue = nullptr;
    size_t   Renderer::BackIndex    = 0;



    void Renderer::AppendToBackBuffer(RenderCommand &cmd)
    {
        Renderer::BackRCQueue->Append(cmd);
    }

    void Renderer::ClearBackRCQueue()
    {
        Renderer::BackRCQueue->Clear();
    }

    void Renderer::ExecuteFrontRCQueue()
    {
        // We execute the resources render commands before everything else.
        Renderer::ExecuteFrontResourceRCQueue();
        Renderer::FrontRCQueue->Execute();
    }


    void Renderer::SwapRCQueues()
    {
        // All we do is swap the pointers. Easy.
        auto temp              = Renderer::BackRCQueue;
        Renderer::BackRCQueue  = Renderer::FrontRCQueue;
        Renderer::FrontRCQueue = temp;

        // We also want to clear the back buffer.
        Renderer::ClearBackRCQueue();

        // Here we swap the back index.
        Renderer::BackIndex = !Renderer::BackIndex;


        // We should let everything know about the queue swaps.
        Renderer::OnSwapRCQueues();
    }


    void Renderer::SetSwapInterval(int interval)
    {
        // We don't actually change the swap interval immediately. Instead we delay it until the next call to SwapBuffer(). Doing
        // it this way means an application doesn't need to worry about calling this function on the rendering thread.
        if (RendererState.SwapInterval != interval)
        {
            RendererState.SwapInterval        = interval;
            RendererState.SwapIntervalChanged = true;
        }
    }

    int Renderer::GetSwapInterval()
    {
        return RendererState.SwapInterval;
    }



    //////////////////////////////////////////
    // Event Handling.

    static GTCore::Vector<RendererEventHandler*> RendererEventHandlers;
    static GTCore::Mutex                         RendererEventHandlerLock;

    void Renderer::AttachEventHandler(RendererEventHandler &eventHandler)
    {
        RendererEventHandlerLock.Lock();
        {
            if (!RendererEventHandlers.Exists(&eventHandler))
            {
                RendererEventHandlers.PushBack(&eventHandler);
            }
        }
        RendererEventHandlerLock.Unlock();
    }

    void Renderer::DetachEventHandler(RendererEventHandler &eventHandler)
    {
        RendererEventHandlerLock.Lock();
        {
            RendererEventHandlers.RemoveFirstOccuranceOf(&eventHandler);
        }
        RendererEventHandlerLock.Unlock();
    }

    void Renderer::OnSwapRCQueues()
    {
        RendererEventHandlerLock.Lock();
        {
            for (size_t i = 0; i < RendererEventHandlers.count; ++i)
            {
                RendererEventHandlers[i]->OnSwapRCQueues();
            }
        }
        RendererEventHandlerLock.Unlock();
    }
}


namespace GTEngine
{
    bool Renderer::Startup()
    {
        if (!IsRendererInitialised)
        {
            size_t contextAttribs[] =
            {
                GTGL_CONTEXT_MAJOR_VERSION, 2,
                GTGL_CONTEXT_MINOR_VERSION, 0,

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
                Log("OpenGL Renderer: %s", glGetString(GL_RENDERER));
                Log("OpenGL Vendor:   %s", glGetString(GL_VENDOR));

                Log("OpenGL Extensions:");
                Log("    ARB_ES2_compatibility:              %s", GTGL_ARB_ES2_compatibility              ? "yes" : "no");
                Log("    ARB_texture_float:                  %s", GTGL_ARB_texture_float                  ? "yes" : "no");
                Log("    ARB_half_float_pixel:               %s", GTGL_ARB_half_float_pixel               ? "yes" : "no");
                Log("    EXT_framebuffer_object:             %s", GTGL_EXT_framebuffer_object             ? "yes" : "no");
                Log("    EXT_framebuffer_blit:               %s", GTGL_EXT_framebuffer_blit               ? "yes" : "no");
                Log("    EXT_texture_compression_s3tc:       %s", GTGL_EXT_texture_compression_s3tc       ? "yes" : "no");
                Log("    EXT_texture_filter_anisotropic:     %s", GTGL_EXT_texture_filter_anisotropic     ? "yes" : "no");
                Log("    NV_bindless_texture:                %s", GTGL_NV_bindless_texture                ? "yes" : "no");

                /*
                const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
                GTCore::Strings::ANSI::Replacer replacer(extensions);
                replacer.Replace(" ", "\n    ");
                Log.Write("    %s", replacer.c_str());
                */

            #if !defined(NDEBUG)
                gtglEnableSynchronousDebugOutput();
                gtglSetDebugMessageCallback(OpenGLDebugMessageHandler);
            #endif

                // Here we'll grab the renderer's capabilities.
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,   &RendererCaps.MaxColourAttachments);
                glGetIntegerv(GL_MAX_DRAW_BUFFERS,        &RendererCaps.MaxDrawBuffers);
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &RendererCaps.MaxTextureUnits);

                Log("Renderer Caps:");
                Log("    Max Colour Attachments: %d", RendererCaps.MaxColourAttachments);
                Log("    Max Draw Buffers:       %d", RendererCaps.MaxDrawBuffers);
                Log("    Max Texture Units:      %d", RendererCaps.MaxTextureUnits);


                Renderer::BackRCQueue  = new RCQueue;
                Renderer::FrontRCQueue = new RCQueue;


                // Now we'll set some defaults.
                Renderer::EnableDepthTest();
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_TEXTURE_CUBE_MAP);
                glEnable(GL_CULL_FACE);

                // We're going to initialise the X11 sub-system from here.
            #ifdef GTGL_GLX
                GTCore::X11::Initialize(gtglGetDisplay());
            #endif

                IsRendererInitialised = true;
            }
            else
            {
                GTEngine::PostError("Error creating OpenGL context. %s", gtglGetLastError()->message);
            }
        }

        // If we make it here, we're already initialised.
        return IsRendererInitialised;
    }

    void Renderer::Shutdown()
    {
        if (IsRendererInitialised)
        {
            // We'll shutdown the GUI renderer first since it will need to delete texture objects and whatnot.
            GUIRenderer::Uninitialise();

            // There may be a bunch of resource commands that will be deleting resource objects. We need to make sure these are executed. To do this, we just
            // do a fake buffer swap and then execute the front buffer.
            Renderer::BackIndex = !Renderer::BackIndex;
            Renderer::ExecuteFrontResourceRCQueue();


            delete Renderer::BackRCQueue;
            delete Renderer::FrontRCQueue;


            gtglShutdown();
            OpenGLContext = nullptr;

            IsRendererInitialised = false;
        }
    }

    GTCore::Window * Renderer::CreateGameWindow()
    {
        if (!IsRendererInitialised)
        {
            if (!Renderer::Startup())
            {
                return nullptr;
            }
        }

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

        // We will set the swap interval to the default value.
        gtglSwapInterval(RendererState.SwapInterval);


        return window;
    }

    void Renderer::SetCurrentWindow(GTCore::Window* window)
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
#else
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


    bool Renderer::HasFlippedTextures()
    {
        return true;        // <-- OpenGL uses upside-down textures.
    }

    void Renderer::SwapBuffers()
    {
        // Before swapping the buffers, lets make sure we have the correct swap interval set.
        if (RendererState.SwapIntervalChanged)
        {
            gtglSwapInterval(RendererState.SwapInterval);
            RendererState.SwapIntervalChanged = false;
        }

        gtglSwapBuffers();
    }

    void Renderer::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
        glViewport(x, y, (GLsizei)width, (GLsizei)height);

        RendererState.ViewportX      = x;
        RendererState.ViewportY      = y;
        RendererState.ViewportWidth  = width;
        RendererState.ViewportHeight = height;
    }

    void Renderer::GetViewport(int &x, int &y, unsigned int &width, unsigned int &height)
    {
        x      = RendererState.ViewportX;
        y      = RendererState.ViewportY;
        width  = RendererState.ViewportWidth;
        height = RendererState.ViewportHeight;
    }

    void Renderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        glScissor(x, y, (GLsizei)width, (GLsizei)height);

        RendererState.ScissorX      = x;
        RendererState.ScissorY      = y;
        RendererState.ScissorWidth  = width;
        RendererState.ScissorHeight = height;
    }

    void Renderer::GetScissor(int &x, int &y, unsigned int &width, unsigned int &height)
    {
        x      = RendererState.ScissorX;
        y      = RendererState.ScissorY;
        width  = RendererState.ScissorWidth;
        height = RendererState.ScissorHeight;
    }

    void Renderer::Clear(unsigned int bufferMask)
    {
        GLbitfield glmask = 0;

        if (bufferMask & GTEngine::ColourBuffer)
        {
            glmask |= GL_COLOR_BUFFER_BIT;
        }
        if (bufferMask & GTEngine::DepthBuffer)
        {
            glmask |= GL_DEPTH_BUFFER_BIT;
        }
        if (bufferMask & GTEngine::StencilBuffer)
        {
            glmask |= GL_STENCIL_BUFFER_BIT;
        }

        glClear(glmask);
    }

    void Renderer::ClearColour(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }
    void Renderer::ClearDepth(float depth)
    {
        glClearDepth(depth);
    }
    void Renderer::ClearStencil(int stencil)
    {
        glClearStencil(static_cast<GLint>(stencil));
    }



    void Renderer::Draw(const VertexArray* vertexArray, DrawMode mode)
    {
        assert(vertexArray != nullptr);

        OpenGL20::BindVertexArray(vertexArray);
        OpenGL20::EnableVertexFormat(vertexArray->GetFormat(), nullptr);

        // Now that everything has been enabled/disabled, we draw the elements.
        glDrawElements(ToOpenGLDrawMode(mode), static_cast<const OpenGL20::VertexArray*>(vertexArray->GetRendererData())->indexCount, GL_UNSIGNED_INT, 0);
    }

    void Renderer::Draw(const float* vertices, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
        // We need to disable any vertex buffer object binding.
        OpenGL20::BindVertexArray(nullptr);
        OpenGL20::EnableVertexFormat(format, vertices);

        // ... then we just draw the elements.
        glDrawElements(ToOpenGLDrawMode(mode), static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, indices);
    }

    void Renderer::EnableScissorTest()
    {
        if (!RendererState.IsScissorEnabled)
        {
            glEnable(GL_SCISSOR_TEST);
            RendererState.IsScissorEnabled = true;
        }
    }
    void Renderer::DisableScissorTest()
    {
        if (RendererState.IsScissorEnabled)
        {
            glDisable(GL_SCISSOR_TEST);
            RendererState.IsScissorEnabled = false;
        }
    }



    void Renderer::EnableBlending()
    {
        if (!RendererState.IsBlendingEnabled)
        {
            glEnable(GL_BLEND);
            RendererState.IsBlendingEnabled = true;
        }
    }

    void Renderer::DisableBlending()
    {
        if (RendererState.IsBlendingEnabled)
        {
            glDisable(GL_BLEND);
            RendererState.IsBlendingEnabled = false;
        }
    }

    bool Renderer::IsBlendingEnabled()
    {
        return RendererState.IsBlendingEnabled;
    }

    void Renderer::SetBlendFunc(BlendFunc sourceFactor, BlendFunc destFactor)
    {
        if (RendererState.CurrentBlendSourceFactor != sourceFactor || RendererState.CurrentBlendDestFactor != destFactor)
        {
            GLenum sfactor = ToOpenGLBlendFunc(sourceFactor);
            GLenum dfactor = ToOpenGLBlendFunc(destFactor);

            glBlendFunc(sfactor, dfactor);

            RendererState.CurrentBlendSourceFactor = sourceFactor;
            RendererState.CurrentBlendDestFactor   = destFactor;
        }
    }

    void Renderer::SetBlendEquation(BlendEquation equation)
    {
        if (RendererState.CurrentBlendEquation != equation)
        {
            glBlendEquation(ToOpenGLBlendEquation(equation));

            RendererState.CurrentBlendEquation = equation;
        }
    }

    void Renderer::EnableAlphaBlending()
    {
        Renderer::EnableBlending();
        Renderer::SetBlendFunc(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
    }


    void Renderer::EnableAlphaTest()
    {
        if (!RendererState.IsAlphaTestEnabled)
        {
            glEnable(GL_ALPHA_TEST);
            RendererState.IsAlphaTestEnabled = true;
        }
    }

    void Renderer::DisableAlphaTest()
    {
        if (RendererState.IsAlphaTestEnabled)
        {
            glDisable(GL_ALPHA_TEST);
            RendererState.IsAlphaTestEnabled = false;
        }
    }

    void Renderer::SetAlphaTestFunc(RendererFunction func, float ref)
    {
        if (RendererState.CurrentAlphaTestFunc != func)
        {
            glAlphaFunc(ToOpenGLFunc(func), static_cast<GLclampf>(ref));

            RendererState.CurrentAlphaTestFunc = func;
            RendererState.CurrentAlphaTestRef  = ref;
        }
    }


    void Renderer::EnableColourWrites()
    {
        if (!RendererState.IsColourWritesEnabled)
        {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            RendererState.IsColourWritesEnabled = true;
        }
    }

    void Renderer::DisableColourWrites()
    {
        if (RendererState.IsColourWritesEnabled)
        {
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            RendererState.IsColourWritesEnabled = false;
        }
    }

    void Renderer::EnableDepthTest()
    {
        if (!RendererState.IsDepthTestEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            RendererState.IsDepthTestEnabled = true;
        }
    }
    void Renderer::DisableDepthTest()
    {
        if (RendererState.IsDepthTestEnabled)
        {
            glDisable(GL_DEPTH_TEST);
            RendererState.IsDepthTestEnabled = false;
        }
    }

    void Renderer::EnableDepthWrites()
    {
        if (!RendererState.IsDepthWritesEnabled)
        {
            glDepthMask(GL_TRUE);
            RendererState.IsDepthWritesEnabled = true;
        }
    }
    void Renderer::DisableDepthWrites()
    {
        if (RendererState.IsDepthWritesEnabled)
        {
            glDepthMask(GL_FALSE);
            RendererState.IsDepthWritesEnabled = false;
        }
    }

    void Renderer::SetDepthFunc(RendererFunction func)
    {
        if (RendererState.CurrentDepthFunc != func)
        {
            glDepthFunc(ToOpenGLFunc(func));
            RendererState.CurrentDepthFunc = func;
        }
    }


    void Renderer::EnableStencilTest()
    {
        if (!RendererState.IsStencilTestEnabled)
        {
            glEnable(GL_STENCIL_TEST);
            RendererState.IsStencilTestEnabled = true;
        }
    }

    void Renderer::DisableStencilTest()
    {
        if (RendererState.IsStencilTestEnabled)
        {
            glDisable(GL_STENCIL_TEST);
            RendererState.IsStencilTestEnabled = false;
        }
    }

    void Renderer::SetStencilMask(unsigned int mask)
    {
        glStencilMask(static_cast<GLuint>(mask));
    }

    void Renderer::SetStencilFunc(RendererFunction func, int ref, unsigned int mask)
    {
        glStencilFunc(ToOpenGLFunc(func), ref, mask);
    }

    void Renderer::SetStencilOp(StencilOp stencilFail, StencilOp depthFail, StencilOp pass)
    {
        glStencilOp(ToOpenGLStencilOp(stencilFail), ToOpenGLStencilOp(depthFail), ToOpenGLStencilOp(pass));
    }

    void Renderer::SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned mask)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

        glStencilMaskSeparate(face, static_cast<GLuint>(mask));
    }

    void Renderer::SetStencilFuncSeparate(bool frontFace, bool backFace, RendererFunction func, int ref, unsigned int mask)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

        glStencilFuncSeparate(face, ToOpenGLFunc(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
    }

    void Renderer::SetStencilOpSeparate(bool frontFace, bool backFace, StencilOp stencilFail, StencilOp depthFail, StencilOp pass)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

        glStencilOpSeparate(face, ToOpenGLStencilOp(stencilFail), ToOpenGLStencilOp(depthFail), ToOpenGLStencilOp(pass));
    }



    void Renderer::SetFaceCulling(bool cullFront, bool cullBack)
    {
        if (cullFront || cullBack)
        {
            glEnable(GL_CULL_FACE);

            if (cullFront && cullBack)
            {
                glCullFace(GL_FRONT_AND_BACK);
            }
            else
            {
                if (cullFront)
                {
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glCullFace(GL_BACK);
                }
            }
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    
    void Renderer::SetPolygonMode(bool frontFaces, bool backFaces, PolygonMode mode)
    {
        GLenum faceGL;
        GLenum modeGL;


        if (frontFaces && backFaces)
        {
            faceGL = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFaces)
            {
                faceGL = GL_FRONT;
            }
            else
            {
                faceGL = GL_BACK;
            }
        }


        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_LINE;
        }
        else
        {
            assert(mode == PolygonMode_Point);
            modeGL = GL_LINE;
        }


        glPolygonMode(faceGL, modeGL);
    }


    void Renderer::EnablePolygonOffset(PolygonMode mode)
    {
        GLenum modeGL;

        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_POLYGON_OFFSET_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_POLYGON_OFFSET_LINE;
        }
        else
        {
            modeGL = GL_POLYGON_OFFSET_POINT;
        }

        glEnable(modeGL);
    }

    void Renderer::DisablePolygonOffset(PolygonMode mode)
    {
        GLenum modeGL;

        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_POLYGON_OFFSET_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_POLYGON_OFFSET_LINE;
        }
        else
        {
            modeGL = GL_POLYGON_OFFSET_POINT;
        }

        glDisable(modeGL);
    }

    void Renderer::SetPolygonOffset(float factor, float units)
    {
        glPolygonOffset(factor, units);
    }


    void Renderer::EnableSRGB()
    {
        if (!IsSRGBEnabled)
        {
            glEnable(GL_FRAMEBUFFER_SRGB);
            IsSRGBEnabled = true;
        }
    }
    void Renderer::DisableSRGB()
    {
        if (IsSRGBEnabled)
        {
            glDisable(GL_FRAMEBUFFER_SRGB);
            IsSRGBEnabled = false;
        }
    }

    void Renderer::SetFramebuffer(Framebuffer* framebuffer)
    {
        if (GTGL_EXT_framebuffer_object)
        {
            if (framebuffer != nullptr)
            {
                auto framebufferData = static_cast<OpenGL20::Framebuffer*>(framebuffer->GetRendererData());
                assert(framebufferData != nullptr);

                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferData->object);
                RendererState.CurrentFramebuffer = framebuffer;
            }
            else
            {
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                RendererState.CurrentFramebuffer = nullptr;
            }
        }
        else
        {
            GTEngine::PostError("Renderer::SetFramebuffer() - GL_EXT_framebuffer_object is not supported. Can not use framebuffer objects.");
        }

        return;
    }

    Shader * Renderer::GetCurrentShader()
    {
        return RendererState.CurrentShader;
    }

    void Renderer::SetShader(Shader *shader)
    {
        if (shader != nullptr)
        {
            glUseProgram(static_cast<OpenGL20::Shader*>(shader->GetRendererData())->program);
            RendererState.CurrentShader = shader;

            Renderer::BindCurrentShaderTextures();
            Renderer::UpdateCurrentShaderParameters();
        }
        else
        {
            // Switch to fixed function...
            glUseProgram(0);
            RendererState.CurrentShader = nullptr;
        }
    }

    void Renderer::UpdateCurrentShaderParameters()
    {
        if (RendererState.CurrentShader != nullptr)
        {
            // With the shader bound, we will now loop through each pending parameter and set them.
            auto &pendingParams = RendererState.CurrentShader->GetPendingParameters();
            for (size_t i = 0; i < pendingParams.count; ++i)
            {
                auto iParam = pendingParams.buffer[i];
                assert(iParam        != nullptr);
                assert(iParam->value != nullptr);

                iParam->value->SetOnCurrentShader(iParam->key);
            }


            // It's important that pending parameters are cleared after setting so that they aren't set again
            // the next time it is made current.
            RendererState.CurrentShader->ClearPendingParameters();
        }
    }

    void Renderer::BindCurrentShaderTextures()
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            // Different texture types need to be bound to different texture units to be usable in the shader. We use an offset to do this.
            for (size_t i = 0; i < RendererState.CurrentShader->currentTextures.count; ++i)
            {
                auto iTexture    = RendererState.CurrentShader->currentTextures.buffer[i];
                auto &attachment = iTexture->value;

                glActiveTexture(GL_TEXTURE0 + i);

                if (attachment.type == ShaderParameterType_Texture2D)
                {
                    auto texture2D = static_cast<Texture2D*>(attachment.texture);
                    if (texture2D != nullptr)
                    {
                        if (texture2D->GetTarget() == Texture2DTarget_Default)
                        {
                            glEnable(GL_TEXTURE_2D);
                            glBindTexture(GL_TEXTURE_2D, static_cast<OpenGL20::Texture2D*>(texture2D->GetRendererData())->object);
                        }
                        else
                        {
                            // We can't bind a cube map face as a texture2D.
                        }
                    }
                }
                else if (attachment.type == ShaderParameterType_TextureCube)
                {
                    auto textureCube = static_cast<TextureCube*>(attachment.texture);
                    if (textureCube != nullptr)
                    {
                        glEnable(GL_TEXTURE_CUBE_MAP);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<OpenGL20::TextureCube*>(textureCube->GetRendererData())->object);
                    }
                }
            }
        }
    }


    void Renderer::SetShaderParameter(const char* paramName, Texture2D* value)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            auto iTexture = RendererState.CurrentShader->currentTextures.Find(paramName);
            if (iTexture != nullptr)
            {
                auto &attachment = iTexture->value;

                // If the texture is the same, fall through.
                if (attachment.texture != value)
                {
                    // We need to keep track of the old texture so we can check if the shader is using it elsewhere and then call Texture2D->OnShaderDetached() if appropriate.
                    auto oldTexture    = static_cast<Texture2D*>(attachment.texture);
                    attachment.texture = value;

                    // All we need to do is change the binding for the texture.
                    glActiveTexture(GL_TEXTURE0 + iTexture->index);
                    OpenGL20::BindTexture(value);

                    // Now we need to let the shader know about the old texture.
                    RendererState.CurrentShader->OnTextureParameterChanged(oldTexture);
                }
            }
            else
            {
                // The texture doesn't exist. We need to add it and then reset the uniform for every texture. We do this because we use a map to store the
                // texture units, which will be rearranged as an entry is added.
                RendererState.CurrentShader->currentTextures.Add(paramName, Shader::AttachedTexture(ShaderParameterType_Texture2D, value));
                Renderer::BindCurrentShaderTextures();

                for (size_t i = 0; i < RendererState.CurrentShader->currentTextures.count; ++i)
                {
                    GLint location = glGetUniformLocation(rendererData->program, RendererState.CurrentShader->currentTextures.buffer[i]->key);
                    glUniform1i(location, static_cast<GLint>(i));
                }
            }


            // The new texture needs to be aware that it is attached to a shader.
            if (value != nullptr)
            {
                value->OnAttachToShader(*RendererState.CurrentShader);
            }
        }
    }

    void Renderer::SetShaderParameter(const char* paramName, TextureCube* value)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            auto iTexture = RendererState.CurrentShader->currentTextures.Find(paramName);
            if (iTexture != nullptr)
            {
                auto &attachment = iTexture->value;

                // If the texture is the same, fall through.
                if (attachment.texture != value)
                {
                    // We need to keep track of the old texture so we can check if the shader is using it elsewhere and then call Texture2D->OnShaderDetached() if appropriate.
                    auto oldTexture = static_cast<TextureCube*>(attachment.texture);
                    attachment.texture = value;

                    // All we need to do is change the binding for the texture.
                    glActiveTexture(GL_TEXTURE0 + iTexture->index);
                    OpenGL20::BindTexture(value);

                    // Now we need to let the shader know about the old texture.
                    RendererState.CurrentShader->OnTextureParameterChanged(oldTexture);
                }
            }
            else
            {
                // The texture doesn't exist. We need to add it and then reset the uniform for every texture. We do this because we use a map to store the
                // texture units, which will be rearranged as an entry is added.
                RendererState.CurrentShader->currentTextures.Add(paramName, Shader::AttachedTexture(ShaderParameterType_TextureCube, value));
                Renderer::BindCurrentShaderTextures();

                for (size_t i = 0; i < RendererState.CurrentShader->currentTextures.count; ++i)
                {
                    const char* name = RendererState.CurrentShader->currentTextures.buffer[i]->key;

                    GLint location = glGetUniformLocation(rendererData->program, name);
                    glUniform1i(location, static_cast<GLint>(i));
                }
            }


            // The new texture needs to be aware that it is attached to a shader.
            if (value != nullptr)
            {
                value->OnAttachToShader(*RendererState.CurrentShader);
            }
        }
    }



    void Renderer::SetShaderParameter(const char *paramName, float x)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLuint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform1f(location, x);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform2f(location, x, y);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform3f(location, x, y, z);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z, float w)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform4f(location, x, y, z, w);
        }
    }

    // Column-major for matrices.
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat2 &value)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix2fv(location, 1, false, &value[0][0]);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat3 &value)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix3fv(location, 1, false, &value[0][0]);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat4 &value)
    {
        auto rendererData = static_cast<OpenGL20::Shader*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr && rendererData->program != 0)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix4fv(location, 1, false, &value[0][0]);
        }
    }


    void Renderer::SetDrawBuffers(size_t count, int *buffers)
    {
        glDrawBuffers((GLsizei)count, ToOpenGLDrawBuffers(count, buffers));
    }

    void Renderer::AttachColourBuffer(Texture2D &colourBuffer, size_t colourBufferIndex)
    {
        if (RendererState.CurrentFramebuffer != nullptr)
        {
            RendererState.CurrentFramebuffer->AttachColourBuffer(&colourBuffer, colourBufferIndex, true);
        }
    }

    void Renderer::DetachColourBuffer(size_t colourBufferIndex)
    {
        if (RendererState.CurrentFramebuffer != nullptr)
        {
            RendererState.CurrentFramebuffer->DetachColourBuffer(colourBufferIndex, true);
        }
    }

    void Renderer::AttachDepthStencilBuffer(Texture2D &depthStencilBuffer)
    {
        if (RendererState.CurrentFramebuffer != nullptr)
        {
            RendererState.CurrentFramebuffer->AttachDepthStencilBuffer(&depthStencilBuffer, true);
        }
    }

    void Renderer::DetachDepthStencilBuffer()
    {
        if (RendererState.CurrentFramebuffer != nullptr)
        {
            RendererState.CurrentFramebuffer->DetachDepthStencilBuffer(true);
        }
    }



    void Renderer::DrawGUI(const GTGUI::Server &gui)
    {
        GUIRenderer::DrawGUI(gui);
    }


    void Renderer::FramebufferBlit(Framebuffer* sourceFramebuffer, unsigned int sourceWidth, unsigned int sourceHeight,
                                   Framebuffer* destFramebuffer,   unsigned int destWidth,   unsigned int destHeight)
    {
        auto sourceGL = sourceFramebuffer != nullptr ? static_cast<OpenGL20::Framebuffer*>(sourceFramebuffer->GetRendererData()) : nullptr;
        auto destGL   = destFramebuffer   != nullptr ? static_cast<OpenGL20::Framebuffer*>(  destFramebuffer->GetRendererData()) : nullptr;

        glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, sourceGL ? sourceGL->object : 0);
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, destGL   ?   destGL->object : 0);

        glBlitFramebufferEXT(0, 0, static_cast<GLint>(sourceWidth), static_cast<GLint>(sourceHeight), 0, 0, static_cast<GLint>(destWidth), static_cast<GLint>(destHeight), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }



    // Support.
    bool Renderer::SupportFloatTextures()
    {
        return GTGL_ARB_texture_float != 0;
    }

    bool Renderer::SupportFramebufferBlit()
    {
        return GTGL_EXT_framebuffer_blit != 0;
    }

    size_t Renderer::GetMaxColourAttachments()
    {
        return static_cast<size_t>(RendererCaps.MaxColourAttachments);
    }

    size_t Renderer::GetMaxDrawBuffers()
    {
        return static_cast<size_t>(RendererCaps.MaxDrawBuffers);
    }
}



/*** Helpers ***/
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>

namespace GTEngine
{
    void Renderer::Helpers::DrawFullscreenQuad(Texture2D* texture)
    {
        if (texture == nullptr)
        {
            texture = Texture2DLibrary::Get1x1BlackTexture();
        }

        Renderer::DisableDepthTest();             // No need for depth testing.
        Renderer::DisableDepthWrites();

        // Here we could apply a post-process effect if we wanted... Instead, we're just going to draw a simple quad
        // using the engine's default fullscreen quad shader. We set the texture to the viewport renderer's colour
        // attachment.
        Renderer::SetShader(ShaderLibrary::GetFullscreenQuadShader());
        Renderer::SetShaderParameter("Texture", texture);

        Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());

        Renderer::EnableDepthTest();              // We best re-enable depth testing...
        Renderer::EnableDepthWrites();
    }
}
