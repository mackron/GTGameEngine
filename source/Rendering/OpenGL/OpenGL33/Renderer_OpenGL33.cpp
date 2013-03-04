// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Renderer2.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Rendering/RCQueue.hpp>
#include <GTEngine/Rendering/RCCache.hpp>


#include <gtgl/gtgl.h>

#if defined(GTCORE_PLATFORM_WINDOWS)
#include <WinGDI.h>
#else
#include <GTCore/Windowing/X11/X11.hpp>
#endif


#include "../RendererCaps.hpp"
#include "../Debugging_OpenGL.hpp"
#include "../TypeConversion.hpp"
#include "State_OpenGL33.hpp"
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
//      Basic State Changes (glClearColor, glEnable/glDisable, etc)
//      Texture Binding
//      Texture State Changes
//      Buffer Binding
//      Buffer State Changes
//      Shader Binding
//      Shader State Chagnes
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


        void Clear()
        {
            this->RCSetGlobalStateCache.Reset();
            this->RCSetVertexArrayStateCache.Reset();
            this->RCSetTextureStateCache.Reset();
            this->RCSetShaderStateCache.Reset();
            this->RCSetFramebufferStateCache.Reset();


            this->RCClearCache.Reset();
            this->RCDrawCache.Reset();
            

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

    }RCCaches[2];



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
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);


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

            ResourceCreationCallCaches[0].Clear();
            ResourceCreationCallCaches[1].Clear();
            ResourceDeletionCallCaches[0].Clear();
            ResourceDeletionCallCaches[1].Clear();

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

        // 2) Clear the new back cache in preparation for filling by another thread. Note how we don't clear the resource creation and deletion
        //    call caches here. The reason is because they are cleared in ExecuteCallCache(). 
        CallCaches[BackCallCacheIndex].Clear();

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

    void Renderer2::ExecuteCallCache()
    {
        // 1) Create resources. We want to lock and clear this all at the same time.
        ResourceCreationLock.Lock();
        ResourceCreationCallCaches[!BackCallCacheIndex].Execute();
        ResourceCreationCallCaches[!BackCallCacheIndex].Clear();
        ResourceCreationLock.Unlock();

        // 2) Normal calls.
        CallCaches[!BackCallCacheIndex].Execute();

        // 3) Delete resources. We want to lock, execute and clear this all at the same time.
        ResourceDeletionLock.Lock();
        ResourceDeletionCallCaches[!BackCallCacheIndex].Execute();
        ResourceDeletionCallCaches[!BackCallCacheIndex].Clear();
        ResourceDeletionLock.Unlock();
    }

    void Renderer2::SwapBuffers()
    {
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

    void Renderer2::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetViewport(x, y, width, height);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer2::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetScissor(x, y, width, height);
        }


        State.currentRCClear = nullptr;
    }


    void Renderer2::SetClearColour(float r, float g, float b, float a)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearColour(r, g, b, a);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer2::SetClearDepth(float depth)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearDepth(depth);
        }


        State.currentRCClear = nullptr;
    }

    void Renderer2::SetClearStencil(int stencil)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearStencil(stencil);
        }


        State.currentRCClear = nullptr;
    }


    void Renderer2::SetCurrentShader(Shader* programToMakeCurrent)
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

    void Renderer2::SetCurrentFramebuffer(Framebuffer* framebufferToMakeCurrent)
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



    void Renderer2::EnableScissorTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
    }

    void Renderer2::DisableScissorTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
    }


    void Renderer2::EnableBlending()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_BLEND);
        }
    }

    void Renderer2::DisableBlending()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_BLEND);
        }
    }

    void Renderer2::SetBlendFunction(BlendFunc sfactor, BlendFunc dfactor)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendFunction(ToOpenGLBlendFunc(sfactor), ToOpenGLBlendFunc(dfactor));
        }
    }

    void Renderer2::SetBlendEquation(BlendEquation equation)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendEquation(ToOpenGLBlendEquation(equation));
        }
    }


    void Renderer2::EnableAlphaTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_ALPHA_TEST);
        }
    }

    void Renderer2::DisableAlphaTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_ALPHA_TEST);
        }
    }

    void Renderer2::SetAlphaTestFunction(RendererFunction function, float ref)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetAlphaTestFunction(ToOpenGLFunc(function), static_cast<GLclampf>(ref));
        }
    }


    void Renderer2::EnableColourWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableColourWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer2::DisableColourWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableColourWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer2::EnableDepthWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableDepthWrites();
        }

        State.currentRCClear = nullptr;
    }

    void Renderer2::DisableDepthWrites()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableDepthWrites();
        }

        State.currentRCClear = nullptr;
    }

    
    void Renderer2::EnableDepthTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_DEPTH_TEST);
        }
    }

    void Renderer2::DisableDepthTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_DEPTH_TEST);
        }
    }

    void Renderer2::SetDepthFunction(RendererFunction function)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetDepthFunction(function);
        }
    }



    void Renderer2::EnableStencilTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_STENCIL_TEST);
        }
    }

    void Renderer2::DisableStencilTest()
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_STENCIL_TEST);
        }
    }

    void Renderer2::SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned int mask)
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

    void Renderer2::SetStencilFuncSeparate(bool frontFace, bool backFace, RendererFunction func, int ref, unsigned int mask)
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

    void Renderer2::SetStencilOpSeparate(bool frontFace, bool backFace, StencilOp stencilFail, StencilOp depthFail, StencilOp pass)
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



    void Renderer2::SetFaceCulling(bool cullFront, bool cullBack)
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


    void Renderer2::EnablePolygonOffset(PolygonMode mode)
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

    void Renderer2::DisablePolygonOffset(PolygonMode mode)
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

    void Renderer2::SetPolygonMode(bool frontFaces, bool backFaces, PolygonMode mode)
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

    void Renderer2::SetPolygonOffset(float factor, float units)
    {
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
        }
    }



    ///////////////////////////
    // Drawing

    void Renderer2::Clear(unsigned int bufferMask)
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

    void Renderer2::Draw(const VertexArray &vertexArray, DrawMode mode)
    {
        UPDATE_CURRENT_RC(RCDraw);
        assert(State.currentRCDraw != nullptr);
        {
            auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL33 &>(vertexArray);
            {
                State.currentRCDraw->Draw(vertexArrayGL33.GetOpenGLObjectPtr(), ToOpenGLDrawMode(mode), vertexArrayGL33.GetIndexCount());
            }
        }


        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
    }



    ///////////////////////////
    // Resources

    VertexArray* Renderer2::CreateVertexArray(VertexArrayUsage usage, const VertexFormat &format)
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

    void Renderer2::DeleteVertexArray(VertexArray* vertexArrayToDelete)
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

    void Renderer2::PushVertexArrayVertexData(const VertexArray &vertexArray)
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
                    State.currentRCSetVertexArrayState->SetVertexData(vertexArrayObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                }
            }
        }
    }

    void Renderer2::PushVertexArrayIndexData(const VertexArray &vertexArray)
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
                }
            }
        }
    }

    void Renderer2::PushVertexArrayData(const VertexArray &vertexArray)
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
                    State.currentRCSetVertexArrayState->SetVertexData(vertexArrayObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    State.currentRCSetVertexArrayState->SetIndexData( vertexArrayObject, vertexArray.GetIndexDataPtr(),  vertexArray.GetIndexCount(),  ToOpenGLBufferUsage(vertexArray.GetUsage()));
                }
            }
        }
    }



    GLuint* Renderer_CreateOpenGL33Texture()
    {
        State.instantiatedTextureObjects.PushBack(new GLuint(0));
        GLuint* textureObject  = State.instantiatedTextureObjects.GetBack();


        ResourceCreationLock.Lock();
        {
            auto &command = RCCaches[BackCallCacheIndex].RCCreateTextureCache.Acquire();
            command.CreateTexture(textureObject);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
        }
        ResourceCreationLock.Unlock();


        return textureObject;
    }

    void Renderer_DeleteOpenGL33Texture(GLuint* textureObjectToDelete)
    {
        assert(textureObjectToDelete  != nullptr);
        {
            ResourceDeletionLock.Lock();
            {
                auto &command = RCCaches[BackCallCacheIndex].RCDeleteTextureCache.Acquire();
                command.DeleteTexture(textureObjectToDelete);

                ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
            }
            ResourceDeletionLock.Unlock();



            // The objects need to be marked for deletion, but not actually deleted yet.
            State.MarkTextureObjectAsDeleted(textureObjectToDelete);
        }
    }

    void Renderer_SetOpenGL33TextureFilter(GLenum textureTarget, GLuint* textureObject, TextureFilter minification, TextureFilter magnification)
    {
        assert(textureObject != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureFilter(textureObject, textureTarget, ToOpenGLTextureFilter(minification), ToOpenGLTextureFilter(magnification));
            }
        }
    }

    void Renderer_SetOpenGL33TextureAnisotropy(GLenum textureTarget, GLuint* textureObject, unsigned int anisotropy)
    {
        assert(textureObject != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureAnisotropy(textureObject, textureTarget, static_cast<GLint>(anisotropy));
            }
        }
    }

    void Renderer_SetOpenGL33TextureWrapMode(GLenum textureTarget, GLuint* textureObject, TextureWrapMode wrapMode)
    {
        assert(textureObject != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureWrapMode(textureObject, textureTarget, ToOpenGLWrapMode(wrapMode));
            }
        }
    }

    void Renderer_SetOpenGL33TextureMipmapLevels(GLenum textureTarget, GLuint* textureObject, unsigned int baseLevel, unsigned int maxLevel)
    {
        assert(textureObject != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureMipmapLevels(textureObject, textureTarget, static_cast<GLint>(baseLevel), static_cast<GLint>(maxLevel));
            }
        }
    }

    void Renderer_GenerateOpenGL33TextureMipmaps(GLenum textureTarget, GLuint* textureObject)
    {
        assert(textureObject != nullptr);
        {
            if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);
            }


            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->GenerateTextureMipmaps(textureObject, textureTarget);
            }
        }
    }




    Texture2D* Renderer2::CreateTexture2D()
    {
        return new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_2D);
    }

    TextureCube* Renderer2::CreateTextureCube()
    {
        auto positiveX = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        auto negativeX = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        auto positiveY = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        auto negativeY = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        auto positiveZ = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        auto negativeZ = new Texture2D_OpenGL33(Renderer_CreateOpenGL33Texture(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        return new TextureCube_OpenGL33(Renderer_CreateOpenGL33Texture(), positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
    }


    void Renderer2::DeleteTexture2D(Texture2D* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<Texture2D_OpenGL33*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL33Texture(textureToDeleteGL33->GetOpenGLObjectPtr());

            // We can safely delete the main object at this point.
            delete textureToDelete;
        }
    }

    void Renderer2::DeleteTextureCube(TextureCube* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<TextureCube_OpenGL33*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL33Texture(textureToDeleteGL33->GetOpenGLObjectPtr());

            // We can safely delete the main object at this point.
            delete textureToDelete;
        }
    }


    void Renderer2::PushTexture2DData(const Texture2D &texture, int mipmapIndex)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            GLenum  textureTarget = textureGL33.GetTarget();
            GLuint* textureObject = textureGL33.GetOpenGLObjectPtr();
            assert(textureObject != nullptr);
            {
                if (State.currentRCSetTextureState == nullptr || (State.currentRCSetTextureState->GetTextureObject() != textureObject || State.currentRCSetTextureState->GetTarget() != textureTarget))
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
                            auto mipmap = texture.GetMipmap(i);
                            if (mipmap.data != nullptr)
                            {
                                State.currentRCSetTextureState->SetTexture2DData(textureObject, textureTarget, static_cast<int>(i), mipmap.format, mipmap.width, mipmap.height, mipmap.data, mipmap.GetDataSizeInBytes());
                            }
                        }
                    }
                    else
                    {
                        auto mipmap = texture.GetMipmap(mipmapIndex);
                        if (mipmap.data != nullptr)
                        {
                            State.currentRCSetTextureState->SetTexture2DData(textureObject, textureTarget, mipmapIndex, mipmap.format, mipmap.width, mipmap.height, mipmap.data, mipmap.GetDataSizeInBytes());
                        }
                    }
                }
            }
        }
    }

    void Renderer2::PushTextureCubeData(const TextureCube &texture, int mipmapIndex)
    {
        // This can, and should be optimized.
        PushTexture2DData(*texture.PositiveX, mipmapIndex);
        PushTexture2DData(*texture.NegativeX, mipmapIndex);
        PushTexture2DData(*texture.PositiveY, mipmapIndex);
        PushTexture2DData(*texture.NegativeY, mipmapIndex);
        PushTexture2DData(*texture.PositiveZ, mipmapIndex);
        PushTexture2DData(*texture.NegativeZ, mipmapIndex);
    }


    void Renderer2::SetTexture2DFilter(const Texture2D &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureFilter(textureGL33.GetTarget(), textureGL33.GetOpenGLObjectPtr(), minification, magnification);
        }
    }

    void Renderer2::SetTextureCubeFilter(const TextureCube &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureFilter(GL_TEXTURE_CUBE_MAP, textureGL33.GetOpenGLObjectPtr(), minification, magnification);
        }
    }


    void Renderer2::SetTexture2DAnisotropy(const Texture2D &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureAnisotropy(textureGL33.GetTarget(), textureGL33.GetOpenGLObjectPtr(), anisotropy);
        }
    }

    void Renderer2::SetTextureCubeAnisotropy(const TextureCube &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureAnisotropy(GL_TEXTURE_CUBE_MAP, textureGL33.GetOpenGLObjectPtr(), anisotropy);
        }
    }


    void Renderer2::SetTexture2DWrapMode(const Texture2D &texture, TextureWrapMode wrapMode)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureWrapMode(textureGL33.GetTarget(), textureGL33.GetOpenGLObjectPtr(), wrapMode);
        }
    }


    void Renderer2::SetTexture2DMipmapLevels(const Texture2D &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureMipmapLevels(textureGL33.GetTarget(), textureGL33.GetOpenGLObjectPtr(), baseLevel, maxLevel);
        }
    }

    void Renderer2::SetTextureCubeMipmapLevels(const TextureCube &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_SetOpenGL33TextureMipmapLevels(GL_TEXTURE_CUBE_MAP, textureGL33.GetOpenGLObjectPtr(), baseLevel, maxLevel);
        }
    }


    void Renderer2::GenerateTexture2DMipmaps(const Texture2D &texture)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL33 &>(texture);
        {
            Renderer_GenerateOpenGL33TextureMipmaps(textureGL33.GetTarget(), textureGL33.GetOpenGLObjectPtr());
        }
    }

    void Renderer2::GenerateTextureCubeMipmaps(const TextureCube &texture)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL33 &>(texture);
        {
            Renderer_GenerateOpenGL33TextureMipmaps(GL_TEXTURE_CUBE_MAP, textureGL33.GetOpenGLObjectPtr());
        }
    }



    Shader* Renderer2::CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource)
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

    void Renderer2::DeleteShader(Shader* shaderToDelete)
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


    void Renderer2::PushShaderPendingProperties(const Shader &shader)
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
                    auto &pendingParameters = shaderGL33.GetPendingParameters();
                    for (size_t i = 0; i < pendingParameters.count; ++i)
                    {
                        auto parameterName = pendingParameters.buffer[i]->key;
                        auto parameter     = pendingParameters.buffer[i]->value;

                        assert(parameterName != nullptr);
                        assert(parameter     != nullptr);
                        {
                            if (parameter->type == ShaderParameterType_Texture1D)
                            {
                            }
                            else if (parameter->type == ShaderParameterType_Texture2D)
                            {
                                auto texture = static_cast<Texture2D_OpenGL33*>(static_cast<ShaderParameter_Texture2D*>(parameter)->value);
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, texture->GetOpenGLObjectPtr(), texture->GetTarget());
                                }
                            }
                            else if (parameter->type == ShaderParameterType_Texture3D)
                            {
                            }
                            else if (parameter->type == ShaderParameterType_TextureCube)
                            {
                                auto texture = static_cast<TextureCube_OpenGL33*>(static_cast<ShaderParameter_TextureCube*>(parameter)->value);
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, texture->GetOpenGLObjectPtr(), GL_TEXTURE_CUBE_MAP);
                                }
                            }
                            else
                            {
                                // It's a basic type.
                                if (parameter->type == ShaderParameterType_Float)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float2)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float2*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float3)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float3*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float4)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float4*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Integer)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Integer*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Integer2)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Integer2*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Integer3)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Integer3*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Integer4)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Integer4*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float2x2)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float2x2*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float3x3)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float3x3*>(parameter)->value);
                                }
                                else if (parameter->type == ShaderParameterType_Float4x4)
                                {
                                    State.currentRCSetShaderState->SetShaderParameter(programState, parameterName, static_cast<ShaderParameter_Float4x4*>(parameter)->value);
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    Framebuffer* Renderer2::CreateFramebuffer()
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

    void Renderer2::DeleteFramebuffer(Framebuffer* framebufferToDelete)
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


    void Renderer2::PushAttachments(const Framebuffer &framebuffer)
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


                assert(State.currentRCSetShaderState != nullptr);
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
                            State.currentRCSetFramebufferState->SetAttachedBuffer(GL_COLOR_ATTACHMENT0_EXT + index, texture->GetTarget(), texture->GetOpenGLObjectPtr());
                        }
                    }

                    // Depth/Stencil.
                    auto depthStencilAttachment = static_cast<const Texture2D_OpenGL33*>(framebufferGL33.GetDepthStencilBuffer());
                    if (depthStencilAttachment != nullptr)
                    {
                        State.currentRCSetFramebufferState->SetAttachedBuffer(GL_DEPTH_ATTACHMENT_EXT,   depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLObjectPtr());
                        State.currentRCSetFramebufferState->SetAttachedBuffer(GL_STENCIL_ATTACHMENT_EXT, depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLObjectPtr());
                    }
                }
            }
        }
    }




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