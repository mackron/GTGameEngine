// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Framebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Framebuffer::Framebuffer()
        : colourAttachments(nullptr), depthStencilAttachment(nullptr),
          rendererData(nullptr)
    {
        // We need to allocate the array of colour attachments. These need to be initialised to null.
        this->colourAttachments = new Texture2D*[Renderer::GetMaxColourAttachments()];

        // All colour attachments needs to start life as nullptr.
        for (size_t i = 0; i < Renderer::GetMaxColourAttachments(); ++i)
        {
            this->colourAttachments[i] = nullptr;
        }


        Renderer::OnFramebufferCreated(*this);
    }

    Framebuffer::~Framebuffer()
    {
        // First we need to detach everything. We do not want to delete the attachments, just detach.
        for (size_t i = 0; i < Renderer::GetMaxColourAttachments(); ++i)
        {
            this->DetachColourBuffer(i);
        }

        this->DetachDepthStencilBuffer();

        // This just deallocates the array. It won't delete any textures.
        delete [] this->colourAttachments;

        // Only after the attachments have been detached should we collect.
        Renderer::OnFramebufferDeleted(*this);
    }

    bool Framebuffer::AttachColourBuffer(Texture2D *buffer, size_t index, bool immediateRendererUpdate)
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            if (this->colourAttachments[index] != buffer)
            {
                // The old attachment needs to be detached.
                this->DetachColourBuffer(index);

                this->colourAttachments[index] = buffer;
                this->colourAttachments[index]->OnAttachToFramebuffer(this);

                Renderer::OnColourBufferAttached(*this, index, immediateRendererUpdate);
            }

            return true;
        }

        return false;
    }

    bool Framebuffer::AttachDepthStencilBuffer(Texture2D *buffer, bool immediateRendererUpdate)
    {
        if (this->depthStencilAttachment != buffer)
        {
            // The old attachment needs to be detached.
            this->DetachDepthStencilBuffer();

            this->depthStencilAttachment = buffer;
            this->depthStencilAttachment->OnAttachToFramebuffer(this);

            Renderer::OnDepthStencilBufferAttached(*this, immediateRendererUpdate);
        }

        return true;
    }

    void Framebuffer::DetachColourBuffer(size_t index, bool immediateRendererUpdate)
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            if (this->colourAttachments[index] != nullptr)
            {
                this->colourAttachments[index]->OnDetachFromFramebuffer(this);
                this->colourAttachments[index] = nullptr;

                Renderer::OnColourBufferDetached(*this, index, immediateRendererUpdate);
            }
        }
    }

    void Framebuffer::DetachDepthStencilBuffer(bool immediateRendererUpdate)
    {
        if (this->depthStencilAttachment != nullptr)
        {
            this->depthStencilAttachment->OnDetachFromFramebuffer(this);
            this->depthStencilAttachment = nullptr;

            Renderer::OnDepthStencilBufferDetached(*this, immediateRendererUpdate);
        }
    }

    void Framebuffer::DetachBuffer(Texture2D* buffer)
    {
        for (size_t i = 0; i < Renderer::GetMaxColourAttachments(); ++i)
        {
            if (this->colourAttachments[i] == buffer)
            {
                this->DetachColourBuffer(i);
            }
        }

        if (this->depthStencilAttachment == buffer)
        {
            this->DetachDepthStencilBuffer();
        }
    }

    void Framebuffer::DetachAllBuffers()
    {
        for (size_t i = 0; i < Renderer::GetMaxColourAttachments(); ++i)
        {
            this->DetachColourBuffer(i);
        }

        this->DetachDepthStencilBuffer();
    }


    Texture2D * Framebuffer::GetColourBuffer(size_t index)
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            return this->colourAttachments[index];
        }

        return nullptr;
    }

    const Texture2D * Framebuffer::GetColourBuffer(size_t index) const
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            return this->colourAttachments[index];
        }

        return nullptr;
    }


    Texture2D * Framebuffer::GetDepthStencilBuffer()
    {
        return this->depthStencilAttachment;
    }

    const Texture2D * Framebuffer::GetDepthStencilBuffer() const
    {
        return this->depthStencilAttachment;
    }


    void Framebuffer::CheckStatus()
    {
        Renderer::OnCheckFramebuffer(*this);
    }
}
