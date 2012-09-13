
// This file contains everything relating to resource management for the renderer.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTEngine/Rendering/RCCache.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>

#include <gtgl/gtgl.h>

#include "../OpenGL/TypeConversion.hpp"
#include "../OpenGL/Resources.hpp"

#include <GTCore/String.hpp>


// Render commands.
namespace GTEngine
{
    /////////////////////////////////////////////////////////////
    // Texture2Ds

    struct RCOnTexture2DCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);

            // If we're a face on a cube map we don't actually want to create any texture objects. Instead, we can assert that the currently bound texture
            // is the cube map object.
            if (target == Texture2DTarget_Default)
            {
                glGenTextures(1, &this->texture->object);
                glBindTexture(GL_TEXTURE_2D, texture->object);

                // Filter.
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,         this->minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,         this->magFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);

                // Wrap Mode.
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapMode);
            }
            else
            {
                // The target is a face on a cube map. We're goint to set the OpenGL object to the main cube map object, which will be bound.
                GLint cubeMapObject;
                glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &cubeMapObject);

                this->texture->object = static_cast<GLuint>(cubeMapObject);
            }            
        }

        Texture2D_GL20* texture;
        Texture2DTarget target;

        GLint minFilter;
        GLint magFilter;
        GLint anisotropy;

        GLint wrapMode;
    };

    struct RCOnTexture2DDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);

            // We don't delete any objects if we're a cube map face.
            if (this->target == Texture2DTarget_Default)
            {
                glDeleteTextures(1, &this->texture->object);
            }

            delete texture;
        }

        Texture2D_GL20* texture;
        Texture2DTarget target;
    };

    struct RCOnTexture2DMipmapChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            if (this->target == Texture2DTarget_Default)
            {
                glBindTexture(GL_TEXTURE_2D, this->texture->object);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(this->baseMipLevel));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  static_cast<GLint>(this->maxMipLevel));

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
            }
            else
            {
                glEnable(GL_TEXTURE_CUBE_MAP);
                glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);
            }



            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            {
                glTexImage2D(ToOpenGLTexture2DTarget(this->target), this->mipmap, this->internalFormat, this->width, this->height, 0, this->format, this->type, this->data);
            }
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


            
            free(this->data);
        }

        Texture2D_GL20* texture;
        Texture2DTarget target;

        GLint mipmap;

        GLsizei width;
        GLsizei height;

        GLint  internalFormat;
        GLenum format;
        GLenum type;

        void* data;

        size_t baseMipLevel;
        size_t maxMipLevel;

        GLint minFilter;
    };

    struct RCOnTexture2DMinificationFilterChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_2D, this->texture->object);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter);
        }

        Texture2D_GL20* texture;
        GLint filter;
    };

    struct RCOnTexture2DMagnificationFilterChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_2D, this->texture->object);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter);
        }

        Texture2D_GL20* texture;
        GLint filter;
    };

    struct RCOnTexture2DAnisotropyChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_2D, this->texture->object);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
        }

        Texture2D_GL20* texture;
        GLint anisotropy;
    };

    struct RCOnTexture2DWrapModeChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_2D, this->texture->object);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapMode);
        }

        Texture2D_GL20* texture;
        GLint wrapMode;
    };



    /////////////////////////////////////////////////////////////
    // TextureCubes

    struct RCOnTextureCubeCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);
            glGenTextures(1, &this->texture->object);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL,  0);

            // Filter.
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,         this->minFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,         this->magFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);

            // Wrap Mode.
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }

        TextureCube_GL20* texture;

        GLint minFilter;
        GLint magFilter;
        GLint anisotropy;
    };

    struct RCOnTextureCubeDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);
            glDeleteTextures(1, &this->texture->object);

            delete texture;
        }

        TextureCube_GL20* texture;
    };

    struct RCOnTextureCubeMinificationFilterChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->filter);
        }

        TextureCube_GL20* texture;
        GLint filter;
    };

    struct RCOnTextureCubeMagnificationFilterChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->filter);
        }

        TextureCube_GL20* texture;
        GLint filter;
    };

    struct RCOnTextureCubeAnisotropyChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
        }

        TextureCube_GL20* texture;
        GLint anisotropy;
    };



    /////////////////////////////////////////////////////////////
    // VertexArrays

    struct RCOnVertexArrayCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->vertexArray != nullptr);

            glGenBuffers(1, &this->vertexArray->verticesObject);
            glGenBuffers(1, &this->vertexArray->indicesObject);
        }

        VertexArray_GL20* vertexArray;
    };

    struct RCOnVertexArrayDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->vertexArray != nullptr);

            glDeleteBuffers(1, &this->vertexArray->verticesObject);
            glDeleteBuffers(1, &this->vertexArray->indicesObject);

            delete this->vertexArray;
        }

        VertexArray_GL20* vertexArray;
    };

    struct RCOnVertexArrayVertexDataChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->vertexArray != nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, this->vertexArray->verticesObject);
            glBufferData(GL_ARRAY_BUFFER, this->dataSize, this->data, this->usage);

            this->vertexArray->vertexCount = this->vertexCount;

            free(this->data);
        }

        VertexArray_GL20* vertexArray;

        GLsizeiptr dataSize;
        void*      data;
        GLenum     usage;

        GLsizei    vertexCount;
    };

    struct RCOnVertexArrayIndexDataChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->vertexArray != nullptr);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexArray->indicesObject);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataSize, this->data, this->usage);

            this->vertexArray->indexCount = this->indexCount;

            free(this->data);
        }

        VertexArray_GL20* vertexArray;

        GLsizeiptr dataSize;
        void*      data;
        GLenum     usage;

        GLsizei    indexCount;
    };



    /////////////////////////////////////////////////////////////
    // Shader

    bool ShaderGL20_LinkShader(GLuint vertexObject, GLuint fragmentObject, GLuint &programOut)
    {
        // First we detach everything.
        if (programOut != 0)
        {
            GLuint attachedShaders[2];
            GLsizei count;
            glGetAttachedShaders(programOut, 2, &count, attachedShaders);

            for (GLsizei i = 0; i < count; ++i)
            {
                glDetachShader(programOut, attachedShaders[i]);
            }
        }


        // We need to have concretely defined vertex attributes for OpenGL 2.0 GLSL since we don't really have much control of vertex attributes
        // from inside the shader code. Thus, we're going to have to use hard coded attributes names. Later on we might make this configurable
        // via the shader library or a config file.
        glBindAttribLocation(programOut, 0, "VertexInput_Position");
        glBindAttribLocation(programOut, 1, "VertexInput_TexCoord");
        glBindAttribLocation(programOut, 2, "VertexInput_Normal");
        glBindAttribLocation(programOut, 3, "VertexInput_Tangent");
        glBindAttribLocation(programOut, 4, "VertexInput_Bitangent");
        glBindAttribLocation(programOut, 5, "VertexInput_Colour");


        // Finally we reattach the shaders, link the program and check for errors.
        if (vertexObject   != 0) glAttachShader(programOut, vertexObject);
        if (fragmentObject != 0) glAttachShader(programOut, fragmentObject);

        glLinkProgram(programOut);


        // Check for link errors.
        GLint linkStatus;
        glGetProgramiv(programOut, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_FALSE)
        {
            GLint logLength;
            glGetProgramiv(programOut, GL_INFO_LOG_LENGTH, &logLength);

            auto log = new char[logLength];
            glGetProgramInfoLog(programOut, logLength, nullptr, log);

            Log("--- Program Link Status ---\n%s", log);

            delete [] log;

            return false;
        }

        return true;
    }

    GLuint ShaderGL20_LinkShader(GLuint vertexObject, GLuint fragmentObject)
    {
        GLuint program = glCreateProgram();
        if (!ShaderGL20_LinkShader(vertexObject, fragmentObject, program))
        {
            glDeleteProgram(program);
            program = 0;
        }

        return program;
    }



    struct RCOnShaderCreated : public GTEngine::RenderCommand
    {
        GLuint CreateShader(GLenum shaderType, const GTCore::String &sourceIn)
        {
            auto source       = sourceIn.c_str();
            auto sourceLength = static_cast<GLint>(GTCore::Strings::SizeInBytes(source));

            auto shaderObject = glCreateShader(shaderType);
            glShaderSource(shaderObject, 1, &source, &sourceLength);
            glCompileShader(shaderObject);


            // Here we need to log any details.
            GLint compileStatus;
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);

            if (compileStatus == GL_FALSE)
            {
                GLint shaderType;
                glGetShaderiv(shaderObject, GL_SHADER_TYPE, &shaderType);

                GLint logLength;
                glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);

                auto log = new char[logLength];
                glGetShaderInfoLog(shaderObject, logLength, nullptr, log);

                Log("--- %s ---\n%s\n%s", (shaderType == GL_VERTEX_SHADER) ? "Vertex Shader Info Log" : "Fragment Shader Info Log", log, source);


                glDeleteShader(shaderObject);
                shaderObject = 0;

                delete [] log;
            }


            return shaderObject;
        }

        void Execute()
        {
            assert(shader != nullptr);


            GLuint vertexShader = 0;
            if (!vertexSource.IsEmpty())
            {
                vertexShader = this->CreateShader(GL_VERTEX_SHADER, this->vertexSource);
            }


            GLuint fragmentShader = 0;
            if (!fragmentSource.IsEmpty())
            {
                fragmentShader = this->CreateShader(GL_FRAGMENT_SHADER, this->fragmentSource);
            }


            this->shader->program = ShaderGL20_LinkShader(vertexShader, fragmentShader);


            if (vertexShader   != 0) glDeleteShader(vertexShader);
            if (fragmentShader != 0) glDeleteShader(fragmentShader);
        }

        Shader_GL20* shader;

        GTCore::String vertexSource;
        GTCore::String fragmentSource;
    };

    struct RCOnShaderDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(shader != nullptr);

            glDeleteProgram(shader->program);

            delete this->shader;
        }

        Shader_GL20* shader;
    };



    /////////////////////////////////////////////////////////////
    // Framebuffer

    struct RCOnFramebufferCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glGenFramebuffersEXT(1, &this->framebuffer->object);
        }

        Framebuffer_GL20* framebuffer;
    };

    struct RCOnFramebufferDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glDeleteFramebuffersEXT(1, &framebuffer->object);

            delete this->framebuffer;
        }

        Framebuffer_GL20* framebuffer;
    };

    struct RCOnColourBufferChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->framebuffer->object);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, this->attachment, ToOpenGLTexture2DTarget(this->textureTarget), (this->texture != nullptr) ? this->texture->object : 0, 0);
        }

        Framebuffer_GL20* framebuffer;
        Texture2D_GL20*   texture;
        Texture2DTarget   textureTarget;
        GLenum            attachment;
    };


    struct RCOnDepthStencilBufferChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->framebuffer->object);

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, (this->texture != nullptr) ? this->texture->object : 0, 0);
            if (this->changeStencil)
            {
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, (this->texture != nullptr) ? this->texture->object : 0, 0);
            }
        }

        Framebuffer_GL20* framebuffer;
        Texture2D_GL20*   texture;
        bool              changeStencil;
    };


    struct RCCheckFramebuffer : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->framebuffer->object);

            GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
            {
                GTEngine::PostError("Renderer: Framebuffer is invalid. OpenGL status code: %s", gtglGetFramebufferStatusString(status));
            }
        }

        Framebuffer_GL20* framebuffer;
    };
}


namespace GTEngine
{
    /// The RC queues for resource events.
    static RCQueue ResourceRCQueues[2];

    static RCCache<RCOnTexture2DCreated>                      RCCache_OnTexture2DCreated[2];
    static RCCache<RCOnTexture2DDeleted>                      RCCache_OnTexture2DDeleted[2];
    static RCCache<RCOnTexture2DMipmapChanged>                RCCache_OnTexture2DMipmapChanged[2];
    static RCCache<RCOnTexture2DMinificationFilterChanged>    RCCache_OnTexture2DMinificationFilterChanged[2];
    static RCCache<RCOnTexture2DMagnificationFilterChanged>   RCCache_OnTexture2DMagnificationFilterChanged[2];
    static RCCache<RCOnTexture2DAnisotropyChanged>            RCCache_OnTexture2DAnisotropyChanged[2];
    static RCCache<RCOnTexture2DWrapModeChanged>              RCCache_OnTexture2DWrapModeChanged[2];

    static RCCache<RCOnTextureCubeCreated>                    RCCache_OnTextureCubeCreated[2];
    static RCCache<RCOnTextureCubeDeleted>                    RCCache_OnTextureCubeDeleted[2];
    static RCCache<RCOnTextureCubeMinificationFilterChanged>  RCCache_OnTextureCubeMinificationFilterChanged[2];
    static RCCache<RCOnTextureCubeMagnificationFilterChanged> RCCache_OnTextureCubeMagnificationFilterChanged[2];
    static RCCache<RCOnTextureCubeAnisotropyChanged>          RCCache_OnTextureCubeAnisotropyChanged[2];

    static RCCache<RCOnVertexArrayCreated>                    RCCache_OnVertexArrayCreated[2];
    static RCCache<RCOnVertexArrayDeleted>                    RCCache_OnVertexArrayDeleted[2];
    static RCCache<RCOnVertexArrayVertexDataChanged>          RCCache_OnVertexArrayVertexDataChanged[2];
    static RCCache<RCOnVertexArrayIndexDataChanged>           RCCache_OnVertexArrayIndexDataChanged[2];

    static RCCache<RCOnShaderCreated>                         RCCache_OnShaderCreated[2];
    static RCCache<RCOnShaderDeleted>                         RCCache_OnShaderDeleted[2];

    static RCCache<RCOnFramebufferCreated>                    RCCache_OnFramebufferCreated[2];
    static RCCache<RCOnFramebufferDeleted>                    RCCache_OnFramebufferDeleted[2];
    static RCCache<RCOnColourBufferChanged>                   RCCache_OnColourBufferChanged[2];
    static RCCache<RCOnDepthStencilBufferChanged>             RCCache_OnDepthStencilBufferChanged[2];
    static RCCache<RCCheckFramebuffer>                        RCCache_OnCheckFramebuffer[2];
    


    void Renderer::OnTexture2DCreated(Texture2D &texture)
    {
        if (!(texture.IsFloatingPointFormat() && !Renderer::SupportFloatTextures()))
        {
            auto rendererData = new Texture2D_GL20;     // This is deleted in RCOnTexture2DDeleted::Execute(), after the OpenGL object has been deleted.
            texture.SetRendererData(rendererData);

            auto &rc = RCCache_OnTexture2DCreated[Renderer::BackIndex].Acquire();
            rc.texture = rendererData;
            rc.target  = texture.GetTarget();


            size_t baseMipLevel, maxMipLevel;
            texture.GetValidMipmapRange(baseMipLevel, maxMipLevel);

            rc.minFilter  = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter(), maxMipLevel != baseMipLevel);
            rc.magFilter  = ToOpenGLTextureMagnificationFilter(texture.GetMagnificationFilter());
            rc.anisotropy = static_cast<GLint>(texture.GetAnisotropy());
            rc.wrapMode   = ToOpenGLWrapMode(texture.GetWrapMode());


            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
        else
        {
            Log("Error Creating Texture2D: Floating-point textures are not supported.");
        }
    }

    void Renderer::OnTexture2DDeleted(Texture2D &texture)
    {
        auto &rc = RCCache_OnTexture2DDeleted[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTexture2DMipmapChanged(Texture2D &texture, size_t mipmapIndex)
    {
        auto &mipmap = texture.GetMipmap(static_cast<unsigned int>(mipmapIndex));

        auto &rc = RCCache_OnTexture2DMipmapChanged[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());
        rc.target  = texture.GetTarget();

        rc.mipmap  = static_cast<GLint>(mipmapIndex);

        rc.width   = static_cast<GLsizei>(mipmap.width);
        rc.height  = static_cast<GLsizei>(mipmap.height);

        rc.internalFormat = ToOpenGLInternalFormat(mipmap.format);
        rc.format         = ToOpenGLFormat(mipmap.format);
        rc.type           = ToOpenGLType(mipmap.format);

        if (mipmap.data != nullptr)
        {
            rc.data = malloc(mipmap.GetDataSizeInBytes());
            memcpy(rc.data, mipmap.data, mipmap.GetDataSizeInBytes());
        }
        else
        {
            rc.data = nullptr;
        }


        texture.GetValidMipmapRange(rc.baseMipLevel, rc.maxMipLevel);


        // With a change in mipmaps, the minification filter may have changed.
        rc.minFilter = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter(), rc.baseMipLevel != rc.maxMipLevel);


        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTexture2DMinificationFilterChanged(Texture2D &texture)
    {
        if (texture.GetTarget() == Texture2DTarget_Default)
        {
            auto &rc = RCCache_OnTexture2DMinificationFilterChanged[Renderer::BackIndex].Acquire();
            rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());

            size_t baseMipLevel, maxMipLevel;
            texture.GetValidMipmapRange(baseMipLevel, maxMipLevel);

            rc.filter = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter(), baseMipLevel != maxMipLevel);


            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
    }

    void Renderer::OnTexture2DMagnificationFilterChanged(Texture2D &texture)
    {
        if (texture.GetTarget() == Texture2DTarget_Default)
        {
            auto &rc = RCCache_OnTexture2DMagnificationFilterChanged[Renderer::BackIndex].Acquire();
            rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());
            rc.filter  = ToOpenGLTextureMinificationFilter(texture.GetMagnificationFilter());

            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
    }

    void Renderer::OnTexture2DAnisotropyChanged(Texture2D &texture)
    {
        if (texture.GetTarget() == Texture2DTarget_Default)
        {
            auto &rc = RCCache_OnTexture2DAnisotropyChanged[Renderer::BackIndex].Acquire();
            rc.texture    = static_cast<Texture2D_GL20*>(texture.GetRendererData());
            rc.anisotropy = static_cast<GLint>(texture.GetAnisotropy());

            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
    }

    void Renderer::OnTexture2DWrapModeChanged(Texture2D &texture)
    {
        if (texture.GetTarget() == Texture2DTarget_Default)
        {
            auto &rc = RCCache_OnTexture2DWrapModeChanged[Renderer::BackIndex].Acquire();
            rc.texture  = static_cast<Texture2D_GL20*>(texture.GetRendererData());
            rc.wrapMode = ToOpenGLWrapMode(texture.GetWrapMode());

            ResourceRCQueues[Renderer::BackIndex].Append(rc);
        }
    }




    void Renderer::OnTextureCubeCreated(TextureCube &texture)
    {
        auto rendererData = new TextureCube_GL20;
        texture.SetRendererData(rendererData);

        auto &rc = RCCache_OnTextureCubeCreated[Renderer::BackIndex].Acquire();
        rc.texture = rendererData;

        rc.minFilter  = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter());
        rc.magFilter  = ToOpenGLTextureMagnificationFilter(texture.GetMagnificationFilter());
        rc.anisotropy = static_cast<GLint>(texture.GetAnisotropy());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTextureCubeDeleted(TextureCube &texture)
    {
        auto &rc = RCCache_OnTextureCubeDeleted[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<TextureCube_GL20*>(texture.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTextureCubeMinificationFilterChanged(TextureCube &texture)
    {
        auto &rc = RCCache_OnTextureCubeMinificationFilterChanged[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<TextureCube_GL20*>(texture.GetRendererData());
        rc.filter  = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTextureCubeMagnificationFilterChanged(TextureCube &texture)
    {
        auto &rc = RCCache_OnTextureCubeMagnificationFilterChanged[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<TextureCube_GL20*>(texture.GetRendererData());
        rc.filter  = ToOpenGLTextureMinificationFilter(texture.GetMagnificationFilter());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTextureCubeAnisotropyChanged(TextureCube &texture)
    {
        auto &rc = RCCache_OnTextureCubeAnisotropyChanged[Renderer::BackIndex].Acquire();
        rc.texture    = static_cast<TextureCube_GL20*>(texture.GetRendererData());
        rc.anisotropy = static_cast<GLint>(texture.GetAnisotropy());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }




    void Renderer::OnVertexArrayCreated(VertexArray &vertexArray)
    {
        auto rendererData = new VertexArray_GL20;     // This is deleted in RCOnVertexArray2DDeleted::Execute(), after the OpenGL object has been deleted.
        vertexArray.SetRendererData(rendererData);

        auto &rc = RCCache_OnVertexArrayCreated[Renderer::BackIndex].Acquire();
        rc.vertexArray = rendererData;

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnVertexArrayDeleted(VertexArray &vertexArray)
    {
        auto &rc = RCCache_OnVertexArrayDeleted[Renderer::BackIndex].Acquire();
        rc.vertexArray = static_cast<VertexArray_GL20*>(vertexArray.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnVertexArrayVertexDataChanged(VertexArray &vertexArray)
    {
        auto &rc = RCCache_OnVertexArrayVertexDataChanged[Renderer::BackIndex].Acquire();
        rc.vertexArray = static_cast<VertexArray_GL20*>(vertexArray.GetRendererData());

        rc.dataSize = static_cast<GLsizeiptr>(vertexArray.GetVertexCount() * vertexArray.GetFormat().GetSize() * sizeof(float));
        
        if (vertexArray.GetVertexDataPtr() != nullptr)
        {
            rc.data = malloc(rc.dataSize);
            memcpy(rc.data, vertexArray.GetVertexDataPtr(), rc.dataSize);
        }
        else
        {
            rc.data = nullptr;
        }

        rc.usage       = ToOpenGLBufferUsage(vertexArray.GetUsage());
        rc.vertexCount = vertexArray.GetVertexCount();


        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnVertexArrayIndexDataChanged(VertexArray &vertexArray)
    {
        auto &rc = RCCache_OnVertexArrayIndexDataChanged[Renderer::BackIndex].Acquire();
        rc.vertexArray = static_cast<VertexArray_GL20*>(vertexArray.GetRendererData());


        rc.dataSize = static_cast<GLsizeiptr>(vertexArray.GetIndexCount() * sizeof(unsigned int));
        
        if (vertexArray.GetIndexDataPtr() != nullptr)
        {
            rc.data = malloc(rc.dataSize);
            memcpy(rc.data, vertexArray.GetIndexDataPtr(), rc.dataSize);
        }
        else
        {
            rc.data = nullptr;
        }

        rc.usage      = ToOpenGLBufferUsage(vertexArray.GetUsage());
        rc.indexCount = vertexArray.GetIndexCount();


        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }





    void Renderer::OnShaderCreated(Shader &shader)
    {
        auto rendererData = new Shader_GL20;
        shader.SetRendererData(rendererData);

        auto &rc = RCCache_OnShaderCreated[Renderer::BackIndex].Acquire();
        rc.shader         = rendererData;
        rc.vertexSource   = shader.GetVertexSource();
        rc.fragmentSource = shader.GetFragmentSource();

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnShaderDeleted(Shader &shader)
    {
        auto &rc = RCCache_OnShaderDeleted[Renderer::BackIndex].Acquire();
        rc.shader = static_cast<Shader_GL20*>(shader.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }





    void Renderer::OnFramebufferCreated(Framebuffer &framebuffer)
    {
        auto rendererData = new Framebuffer_GL20;
        framebuffer.SetRendererData(rendererData);

        auto &rc = RCCache_OnFramebufferCreated[Renderer::BackIndex].Acquire();
        rc.framebuffer = rendererData;

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnFramebufferDeleted(Framebuffer &framebuffer)
    {
        auto &rc = RCCache_OnFramebufferDeleted[Renderer::BackIndex].Acquire();
        rc.framebuffer = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnColourBufferAttached(Framebuffer &framebuffer, size_t index)
    {
        auto texture = framebuffer.GetColourBuffer(index);

        auto &rc = RCCache_OnColourBufferChanged[Renderer::BackIndex].Acquire();
        rc.framebuffer   = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());
        rc.texture       = static_cast<Texture2D_GL20*>(texture->GetRendererData());
        rc.textureTarget = texture->GetTarget();
        rc.attachment    = GL_COLOR_ATTACHMENT0_EXT + index;
        
        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnColourBufferDetached(Framebuffer &framebuffer, size_t index)
    {
        auto &rc = RCCache_OnColourBufferChanged[Renderer::BackIndex].Acquire();
        rc.framebuffer = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());
        rc.texture     = nullptr;
        rc.attachment  = GL_COLOR_ATTACHMENT0_EXT + index;

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnDepthStencilBufferAttached(Framebuffer &framebuffer)
    {
        auto &rc = RCCache_OnDepthStencilBufferChanged[Renderer::BackIndex].Acquire();
        rc.framebuffer   = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());
        rc.texture       = static_cast<Texture2D_GL20*>(framebuffer.GetDepthStencilBuffer()->GetRendererData());
        rc.changeStencil = framebuffer.GetDepthStencilBuffer()->GetFormat() == GTImage::ImageFormat_Depth24_Stencil8;

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnDepthStencilBufferDetached(Framebuffer &framebuffer)
    {
        auto &rc = RCCache_OnDepthStencilBufferChanged[Renderer::BackIndex].Acquire();
        rc.framebuffer   = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());
        rc.texture       = nullptr;
        rc.changeStencil = true;

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnCheckFramebuffer(Framebuffer &framebuffer)
    {
        auto &rc = RCCache_OnCheckFramebuffer[Renderer::BackIndex].Acquire();
        rc.framebuffer = static_cast<Framebuffer_GL20*>(framebuffer.GetRendererData());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }




    void Renderer::ExecuteFrontResourceRCQueue()
    {
        ResourceRCQueues[!Renderer::BackIndex].Execute();
        ResourceRCQueues[!Renderer::BackIndex].Clear();

        RCCache_OnTexture2DCreated[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DDeleted[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DMipmapChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DMinificationFilterChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DMagnificationFilterChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DAnisotropyChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTexture2DWrapModeChanged[!Renderer::BackIndex].Reset();

        RCCache_OnTextureCubeCreated[!Renderer::BackIndex].Reset();
        RCCache_OnTextureCubeDeleted[!Renderer::BackIndex].Reset();
        RCCache_OnTextureCubeMinificationFilterChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTextureCubeMagnificationFilterChanged[!Renderer::BackIndex].Reset();
        RCCache_OnTextureCubeAnisotropyChanged[Renderer::BackIndex].Reset();

        RCCache_OnVertexArrayCreated[!Renderer::BackIndex].Reset();
        RCCache_OnVertexArrayDeleted[!Renderer::BackIndex].Reset();
        RCCache_OnVertexArrayVertexDataChanged[!Renderer::BackIndex].Reset();
        RCCache_OnVertexArrayIndexDataChanged[!Renderer::BackIndex].Reset();

        RCCache_OnShaderCreated[!Renderer::BackIndex].Reset();
        RCCache_OnShaderDeleted[!Renderer::BackIndex].Reset();

        RCCache_OnFramebufferCreated[!Renderer::BackIndex].Reset();
        RCCache_OnFramebufferDeleted[!Renderer::BackIndex].Reset();
        RCCache_OnColourBufferChanged[!Renderer::BackIndex].Reset();
        RCCache_OnDepthStencilBufferChanged[!Renderer::BackIndex].Reset();
        RCCache_OnCheckFramebuffer[!Renderer::BackIndex].Reset();
    }
}