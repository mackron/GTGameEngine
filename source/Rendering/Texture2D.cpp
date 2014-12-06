// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Texture2D::Texture2D()
        : GTLib::Image(), relativePath(),
          framebuffers(), shaders(),
          refCount(1)
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }
    }

    Texture2D::Texture2D(unsigned int width, unsigned int height, GTLib::ImageFormat format, const void *data)
        : GTLib::Image(), relativePath(),
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
    }


    const char* Texture2D::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }

    void Texture2D::SetRelativePath(const char* newRelativePath)
    {
        this->relativePath = newRelativePath;
    }


    void Texture2D::SetData(unsigned int width, unsigned int height, GTLib::ImageFormat format, const void* data)
    {
        GTLib::Image::Load(width, height, format, data);
    }
    
    void Texture2D::SetSubData(unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, const void* data)
    {
        GTLib::Image::SetSubData(xPos, yPos, width, height, data);
    }
    

    void Texture2D::Resize(unsigned int newWidth, unsigned int newHeight)
    {
        this->SetData(newWidth, newHeight, this->GetFormat(), nullptr);
    }



    unsigned int Texture2D::GetWidth() const
    {
        return GTLib::Image::GetWidth();
    }

    unsigned int Texture2D::GetHeight() const
    {
        return GTLib::Image::GetHeight();
    }

    GTLib::ImageFormat Texture2D::GetFormat() const
    {
        return GTLib::Image::GetFormat();
    }


    size_t Texture2D::GetMipmapCount() const
    {
        return GTLib::Image::GetMipmapCount();
    }

    const GTLib::Mipmap & Texture2D::GetMipmap(size_t index) const
    {
        return GTLib::Image::GetMipmap(index);
    }




    void Texture2D::DeleteLocalData()
    {
        for (size_t i = 0; i < this->GetMipmapCount(); ++i)
        {
            GTLib::Image::GetMipmap(i).DeleteLocalData();
        }
    }



    //////////////////////////////////////
    // Internal Use.

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


    int Texture2D::IncrementReferenceCounter()
    {
        this->refCount += 1;

        return this->refCount;
    }

    int Texture2D::DecrementReferenceCounter()
    {
        if (this->refCount > 0)
        {
            this->refCount -= 1;
        }

        return this->refCount;
    }
}
