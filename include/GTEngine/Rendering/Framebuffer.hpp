// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Framebuffer_hpp_
#define __GTEngine_Framebuffer_hpp_

#include <GTCore/Map.hpp>
#include <cstddef>

namespace GTEngine
{
    class Texture2D;

    /**
    *   \brief  Class representing a framebuffer.
    *
    *   Framebuffers are made up of three sub-buffers: Colour, Depth and Stencil. The depth and stencil buffers can
    *   be combined. To do this, the depth buffer must be 24-bit and the stencil buffer must be 8-bit (totalling 32
    *   bits). Also, combineDepthStencil must be set to true in the constructor.
    *
    *   Framebuffers use textures internally. This is how render-to-texture is performed in the GTEngine.
    *
    *   Once a framebuffer is constructed, it is not possible to change any of it's properties. In this case, the
    *   framebuffer will need to be re-created.
    */
    class Framebuffer
    {
    public:

        /// Constructor.
        Framebuffer();

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
        const GTCore::Map<unsigned int, Texture2D*> & GetAttachedColourBuffers() const { return this->colourAttachments; }



    private:

        /// The current colour attachments. This is keyed by their index.
        GTCore::Map<unsigned int, Texture2D*> colourAttachments;

        /// There can only be one depth/stencil texture.
        Texture2D* depthStencilAttachment;


    private:    // No copying.
        Framebuffer(const Framebuffer &);
        Framebuffer & operator=(const Framebuffer &);
    };
}

#endif
