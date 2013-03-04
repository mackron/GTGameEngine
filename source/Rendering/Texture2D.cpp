// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Texture2D::Texture2D()
        : GTImage::Image(), relativePath(),
          framebuffers(), shaders(),
          refCount(1)
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }
    }

    Texture2D::Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void *data)
        : GTImage::Image(), relativePath(),
          framebuffers(), shaders(),
          refCount(1)
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }

        this->SetData(width, height, format, data);
    }

    Texture2D::~Texture2D()
    {
        // The texture needs to be detached from all framebuffers.
        while (this->framebuffers.root != nullptr)
        {
            this->framebuffers.root->value->DetachBuffer(this);
        }

        // We also need to detach from all shaders.
        while (this->shaders.root != nullptr)
        {
            this->shaders.root->value->OnTextureDeleted(this);
            this->shaders.RemoveRoot();
        }
    }


    const char* Texture2D::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }


    void Texture2D::SetData(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void* data)
    {
        GTImage::Image::Load(width, height, format, data);
    }

    void Texture2D::Resize(unsigned int newWidth, unsigned int newHeight)
    {
        this->SetData(newWidth, newHeight, this->GetFormat(), nullptr);
    }



    unsigned int Texture2D::GetWidth() const
    {
        return GTImage::Image::GetWidth();
    }

    unsigned int Texture2D::GetHeight() const
    {
        return GTImage::Image::GetHeight();
    }

    GTImage::ImageFormat Texture2D::GetFormat() const
    {
        return GTImage::Image::GetFormat();
    }


    size_t Texture2D::GetMipmapCount() const
    {
        return GTImage::Image::GetMipmapCount();
    }

    const GTImage::Mipmap & Texture2D::GetMipmap(size_t index) const
    {
        return GTImage::Image::GetMipmap(index);
    }




    void Texture2D::DeleteLocalData()
    {
        for (size_t i = 0; i < this->GetMipmapCount(); ++i)
        {
            GTImage::Image::GetMipmap(i).DeleteLocalData();
        }
    }



    void Texture2D::OnAttachToFramebuffer(Framebuffer *framebuffer)
    {
        if (this->framebuffers.Find(framebuffer) == nullptr)
        {
            this->framebuffers.Append(framebuffer);
        }
    }

    void Texture2D::OnDetachFromFramebuffer(Framebuffer *framebuffer)
    {
        this->framebuffers.Remove(this->framebuffers.Find(framebuffer));
    }


    void Texture2D::OnAttachToShader(Shader &shader)
    {
        if (this->shaders.Find(&shader) == nullptr)
        {
            this->shaders.Append(&shader);
        }
    }

    void Texture2D::OnDetachFromShader(Shader &shader)
    {
        this->shaders.Remove(this->shaders.Find(&shader));
    }
}
