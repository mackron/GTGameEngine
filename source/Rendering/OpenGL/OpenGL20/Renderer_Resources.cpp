// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

// This file contains everything relating to resource management for the renderer.


#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTEngine/Rendering/OpenGL/OpenGL20.hpp>
#include <GTEngine/Rendering/RCCache.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/String.hpp>

#include <gtgl/gtgl.h>

#include "../TypeConversion.hpp"

// We're going to silence warnings about members being initialised in constructors. Makes the render commands ugly.
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif


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
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->magFilter);

                if (GTGL_EXT_texture_filter_anisotropic)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
                }


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

        OpenGL20::Texture2D* texture;
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

        OpenGL20::Texture2D* texture;
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

        OpenGL20::Texture2D* texture;
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

        OpenGL20::Texture2D* texture;
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

        OpenGL20::Texture2D* texture;
        GLint filter;
    };

    struct RCOnTexture2DAnisotropyChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            if (GTGL_EXT_texture_filter_anisotropic)
            {
                glBindTexture(GL_TEXTURE_2D, this->texture->object);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
            }
        }

        OpenGL20::Texture2D* texture;
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

        OpenGL20::Texture2D* texture;
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
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->minFilter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->magFilter);

            if (GTGL_EXT_texture_filter_anisotropic)
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);
            }

            // Wrap Mode.
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }

        OpenGL20::TextureCube* texture;

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

        OpenGL20::TextureCube* texture;
    };

    struct RCOnTextureCubeMinificationFilterChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture->object);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->filter);
        }

        OpenGL20::TextureCube* texture;
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

        OpenGL20::TextureCube* texture;
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

        OpenGL20::TextureCube* texture;
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

        OpenGL20::VertexArray* vertexArray;
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

        OpenGL20::VertexArray* vertexArray;
    };

    struct RCOnVertexArrayVertexDataChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->vertexArray != nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, this->vertexArray->verticesObject);
            glBufferData(GL_ARRAY_BUFFER, this->dataSize, this->data, this->usage);


            // The previous binding needs to be restored.
            glBindBuffer(GL_ARRAY_BUFFER, OpenGL20::GetCurrentOpenGLVertexBufferObject());


            this->vertexArray->vertexCount = this->vertexCount;

            free(this->data);
        }

        OpenGL20::VertexArray* vertexArray;

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


            // The previous binding needs to be restored.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL20::GetCurrentOpenGLIndexBufferObject());


            this->vertexArray->indexCount = this->indexCount;

            free(this->data);
        }

        OpenGL20::VertexArray* vertexArray;

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

        OpenGL20::Shader* shader;

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

        OpenGL20::Shader* shader;
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

        OpenGL20::Framebuffer* framebuffer;
    };

    struct RCOnFramebufferDeleted : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glDeleteFramebuffersEXT(1, &framebuffer->object);

            delete this->framebuffer;
        }

        OpenGL20::Framebuffer* framebuffer;
    };

    struct RCOnColourBufferChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);
            
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->framebuffer->object);
            OpenGL20::SetFramebufferColourBuffer((this->texture != nullptr) ? this->texture->object : 0, this->attachmentIndex, this->textureTarget);
        }

        OpenGL20::Framebuffer* framebuffer;
        OpenGL20::Texture2D*   texture;
        size_t                 attachmentIndex;
        Texture2DTarget        textureTarget;
    };


    struct RCOnDepthStencilBufferChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->framebuffer != nullptr);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->framebuffer->object);

            OpenGL20::SetFramebufferDepthBuffer((this->texture != nullptr) ? this->texture->object : 0);
            if (this->changeStencil)
            {
                OpenGL20::SetFramebufferStencilBuffer((this->texture != nullptr) ? this->texture->object : 0);
            }
        }

        OpenGL20::Framebuffer* framebuffer;
        OpenGL20::Texture2D*   texture;
        bool                   changeStencil;
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

        OpenGL20::Framebuffer* framebuffer;
    };
}
