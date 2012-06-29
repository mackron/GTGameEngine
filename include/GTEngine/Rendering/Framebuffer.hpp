
#ifndef __GTEngine_Framebuffer_hpp_
#define __GTEngine_Framebuffer_hpp_

#include <cstdint>

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

        /**
        *   \brief Constructor.
        */
        Framebuffer();

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       The destructor will detach any buffers, but will not delete them. It is up to the application to manage the
        *       deletion of attachments.
        */
        virtual ~Framebuffer();

        /**
        *   \brief              Attaches a colour buffer to the framebuffer.
        *   \param  buffer [in] The colour buffer to attach to the framebuffer. The buffer is always a texture.
        *   \param  index  [in] The index to attach the colour buffer to. Must be in the range [0, Renderer::GetMaxColourAttachments)
        *   \return             True if the colour buffer was attached successfully; false otherwise.
        *
        *   \remarks
        *       This function can still return true if the hardware doesn't support the current structure of the framebuffer. This can
        *       only be detected after the framebuffer has been made current on the renderer. Do not rely on the return value in
        *       determining whether or not the framebuffer is valid.
        */
        bool AttachColourBuffer(Texture2D *buffer, size_t index);

        /**
        *   \brief              Attaches a depth/stencil buffer to the framebuffer.
        *   \param  buffer [in] The depth/stencil buffer to attach to the framebuffer. The buffer is always a texture.
        *   \return             Always returns true. We return a boolean to keep it consistent with AttachColourBuffer(), which requires a boolean return value.
        */
        bool AttachDepthStencilBuffer(Texture2D *buffer);

        /**
        *   \brief             Detaches the colour buffer at the given index.
        *   \param  index [in] The index of the colour buffer to detach.
        */
        void DetachColourBuffer(size_t index);

        /**
        *   \brief  Detaches the depth/stencil attachment, if it has one.
        */
        void DetachDepthStencilBuffer();

        /**
        *   \brief  Generically detaches a buffer based on it's object pointer.
        *
        *   \remarks
        *       This is slower than the non-generic versions.
        */
        void DetachBuffer(Texture2D *buffer);


        /**
        *   \brief  Detaches every buffer.
        */
        void DetachAllBuffers();


        /**
        *   \brief             Retrieves a colour buffer attachment.
        *   \param  index [in] The index of the colour buffer to attach.
        */
        Texture2D * GetColourBuffer(size_t index);

        /**
        *   \brief  Retrieves the depth/stencil attachment.
        */
        Texture2D * GetDepthStencilBuffer();


        /**
        *   \brief  Determines whether or not the framebuffer is valid.
        *
        *   \remarks
        *       All framebuffers start out invalid. Only after the framebuffer is used for the first time can you know whether or
        *       not the framebuffer is valid. It is done this way because only the renderer can determine whether or not the framebuffer
        *       is valid.
        */
        bool IsValid() const { return this->isValid; }


    // The methods below should only be called by the renderer and it's support functions.
    public:

        /**
        *   \brief  Sets whether or not the framebuffer is valid. This should only be called by the renderer and it's support functions.
        */
        void IsValid(bool valid) { this->isValid = valid; }

        /**
        *   \brief  Retrieves a pointer to the internal renderer data.
        */
        const void * GetRendererData() const { return this->rendererData; }
              void * GetRendererData()       { return this->rendererData; }

        /**
        *   \brief  Sets the pointer to the internal renderer data. This should only be called by the renderer and it's support functions.
        *
        *   \remarks
        *       This does not delete the previous renderer data. That is the responsibility of the caller.
        */
        void SetRendererData(void *rendererData) { this->rendererData = rendererData; }


    private:

        /// There can be multiple colour textures. We use an array here that will be allocated the number of colour attachments
        /// supported by the renderer.
        Texture2D** colourAttachments;

        /// There can only be one depth/stencil texture.
        Texture2D* depthStencilAttachment;

        /// Keeps track of the maximum number of colour attachments supported by the renderer.
        size_t maxColourAttachments;

        /// Keeps track of whether or not the framebuffer is valid. All framebuffers start life as invalid. When the framebuffer
        /// is used for the first time, the renderer will set this attribute.
        bool isValid;

        /// A pointer to renderer-specific data.
        void *rendererData;


    public:

        /// This is used to keep a framebuffer in sync between client and server.
        struct _syncinfo
        {
            _syncinfo(size_t colourCount)
                : colourCount(colourCount), colourChanged(nullptr), depthStencilChanged(true)
            {
                this->colourChanged = new bool[colourCount];
                for (size_t i = 0; i < colourCount; ++i)
                {
                    this->colourChanged[i] = true;
                }
            }

            ~_syncinfo()
            {
                delete [] this->colourChanged;
            }

            size_t colourCount;

            bool* colourChanged;
            bool  depthStencilChanged;


        private:    // No copying.
            _syncinfo(const _syncinfo &);
            _syncinfo & operator=(const _syncinfo &);

        }syncinfo;
        

    private:    // No copying.
        Framebuffer(const Framebuffer &);
        Framebuffer & operator=(const Framebuffer &);

    friend class Renderer;
    };
}

#endif
