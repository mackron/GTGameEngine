
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
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#if defined(LICK_PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <GTCore/X11/X11.hpp>
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
        case GTImage::ImageFormat_R8:                return GL_R8;
        case GTImage::ImageFormat_RG8:               return GL_RG8;
        case GTImage::ImageFormat_RGB8:              return GL_RGB8;
        case GTImage::ImageFormat_RGBA8:             return GL_RGBA8;
        case GTImage::ImageFormat_RGBA16:            return GL_RGBA16;
        case GTImage::ImageFormat_RGBA32:            return GL_RGBA32UI;
        case GTImage::ImageFormat_RGBA16F:           return GL_RGBA16F;
        case GTImage::ImageFormat_RGBA32F:           return GL_RGBA32F;

        case GTImage::ImageFormat_Depth16:           return GL_DEPTH_COMPONENT16;
        case GTImage::ImageFormat_Depth24:           return GL_DEPTH_COMPONENT24;
        case GTImage::ImageFormat_Depth32:           return GL_DEPTH_COMPONENT32;
        case GTImage::ImageFormat_Depth24_Stencil8:  return GL_DEPTH24_STENCIL8;

        case GTImage::ImageFormat_Auto:
        default: break;
        }

        return GL_RGBA8;
    }

    GLenum ToOpenGLFormat(GTImage::ImageFormat format)
    {
        switch (format)
        {
        case GTImage::ImageFormat_R8:        return GL_RED;
        case GTImage::ImageFormat_RG8:       return GL_RG;
        case GTImage::ImageFormat_RGB8:      return GL_RGB;

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

        case GTImage::ImageFormat_RGBA16F:
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
}

// Renderer Globals. Needs to be before the renderer support utils.
namespace GTEngine
{
    /// The main OpenGL context.
    GTGLcontext OpenGLContext = nullptr;

    bool IsRendererInitialised = false;
    bool IsCgInitialised       = false;
    bool IsDepthTestingEnabled = false;     // <-- Always ensure this is initialised to false.
    bool IsSRGBEnabled         = false;

    // For some VirtualBox hacking...
    bool RendererIsChromium = false;

    // This is a bitfield containing bits representing which vertex attributes are currently enabled on the OpenGL side.
    uint32_t VertexAttribEnableBits = 0x0;

    // The global Cg context for shaders.
    CGcontext ShaderContext         = nullptr;
    CGprofile VertexShaderProfile   = CG_PROFILE_GLSLV;
    CGprofile FragmentShaderProfile = CG_PROFILE_GLSLF;

    // Hardware capabilities.
    struct _RendererCaps
    {
        GLint MaxColourAttachments;         //< The maximum number of framebuffer colour attachments.
        GLint MaxDrawBuffers;               //< The maximum number of draw buffers for MRT.

    }RendererCaps;


    /// Structure containing the current state of the renderer.
    struct _RendererState
    {
        _RendererState()
            : CurrentShader(nullptr),
              ViewportX(0), ViewportY(0), ViewportWidth(0), ViewportHeight(0),
              ScissorX(0), ScissorY(0), ScissorWidth(0), ScissorHeight(0), IsScissorEnabled(false),
              IsBlendingEnabled(false),
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

        /// The current blending state.
        bool IsBlendingEnabled;

        /// Swap interval.
        int  SwapInterval;
        bool SwapIntervalChanged;


    private:    // No copying.
        _RendererState(const _RendererState &);
        _RendererState & operator=(const _RendererState &);

    }RendererState;


    /// We create a global instantiation of this struct so that when it's destructed, it will call Renderer::Uninitialise().
    struct _RendererKiller
    {
        ~_RendererKiller()
        {
            Renderer::Shutdown();
        }
    }RendererKiller;


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

        Texture2D ** colourAttachments;
        Texture2D  * depthStencilAttachment;

    private:    // No copying.
        Framebuffer_GL20(const Framebuffer_GL20 &);
        Framebuffer_GL20 & operator=(const Framebuffer_GL20 &);
    };

    struct Shader_GL20
    {
        Shader_GL20()
            : object(0), usingVertex(false), usingFragment(false)
        {
        }

        CGprogram object;
        bool usingVertex;
        bool usingFragment;
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
}

// Renderer support functions.
namespace GTEngine
{
    /**
    *   \brief  Callback function for Cg errors.
    */
    void CgErrorCallback()
    {
        auto error = cgGetError();
        auto msg   = cgGetErrorString(error);

        GTEngine::PostError("Cg error: %s", msg);
    }

    /**
    *   \brief  Initialises Cg.
    */
    bool InitialiseCg()
    {
        if (!IsCgInitialised)
        {
            Log("Initialising Cg");

            // Here we are initialising Cg.
            ShaderContext = cgCreateContext();
            if (ShaderContext != nullptr)
            {
                cgGLSetDebugMode(CG_TRUE);

                // If we're Chromium, we must always use the GLSL profiles (the default).
                if (!RendererIsChromium)
                {
                    VertexShaderProfile   = cgGLGetLatestProfile(CG_GL_VERTEX);
                    FragmentShaderProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

                    // Some profiles don't work well with MRT. We want to use at least GLSL, or ARBVP1/ARBFP1. If we retrieve FP20, FP30 or FP40, we want
                    // to switch to either ARBVP1/ARBFP1 or GLSL.
                    if (FragmentShaderProfile < CG_PROFILE_GLSLF && FragmentShaderProfile != CG_PROFILE_ARBFP1)
                    {
                        // We actually prefer ARB shaders here. In practice, most drivers should support ARBVP1 and ARBFP1. However, since it's an extension,
                        // we'll do it correctly and check for it first.
                        if (GTGL_ARB_fragment_program)
                        {
                            VertexShaderProfile   = CG_PROFILE_ARBVP1;
                            FragmentShaderProfile = CG_PROFILE_ARBFP1;
                        }
                        else
                        {
                            VertexShaderProfile   = CG_PROFILE_GLSLV;
                            FragmentShaderProfile = CG_PROFILE_GLSLF;
                        }
                    }
                }
                
                cgGLSetOptimalOptions(VertexShaderProfile);
                cgGLSetOptimalOptions(FragmentShaderProfile);

                Log("    Vertex Shader Profile:   %s", cgGetProfileString(VertexShaderProfile));
                Log("    Fragment Shader Profile: %s", cgGetProfileString(FragmentShaderProfile));

                cgGLSetManageTextureParameters(ShaderContext, CG_TRUE);

                IsCgInitialised = true;
            }
        }

        return IsCgInitialised;
    }

    /**
    *   \brief  Retrieves the Cg program object associated with the given shader.
    */
    CGprogram Renderer_GetCgProgramFromShader(const Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = (const Shader_GL20 *)shader->GetRendererData();
            if (rendererData != nullptr)
            {
                return rendererData->object;
            }
        }

        return nullptr;
    }

    CGprogram Renderer_GetCurrentCgProgram()
    {
        return Renderer_GetCgProgramFromShader(RendererState.CurrentShader);
    }

    bool Renderer_IsShaderUsingVertex(const Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = (const Shader_GL20 *)shader->GetRendererData();
            if (rendererData != nullptr)
            {
                return rendererData->usingVertex;
            }
        }

        return false;
    }

    bool Renderer_IsShaderUsingFragment(const Shader *shader)
    {
        if (shader != nullptr)
        {
            auto rendererData = (const Shader_GL20 *)shader->GetRendererData();
            if (rendererData != nullptr)
            {
                return rendererData->usingFragment;
            }
        }

        return false;
    }

    /**
    *   \brief  Logs the last Cg listing. This needs to be called after every program is compiled.
    */
    void Renderer_LogLastCgListing(const char* source = nullptr)
    {
        const char *listing = cgGetLastListing(ShaderContext);
        if (listing != nullptr)
        {
            Log("%s", listing);

            // When we have a shader listing, we'll be best off showing the source so we can easily fix the error. We will also need
            // line numbers. We're going to construct a string with line numbers so we can easily see where any errors are located.
            if (source != nullptr)
            {
                GTCore::String shaderString;

                int lineNumber = 1;

                // We can use a tokenizer to filter the lines.
                GTCore::Strings::Tokenizer line(source, "\n");
                while (line)
                {
                    shaderString.Append(GTCore::ToString(lineNumber));
                    shaderString.Append(":");
                    shaderString.Append(line.start, line.GetSizeInTs());
                    shaderString.Append("\n");

                    ++line;
                    ++lineNumber;
                }

                Log("%s", shaderString.c_str());
            }
        }
    }

    /**
    *   \brief  Logs the Cg generated shader string for the given Cg shader.
    */
    void Renderer_LogCgShaderString(CGprogram shader)
    {
        Log("Shader String for %p:\n%s", shader, cgGetProgramString(shader, CG_COMPILED_PROGRAM));
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


            // If anything is out of sync, it needs to be re-synced.
            if (texture->syncinfo.filterChanged)
            {
                // The minification filter is different depending on whether or not we are using mipmapping.
                size_t baseMipLevel, maxMipLevel;
                texture->GetValidMipmapRange(baseMipLevel, maxMipLevel);

                if (maxMipLevel > baseMipLevel)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->GetFilter() == TextureFilter_Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
                }
                else
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->GetFilter() == TextureFilter_Linear ? GL_LINEAR : GL_NEAREST);
                }
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->GetFilter() == TextureFilter_Linear ? GL_LINEAR : GL_NEAREST);

                // Anisotropy is considered part of the filter.
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, texture->GetAnisotropy());

                texture->syncinfo.filterChanged = false;
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
    *   \brief  Synchronizes a Shader object with it's Cg/OpenGL counterpart.
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

                CGprogram vertexProgram   = nullptr;
                CGprogram fragmentProgram = nullptr;

                if (shader->GetVertexSource() != nullptr)
                {
                    vertexProgram = cgCreateProgram(ShaderContext, CG_SOURCE, shader->GetVertexSource(), VertexShaderProfile, "main", nullptr);
                    Renderer_LogLastCgListing(shader->GetVertexSource());
                    //Renderer_LogCgShaderString(vertexProgram);
                }

                if (shader->GetFragmentSource() != nullptr)
                {
                    fragmentProgram = cgCreateProgram(ShaderContext, CG_SOURCE, shader->GetFragmentSource(), FragmentShaderProfile, "main", nullptr);
                    Renderer_LogLastCgListing(shader->GetFragmentSource());
                    //Renderer_LogCgShaderString(fragmentProgram);
                }
                
                // Vertex and fragment shaders need to be combined where applicable.
                if (vertexProgram != nullptr && fragmentProgram != nullptr)
                {
                    rendererData->object = cgCombinePrograms2(vertexProgram, fragmentProgram);
                    rendererData->usingVertex   = true;
                    rendererData->usingFragment = true;
                    
                    cgDestroyProgram(vertexProgram);
                    cgDestroyProgram(fragmentProgram);

                    Renderer_LogLastCgListing();
                }
                else
                {
                    if (vertexProgram != nullptr)
                    {
                        rendererData->object      = vertexProgram;
                        rendererData->usingVertex = true;
                    }
                    else if (fragmentProgram != nullptr)
                    {
                        rendererData->object        = fragmentProgram;
                        rendererData->usingFragment = true;
                    }
                }

                // Here we need to load the shader.
                if (rendererData->object != nullptr)
                {
                    cgGLLoadProgram(rendererData->object);
                }
                else
                {
                    GTEngine::PostError("Error creating shader. All domains failed to compile.");
                    return false;
                }
            }

            // Now we need to bind everything.
            if (rendererData->usingVertex)
            {
                cgGLEnableProfile(VertexShaderProfile);
            }
            else
            {
                cgGLDisableProfile(VertexShaderProfile);
            }

            if (rendererData->usingFragment)
            {
                cgGLEnableProfile(FragmentShaderProfile);
            }
            else
            {
                cgGLDisableProfile(FragmentShaderProfile);
            }

            // Here we just bind the shader like normal...
            cgGLBindProgram(rendererData->object);

            // With the shader bound, we will now loop through each pending parameter and set them.
            auto &pendingParams = shader->GetPendingParameters();
            for (size_t i = 0; i < pendingParams.count; ++i)
            {
                auto iParam = pendingParams.buffer[i];
                assert(iParam        != nullptr);
                assert(iParam->value != nullptr);

                iParam->value->SetOnCurrentShader(iParam->key);
            }

            // It's important that pending parameters are cleared after setting so that they aren't set again
            // the next time it is made current.
            shader->ClearPendingParameters();

            return true;
        }

        return false;
    }

    bool Renderer_SyncVertexArray(const VertexArray *vertexArray)
    {
        assert(vertexArray != nullptr);

        auto vertexArrayData = (VertexArray_GL20 *)vertexArray->GetRendererData();
        if (vertexArrayData == nullptr)
        {
            vertexArrayData = new VertexArray_GL20;
            vertexArray->SetRendererData(vertexArrayData);
                
            glGenBuffers(1, &vertexArrayData->verticesObject);
            glGenBuffers(1, &vertexArrayData->indicesObject);
        }

        // Syncing binds...
        glBindBuffer(GL_ARRAY_BUFFER, vertexArrayData->verticesObject);
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
            // NOTE: We can't use a forward-compatible context because it will break Cg.
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
                //Log("    NV_bindless_texture:                %s", GTGL_NV_bindless_texture                ? "yes" : "no");
                
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
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &RendererCaps.MaxColourAttachments);
                glGetIntegerv(GL_MAX_DRAW_BUFFERS,      &RendererCaps.MaxDrawBuffers);

                Log("Renderer Caps:");
                Log("    Max Colour Attachments: %d", RendererCaps.MaxColourAttachments);
                Log("    Max Draw Buffers:       %d", RendererCaps.MaxDrawBuffers);
                
                // VirtualBox with Cg doesn't work very well at all. So far I've got it working in a very specific situation. The Cg profiles must
                // be GLSL. Anything else, and it will crash. Also, whenever setting a parameter, the shader needs to be re-bound.
                RendererIsChromium = GTCore::Strings::Equal((const char *)glGetString(GL_RENDERER), "Chromium");

                if (InitialiseCg())
                {
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
                    GTEngine::PostError("Failed to initialise Cg.");
                }
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

            gtglShutdown();
            OpenGLContext = nullptr;

            cgDestroyContext(ShaderContext);
            ShaderContext = nullptr;

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
#ifdef LICK_PLATFORM_WINDOWS
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
            uint32_t nextBit      = (1 << GTCore::NextBitIndex(oldVertexAttribEnableBits));
            GLuint   attribIndex  = static_cast<GLuint>(nextBit);

            glDisableVertexAttribArray(attribIndex);

            // The bit needs to be cleared in preperation for the next iteration.
            oldVertexAttribEnableBits &= ~nextBit;
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
        glEnable(GL_BLEND);
        RendererState.IsBlendingEnabled = true;
    }

    void Renderer::DisableBlending()
    {
        glDisable(GL_BLEND);
        RendererState.IsBlendingEnabled = false;
    }

    void Renderer::SetBlendFunc(BlendFunc sourceFactor, BlendFunc destFactor)
    {
        GLenum sfactor = ToOpenGLBlendFunc(sourceFactor);
        GLenum dfactor = ToOpenGLBlendFunc(destFactor);

        glBlendFunc(sfactor, dfactor);
    }

    void Renderer::EnableAlphaBlending()
    {
        Renderer::EnableBlending();
        Renderer::SetBlendFunc(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
    }


    void Renderer::EnableDepthTest()
    {
        if (!IsDepthTestingEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            IsDepthTestingEnabled = true;
        }
    }
    void Renderer::DisableDepthTest()
    {
        if (IsDepthTestingEnabled)
        {
            glDisable(GL_DEPTH_TEST);
            IsDepthTestingEnabled = false;
        }
    }

    void Renderer::EnableDepthWrites()
    {
        glDepthMask(GL_TRUE);
    }
    void Renderer::DisableDepthWrites()
    {
        glDepthMask(GL_FALSE);
    }

    void Renderer::SetDepthFunc(DepthFunc func)
    {
        GLenum funcGL = GL_LESS;

        switch (func)
        {
        case DepthFunc_Never:    funcGL = GL_NEVER;    break;
        case DepthFunc_Less:     funcGL = GL_LESS;     break;
        case DepthFunc_Equal:    funcGL = GL_EQUAL;    break;
        case DepthFunc_LEqual:   funcGL = GL_LEQUAL;   break;
        case DepthFunc_Greater:  funcGL = GL_GREATER;  break;
        case DepthFunc_NotEqual: funcGL = GL_NOTEQUAL; break;
        case DepthFunc_GEqual:   funcGL = GL_GEQUAL;   break;
        case DepthFunc_Always:   funcGL = GL_ALWAYS;   break;
        }

        glDepthFunc(funcGL);
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
        }
        else
        {
            // Switch to fixed function...
            cgGLUnbindProgram(VertexShaderProfile);
            cgGLUnbindProgram(FragmentShaderProfile);

            cgGLDisableProfile(VertexShaderProfile);
            cgGLDisableProfile(FragmentShaderProfile);

            RendererState.CurrentShader = nullptr;
        }
    }

    void Renderer::SetShaderParameter(const char *paramName, Texture2D *value)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);

            // NOTE: We're using automatic texture management using cgGLSetManageTextureParameters(), which is called in InitialiseCg(). The texture
            //       state is only updated in cgGLBindProgram(), meaning we need to re-bind the program in order for us to make use of the texture.
            //       This will need to be tested for efficiency. We'll probably need to rework our API to account for this better, or do manual state
            //       management.
            //
            //       I'm thinking the problem may be that without auto state management, CG expects the appropriate texture unit to be activated.

            // The texture needs to be synced before attempting to use it. If we fail to sync we disable that texture.
            if (Renderer_SyncTexture2D(value))
            {
                cgGLSetTextureParameter(param, static_cast<Texture2D_GL20*>(value->rendererData)->object);
                //cgGLEnableTextureParameter(param);
            }
            else
            {
                //cgGLDisableTextureParameter(param);
            }

            // If we're Chromium, setting a parameter will change the program binding. We don't want that.
            //if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }

    void Renderer::SetShaderParameter(const char *paramName, float x)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetParameter1f(param, x);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetParameter2f(param, x, y);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetParameter3f(param, x, y, z);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, float x, float y, float z, float w)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetParameter4f(param, x, y, z, w);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }

    // Column-major for matrices.
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat2 &value)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetMatrixParameterfc(param, &value[0][0]);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat3 &value)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetMatrixParameterfc(param, &value[0][0]);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }
    void Renderer::SetShaderParameter(const char *paramName, const glm::mat4 &value)
    {
        CGprogram program = Renderer_GetCurrentCgProgram();
        if (program != nullptr)
        {
            CGparameter param = cgGetNamedParameter(program, paramName);
            cgGLSetMatrixParameterfc(param, &value[0][0]);

            if (RendererIsChromium)
            {
                cgGLBindProgram(program);
            }
        }
    }

    void Renderer::SetDrawBuffers(size_t count, int *buffers)
    {
        glDrawBuffers((GLsizei)count, ToOpenGLDrawBuffers(count, buffers));
    }


    void Renderer::DeleteTexture2DData(void *rendererData)
    {
        if (rendererData != nullptr)
        {
            glDeleteTextures(1, &((Texture2D_GL20 *)rendererData)->object);
        }
    }

    void Renderer::DeleteFramebufferData(void *rendererData)
    {
        if (rendererData != nullptr)
        {
            glDeleteFramebuffersEXT(1, &((Framebuffer_GL20 *)rendererData)->object);
        }
    }

    void Renderer::DeleteShaderData(void *rendererData)
    {
        if (rendererData != nullptr)
        {
            cgDestroyProgram(((Shader_GL20 *)rendererData)->object);
        }
    }

    void Renderer::DeleteVertexArrayData(void *rendererData)
    {
        if (rendererData != nullptr)
        {
            glDeleteBuffers(1, &((VertexArray_GL20 *)rendererData)->verticesObject);
            glDeleteBuffers(1, &((VertexArray_GL20 *)rendererData)->indicesObject);
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
