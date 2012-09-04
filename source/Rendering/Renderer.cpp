
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/GUIRenderer.hpp>
#include <GTEngine/Rendering/VertexAttributes.hpp>
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

    GLint ToOpenGLTextureFilter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter_Nearest: return GL_NEAREST;
        case TextureFilter_Linear:  return GL_LINEAR;

        case TextureFilter_NearestNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter_NearestLinear:  return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter_LinearNearest:  return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter_LinearLinear:   return GL_LINEAR_MIPMAP_LINEAR;

        default: break;
        }

        return GL_LINEAR;
    }

    GLint ToOpenGLWrapMode(TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case TextureWrapMode_Repeat:        return GL_REPEAT;
        case TextureWrapMode_Clamp:         return GL_CLAMP;
        case TextureWrapMode_ClampToEdge:   return GL_CLAMP_TO_EDGE;

        default: break;
        }

        return GL_REPEAT;
    }

    GLint ToOpenGLInternalFormat(GTImage::ImageFormat format)
    {
        switch (format)
        {
        case GTImage::ImageFormat_R8:               return GL_R8;
        case GTImage::ImageFormat_RG8:              return GL_RG8;
        case GTImage::ImageFormat_RGB8:             return GL_RGB8;
        case GTImage::ImageFormat_RGBA8:            return GL_RGBA8;
        case GTImage::ImageFormat_RGBA16:           return GL_RGBA16;
        case GTImage::ImageFormat_RGBA32:           return GL_RGBA32UI;
        case GTImage::ImageFormat_R16F:             return GL_R16F;
        case GTImage::ImageFormat_RG16F:            return GL_RG16F;
        case GTImage::ImageFormat_RGB16F:           return GL_RGB16F;
        case GTImage::ImageFormat_RGBA16F:          return GL_RGBA16F;
        case GTImage::ImageFormat_R32F:             return GL_R32F;
        case GTImage::ImageFormat_RG32F:            return GL_RG32F;
        case GTImage::ImageFormat_RGB32F:           return GL_RGB32F;
        case GTImage::ImageFormat_RGBA32F:          return GL_RGBA32F;

        case GTImage::ImageFormat_Depth16:          return GL_DEPTH_COMPONENT16;
        case GTImage::ImageFormat_Depth24:          return GL_DEPTH_COMPONENT24;
        case GTImage::ImageFormat_Depth32:          return GL_DEPTH_COMPONENT32;
        case GTImage::ImageFormat_Depth24_Stencil8: return GL_DEPTH24_STENCIL8;

        case GTImage::ImageFormat_Auto:
        default: break;
        }

        return GL_RGBA8;
    }

    GLenum ToOpenGLFormat(GTImage::ImageFormat format)
    {
        switch (format)
        {
        case GTImage::ImageFormat_R8:
        case GTImage::ImageFormat_R16F:
        case GTImage::ImageFormat_R32F:
            {
                return GL_RED;
            }

        case GTImage::ImageFormat_RG8:
        case GTImage::ImageFormat_RG16F:
        case GTImage::ImageFormat_RG32F:
            {
                return GL_RG;
            }

        case GTImage::ImageFormat_RGB8:
        case GTImage::ImageFormat_RGB16F:
        case GTImage::ImageFormat_RGB32F:
            {
                return GL_RGB;
            }

        case GTImage::ImageFormat_RGBA8:
        case GTImage::ImageFormat_RGBA16:
        case GTImage::ImageFormat_RGBA32:
        case GTImage::ImageFormat_RGBA16F:
        case GTImage::ImageFormat_RGBA32F:
            {
                return GL_RGBA;
            }

        case GTImage::ImageFormat_Depth16:
        case GTImage::ImageFormat_Depth24:
        case GTImage::ImageFormat_Depth32:
            {
                return GL_DEPTH_COMPONENT;
            }

        case GTImage::ImageFormat_Depth24_Stencil8:
            {
                return GL_DEPTH_STENCIL;
            }

        case GTImage::ImageFormat_Auto:
        default: break;
        }

        return GL_RGBA;
    }

    GLenum ToOpenGLType(GTImage::ImageFormat format)
    {
        switch (format)
        {
        case GTImage::ImageFormat_R8:
        case GTImage::ImageFormat_RG8:
        case GTImage::ImageFormat_RGB8:
        case GTImage::ImageFormat_RGBA8:
            {
                return GL_UNSIGNED_BYTE;
            }

        case GTImage::ImageFormat_RGBA16:
            {
                return GL_UNSIGNED_SHORT;
            }

        case GTImage::ImageFormat_RGBA32:
            {
                return GL_UNSIGNED_INT;
            }

        case GTImage::ImageFormat_R16F:
        case GTImage::ImageFormat_RG16F:
        case GTImage::ImageFormat_RGB16F:
        case GTImage::ImageFormat_RGBA16F:
        case GTImage::ImageFormat_R32F:
        case GTImage::ImageFormat_RG32F:
        case GTImage::ImageFormat_RGB32F:
        case GTImage::ImageFormat_RGBA32F:
            {
                return GL_FLOAT;
            }


        case GTImage::ImageFormat_Depth16:
        case GTImage::ImageFormat_Depth24:
        case GTImage::ImageFormat_Depth32:
            {
                return GL_FLOAT;
            }

        case GTImage::ImageFormat_Depth24_Stencil8:
            {
                return GL_UNSIGNED_INT_24_8;
            }


        case GTImage::ImageFormat_Auto:
        default: break;
        }

        return GL_UNSIGNED_BYTE;
    }

    GLenum ToOpenGLBufferUsage(VertexArrayUsage usage)
    {
        switch (usage)
        {
        case VertexArrayUsage_Static:   return GL_STATIC_DRAW;
        case VertexArrayUsage_Dynamic:  return GL_DYNAMIC_DRAW;
        case VertexArrayUsage_Stream:   return GL_STREAM_DRAW;

        default: break;
        }

        return GL_STATIC_DRAW;
    }

    GLenum ToOpenGLDrawMode(DrawMode mode)
    {
        switch (mode)
        {
        case DrawMode_Triangles:    return GL_TRIANGLES;
        case DrawMode_Lines:        return GL_LINES;

        default: break;
        }

        return GL_TRIANGLES;
    }

    GLenum * ToOpenGLDrawBuffers(size_t count, int *buffers)
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

    GLenum ToOpenGLBlendFunc(BlendFunc func)
    {
        switch (func)
        {
        case BlendFunc_Zero:                    return GL_ZERO;
        case BlendFunc_One:                     return GL_ONE;

        case BlendFunc_SourceColour:            return GL_SRC_COLOR;
        case BlendFunc_OneMinusSourceColour:    return GL_ONE_MINUS_SRC_COLOR;
        case BlendFunc_SourceAlpha:             return GL_SRC_ALPHA;
        case BlendFunc_OneMinusSourceAlpha:     return GL_ONE_MINUS_SRC_ALPHA;

        case BlendFunc_DestColour:              return GL_DST_COLOR;
        case BlendFunc_OneMinusDestColour:      return GL_ONE_MINUS_DST_COLOR;
        case BlendFunc_DestAlpha:               return GL_DST_ALPHA;
        case BlendFunc_OneMinusDestAlpha:       return GL_ONE_MINUS_DST_ALPHA;

        case BlendFunc_ConstantColour:          return GL_CONSTANT_COLOR;
        case BlendFunc_OneMinusConstantColour:  return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFunc_ConstantAlpha:           return GL_CONSTANT_ALPHA;
        case BlendFunc_OneMinusConstantAlpha:   return GL_ONE_MINUS_CONSTANT_ALPHA;

        case BlendFunc_SourceAlphaSaturate:     return GL_SRC_ALPHA_SATURATE;

        default: break;
        }

        return GL_SRC_COLOR;
    }

    GLenum ToOpenGLBlendEquation(BlendEquation equation)
    {
        switch (equation)
        {
        case BlendEquation_Add:             return GL_FUNC_ADD;
        case BlendEquation_Subtract:        return GL_FUNC_SUBTRACT;
        case BlendEquation_ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEquation_Min:             return GL_MIN;
        case BlendEquation_Max:             return GL_MAX;

        default: break;
        }

        return GL_FUNC_ADD;
    }

    GLenum ToOpenGLFunc(RendererFunction func)
    {
        switch (func)
        {
        case RendererFunction_Never:    return GL_NEVER;
        case RendererFunction_Less:     return GL_LESS;
        case RendererFunction_Equal:    return GL_EQUAL;
        case RendererFunction_LEqual:   return GL_LEQUAL;
        case RendererFunction_Greater:  return GL_GREATER;
        case RendererFunction_NotEqual: return GL_NOTEQUAL;
        case RendererFunction_GEqual:   return GL_GEQUAL;
        case RendererFunction_Always:   return GL_ALWAYS;
        default: break;
        }

        return GL_ALWAYS;
    }

    GLenum ToOpenGLStencilOp(StencilOp op)
    {
        switch (op)
        {
        case StencilOp_Keep:          return GL_KEEP;
        case StencilOp_Zero:          return GL_ZERO;
        case StencilOp_Replace:       return GL_REPLACE;
        case StencilOp_Increment:     return GL_INCR;
        case StencilOp_IncrementWrap: return GL_INCR_WRAP;
        case StencilOp_Decrement:     return GL_DECR;
        case StencilOp_DecrementWrap: return GL_DECR_WRAP;
        case StencilOp_Invert:        return GL_INVERT;
        default: break;
        }

        return GL_KEEP;
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
              IsDepthTestEnabled(false), IsDepthWritesEnabled(true), CurrentDepthFunc(RendererFunction_Less),
              IsStencilTestEnabled(false),
              IsBlendingEnabled(false), CurrentBlendSourceFactor(BlendFunc_One), CurrentBlendDestFactor(BlendFunc_Zero), CurrentBlendEquation(BlendEquation_Add),
              IsAlphaTestEnabled(false), CurrentAlphaTestFunc(RendererFunction_Always), CurrentAlphaTestRef(0.0f),
              SwapInterval(1), SwapIntervalChanged(false)
        {
        }

        /// The current shader.
        Shader *CurrentShader;

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


    private:    // No copying.
        _RendererState(const _RendererState &);
        _RendererState & operator=(const _RendererState &);

    }RendererState;



    struct Texture2D_GL20
    {
        Texture2D_GL20()
            : object(0)
        {
        }

        Texture2D_GL20(GLuint object)
            : object(object)
        {
        }

        GLuint object;
    };

    struct Framebuffer_GL20
    {
        Framebuffer_GL20()
            : object(0), colourAttachments(nullptr), depthStencilAttachment(nullptr)
        {
            size_t colourCount = Renderer::GetMaxColourAttachments();

            this->colourAttachments = new Texture2D*[colourCount];
            for (size_t i = 0; i < colourCount; ++i)
            {
                this->colourAttachments[i] = nullptr;
            }
        }

        ~Framebuffer_GL20()
        {
            delete [] this->colourAttachments;
        }

        GLuint object;

        Texture2D** colourAttachments;
        Texture2D * depthStencilAttachment;

    private:    // No copying.
        Framebuffer_GL20(const Framebuffer_GL20 &);
        Framebuffer_GL20 & operator=(const Framebuffer_GL20 &);
    };

    struct Shader_GL20
    {
        Shader_GL20()
            : program(0), vertexShader(0), fragmentShader(0)
        {
        }

        /// The main program object.
        GLuint program;

        /// The vertex shader object. We keep hold of this so we can relink when vertex atribute positions are changed.
        GLuint vertexShader;

        /// The fragment shader object. We keep hold of this so we can relink when vertex attribute positions are changed.
        GLuint fragmentShader;
    };

    struct VertexArray_GL20
    {
        VertexArray_GL20()
            : verticesObject(0), indicesObject(0)
        {
        }

        GLuint verticesObject;
        GLuint indicesObject;
    };
}

// Renderer globals.
namespace GTEngine
{
    RCQueue* Renderer::BackRCQueue  = nullptr;
    RCQueue* Renderer::FrontRCQueue = nullptr;
    size_t   Renderer::BackIndex    = 0;

    struct _RendererGC
    {
        _RendererGC()
            : Texture2Ds(), Framebuffers(), Shaders(), VertexArrays(), Lock(), NeedsCollection(false)
        {
        }

        GTCore::List<void*> Texture2Ds;
        GTCore::List<void*> Framebuffers;
        GTCore::List<void*> Shaders;
        GTCore::List<void*> VertexArrays;

        GTCore::Mutex Lock;

        bool NeedsCollection;
    };
    _RendererGC RendererGC;
}

namespace GTEngine
{
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
        Renderer::FrontRCQueue->Execute();
        Renderer::CollectGarbage();
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


    void Renderer::MarkForCollection(Texture2D *texture)
    {
        assert(texture != nullptr);

        RendererGC.Lock.Lock();
            RendererGC.Texture2Ds.Append(texture->GetRendererData());
            RendererGC.NeedsCollection = true;
        RendererGC.Lock.Unlock();
    }
    void Renderer::MarkForCollection(Framebuffer *framebuffer)
    {
        assert(framebuffer != nullptr);

        RendererGC.Lock.Lock();
            RendererGC.Framebuffers.Append(framebuffer->GetRendererData());
            RendererGC.NeedsCollection = true;
        RendererGC.Lock.Unlock();
    }
    void Renderer::MarkForCollection(Shader *shader)
    {
        assert(shader != nullptr);

        RendererGC.Lock.Lock();
            RendererGC.Shaders.Append(shader->GetRendererData());
            RendererGC.NeedsCollection = true;
        RendererGC.Lock.Unlock();
    }
    void Renderer::MarkForCollection(VertexArray *vertexArray)
    {
        assert(vertexArray != nullptr);

        RendererGC.Lock.Lock();
            RendererGC.VertexArrays.Append(vertexArray->GetRendererData());
            RendererGC.NeedsCollection = true;
        RendererGC.Lock.Unlock();
    }


    void Renderer::CollectGarbage() //[Renderer Thread]
    {
        if (RendererGC.NeedsCollection)
        {
            RendererGC.Lock.Lock();
                // Textures.
                while (RendererGC.Texture2Ds.root != nullptr)
                {
                    Renderer::DeleteTexture2DData(RendererGC.Texture2Ds.root->value);

                    RendererGC.Texture2Ds.RemoveRoot();
                }

                // Framebuffers.
                while (RendererGC.Framebuffers.root != nullptr)
                {
                    Renderer::DeleteFramebufferData(RendererGC.Framebuffers.root->value);

                    RendererGC.Framebuffers.RemoveRoot();
                }

                // Shaders.
                while (RendererGC.Shaders.root != nullptr)
                {
                    Renderer::DeleteShaderData(RendererGC.Shaders.root->value);

                    RendererGC.Shaders.RemoveRoot();
                }

                // Vertex Arrays.
                while (RendererGC.VertexArrays.root != nullptr)
                {
                    Renderer::DeleteVertexArrayData(RendererGC.VertexArrays.root->value);

                    RendererGC.VertexArrays.RemoveRoot();
                }

                RendererGC.NeedsCollection = false;
            RendererGC.Lock.Unlock();
        }
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
            RendererEventHandlers.RemoveFirst(&eventHandler);
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

// Renderer support functions.
namespace GTEngine
{
    /**
    *   \brief  Retrieves the shader program object associated with the given shader.
    */
    GLuint Renderer_GetShaderProgramFromShader(const Shader* shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = static_cast<const Shader_GL20*>(shader->GetRendererData());
            if (rendererData != nullptr)
            {
                return rendererData->program;
            }
        }

        return 0;
    }

    GLuint Renderer_GetCurrentShaderProgram()
    {
        return Renderer_GetShaderProgramFromShader(RendererState.CurrentShader);
    }

    bool Renderer_IsShaderUsingVertex(const Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = static_cast<const Shader_GL20*>(shader->GetRendererData());
            if (rendererData != nullptr)
            {
                return rendererData->vertexShader != 0;
            }
        }

        return false;
    }

    bool Renderer_IsShaderUsingFragment(const Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = static_cast<const Shader_GL20*>(shader->GetRendererData());
            if (rendererData != nullptr)
            {
                return rendererData->fragmentShader != 0;
            }
        }

        return false;
    }

    // Helper for linking the given shader.
    void Renderer_LinkShader(Shader* shader)
    {
        auto rendererData = static_cast<const Shader_GL20*>(shader->GetRendererData());
        if (rendererData != nullptr)
        {
            // First we detach everything.
            if (rendererData->program != 0)
            {
                GLuint attachedShaders[2];
                GLsizei count;
                glGetAttachedShaders(rendererData->program, 2, &count, attachedShaders);

                for (GLsizei i = 0; i < count; ++i)
                {
                    glDetachShader(rendererData->program, attachedShaders[i]);
                }
            }


            // Now we need to set the vertex attribute locations from the shader.
            auto &attribs = shader->GetVertexAttributeLocations();
            for (size_t i = 0; i < attribs.count; ++i)
            {
                auto iAttrib = attribs.buffer[i];
                assert(iAttrib != nullptr);

                glBindAttribLocation(rendererData->program, iAttrib->value, iAttrib->key);
            }


            // Finally we reattach the shaders, link the program and check for errors.
            if (rendererData->vertexShader   != 0) glAttachShader(rendererData->program, rendererData->vertexShader);
            if (rendererData->fragmentShader != 0) glAttachShader(rendererData->program, rendererData->fragmentShader);

            glLinkProgram(rendererData->program);


            // Check for link errors.
            GLint linkStatus;
            glGetProgramiv(rendererData->program, GL_LINK_STATUS, &linkStatus);

            if (linkStatus == GL_FALSE)
            {
                GLint logLength;
                glGetProgramiv(rendererData->program, GL_INFO_LOG_LENGTH, &logLength);

                auto log = new char[logLength];
                glGetProgramInfoLog(rendererData->program, logLength, nullptr, log);

                Log("--- Program Link Status ---\n%s", log);

                delete [] log;
            }

            // Shader needs to know that it was linked.
            shader->OnLink();
        }
    }


    void Renderer_LogShaderInfoLog(GLuint shader, const char* source)
    {
        GLint compileStatus;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE)
        {
            GLint shaderType;
            glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);

            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            auto log = new char[logLength];
            glGetShaderInfoLog(shader, logLength, nullptr, log);

            Log("--- %s ---\n%s\n%s", (shaderType == GL_VERTEX_SHADER) ? "Vertex Shader Info Log" : "Fragment Shader Info Log", log, source);

            delete [] log;
        }
    }




    /**
    *   \brief  Retrieves the OpenGL framebuffer object from the given framebuffer.
    */
    GLuint Renderer_GetGLObjectFromFramebuffer(const Framebuffer *framebuffer)
    {
        if (framebuffer != nullptr)
        {
            const Framebuffer_GL20 *rendererData = (const Framebuffer_GL20 *)framebuffer->GetRendererData();
            if (rendererData != nullptr)
            {
                return rendererData->object;
            }
        }

        return 0;
    }

    /**
    *   \brief  Synchronizes a Texture2D object with it's OpenGL counterpart.
    */
    bool Renderer_SyncTexture2D(Texture2D *texture)
    {
        if (texture != nullptr)
        {
            auto textureData = static_cast<Texture2D_GL20*>(texture->GetRendererData());
            if (textureData == nullptr)
            {
                // We can't sync if we don't support the given format.
                if (texture->IsFloatingPointFormat() && !Renderer::SupportFloatTextures())
                {
                    Log("Error synchronizing Texture2D: Floating-point textures are not supported.");
                    return false;
                }

                textureData = new Texture2D_GL20;
                glGenTextures(1, &textureData->object);

                texture->SetRendererData(textureData);
            }


            // Synching binds...
            glBindTexture(GL_TEXTURE_2D, textureData->object);


            if (texture->syncinfo.minFilterChanged)
            {
                // The minification filter is different depending on whether or not we are using mipmapping.
                size_t baseMipLevel, maxMipLevel;
                texture->GetValidMipmapRange(baseMipLevel, maxMipLevel);

                GLint filterGL = ToOpenGLTextureFilter(texture->GetMinificationFilter());

                if (maxMipLevel == baseMipLevel)
                {
                    // If we get here, it means we only have a single mip level. In this case, we don't want to be using a mipmap filter.
                    if (filterGL == GL_LINEAR_MIPMAP_LINEAR || filterGL == GL_LINEAR_MIPMAP_NEAREST)
                    {
                        filterGL = GL_LINEAR;
                    }
                    else if (filterGL == GL_NEAREST_MIPMAP_LINEAR || filterGL == GL_NEAREST_MIPMAP_NEAREST)
                    {
                        filterGL = GL_NEAREST;
                    }
                }

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGL);

                texture->syncinfo.minFilterChanged = false;
            }

            if (texture->syncinfo.magFilterChanged)
            {
                // The texture filter for magnification can only be GL_LINEAR or GL_NEAREST. Best to do a sanity check here.
                GLint filterGL = ToOpenGLTextureFilter(texture->GetMagnificationFilter());
                if (filterGL == GL_LINEAR_MIPMAP_LINEAR || filterGL == GL_LINEAR_MIPMAP_NEAREST)
                {
                    filterGL = GL_LINEAR;
                }
                else if (filterGL == GL_NEAREST_MIPMAP_LINEAR || filterGL == GL_NEAREST_MIPMAP_NEAREST)
                {
                    filterGL = GL_NEAREST;
                }

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGL);

                texture->syncinfo.magFilterChanged = false;
            }

            if (texture->syncinfo.anisotropyChanged)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, texture->GetAnisotropy());

                texture->syncinfo.anisotropyChanged = false;
            }

            if (texture->syncinfo.wrapModeChanged)
            {
                GLint wrapMode = ToOpenGLWrapMode(texture->GetWrapMode());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

                texture->syncinfo.wrapModeChanged = false;
            }

            if (texture->syncinfo.dataChanged && texture->GetMipmapCount() > 0)
            {
                size_t baseMipLevel, maxMipLevel;
                texture->GetValidMipmapRange(baseMipLevel, maxMipLevel);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(baseMipLevel));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  static_cast<GLint>(maxMipLevel));

                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                // Now we need to loop over each mip level and update the data if required.
                for (size_t i = baseMipLevel; i <= maxMipLevel; ++i)
                {
                    if (texture->syncinfo.changedMipmaps.Exists(i))
                    {
                        auto &mipmap = texture->GetMipmap(i);
                        glTexImage2D(GL_TEXTURE_2D, i, ToOpenGLInternalFormat(mipmap.format), static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, ToOpenGLFormat(mipmap.format), ToOpenGLType(mipmap.format), mipmap.data);

                        mipmap.DeleteLocalData();
                    }
                }

                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                texture->syncinfo.dataChanged = false;
                texture->syncinfo.changedMipmaps.Clear();
            }

            return true;
        }

        Log("Warning: Attempting to synchronize null texture.");
        return false;
    }

    bool Renderer_SyncFramebuffer(Framebuffer *framebuffer)
    {
        if (framebuffer != nullptr)
        {
            // This boolean is going to keep track of whether or not we need to check the framebuffer for completeness after
            // everything has been synced. This will occur whenever an attachment is added or removed.
            bool needToValidate = false;

            auto framebufferData = (Framebuffer_GL20 *)framebuffer->GetRendererData();
            if (framebufferData == nullptr)
            {
                framebufferData = new Framebuffer_GL20;
                glGenFramebuffersEXT(1, &framebufferData->object);

                framebuffer->SetRendererData(framebufferData);
            }

            // Syncing binds...
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferData->object);

            // First we need to make sure all colour buffers are synced.
            for (GLint i = 0; i < RendererCaps.MaxColourAttachments; ++i)
            {
                if (framebuffer->syncinfo.colourChanged[i])
                {
                    Texture2D *colourBuffer = framebuffer->GetColourBuffer(i);
                    if (colourBuffer != nullptr)
                    {
                        Renderer_SyncTexture2D(colourBuffer);
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, ((Texture2D_GL20 *)colourBuffer->GetRendererData())->object, 0);
                    }
                    else
                    {
                        // We want to remove the attachment.
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0, 0);
                    }

                    framebufferData->colourAttachments[i]  = colourBuffer;
                    framebuffer->syncinfo.colourChanged[i] = false;

                    needToValidate = true;
                }
            }

            // And now the depth/stencil buffer needs to be synced.
            if (framebuffer->syncinfo.depthStencilChanged)
            {
                Texture2D *depthStencilBuffer = framebuffer->GetDepthStencilBuffer();
                if (depthStencilBuffer != nullptr)
                {
                    Renderer_SyncTexture2D(depthStencilBuffer);

                    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, ((Texture2D_GL20 *)depthStencilBuffer->GetRendererData())->object, 0);
                    if (depthStencilBuffer->GetFormat() == GTImage::ImageFormat_Depth24_Stencil8)
                    {
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, ((Texture2D_GL20 *)depthStencilBuffer->GetRendererData())->object, 0);
                    }
                }
                else
                {
                    // We want to remove the attachment.
                    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
                    if (framebufferData->depthStencilAttachment->GetFormat() == GTImage::ImageFormat_Depth24_Stencil8)
                    {
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
                    }
                }

                framebufferData->depthStencilAttachment   = depthStencilBuffer;
                framebuffer->syncinfo.depthStencilChanged = false;

                needToValidate = true;
            }

            // If we need to validate, we should do that now.
            if (needToValidate)
            {
                GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
                if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
                {
                    framebuffer->IsValid(true);
                }
                else
                {
                    framebuffer->IsValid(false);
                    GTEngine::PostError("Renderer: Framebuffer is invalid. OpenGL status code: %s", gtglGetFramebufferStatusString(status));

                    return false;
                }
            }

            return true;
        }

        return false;
    }

    /**
    *   \brief  Synchronizes a Shader object with it's OpenGL counterpart.
    */
    bool Renderer_SyncShader(Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = (Shader_GL20 *)shader->GetRendererData();
            if (rendererData == nullptr)
            {
                rendererData = new Shader_GL20;
                shader->SetRendererData(rendererData);

                if (shader->GetVertexSource() != nullptr)
                {
                    auto source       = shader->GetVertexSource();
                    auto sourceLength = static_cast<GLint>(GTCore::Strings::SizeInBytes(source));

                    rendererData->vertexShader = glCreateShader(GL_VERTEX_SHADER);
                    glShaderSource(rendererData->vertexShader, 1, &source, &sourceLength);
                    glCompileShader(rendererData->vertexShader);

                    Renderer_LogShaderInfoLog(rendererData->vertexShader, source);
                }

                if (shader->GetFragmentSource() != nullptr)
                {
                    auto source = shader->GetFragmentSource();
                    auto sourceLength = static_cast<GLint>(GTCore::Strings::SizeInBytes(source));

                    rendererData->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                    glShaderSource(rendererData->fragmentShader, 1, &source, &sourceLength);
                    glCompileShader(rendererData->fragmentShader);

                    Renderer_LogShaderInfoLog(rendererData->fragmentShader, source);
                }


                // With the individual shaders created, we now create the main program.
                rendererData->program = glCreateProgram();

                // Now we just link.
                Renderer_LinkShader(shader);
            }
            else
            {
                // Here we will check if anything needs to be updated. If vertex attribute locations has changed, the program needs to be relinked.
                if (shader->NeedsRelink())
                {
                    Renderer_LinkShader(shader);
                }
            }

            // Syncing binds, so we need to "use" the shader.
            glUseProgram(rendererData->program);

            return true;
        }

        return false;
    }

    bool Renderer_SyncVertexArray(const VertexArray* vertexArray)
    {
        assert(vertexArray != nullptr);

        auto vertexArrayData = (VertexArray_GL20*)vertexArray->GetRendererData();
        if (vertexArrayData == nullptr)
        {
            vertexArrayData = new VertexArray_GL20;
            vertexArray->SetRendererData(vertexArrayData);

            glGenBuffers(1, &vertexArrayData->verticesObject);
            glGenBuffers(1, &vertexArrayData->indicesObject);
        }

        // Syncing binds...
        glBindBuffer(GL_ARRAY_BUFFER,         vertexArrayData->verticesObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayData->indicesObject);

        if (vertexArray->syncinfo.verticesChanged)
        {
            auto size = static_cast<GLsizeiptr>(vertexArray->GetVertexCount() * vertexArray->GetFormat().GetSize() * sizeof(float));

            glBufferData(GL_ARRAY_BUFFER, size, vertexArray->GetVertexDataPtr(), ToOpenGLBufferUsage(vertexArray->GetUsage()));

            vertexArray->syncinfo.verticesChanged = false;
        }

        if (vertexArray->syncinfo.indicesChanged)
        {
            auto size = static_cast<GLsizeiptr>(vertexArray->GetIndexCount() * sizeof(unsigned int));

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, vertexArray->GetIndexDataPtr(), ToOpenGLBufferUsage(vertexArray->GetUsage()));

            vertexArray->syncinfo.indicesChanged = false;
        }

        return true;
    }


    void OpenGLDebugMessageHandler(const char* message)
    {
        GTEngine::Log("%s", message);
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
                Log("    EXT_framebuffer_object:             %s", GTGL_EXT_framebuffer_object             ? "yes" : "no");
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

                // We need a GUI renderer.
                GUIRenderer::Initialise();

                // Now we'll set some defaults.
                Renderer::EnableDepthTest();
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_TEXTURE_2D);
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
            delete Renderer::BackRCQueue;
            delete Renderer::FrontRCQueue;

            GUIRenderer::Uninitialise();

            Renderer::CollectGarbage();

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


    void Renderer_EnableVertexFormat(const VertexFormat &format, const float *vertices)
    {
        // We need to enable all of the relevant vertex attributes and set their pointers. What we do is we loop over the
        // attributes in the vertex array's format and set the pointers and enable. Then, we disable any previously enabled
        // attributes that we no longer want enabled.
        uint32_t  newVertexAttribEnableBits = 0x0;
        uint32_t& oldVertexAttribEnableBits = VertexAttribEnableBits;
        size_t    formatArraySize           = format.GetAttributeCount();

        GLsizei formatSizeInBytes = static_cast<GLsizei>(format.GetSizeInBytes());
        int offset = 0;

        for (size_t i = 0; i < formatArraySize; i += 2)
        {
            GLuint   attribIndex = static_cast<GLuint>(format[i]);
            GLint    attribSize  = static_cast<GLint>(format[i + 1]);
            uint32_t bit         = static_cast<uint32_t>(1 << attribIndex);

            newVertexAttribEnableBits |= bit;

            if (!(oldVertexAttribEnableBits & bit))
            {
                glEnableVertexAttribArray(attribIndex);
            }
            else
            {
                // We clear the bit from the old bitfield because after this loop we're going to check if any bits are remaining. If so,
                // those attributes need to be disabled. By clearing, we can just check if the bitfield is 0, in which case nothing else
                // needs disabling.
                oldVertexAttribEnableBits &= ~bit;
            }

            glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, (vertices +  offset));

            // The offset must be set AFTER glVertexAttribPointer().
            offset += attribSize;
        }

        // If any attributes need to be disable, we need to do that now. This is where our enabled bitfields come in handy.
        while (oldVertexAttribEnableBits != 0)
        {
            GLuint attribIndex  = static_cast<GLuint>(GTCore::NextBitIndex(oldVertexAttribEnableBits));

            glDisableVertexAttribArray(attribIndex);

            // The bit needs to be cleared in preperation for the next iteration.
            oldVertexAttribEnableBits &= ~(1 << attribIndex);
        }

        // Now we need to set the renderer's enabled bits to our new bits.
        VertexAttribEnableBits = newVertexAttribEnableBits;
    }

    void Renderer::Draw(const VertexArray *vertexArray, DrawMode mode)
    {
        assert(vertexArray != nullptr);

        // Syncing the vertex array will also bind the applicable OpenGL objects.
        if (Renderer_SyncVertexArray(vertexArray))
        {
            // First we enable the vertex format. We pass null to this one to indicate that we're using a VBO.
            Renderer_EnableVertexFormat(vertexArray->GetFormat(), nullptr);

            // Now that everything has been enabled/disabled, we draw the elements.
            glDrawElements(ToOpenGLDrawMode(mode), static_cast<GLsizei>(vertexArray->GetIndexCount()), GL_UNSIGNED_INT, 0);
        }
    }

    void Renderer::Draw(const float *vertices, const unsigned int *indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
        // Any previously bound vertex buffer needs to be unbound.
        glBindBuffer(GL_ARRAY_BUFFER,         0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // First we enable the vertex format...
        Renderer_EnableVertexFormat(format, vertices);

        // ... then we just draw the elements.
        glDrawElements(ToOpenGLDrawMode(mode), static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, indices);
    }

    void Renderer::EnableScissorTest()
    {
        glEnable(GL_SCISSOR_TEST);
        RendererState.IsScissorEnabled = true;
    }
    void Renderer::DisableScissorTest()
    {
        glDisable(GL_SCISSOR_TEST);
        RendererState.IsScissorEnabled = false;
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

    void Renderer::SetFramebuffer(Framebuffer *framebuffer)
    {
        if (GTGL_EXT_framebuffer_object)
        {
            if (framebuffer != nullptr)
            {
                // We need to synchronize the framebuffer before attempting to use it. This will bind the framebuffer.
                Renderer_SyncFramebuffer(framebuffer);
            }
            else
            {
                // 'framebuffer' is null, so we just use the default framebuffer.
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
        if (shader != nullptr && Renderer_SyncShader(shader))
        {
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
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            for (size_t i = 0; i < RendererState.CurrentShader->currentTexture2Ds.count; ++i)
            {
                auto iTexture = RendererState.CurrentShader->currentTexture2Ds.buffer[i];

                glActiveTexture(GL_TEXTURE0 + i);
                glEnable(GL_TEXTURE_2D);
                Renderer_SyncTexture2D(iTexture->value);        // <-- syncing binds.
            }
        }
    }


    void Renderer::SetShaderParameter(const char *paramName, Texture2D *value)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            auto iTexture = RendererState.CurrentShader->currentTexture2Ds.Find(paramName);
            if (iTexture != nullptr)
            {
                // If the texture is the same, fall through.
                if (iTexture->value != value)
                {
                    // We need to keep track of the old texture so we can check if the shader is using it elsewhere and then call Texture2D->OnShaderDetached() if appropriate.
                    auto oldTexture = iTexture->value;
                    iTexture->value = value;

                    // All we need to do is change the binding for the texture.
                    glActiveTexture(GL_TEXTURE0 + iTexture->index);
                    Renderer_SyncTexture2D(iTexture->value);            // <-- syncing binds.

                    // Now we need to let the shader know about the old texture.
                    RendererState.CurrentShader->OnTextureParameterChanged(oldTexture);
                }
            }
            else
            {
                // The texture doesn't exist. We need to add it and then reset the uniform for every texture. We do this because we use a map to store the
                // texture units, which will be rearranged as an entry is added.
                RendererState.CurrentShader->currentTexture2Ds.Add(paramName, value);
                Renderer::BindCurrentShaderTextures();

                for (size_t i = 0; i < RendererState.CurrentShader->currentTexture2Ds.count; ++i)
                {
                    GLint location = glGetUniformLocation(rendererData->program, RendererState.CurrentShader->currentTexture2Ds.buffer[i]->key);
                    glUniform1i(location, static_cast<GLint>(i));
                }
            }


            // The new texture needs to be aware that it is attached to a shader.
            if (value != nullptr)
            {
                value->OnAttachToShader(RendererState.CurrentShader);
            }
        }
    }

    void Renderer::SetShaderParameter(const char *paramName, float x)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLuint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform1f(location, x);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform2f(location, x, y);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform3f(location, x, y, z);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z, float w)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniform4f(location, x, y, z, w);
        }
    }

    // Column-major for matrices.
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat2 &value)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix2fv(location, 1, false, &value[0][0]);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat3 &value)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix3fv(location, 1, false, &value[0][0]);
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat4 &value)
    {
        auto rendererData = static_cast<Shader_GL20*>(RendererState.CurrentShader->GetRendererData());
        if (rendererData != nullptr)
        {
            GLint location = glGetUniformLocation(rendererData->program, paramName);
            glUniformMatrix4fv(location, 1, false, &value[0][0]);
        }
    }

    void Renderer::SetDrawBuffers(size_t count, int *buffers)
    {
        glDrawBuffers((GLsizei)count, ToOpenGLDrawBuffers(count, buffers));
    }


    void Renderer::DeleteTexture2DData(void* rendererDataIn)
    {
        auto rendererData = static_cast<Texture2D_GL20*>(rendererDataIn);
        if (rendererData != nullptr)
        {
            glDeleteTextures(1, &rendererData->object);

            delete rendererData;
        }
    }

    void Renderer::DeleteFramebufferData(void* rendererDataIn)
    {
        auto rendererData = static_cast<Framebuffer_GL20*>(rendererDataIn);
        if (rendererData != nullptr)
        {
            glDeleteFramebuffersEXT(1, &rendererData->object);

            delete rendererData;
        }
    }

    void Renderer::DeleteShaderData(void* rendererDataIn)
    {
        auto rendererData = static_cast<Shader_GL20*>(rendererDataIn);
        if (rendererData != nullptr)
        {
            glDeleteShader(rendererData->vertexShader);
            glDeleteShader(rendererData->fragmentShader);
            glDeleteProgram(rendererData->program);

            delete rendererData;
        }
    }

    void Renderer::DeleteVertexArrayData(void* rendererDataIn)
    {
        auto rendererData = static_cast<VertexArray_GL20*>(rendererDataIn);
        if (rendererData != nullptr)
        {
            glDeleteBuffers(1, &rendererData->verticesObject);
            glDeleteBuffers(1, &rendererData->indicesObject);

            delete rendererData;
        }
    }


    void Renderer::DrawGUI(const GTGUI::Server &gui)
    {
        GUIRenderer::DrawGUI(gui);
    }


    // Support.
    bool Renderer::SupportFloatTextures()
    {
        return GTGL_ARB_texture_float != 0;
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
