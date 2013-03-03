// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Framebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    Framebuffer::Framebuffer()
        : colourAttachments(), depthStencilAttachment(nullptr),
          rendererData(nullptr)
    {
        Renderer::OnFramebufferCreated(*this);
    }

    Framebuffer::~Framebuffer()
    {
        // First we need to detach everything. We do not want to delete the attachments, just detach.
        this->DetachAllBuffers();

        // Only after the attachments have been detached should we collect.
        Renderer::OnFramebufferDeleted(*this);
    }

    bool Framebuffer::AttachColourBuffer(Texture2D *buffer, unsigned int index, bool immediateRendererUpdate)
    {
        assert(buffer != nullptr);      // <-- Should use DetachColourBuffer() to remove a buffer. Will probably turn this argument into a reference instead of a pointer.

        if (index < Renderer::GetMaxColourAttachments())
        {
            auto iColourAttachment = this->colourAttachments.Find(index);
            if (iColourAttachment != nullptr)
            {
                if (iColourAttachment->value != buffer)
                {
                    // The old attachment needs to be detached.
                    Renderer::OnColourBufferDetached(*this, index, immediateRendererUpdate);        // TODO: Remove this when the new renderer is complete.

                    assert(iColourAttachment->value != nullptr);
                    {
                        iColourAttachment->value->OnDetachFromFramebuffer(this);
                        iColourAttachment->value = buffer;
                        iColourAttachment->value->OnAttachToFramebuffer(this);
                    }
                }
            }
            else
            {
                this->colourAttachments.Add(index, buffer);
                buffer->OnAttachToFramebuffer(this);
            }

            Renderer::OnColourBufferAttached(*this, index, immediateRendererUpdate);        // TODO: Delete this when the new renderer is complete.


            return true;
        }

        return false;
    }

    bool Framebuffer::AttachDepthStencilBuffer(Texture2D *buffer, bool immediateRendererUpdate)
    {
        assert(buffer != nullptr);      // <-- Should use DetachDepthStencilBuffer() to remove a buffer. Will probably turn this argument into a reference instead of a pointer.

        if (this->depthStencilAttachment != buffer)
        {
            if (this->depthStencilAttachment != nullptr)
            {
                Renderer::OnDepthStencilBufferDetached(*this, immediateRendererUpdate);     // TODO: Remove this when the new renderer is complete.
                this->depthStencilAttachment->OnDetachFromFramebuffer(this);
            }
                
            this->depthStencilAttachment = buffer;
            this->depthStencilAttachment->OnAttachToFramebuffer(this);

            Renderer::OnDepthStencilBufferAttached(*this, immediateRendererUpdate);     // TODO: Remove this when the new renderer is complete.
        }

        return true;
    }

    void Framebuffer::DetachColourBuffer(size_t index, bool immediateRendererUpdate)
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            auto iColourAttachment = this->colourAttachments.Find(index);
            if (iColourAttachment != nullptr)
            {
                Renderer::OnColourBufferDetached(*this, index, immediateRendererUpdate);

                iColourAttachment->value->OnDetachFromFramebuffer(this);
                this->colourAttachments.RemoveByIndex(iColourAttachment->index);
            }
        }
    }

    void Framebuffer::DetachDepthStencilBuffer(bool immediateRendererUpdate)
    {
        if (this->depthStencilAttachment != nullptr)
        {
            Renderer::OnDepthStencilBufferDetached(*this, immediateRendererUpdate);

            this->depthStencilAttachment->OnDetachFromFramebuffer(this);
            this->depthStencilAttachment = nullptr;
        }
    }

    void Framebuffer::DetachBuffer(Texture2D* buffer)
    {
        // Depth/Stencil.
        if (this->depthStencilAttachment == buffer)
        {
            this->DetachDepthStencilBuffer();
        }


        // Colours.
        for (size_t i = 0; i < this->colourAttachments.count; )
        {
            if (this->colourAttachments.buffer[i]->value == buffer)
            {
                this->DetachColourBuffer(this->colourAttachments.buffer[i]->key);
            }
            else
            {
                ++i;
            }
        }
    }

    void Framebuffer::DetachAllColourBuffers()
    {
        while (this->colourAttachments.count > 0)
        {
            this->DetachColourBuffer(this->colourAttachments.buffer[0]->key);
        }
    }


    void Framebuffer::DetachAllBuffers()
    {
        this->DetachAllColourBuffers();
        this->DetachDepthStencilBuffer();
    }


    Texture2D * Framebuffer::GetColourBuffer(size_t index)
    {
        auto iColourAttachment = this->colourAttachments.Find(index);
        if (iColourAttachment != nullptr)
        {
            return iColourAttachment->value;
        }

        return nullptr;
    }

    const Texture2D * Framebuffer::GetColourBuffer(size_t index) const
    {
        auto iColourAttachment = this->colourAttachments.Find(index);
        if (iColourAttachment != nullptr)
        {
            return iColourAttachment->value;
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
