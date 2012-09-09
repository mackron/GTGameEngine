
// This file contains everything relating to resource management for the renderer.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>
#include <GTEngine/Rendering/RCCache.hpp>
#include <GTEngine/Logging.hpp>

#include <gtgl/gtgl.h>

#include "../OpenGL/TypeConversion.hpp"


// Render commands.
namespace GTEngine
{
    /////////////////////////////////////////////////////////////
    // Texture2Ds

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

    struct RCOnTexture2DCreated : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(texture != nullptr);
            glGenTextures(1, &this->texture->object);
            glBindTexture(GL_TEXTURE_2D, texture->object);

            // Filter.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,         this->minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,         this->magFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropy);

            // Wrap Mode.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapMode);
        }

        Texture2D_GL20* texture;

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
            glDeleteTextures(1, &this->texture->object);

            delete texture;
        }

        Texture2D_GL20* texture;
    };

    struct RCOnTexture2DMipmapChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
            assert(this->texture != nullptr);

            glBindTexture(GL_TEXTURE_2D, this->texture->object);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            {
                glTexImage2D(GL_TEXTURE_2D, this->mipmap, this->internalFormat, this->width, this->height, 0, this->format, this->type, this->data);
            }
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(this->baseMipLevel));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  static_cast<GLint>(this->maxMipLevel));

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);

            free(this->data);
        }

        Texture2D_GL20* texture;

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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter);
        }

        Texture2D_GL20* texture;
        GLint filter;
    };

    struct RCOnTexture2DAnisotropyChanged : public GTEngine::RenderCommand
    {
        void Execute()
        {
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
            glBindTexture(GL_TEXTURE_2D, this->texture->object);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapMode);
        }

        Texture2D_GL20* texture;
        GLint wrapMode;
    };
}


namespace GTEngine
{
    /// The RC queues for resource events.
    static RCQueue ResourceRCQueues[2];

    static RCCache<RCOnTexture2DCreated>                    RCCache_OnTexture2DCreated[2];
    static RCCache<RCOnTexture2DDeleted>                    RCCache_OnTexture2DDeleted[2];
    static RCCache<RCOnTexture2DMipmapChanged>              RCCache_OnTexture2DMipmapChanged[2];
    static RCCache<RCOnTexture2DMinificationFilterChanged>  RCCache_OnTexture2DMinificationFilterChanged[2];
    static RCCache<RCOnTexture2DMagnificationFilterChanged> RCCache_OnTexture2DMagnificationFilterChanged[2];
    static RCCache<RCOnTexture2DAnisotropyChanged>          RCCache_OnTexture2DAnisotropyChanged[2];
    static RCCache<RCOnTexture2DWrapModeChanged>            RCCache_OnTexture2DWrapModeChanged[2];


    void Renderer::OnTexture2DCreated(Texture2D &texture)
    {
        if (!(texture.IsFloatingPointFormat() && !Renderer::SupportFloatTextures()))
        {
            auto rendererData = new Texture2D_GL20;     // This is deleted in RCOnTexture2DDeleted::Execute(), after the OpenGL object has been deleted.
            texture.SetRendererData(rendererData);

            auto &rc = RCCache_OnTexture2DCreated[Renderer::BackIndex].Acquire();
            rc.texture = rendererData;


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
        auto &rc = RCCache_OnTexture2DMinificationFilterChanged[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());

        size_t baseMipLevel, maxMipLevel;
        texture.GetValidMipmapRange(baseMipLevel, maxMipLevel);

        rc.filter = ToOpenGLTextureMinificationFilter(texture.GetMinificationFilter(), baseMipLevel != maxMipLevel);


        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTexture2DMagnificationFilterChanged(Texture2D &texture)
    {
        auto &rc = RCCache_OnTexture2DMagnificationFilterChanged[Renderer::BackIndex].Acquire();
        rc.texture = static_cast<Texture2D_GL20*>(texture.GetRendererData());

        size_t baseMipLevel, maxMipLevel;
        texture.GetValidMipmapRange(baseMipLevel, maxMipLevel);

        rc.filter = ToOpenGLTextureMinificationFilter(texture.GetMagnificationFilter());


        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTexture2DAnisotropyChanged(Texture2D &texture)
    {
        auto &rc = RCCache_OnTexture2DAnisotropyChanged[Renderer::BackIndex].Acquire();
        rc.texture    = static_cast<Texture2D_GL20*>(texture.GetRendererData());
        rc.anisotropy = static_cast<GLint>(texture.GetAnisotropy());

        ResourceRCQueues[Renderer::BackIndex].Append(rc);
    }

    void Renderer::OnTexture2DWrapModeChanged(Texture2D &texture)
    {
        auto &rc = RCCache_OnTexture2DWrapModeChanged[Renderer::BackIndex].Acquire();
        rc.texture  = static_cast<Texture2D_GL20*>(texture.GetRendererData());
        rc.wrapMode = ToOpenGLWrapMode(texture.GetWrapMode());

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
    }
}