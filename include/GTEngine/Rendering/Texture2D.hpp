
#ifndef __GTEngine_Texture2D_hpp_
#define __GTEngine_Texture2D_hpp_

#include <GTImage/Image.hpp>
#include <GTCore/Map.hpp>

namespace GTEngine
{
    class Framebuffer;
    class Shader;

    enum TextureFilter
    {
        TextureFilter_Nearest = 1,
        TextureFilter_Linear,

        TextureFilter_NearestNearest,
        TextureFilter_NearestLinear,
        TextureFilter_LinearNearest,
        TextureFilter_LinearLinear,

        TextureFilter_Point = TextureFilter_Nearest
    };

    enum TextureWrapMode
    {
        TextureWrapMode_Repeat,
        TextureWrapMode_Clamp,
        TextureWrapMode_ClampToEdge,
    };

    /**
    *   \brief  Class representing a 2D texture.
    *
    *   When a Texture2D object is first created, it will contain no data. You must manually load data into
    *   the texture.
    *
    *   Although a texture is tightly bound to the renderer, you do not need to worry about updating the texture
    *   data on the rendering thread. The data will be synced as the renderer uses the texture.
    *
    *   Once the texture has been constructed, it can not be resized. You must delete and then re-create a new
    *   texture.
    *
    *   A texture is constructed of mipmaps. When setting data, you actually set the data of each mipmap. The
    *   GenerateMipmaps() method can be used to generate mipmaps from the base mipmap. 
    */
    class Texture2D : public GTImage::Image
    {
    public:

        /**
        *   \brief  Constructor.
        */
        Texture2D();
        Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void *data = nullptr);
        Texture2D(const char *filename);

        /**
        *   \brief  Destructor.
        */
        ~Texture2D();

        /**
        *   \brief  Sets the filter.
        */
        void SetFilter(TextureFilter filter);

        /**
        *   \brief  Retrieves the filter.
        */
        TextureFilter GetFilter() const;


        /// Sets the level of anisotropic filtering to use with this texture.
        /// @param newAnisotropy [in] The new level of anisotropic filtering to use. This is clamped against the renderers maxiumum.
        void SetAnisotropy(unsigned int newAnisotropy);

        /// Retrieves the level of anisotropic filtering being used with this texture.
        unsigned int GetAnisotropy() const;


        /**
        *   \brief  Sets the wrapping mode (repeat, clamp, etc).
        */
        void SetWrapMode(TextureWrapMode wrapMode);

        /**
        *   \brief  Retrieves the wrapping mode.
        */
        TextureWrapMode GetWrapMode() const { return this->wrapMode; }


        /**
        *   \brief  Retrieves a pointer to the internal renderer data.
        *
        *   \remarks
        *       This should only be used by the renderer.
        */
        const void * GetRendererData() const { return this->rendererData; }
              void * GetRendererData()       { return this->rendererData; }

        /**
        *   \brief  Sets the pointer to the internal renderer data.
        *
        *   \remarks
        *       This does not deallocate the previous renderer data. That is the responsibility of the renderer itself.
        */
        void SetRendererData(void *rendererData) { this->rendererData = rendererData; }


        /// Sets whether or not client-side image data should be kept after syncing with the renderer.
        void KeepClientSideData(bool keepClientSideData) { this->keepClientSideData = keepClientSideData; }

        /// Determines whether or not client-side data should be kept after syncing with the renderer.
        bool KeepClientSideData() const { return this->keepClientSideData; }



    private:    // GTImage Image events.

        void OnMipmapCreated(unsigned int mipmapIndex);
        void OnMipmapDeleted(unsigned int mipmapIndex);
        void OnMipmapChanged(unsigned int mipmapIndex);


    private:    // Called internally by GTEngine

        /**
        *   \brief  Called when the texture is attached to a framebuffer.
        */
        void OnAttachToFramebuffer(Framebuffer* framebuffer);

        /**
        *   \brief  Called when the texture is detached from a framebuffer.
        */
        void OnDetachFromFramebuffer(Framebuffer* framebuffer);


        /// Called when the texture is attached to a shader.
        void OnAttachToShader(Shader* shader);

        /// Called when the texture is detached from a shader.
        void OnDetachFromShader(Shader* shader);


    private:

        /// The filtering mode.
        TextureFilter filter;

        /// The level of aniostropic filtering to use with the image. Defaults to 1.
        unsigned int anisotropy;

        /// The wrapping mode.
        TextureWrapMode wrapMode;

        /// The list of framebuffers that this texture is attached to.
        GTCore::List<Framebuffer*> framebuffers;

        /// The list of shaders that this texture is attached to.
        GTCore::List<Shader*> shaders;


        /// The renderer will need to store it's own properties about the texture. This pointer can be used by the renderer
        /// to hold a pointer to some renderer-specific data.
        void *rendererData;

        /// We store a reference count which will be used by the Texture2DLibrary. Initializes to 1.
        mutable int refCount;

        /// Keeps track of whether or not the client-side texture data should be kept after it has been synced with the renderer. Defaults to false.
        bool keepClientSideData;


    public:

        // When the renderer uses the texture, it needs to ensure it's own texture data is synchronized with the Texture2D
        // object. To help with this, we use a 'sync' object for storing synchronization information. This will be modified
        // by both Texture2D and the renderer. The variables in this structure are just booleans indicating which attribute
        // have been modified. A renderer will always set the variables to false. Texture2D will always set the variables to
        // true.
        struct _syncinfo
        {
            _syncinfo()
                : filterChanged(true), wrapModeChanged(true), dataChanged(true), changedMipmaps()
            {
            }

            bool filterChanged;     ///< Whether or not a texture filter has changed.
            bool wrapModeChanged;   ///< Whether or not the wrapping mode has changed.
            bool dataChanged;       ///< Whether or not the texture data has been updated.

            GTCore::Vector<unsigned int> changedMipmaps;

        private:    // No copying.
            _syncinfo(const _syncinfo &);
            _syncinfo & operator=(const _syncinfo &);

        }syncinfo;


    private:    // No copying.
        Texture2D(const Texture2D &);
        Texture2D & operator=(const Texture2D &);



    friend class Renderer;
    friend class Framebuffer;
    friend class Shader;
    friend class Texture2DLibrary;
    };
}

#endif