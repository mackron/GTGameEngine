// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Framebuffer
#define GT_Framebuffer

#include <GTGE/Core/Map.hpp>
#include <cstddef>

namespace GT
{
    class Texture2D;

    /// Class representing a framebuffer.
    class Framebuffer
    {
    protected:

        /// Constructor.
        Framebuffer();


    public:

        /// Destructor.
        ///
        /// @remarks
        ///     The destructor will detach any buffers, but will not delete them. It is up to the application to manage the
        ///     deletion of attachments.
        virtual ~Framebuffer();

        /// Attaches a colour buffer to the framebuffer.
        ///
        /// @param buffer                  [in] The colour buffer to attach to the framebuffer. The buffer is always a texture.
        /// @param immediateRendererUpdate [in] Whether or not the renderer-side state should be updated immediately.
        ///
        /// @return True if the colour buffer was attached successfully; false otherwise.
        ///
        /// @remarks
        ///     This function can still return true if the hardware doesn't support the current structure of the framebuffer. This can
        ///     only be detected after the framebuffer has been made current on the renderer. Do not rely on the return value in
        ///     determining whether or not the framebuffer is valid.
        bool AttachColourBuffer(Texture2D* buffer, unsigned int index);

        /// Attaches a depth/stencil buffer to the framebuffer.
        ///
        /// @param buffer [in]                  The depth/stencil buffer to attach to the framebuffer. The buffer is always a texture.
        /// @param immediateRendererUpdate [in] Whether or not the renderer-side state should be updated immediately.
        ///
        /// @return Always returns true. We return a boolean to keep it consistent with AttachColourBuffer(), which requires a boolean return value.
        ///
        /// @remarks
        ///     If a write-only depth/stencil buffer is already attached, it will be detached.
        bool AttachDepthStencilBuffer(Texture2D* buffer);

        /// Detaches the colour buffer at the given index.
        ///
        /// @param index [in]                   The index of the colour buffer to detach.
        /// @param immediateRendererUpdate [in] Whether or not the renderer-side state should be updated immediately.
        void DetachColourBuffer(size_t index);

        /// Detaches the depth/stencil attachment, if it has one.
        ///
        /// @param immediateRendererUpdate [in] Whether or not the renderer-side state should be updated immediately.
        void DetachDepthStencilBuffer();

        

        /// Generically detaches a buffer based on it's object pointer.
        ///
        /// @remarks
        ///     This is slower than the non-generic versions.
        void DetachBuffer(Texture2D *buffer);

        /// Detaches all colour buffers.
        void DetachAllColourBuffers();


        /// Detaches every buffer.
        void DetachAllBuffers();


        /// Attaches a write-only depth/stencil buffer.
        ///
        /// @param width  [in] The initial width of the depth/stencil buffer.
        /// @param height [in] The initial height of the depth/stencil buffer.
        ///
        /// @remarks
        ///     If a depth/stencil texture has already been attached, it will be detached.
        ///     @par
        ///     The buffer can be resized with ResizeWriteOnlyDepthStencilBuffer().
        ///     @par
        ///     Detach the buffer with DetachWriteOnlyDepthStencilBuffer().
        bool AttachWriteOnlyDepthStencilBuffer(unsigned int width, unsigned int height);

        /// Detaches the write-only depth/stencil buffer.
        void DetachWriteOnlyDepthStencilBuffer();

        /// Determines whether or not a write-only depth/stencil buffer is attached.
        bool IsWriteOnlyDepthStencilBufferAttached() const { return this->usingWriteOnlyDepthStencilBuffer; }

        /// Resizes the write-only depth/stencil buffer.
        ///
        /// @param width  [in] the new width of the depth/stencil buffer.
        /// @param height [in] The new height of the depth/stencil buffer.
        void ResizeWriteOnlyDepthStencilBuffer(unsigned int width, unsigned int height);

        /// Retrieves the width of the write-only depth/stencil buffer.
        unsigned int GetWriteOnlyDepthStencilBufferWidth() const { return this->writeOnlyDepthStencilBufferWidth; }

        /// Retrieves the height of the write-only depth/stencil buffer.
        unsigned int GetWriteOnlyDepthStencilBufferHeight() const { return this->writeOnlyDepthStencilBufferHeight; }


        /// Retrieves a colour buffer attachment.
        ///
        /// @param index [in] The index of the colour buffer to retrieve.
              Texture2D* GetColourBuffer(size_t index);
        const Texture2D* GetColourBuffer(size_t index) const;

        /// Retrieves the depth/stencil attachment.
              Texture2D* GetDepthStencilBuffer();
        const Texture2D* GetDepthStencilBuffer() const;


        /// Checks whether or not the framebuffer is valid.
        ///
        /// @remarks
        ///     This is run on the GPU side, so the actual check will only occur at the start of the next render-command queue swap.
        ///     @par
        ///     An error will be logged if the framebuffer is invalid. This method is designed for debugging, mostly.
        void CheckStatus();


        /// Retrieves a direct reference to the internal map of colour attachments.
        ///
        /// @remarks
        ///     This is mainly for use by renderers.
        const Map<unsigned int, Texture2D*> & GetAttachedColourBuffers() const { return this->colourAttachments; }



    private:

        /// The current colour attachments. This is keyed by their index.
        Map<unsigned int, Texture2D*> colourAttachments;

        /// There can only be one depth/stencil texture.
        Texture2D* depthStencilAttachment;

        
        /// Whether or not we are using a write-only depth/stencil buffer.
        bool usingWriteOnlyDepthStencilBuffer;

        /// The width of the write-only depth/stencil buffer.
        unsigned int writeOnlyDepthStencilBufferWidth;

        /// The height of the write-only depth/stencil buffer.
        unsigned int writeOnlyDepthStencilBufferHeight;


    private:    // No copying.
        Framebuffer(const Framebuffer &);
        Framebuffer & operator=(const Framebuffer &);
    };
}

#endif
