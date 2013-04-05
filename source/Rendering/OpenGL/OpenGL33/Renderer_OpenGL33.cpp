// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>


#include <gtgl/gtgl.h>

#if defined(GTCORE_PLATFORM_WINDOWS)
#include <WinGDI.h>
#else
#include <GTCore/Windowing/X11/X11.hpp>
#endif


#include "../../RCCache.hpp"
#include "../../RCQueue.hpp"
#include "../RendererCaps.hpp"
#include "../Debugging_OpenGL.hpp"
#include "../TypeConversion.hpp"
#include "State_OpenGL33.hpp"
#include "ServerState_OpenGL33.hpp"
#include "VertexArray_OpenGL33.hpp"
#include "Texture2D_OpenGL33.hpp"
#include "TextureCube_OpenGL33.hpp"
#include "Shader_OpenGL33.hpp"
#include "Framebuffer_OpenGL33.hpp"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a general explanation on how the caching system works.
//
// Conceptually, the idea is that each individual call has it's own cachable version. In reality this is too inefficient because there exists
// overhead when performing the actual caching. Instead, calls are batched based on their general type. For example, many state changes don't
// affect other state changes. Thus, state changes can often be performed in any order. If two state changes occur straight after each other,
// they can be batched into a single call instead of being separated into separate calls.
//
// Here are the different categories of calls.
//      Global State (glClearColor, glEnable/glDisable, bindings, etc)
//      Texture State
//      Buffer State
//      Shader State
//      Framebuffer State
//      Drawing (glClear, glDrawElements, etc)
//
// You will see how many categories don't actually affect each other. For example, texture bindings will never be affected by basic state
// changes. What we do is keep track of a pointer to a cached call that can be used for a call that falls into that same category. When that
// pointer is null, we just create a new cached call. It will be set to null when another call is performed where it can not longer be used
// with batching.
//
// The state of the renderer is only relevant for the construction of the cached calls. When calls are actually getting executed, they won't
// ever care about state. Thus, we use a State object that will contain the state at the time in which a cached call is created. This state
// object is used in determine when a particular operation actually needs to be performed. For example, if the current texture is the same as
// the one getting bound, it'll just skip over the call.
//
// The aforementioned state object will also contain pointers to the different cached calls where an individual call can be cached. These
// pointers will be cleared to null when the call caches are swapped. When the pointers a null, it means the call can not be batched and a
// new call will need to be created and cached.




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

    /// The current state.
    static State_OpenGL33 State;


    /// The two call caches. The back call back is identified with BackCallCacheIndex.
    static RCQueue CallCaches[2];

    /// The two call caches for creating resources.
    static RCQueue ResourceCreationCallCaches[2];

    /// The two call caches for deleting resources.
    static RCQueue ResourceDeletionCallCaches[2];

    /// The index identifying the back call cache.
    static unsigned int BackCallCacheIndex = 0;


    /// The mutex for synching applicable resource creation operations.
    static GTCore::Mutex ResourceCreationLock;

    /// The mutex for synching applicable resource deletion operations.
    static GTCore::Mutex ResourceDeletionLock;


    /// The caches for individual commands. There are two of each - one for the back and one for the front.
    static struct _RCCaches
    {
        // State RCs.
        RCCache<RCSetGlobalState>      RCSetGlobalStateCache;
        RCCache<RCSetVertexArrayState> RCSetVertexArrayStateCache;
        RCCache<RCSetTextureState>     RCSetTextureStateCache;
        RCCache<RCSetShaderState>      RCSetShaderStateCache;
        RCCache<RCSetFramebufferState> RCSetFramebufferStateCache;

        // Drawing RCs.
        RCCache<RCClear>               RCClearCache;
        RCCache<RCDraw>                RCDrawCache;

        // Create and Delete RCs.
        RCCache<RCCreateVertexArray>   RCCreateVertexArrayCache;
        RCCache<RCDeleteVertexArray>   RCDeleteVertexArrayCache;
        RCCache<RCCreateTexture>       RCCreateTextureCache;
        RCCache<RCDeleteTexture>       RCDeleteTextureCache;
        RCCache<RCCreateShader>        RCCreateShaderCache;
        RCCache<RCDeleteShader>        RCDeleteShaderCache;
        RCCache<RCCreateFramebuffer>   RCCreateFramebufferCache;
        RCCache<RCDeleteFramebuffer>   RCDeleteFramebufferCache;


        _RCCaches()
            : RCSetGlobalStateCache(), RCSetVertexArrayStateCache(), RCSetTextureStateCache(), RCSetShaderStateCache(), RCSetFramebufferStateCache(),
              RCClearCache(), RCDrawCache(),
              RCCreateVertexArrayCache(), RCDeleteVertexArrayCache(),
              RCCreateTextureCache(),     RCDeleteTextureCache(),
              RCCreateShaderCache(),      RCDeleteShaderCache(),
              RCCreateFramebufferCache(), RCDeleteFramebufferCache()
        {
        }

        void Clear(bool clearResources = false)
        {
            this->RCSetGlobalStateCache.Reset();
            this->RCSetVertexArrayStateCache.Reset();
            this->RCSetTextureStateCache.Reset();
            this->RCSetShaderStateCache.Reset();
            this->RCSetFramebufferStateCache.Reset();


            this->RCClearCache.Reset();
            this->RCDrawCache.Reset();


            if (clearResources)
            {
                ResourceCreationLock.Lock();
                {
                    this->RCCreateVertexArrayCache.Reset();
                    this->RCCreateTextureCache.Reset();
                    this->RCCreateShaderCache.Reset();
                    this->RCCreateFramebufferCache.Reset();
                }
                ResourceCreationLock.Unlock();


                ResourceDeletionLock.Lock();
                {
                    this->RCDeleteVertexArrayCache.Reset();
                    this->RCDeleteTextureCache.Reset();
                    this->RCDeleteShaderCache.Reset();
                    this->RCDeleteFramebufferCache.Reset();
                }
                ResourceDeletionLock.Unlock();
            }
        }

    }RCCaches[2];



    bool Renderer::Startup()
    {
        assert(!IsInitialised);
        {
            size_t contextAttribs[] =
            {
                GTGL_CONTEXT_MAJOR_VERSION,      3,
                GTGL_CONTEXT_MINOR_VERSION,      3,
                GTGL_CONTEXT_FORWARD_COMPATIBLE, GL_TRUE,

            #if !defined(NDEBUG)
                GTGL_CONTEXT_DEBUG,              GL_TRUE,
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
                Log("    ARB_get_program_binary:             %s", GTGL_ARB_get_program_binary             ? "yes" : "no");
                Log("    ARB_separate_shader_objects:        %s", GTGL_ARB_separate_shader_objects        ? "yes" : "no");
                Log("    ARB_shader_subroutine:              %s", GTGL_ARB_shader_subroutine              ? "yes" : "no");
                Log("    EXT_texture_compression_s3tc:       %s", GTGL_EXT_texture_compression_s3tc       ? "yes" : "no");
                Log("    EXT_texture_filter_anisotropic:     %s", GTGL_EXT_texture_filter_anisotropic     ? "yes" : "no");
                Log("    NV_bindless_texture:                %s", GTGL_NV_bindless_texture                ? "yes" : "no");
                Log("    NV_shader_buffer_load:              %s", GTGL_NV_shader_buffer_load              ? "yes" : "no");
                Log("    NV_vertex_buffer_unified_memory:    %s", GTGL_NV_vertex_buffer_unified_memory    ? "yes" : "no");


            #if !defined(NDEBUG)
                gtglEnableSynchronousDebugOutput();
                gtglSetDebugMessageCallback(OpenGLDebugMessageHandler);
            #endif


                // Here we'll grab the renderer's capabilities.
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,   &RendererCaps.MaxColourAttachments);
                glGetIntegerv(GL_MAX_DRAW_BUFFERS,        &RendererCaps.MaxDrawBuffers);
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &RendererCaps.MaxTextureUnits);


                // Now we'll set some defaults.
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


                // The default VAO/VBO.
                glGenVertexArrays(1, &ServerState_DefaultVAO);
                glGenBuffers(1, &ServerState_DefaultVBO);
                glGenBuffers(1, &ServerState_DefaultIBO);

                glBindVertexArray(ServerState_DefaultVAO);
                glBindBuffer(GL_ARRAY_BUFFER,         ServerState_DefaultVBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ServerState_DefaultIBO);

                ServerState_GL_VERTEX_ARRAY_BINDING = ServerState_DefaultVAO;
                ServerState_GL_ARRAY_BUFFER_BINDING = ServerState_DefaultVBO;



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

    void Renderer::Shutdown()
    {
        assert(IsInitialised);
        {
            // TODO: Delete renderer-created objects.


            // Both caches should be cleared.
            CallCaches[0].Clear();
            CallCaches[1].Clear();

            ResourceCreationCallCaches[0].Clear();
            ResourceCreationCallCaches[1].Clear();
            ResourceDeletionCallCaches[0].Clear();
            ResourceDeletionCallCaches[1].Clear();


            // The client-side state needs to be shutdown.
            State.Shutdown();


            // GTGL needs to be shutdown.
            gtglShutdown();
            OpenGLContext = nullptr;


            // Finally, we need to mark the renderer as uninitialised.
            IsInitialised = false;
        }
    }


    GTCore::Window* Renderer::CreateWindow()
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

    void Renderer::SetCurrentWindow(GTCore::Window *window)
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

    void Renderer::SwapCallCaches()
    {
        // 1) Swap the back call cache index.
        BackCallCacheIndex = !BackCallCacheIndex;

        // 2) Clear the new back cache in preparation for filling by another thread. Note how we don't clear the resource creation and deletion
        //    call caches here. The reason is because they are cleared in ExecuteCallCache().
        CallCaches[BackCallCacheIndex].Clear();

        ResourceCreationLock.Lock();
        ResourceCreationCallCaches[BackCallCacheIndex].Clear();
        ResourceCreationLock.Unlock();

        ResourceDeletionLock.Lock();
        ResourceDeletionCallCaches[BackCallCacheIndex].Clear();
        ResourceDeletionLock.Unlock();



        // 3) Clear the sub-caches.
        RCCaches[BackCallCacheIndex].Clear();


        // 4) Cleanup deleted objects.
        State.ClearDeletedOpenGLObjects();


        // 5) Reset all of the "current" commands in order to force new ones in preparation for the new frame.
        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
    }

    void Renderer::ExecuteCallCache()
    {
        // 1) Create resources. We want to lock and clear this all at the same time.
        ResourceCreationLock.Lock();
        ResourceCreationCallCaches[!BackCallCacheIndex].Execute();
        //ResourceCreationCallCaches[!BackCallCacheIndex].Clear();
        ResourceCreationLock.Unlock();

        // 2) Normal calls.
        CallCaches[!BackCallCacheIndex].Execute();

        // 3) Delete resources. We want to lock, execute and clear this all at the same time.
        ResourceDeletionLock.Lock();
        ResourceDeletionCallCaches[!BackCallCacheIndex].Execute();
        //ResourceDeletionCallCaches[!BackCallCacheIndex].Clear();
        ResourceDeletionLock.Unlock();
    }

    void Renderer::SwapBuffers()
    {
        if (State.swapIntervalNeedsUpdate)
        {
            gtglSwapInterval(State.swapInterval);
            State.swapIntervalNeedsUpdate = false;
        }

        gtglSwapBuffers();
    }





    /////////////////////////////////////////////////////////////
    // Cached Calls

    #define UPDATE_CURRENT_RC(renderCommandName) \
        if (State.current##renderCommandName == nullptr) \
        { \
            State.current##renderCommandName = &RCCaches[BackCallCacheIndex].renderCommandName##Cache.Acquire(); \
            CallCaches[BackCallCacheIndex].Append(*State.current##renderCommandName); \
        }



    ///////////////////////////
    // Simple State Changes

    void Renderer::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetViewport(x, y, width, height);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetScissor(x, y, width, height);
        }


        State.currentRCClear = nullptr;
    }


    void Renderer::SetClearColour(float r, float g, float b, float a)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearColour(r, g, b, a);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer::SetClearDepth(float depth)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearDepth(depth);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer::SetClearStencil(int stencil)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearStencil(stencil);
        }


        State.currentRCClear = nullptr;
    }


    void Renderer::SetCurrentShader(Shader* programToMakeCurrent)
    {
        if (programToMakeCurrent != nullptr)
        {
            auto programStateToMakeCurrent = static_cast<Shader_OpenGL33*>(programToMakeCurrent)->GetOpenGLState();
            if (programStateToMakeCurrent != State.currentProgramState)
            {
                UPDATE_CURRENT_RC(RCSetGlobalState);
                assert(State.currentRCSetGlobalState != nullptr);
                {
                    State.currentRCSetGlobalState->SetCurrentShader(programStateToMakeCurrent);
                    State.currentProgramState = programStateToMakeCurrent;
                }
            }
        }
    }

    void Renderer::SetCurrentFramebuffer(Framebuffer* framebufferToMakeCurrent)
    {
        FramebufferState_OpenGL33* framebufferState = nullptr;
        if (framebufferToMakeCurrent != nullptr)
        {
            framebufferState = static_cast<Framebuffer_OpenGL33*>(framebufferToMakeCurrent)->GetOpenGLState();
        }

        if (framebufferState != State.currentFramebufferState)
        {
            UPDATE_CURRENT_RC(RCSetGlobalState);
            assert(State.currentRCSetGlobalState != nullptr);
            {
                State.currentRCSetGlobalState->SetCurrentFramebuffer(framebufferState);
                State.currentFramebufferState = framebufferState;
            }
        }
    }



    void Renderer::EnableScissorTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
    }

    void Renderer::DisableScissorTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
    }


    void Renderer::EnableBlending()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_BLEND);
        }
    }

    void Renderer::DisableBlending()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_BLEND);
        }
    }

    void Renderer::SetBlendFunction(BlendFunc sfactor, BlendFunc dfactor)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendFunction(ToOpenGLBlendFunc(sfactor), ToOpenGLBlendFunc(dfactor));
        }
    }

    void Renderer::SetBlendEquation(BlendEquation equation)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendEquation(ToOpenGLBlendEquation(equation));
        }
    }

    void Renderer::SetBlendColour(float r, float g, float b, float a)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendColour(r, g, b, a);
        }
    }


    void Renderer::EnableAlphaTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_ALPHA_TEST);
        }
    }

    void Renderer::DisableAlphaTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_ALPHA_TEST);
        }
    }

    void Renderer::SetAlphaTestFunction(RendererFunction function, float ref)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetAlphaTestFunction(ToOpenGLFunc(function), static_cast<GLclampf>(ref));
        }
    }


    void Renderer::EnableColourWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableColourWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer::DisableColourWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableColourWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer::EnableDepthWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableDepthWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer::DisableDepthWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableDepthWrites();
        }

        State.currentRCClear = nullptr;
    }


    void Renderer::EnableDepthTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_DEPTH_TEST);
        }
    }

    void Renderer::DisableDepthTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_DEPTH_TEST);
        }
    }

    void Renderer::SetDepthFunction(RendererFunction function)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetDepthFunction(ToOpenGLFunc(function));
        }
    }



    void Renderer::EnableStencilTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_STENCIL_TEST);
        }
    }

    void Renderer::DisableStencilTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_STENCIL_TEST);
        }
    }

    void Renderer::SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned int mask)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
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

            State.currentRCSetGlobalState->SetStencilMask(face, static_cast<GLuint>(mask));
        }
    }

    void Renderer::SetStencilFuncSeparate(bool frontFace, bool backFace, RendererFunction func, int ref, unsigned int mask)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
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

            State.currentRCSetGlobalState->SetStencilFunc(face, ToOpenGLFunc(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
        }
    }

    void Renderer::SetStencilOpSeparate(bool frontFace, bool backFace, StencilOp stencilFail, StencilOp depthFail, StencilOp pass)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
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

            State.currentRCSetGlobalState->SetStencilOp(face, ToOpenGLStencilOp(stencilFail), ToOpenGLStencilOp(depthFail), ToOpenGLStencilOp(pass));
        }
    }



    void Renderer::SetFaceCulling(bool cullFront, bool cullBack)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            GLenum face = GL_NONE;
            if (cullFront && cullBack)
            {
                face = GL_FRONT_AND_BACK;
            }
            else
            {
                if (cullFront)
                {
                    face = GL_FRONT;
                }
                else
                {
                    face = GL_BACK;
                }
            }

            State.currentRCSetGlobalState->SetFaceCulling(face);
        }
    }


    void Renderer::EnablePolygonOffset(PolygonMode mode)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
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

            State.currentRCSetGlobalState->Enable(modeGL);
        }
    }

    void Renderer::DisablePolygonOffset(PolygonMode mode)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
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

            State.currentRCSetGlobalState->Disable(modeGL);
        }
    }

    void Renderer::SetPolygonMode(bool frontFaces, bool backFaces, PolygonMode mode)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            GLenum faceGL;
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


            GLenum modeGL;
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

            State.currentRCSetGlobalState->SetPolygonMode(faceGL, modeGL);
        }
    }

    void Renderer::SetPolygonOffset(float factor, float units)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
        }
    }


    void Renderer::SetDrawBuffers(size_t count, int* buffers)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetDrawBuffers(count, buffers);
        }

        State.currentRCClear = nullptr;
    }


    void Renderer::SetSwapInterval(int interval)
    {
        State.swapInterval            = interval;
        State.swapIntervalNeedsUpdate = true;
    }



    ///////////////////////////
    // Drawing

    void Renderer::Clear(unsigned int bufferMask)
    {
        UPDATE_CURRENT_RC(RCClear);
        assert(State.currentRCClear != nullptr);
        {
            GLbitfield glmask = 0;

            if (bufferMask & BufferType_Colour)
            {
                glmask |= GL_COLOR_BUFFER_BIT;
            }
            if (bufferMask & BufferType_Depth)
            {
                glmask |= GL_DEPTH_BUFFER_BIT;
            }
            if (bufferMask & BufferType_Stencil)
            {
                glmask |= GL_STENCIL_BUFFER_BIT;
            }

            State.currentRCClear->Clear(glmask);
        }


        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
    }

    void Renderer::Draw(const VertexArray &vertexArray, DrawMode mode)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL33 &>(vertexArray);

        // If the vertex array needs anything updated, do so now.
        if (vertexArrayGL33.DoesVertexDataNeedUpdating())
        {
            Renderer::PushVertexArrayVertexData(vertexArray);
        }
        if (vertexArrayGL33.DoesIndexDataNeedUpdating())
        {
            Renderer::PushVertexArrayIndexData(vertexArray);
        }



        UPDATE_CURRENT_RC(RCDraw);
        assert(State.currentRCDraw != nullptr);
        {
            State.currentRCDraw->Draw(vertexArrayGL33.GetOpenGLObjectPtr(), vertexArrayGL33.GetOpenGLVertexObjectPtr(), ToOpenGLDrawMode(mode), static_cast<GLsizei>(vertexArrayGL33.GetIndexCount()));
        }

        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
    }

    void Renderer::Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
        UPDATE_CURRENT_RC(RCDraw);
        assert(State.currentRCDraw != nullptr);
        {
            State.currentRCDraw->Draw(vertices, static_cast<GLsizei>(vertexCount), indices, static_cast<GLsizei>(indexCount), format, ToOpenGLDrawMode(mode));
        }

        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
    }


    void Renderer::Draw(const float* vertices, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
        size_t vertexCount = 0;

        // We need to determine the vertex count by looking at the indices. We need the vertex count so we can know how much data to copy.
        for (size_t i = 0; i < indexCount; ++i)
        {
            if (indices[i] > vertexCount)
            {
                vertexCount = indices[i];
            }
        }

        // We just +1 to make the vertex count 1 based.
        vertexCount += 1;


        // Now we just draw like normal.
        Renderer::Draw(vertices, vertexCount, indices, indexCount, format, mode);
    }



    ///////////////////////////
    // Resources

    VertexArray* Renderer::CreateVertexArray(VertexArrayUsage usage, const VertexFormat &format)
    {
        State.instantiatedVertexArrayObjects.PushBack(new GLuint(0));
        GLuint* vertexArrayObject  = State.instantiatedVertexArrayObjects.GetBack();

        State.instantiatedBufferObjects.PushBack(new GLuint(0));
        GLuint* vertexBufferObject = State.instantiatedBufferObjects.GetBack();

        State.instantiatedBufferObjects.PushBack(new GLuint(0));
        GLuint* indexBufferObject  = State.instantiatedBufferObjects.GetBack();


        ResourceCreationLock.Lock();
        {
            auto &command = RCCaches[BackCallCacheIndex].RCCreateVertexArrayCache.Acquire();
            command.CreateVertexArray(vertexArrayObject, vertexBufferObject, indexBufferObject, format);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
        }
        ResourceCreationLock.Unlock();




        return new VertexArray_OpenGL33(usage, format, vertexArrayObject, vertexBufferObject, indexBufferObject);
    }

    void Renderer::DeleteVertexArray(VertexArray* vertexArrayToDelete)
    {
        auto vertexArrayToDeleteGL33 = static_cast<VertexArray_OpenGL33*>(vertexArrayToDelete);
        if (vertexArrayToDeleteGL33 != nullptr)
        {
            // The OpenGL objects need to be marked for deletion.
            GLuint* vertexArrayObject  = vertexArrayToDeleteGL33->GetOpenGLObjectPtr();
            GLuint* vertexBufferObject = vertexArrayToDeleteGL33->GetOpenGLVertexObjectPtr();
            GLuint* indexBufferObject  = vertexArrayToDeleteGL33->GetOpenGLIndexObjectPtr();

            assert(vertexArrayObject  != nullptr);
            assert(vertexBufferObject != nullptr);
            assert(indexBufferObject  != nullptr);
            {
                ResourceDeletionLock.Lock();
                {
                    auto &command = RCCaches[BackCallCacheIndex].RCDeleteVertexArrayCache.Acquire();
                    command.DeleteVertexArray(vertexArrayObject, vertexBufferObject, indexBufferObject);

                    ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
                }
                ResourceDeletionLock.Unlock();



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkVertexArrayObjectAsDeleted(vertexArrayObject);
                State.MarkBufferObjectAsDeleted(vertexBufferObject);
                State.MarkBufferObjectAsDeleted(indexBufferObject);
            }


            // We can safely delete the main object at this point.
            delete vertexArrayToDelete;
        }
    }

    void Renderer::PushVertexArrayVertexData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL33 &>(vertexArray);
        {
            GLuint* vertexArrayObject  = vertexArrayGL33.GetOpenGLObjectPtr();
            GLuint* vertexBufferObject = vertexArrayGL33.GetOpenGLVertexObjectPtr();

            assert(vertexArrayObject  != nullptr);
            assert(vertexBufferObject != nullptr);
            {
                if (State.currentRCSetVertexArrayState == nullptr || State.currentRCSetVertexArrayState->GetVertexArrayObject() != vertexArrayObject)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }


                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetVertexData(vertexArrayObject, vertexBufferObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    vertexArrayGL33.MarkVertexDataAsUpdated();
                }
            }
        }
    }

    void Renderer::PushVertexArrayIndexData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL33 &>(vertexArray);
        {
            GLuint* vertexArrayObject = vertexArrayGL33.GetOpenGLObjectPtr();
            assert(vertexArrayObject != nullptr);
            {
                if (State.currentRCSetVertexArrayState == nullptr || State.currentRCSetVertexArrayState->GetVertexArrayObject() != vertexArrayObject)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }


                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetIndexData(vertexArrayObject, vertexArray.GetIndexDataPtr(), vertexArray.GetIndexCount(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    vertexArrayGL33.MarkIndexDataAsUpdated();
                }
            }
        }
    }

    void Renderer::PushVertexArrayData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL33 &>(vertexArray);
        {
            GLuint* vertexArrayObject  = vertexArrayGL33.GetOpenGLObjectPtr();
            GLuint* vertexBufferObject = vertexArrayGL33.GetOpenGLVertexObjectPtr();

            assert(vertexArrayObject  != nullptr);
            assert(vertexBufferObject != nullptr);
            {
                if (State.currentRCSetVertexArrayState == nullptr || State.currentRCSetVertexArrayState->GetVertexArrayObject() != vertexArrayObject)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }


                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetVertexData(vertexArrayObject, vertexBufferObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    State.currentRCSetVertexArrayState->SetIndexData( vertexArrayObject, vertexArray.GetIndexDataPtr(),  vertexArray.GetIndexCount(),  ToOpenGLBufferUsage(vertexArray.GetUsage()));

                    vertexArrayGL33.MarkVertexDataAsUpdated();
                    vertexArrayGL33.MarkIndexDataAsUpdated();
                }
            }
        }
    }



    TextureState_OpenGL33* Renderer_CreateOpenGL33Texture()
    {
        State.instantiatedTextureObjects.PushBack(new TextureState_OpenGL33);
        TextureState_OpenGL33* textureState  = State.instantiatedTextureObjects.GetBack();

        ResourceCreationLock.Lock();
        {
            auto &command = RCCaches[BackCallCacheIndex].RCCreateTextureCache.Acquire();
            command.CreateTexture(textureState);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
        }
        ResourceCreationLock.Unlock();


        return textureState;
    }

    void Renderer_DeleteOpenGL33Texture(TextureState_OpenGL33* textureStateToDelete)
    {
        assert(textureStateToDelete  != nullptr);
        {
            ResourceDeletionLock.Lock();
            {
                auto &command = RCCaches[BackCallCacheIndex].RCDeleteTextureCache.Acquire();
                command.DeleteTexture(textureStateToDelete);

                ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
            }
            ResourceDeletionLock.Unlock();



            // The objects need to be marked for deletion, but not actually deleted yet.
            State.MarkTextureObjectAsDeleted(textureStateToDelete);
        }
    }

    void Renderer_SetOpenGL33TextureFilter(TextureState_OpenGL33* textureState, GLenum textureTarget, TextureFilter minification, TextureFilter magnification)
    {
        assert(textureState != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureFilter(textureState, textureTarget, ToOpenGLTextureFilter(minification), ToOpenGLTextureFilter(magnification));
            }
        }
    }

    void Renderer_SetOpenGL33TextureAnisotropy(TextureState_OpenGL33* textureState, GLenum textureTarget, unsigned int anisotropy)
    {
        assert(textureState != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureAnisotropy(textureState, textureTarget, static_cast<GLint>(anisotropy));
            }
        }
    }

    void Renderer_SetOpenGL33TextureWrapMode(TextureState_OpenGL33* textureState, GLenum textureTarget, TextureWrapMode wrapMode)
    {
        assert(textureState != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureWrapMode(textureState, textureTarget, ToOpenGLWrapMode(wrapMode));
            }
        }
    }

    void Renderer_SetOpenGL33TextureMipmapLevels(TextureState_OpenGL33* textureState, GLenum textureTarget, unsigned int baseLevel, unsigned int maxLevel)
    {
        assert(textureState != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureMipmapLevels(textureState, textureTarget, static_cast<GLint>(baseLevel), static_cast<GLint>(maxLevel));
            }
        }
    }

    void Renderer_GenerateOpenGL33TextureMipmaps(TextureState_OpenGL33* textureState, GLenum textureTarget)
    {
        assert(textureState != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->GenerateTextureMipmaps(textureState, textureTarget);
            }
        }
    }




    Texture2D* Renderer::CreateTexture2D()
    {
        return new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_2D);
    }

    TextureCube* Renderer::CreateTextureCube()
    {
        // The texture object for each of the faces is actually the main cube map object.
        TextureState_OpenGL33* textureStateGL = Renderer_CreateOpenGL33Texture();

        auto positiveX = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        auto negativeX = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        auto positiveY = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        auto negativeY = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        auto positiveZ = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        auto negativeZ = new Texture2D_OpenGL33(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        return new TextureCube_OpenGL33(textureStateGL, positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
    }


    void Renderer::DeleteTexture2D(Texture2D* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<Texture2D_OpenGL33*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL33Texture(textureToDeleteGL33->GetOpenGLState());

            // We can safely delete the main object at this point.
            delete textureToDelete;
        }
    }

    void Renderer::DeleteTextureCube(TextureCube* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<TextureCube_OpenGL33*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL33Texture(textureToDeleteGL33->GetOpenGLState());

            // We can safely delete the main object at this point.
            delete textureToDeleteGL33->PositiveX;
            delete textureToDeleteGL33->NegativeX;
            delete textureToDeleteGL33->PositiveY;
            delete textureToDeleteGL33->NegativeY;
            delete textureToDeleteGL33->PositiveZ;
            delete textureToDeleteGL33->NegativeZ;

            delete textureToDelete;
        }
    }


    void Renderer::PushTexture2DData(const Texture2D &texture, int mipmapIndex)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            GLenum textureTarget = textureGL33.GetTarget();
            auto   textureState  = textureGL33.GetOpenGLState();

            assert(textureState != nullptr);
            {
                if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
                {
                    State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
                }


                assert(State.currentRCSetTextureState != nullptr);
                {
                    if (mipmapIndex == -1)
                    {
                        for (size_t i = 0; i < texture.GetMipmapCount(); ++i)
                        {
                            auto &mipmap = texture.GetMipmap(i);
                            State.currentRCSetTextureState->SetTexture2DData(textureState, textureTarget, static_cast<int>(i), mipmap.format, mipmap.width, mipmap.height, mipmap.data, mipmap.GetDataSizeInBytes());
                        }
                    }
                    else
                    {
                        auto &mipmap = texture.GetMipmap(mipmapIndex);
                        State.currentRCSetTextureState->SetTexture2DData(textureState, textureTarget, mipmapIndex, mipmap.format, mipmap.width, mipmap.height, mipmap.data, mipmap.GetDataSizeInBytes());
                    }
                }
            }
        }
    }

    void Renderer::PushTextureCubeData(const TextureCube &texture)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            GLenum textureTarget = GL_TEXTURE_CUBE_MAP;
            auto   textureState  = textureGL33.GetOpenGLState();

            assert(textureState != nullptr);
            {
                if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureState() != textureState || State.currentRCSetTextureState->GetTarget() != textureTarget))
                {
                    State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
                }


                assert(State.currentRCSetTextureState != nullptr);
                {
                    auto &positiveX = static_cast<const Texture2D_OpenGL33*>(texture.PositiveX)->GetMipmap(0);
                    auto &negativeX = static_cast<const Texture2D_OpenGL33*>(texture.NegativeX)->GetMipmap(0);
                    auto &positiveY = static_cast<const Texture2D_OpenGL33*>(texture.PositiveY)->GetMipmap(0);
                    auto &negativeY = static_cast<const Texture2D_OpenGL33*>(texture.NegativeY)->GetMipmap(0);
                    auto &positiveZ = static_cast<const Texture2D_OpenGL33*>(texture.PositiveZ)->GetMipmap(0);
                    auto &negativeZ = static_cast<const Texture2D_OpenGL33*>(texture.NegativeZ)->GetMipmap(0);

                    auto width           = positiveX.width;
                    auto height          = positiveX.height;
                    auto format          = positiveX.format;
                    auto dataSizeInBytes = positiveX.GetDataSizeInBytes();

                    State.currentRCSetTextureState->SetTextureCubeData(textureState, format, width, height, dataSizeInBytes,
                        positiveX.data, negativeX.data,
                        positiveY.data, negativeY.data,
                        positiveZ.data, negativeZ.data);
                }
            }
        }
    }


    void Renderer::SetTexture2DFilter(const Texture2D &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureFilter(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), minification, magnification);
        }
    }

    void Renderer::SetTextureCubeFilter(const TextureCube &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureFilter(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, minification, magnification);
        }
    }


    void Renderer::SetTexture2DAnisotropy(const Texture2D &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureAnisotropy(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), anisotropy);
        }
    }

    void Renderer::SetTextureCubeAnisotropy(const TextureCube &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureAnisotropy(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, anisotropy);
        }
    }


    void Renderer::SetTexture2DWrapMode(const Texture2D &texture, TextureWrapMode wrapMode)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureWrapMode(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), wrapMode);
        }
    }

    void Renderer::SetTextureCubeWrapMode(const TextureCube &texture, TextureWrapMode wrapMode)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureWrapMode(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, wrapMode);
        }
    }


    void Renderer::SetTexture2DMipmapLevels(const Texture2D &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureMipmapLevels(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), baseLevel, maxLevel);
        }
    }

    void Renderer::SetTextureCubeMipmapLevels(const TextureCube &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureMipmapLevels(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, baseLevel, maxLevel);
        }
    }


    void Renderer::GenerateTexture2DMipmaps(const Texture2D &texture)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_GenerateOpenGL33TextureMipmaps(textureGL33.GetOpenGLState(), textureGL33.GetTarget());
        }
    }

    void Renderer::GenerateTextureCubeMipmaps(const TextureCube &texture)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_GenerateOpenGL33TextureMipmaps(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP);
        }
    }



    Shader* Renderer::CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource)
    {
        State.instantiatedProgramObjects.PushBack(new ShaderState_OpenGL33);
        auto programState = State.instantiatedProgramObjects.GetBack();


        ResourceCreationLock.Lock();
        {
            auto &command = RCCaches[BackCallCacheIndex].RCCreateShaderCache.Acquire();
            command.CreateShader(programState, vertexShaderSource, fragmentShaderSource, geometryShaderSource);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
        }
        ResourceCreationLock.Unlock();



        return new Shader_OpenGL33(programState, vertexShaderSource, fragmentShaderSource, geometryShaderSource);
    }

    void Renderer::DeleteShader(Shader* shaderToDelete)
    {
        auto shaderToDeleteGL33 = static_cast<Shader_OpenGL33*>(shaderToDelete);
        if (shaderToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            auto programState = shaderToDeleteGL33->GetOpenGLState();

            assert(programState  != nullptr);
            {
                ResourceDeletionLock.Lock();
                {
                    auto &command = RCCaches[BackCallCacheIndex].RCDeleteShaderCache.Acquire();
                    command.DeleteShader(programState);

                    ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
                }
                ResourceDeletionLock.Unlock();



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkProgramObjectAsDeleted(programState);
            }


            // We can safely delete the main object at this point.
            delete shaderToDelete;
        }
    }


    void Renderer::PushPendingUniforms(const Shader &shader)
    {
        auto &shaderGL33 = static_cast<const Shader_OpenGL33 &>(shader);
        {
            auto programState = shaderGL33.GetOpenGLState();
            assert(programState != nullptr);
            {
                if (State.currentRCSetShaderState == nullptr || State.currentRCSetShaderState->GetProgramState() != programState)
                {
                    State.currentRCSetShaderState = &RCCaches[BackCallCacheIndex].RCSetShaderStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetShaderState);
                }


                assert(State.currentRCSetShaderState != nullptr);
                {
                    State.currentRCSetShaderState->SetFloatUniforms(programState);
                    State.currentRCSetShaderState->SetFloat2Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat3Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat4Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat2x2Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat3x3Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat4x4Uniforms(programState);
                    State.currentRCSetShaderState->SetTextureUniforms(programState);

                    programState->ClearPendingUniforms();
                }
            }
        }
    }



    Framebuffer* Renderer::CreateFramebuffer()
    {
        State.instantiatedFramebufferObjects.PushBack(new FramebufferState_OpenGL33);
        auto framebufferState = State.instantiatedFramebufferObjects.GetBack();


        ResourceCreationLock.Lock();
        {
            auto &command = RCCaches[BackCallCacheIndex].RCCreateFramebufferCache.Acquire();
            command.CreateFramebuffer(framebufferState);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
        }
        ResourceCreationLock.Unlock();



        return new Framebuffer_OpenGL33(framebufferState);
    }

    void Renderer::DeleteFramebuffer(Framebuffer* framebufferToDelete)
    {
        auto framebufferToDeleteGL33 = static_cast<Framebuffer_OpenGL33*>(framebufferToDelete);
        if (framebufferToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            auto framebufferState = framebufferToDeleteGL33->GetOpenGLState();

            assert(framebufferState  != nullptr);
            {
                ResourceDeletionLock.Lock();
                {
                    auto &command = RCCaches[BackCallCacheIndex].RCDeleteFramebufferCache.Acquire();
                    command.DeleteFramebuffer(framebufferState);

                    ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
                }
                ResourceDeletionLock.Unlock();



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkFramebufferObjectAsDeleted(framebufferState);
            }


            // We can safely delete the main object at this point.
            delete framebufferToDeleteGL33;
        }
    }


    void Renderer::PushAttachments(const Framebuffer &framebuffer)
    {
        auto &framebufferGL33 = static_cast<const Framebuffer_OpenGL33 &>(framebuffer);
        {
            auto framebufferState = framebufferGL33.GetOpenGLState();
            assert(framebufferState != nullptr);
            {
                if (State.currentRCSetFramebufferState == nullptr || State.currentRCSetFramebufferState->GetFramebufferState() != framebufferState)
                {
                    State.currentRCSetFramebufferState = &RCCaches[BackCallCacheIndex].RCSetFramebufferStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetFramebufferState);
                }


                assert(State.currentRCSetFramebufferState != nullptr);
                {
                    // We just notify the render command of the currently attached buffers. It will detach and switch stuff around appropriately when it's
                    // executed. We can't know at this point which attachments should be attached or detached because the server-side state may be in the
                    // middle of changing on another thread.

                    // Colour Buffers.
                    auto &colourAttachments = framebufferGL33.GetAttachedColourBuffers();
                    for (size_t i = 0; i < colourAttachments.count; ++i)
                    {
                        auto index   = static_cast<GLuint>(colourAttachments.buffer[i]->key);
                        auto texture = static_cast<Texture2D_OpenGL33*>(colourAttachments.buffer[i]->value);

                        assert(texture != nullptr);
                        {
                            State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_COLOR_ATTACHMENT0_EXT + index, texture->GetTarget(), texture->GetOpenGLState());
                        }
                    }

                    // Depth/Stencil.
                    auto depthStencilAttachment = static_cast<const Texture2D_OpenGL33*>(framebufferGL33.GetDepthStencilBuffer());
                    if (depthStencilAttachment != nullptr)
                    {
                        State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_DEPTH_ATTACHMENT_EXT,   depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());
                        State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_STENCIL_ATTACHMENT_EXT, depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());
                    }
                }
            }
        }
    }




    /////////////////////////////////////////////////////////////
    // Support Queries

    bool Renderer::HasFlippedTextures()
    {
        return true;
    }

    unsigned int Renderer::GetMaxColourAttachments()
    {
        return static_cast<unsigned int>(RendererCaps.MaxColourAttachments);
    }

    unsigned int Renderer::GetMaxDrawBuffers()
    {
        return static_cast<unsigned int>(RendererCaps.MaxDrawBuffers);
    }

    unsigned int Renderer::GetMaxTextureUnits()
    {
        return static_cast<unsigned int>(RendererCaps.MaxTextureUnits);
    }
}
