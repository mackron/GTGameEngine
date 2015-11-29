// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Rendering/Framebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GT
{
    Framebuffer::Framebuffer()
        : colourAttachments(), depthStencilAttachment(nullptr),
          usingWriteOnlyDepthStencilBuffer(false), writeOnlyDepthStencilBufferWidth(0), writeOnlyDepthStencilBufferHeight(0)
    {
    }

    Framebuffer::~Framebuffer()
    {
        // First we need to detach everything. We do not want to delete the attachments, just detach.
        this->DetachAllBuffers();
    }

    bool Framebuffer::AttachColourBuffer(Texture2D *buffer, unsigned int index)
    {
        assert(buffer != nullptr);      // <-- Should use DetachColourBuffer() to remove a buffer. Will probably turn this argument into a reference instead of a pointer.

        if (index < Renderer::GetMaxColourAttachments())
        {
            auto iColourAttachment = this->colourAttachments.Find(index);
            if (iColourAttachment != nullptr)
            {
                if (iColourAttachment->value != buffer)
                {
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


            return true;
        }

        return false;
    }

    bool Framebuffer::AttachDepthStencilBuffer(Texture2D *buffer)
    {
        assert(buffer != nullptr);      // <-- Should use DetachDepthStencilBuffer() to remove a buffer. Will probably turn this argument into a reference instead of a pointer.

        // The write-only depth/stencil buffer needs to be detached.
        this->DetachWriteOnlyDepthStencilBuffer();

        if (this->depthStencilAttachment != buffer)
        {
            if (this->depthStencilAttachment != nullptr)
            {
                this->depthStencilAttachment->OnDetachFromFramebuffer(this);
            }
                
            this->depthStencilAttachment = buffer;
            this->depthStencilAttachment->OnAttachToFramebuffer(this);
        }

        return true;
    }

    void Framebuffer::DetachColourBuffer(size_t index)
    {
        if (index < Renderer::GetMaxColourAttachments())
        {
            auto iColourAttachment = this->colourAttachments.Find(index);
            if (iColourAttachment != nullptr)
            {
                iColourAttachment->value->OnDetachFromFramebuffer(this);
                this->colourAttachments.RemoveByIndex(iColourAttachment->index);
            }
        }
    }

    void Framebuffer::DetachDepthStencilBuffer()
    {
        if (this->depthStencilAttachment != nullptr)
        {
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
        this->DetachWriteOnlyDepthStencilBuffer();
    }


    bool Framebuffer::AttachWriteOnlyDepthStencilBuffer(unsigned int width, unsigned int height)
    {
        // First we want to detach the already-attached depth/stencil buffer if there is one.
        this->DetachDepthStencilBuffer();

        this->usingWriteOnlyDepthStencilBuffer = true;
        this->ResizeWriteOnlyDepthStencilBuffer(width, height);

        return true;
    }

    void Framebuffer::DetachWriteOnlyDepthStencilBuffer()
    {
        this->usingWriteOnlyDepthStencilBuffer  = false;
        this->writeOnlyDepthStencilBufferWidth  = 0;
        this->writeOnlyDepthStencilBufferHeight = 0;
    }

    void Framebuffer::ResizeWriteOnlyDepthStencilBuffer(unsigned int width, unsigned int height)
    {
        this->writeOnlyDepthStencilBufferWidth  = width;
        this->writeOnlyDepthStencilBufferHeight = height;
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
}
