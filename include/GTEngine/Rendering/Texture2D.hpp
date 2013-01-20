
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

    enum Texture2DTarget
    {
        Texture2DTarget_Default,                ///< Normal 2D texture.

        Texture2DTarget_Cube_PositiveX,         ///< Positive X cube map face.
        Texture2DTarget_Cube_NegativeX,         ///< Negative X cube map face.
        Texture2DTarget_Cube_PositiveY,         ///< Positive Y cube map face.
        Texture2DTarget_Cube_NegativeY,         ///< Negative Y cube map face.
        Texture2DTarget_Cube_PositiveZ,         ///< Positive Z cube map face.
        Texture2DTarget_Cube_NegativeZ,         ///< Negative Z cube map face.
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

        /// Constructor.
        Texture2D(Texture2DTarget target = Texture2DTarget_Default);
        Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void* data = nullptr);
        Texture2D(const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~Texture2D();


        /// Retrieves the relative path of the texture file.
        ///
        /// @return The relative path of the texture, as specified in the constructor.
        ///
        /// @remarks
        ///     This will return an empty string if the image was not loaded from a file.
        const char* GetRelativePath() const;



        /// Sets the base data of the texture.
        ///
        /// @param width  [in] The new width of the image.
        /// @param height [in] The new height of the image.
        /// @param format [in] The new format of the image data.
        /// @param data   [in] The image data.
        ///
        /// @remarks
        ///     If 'data' is null, the storage will be allocated, but will remain undefined. Very useful for framebuffer targets.
        void SetData(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void* data = nullptr);

        /// Resizes the texture.
        ///
        /// @param newWidth  [in] The new width of the texture.
        /// @param newHeight [in] The new height of the texture.
        ///
        /// @remarks
        ///     Any existing data will be deleted and made undefined. This should only really be used for things like framebuffer attachments. Consider SetData(), also.
        void Resize(unsigned int width, unsigned int height);


        /// Retrieves the target.
        ///
        /// @remarks
        ///     The texture target basically defines the usage of the texture.
        Texture2DTarget GetTarget() const { return this->target; }


        /// Sets the minification filter.
        ///
        /// @param newFilter [in] The new minification filter.
        void SetMinificationFilter(TextureFilter newFilter);

        /// Sets the magnification filter.
        ///
        /// @param newFilter [in] The new magnification filter.
        void SetMagnificationFilter(TextureFilter newFilter);

        /// Retrieves the minification filter.
        TextureFilter GetMinificationFilter() const;

        /// Retrieves the magnification filter.
        TextureFilter GetMagnificationFilter() const;


        /// Helper method for setting both the minification and magnification filter.
        ///
        /// @param newMinFilter [in] The new minification filter.
        /// @param newMagFilter [in] The new magnification filter.
        void SetFilter(TextureFilter newMinFilter, TextureFilter newMagFilter)
        {
            this->SetMinificationFilter(newMinFilter);
            this->SetMagnificationFilter(newMagFilter);
        }



        /// Sets the level of anisotropic filtering to use with this texture.
        /// @param newAnisotropy [in] The new level of anisotropic filtering to use. This is clamped against the renderers maxiumum.
        void SetAnisotropy(unsigned int newAnisotropy);

        /// Retrieves the level of anisotropic filtering being used with this texture.
        unsigned int GetAnisotropy() const;


        /// Sets the wrapping mode (repeat, clamp, etc).
        void SetWrapMode(TextureWrapMode wrapMode);

        /// Retrieves the wrapping mode.
        TextureWrapMode GetWrapMode() const { return this->wrapMode; }


        /// Retrieves a pointer to the internal renderer data.
        ///
        /// @remarks
        ///     This should only be used by the renderer.
              void* GetRendererData()       { return this->rendererData; }
        const void* GetRendererData() const { return this->rendererData; }

        /// Sets the pointer to the internal renderer data.
        ///
        /// @remarks
        ///     This does not deallocate the previous renderer data. That is the responsibility of the renderer itself.
        void SetRendererData(void *rendererData) { this->rendererData = rendererData; }


        /// Sets whether or not client-side image data should be kept after syncing with the renderer.
        void KeepClientSideData(bool keepClientSideData) { this->keepClientSideData = keepClientSideData; }

        /// Determines whether or not client-side data should be kept after syncing with the renderer.
        bool KeepClientSideData() const { return this->keepClientSideData; }


        /// Determines whether or not the texture is attached to a framebuffer.
        bool IsAttachedToFramebuffer() const { return this->framebuffers.root != nullptr; }




    private:    // GTImage Image events.

        void OnMipmapCreated(unsigned int mipmapIndex);
        void OnMipmapDeleted(unsigned int mipmapIndex);
        void OnMipmapChanged(unsigned int mipmapIndex);


    private:    // Called internally by GTEngine

        /// Called when the texture is attached to a framebuffer.
        void OnAttachToFramebuffer(Framebuffer* framebuffer);

        /// Called when the texture is detached from a framebuffer.
        void OnDetachFromFramebuffer(Framebuffer* framebuffer);


        /// Called when the texture is attached to a shader.
        void OnAttachToShader(Shader &shader);

        /// Called when the texture is detached from a shader.
        void OnDetachFromShader(Shader &shader);


    private:

        /// The relative path of the texture.
        GTCore::String relativePath;

        /// The target. Basically, this just specifies the usage of the texture 2D, such as whether or not it's being used in a cube map or whatnot.
        Texture2DTarget target;


        /// The current minification filter.
        TextureFilter minFilter;

        /// The current magnification filter.
        TextureFilter magFilter;

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
        void* rendererData;

        /// We store a reference count which will be used by the Texture2DLibrary. Initializes to 1.
        mutable int refCount;

        /// Keeps track of whether or not the client-side texture data should be kept after it has been synced with the renderer. Defaults to false.
        bool keepClientSideData;



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
