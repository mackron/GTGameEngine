// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Texture2D_hpp_
#define __GTEngine_Texture2D_hpp_

#include <GTImage/Image.hpp>
#include <GTCore/Map.hpp>

namespace GTEngine
{
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


    class Framebuffer;
    class Shader;


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
    class Texture2D : private GTImage::Image
    {
    public:

        /// Default constructor.
        Texture2D();

        /// Constructor.
        Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void* data = nullptr);

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


        /// Retrieves the width of the texture.
        unsigned int GetWidth() const;

        /// Retrieves the height of the texture.
        unsigned int GetHeight() const;

        /// Retrieves the format of the image data.
        GTImage::ImageFormat GetFormat() const;



        /// Retrieves the number of mipmaps making up this texture.
        size_t GetMipmapCount() const;

        /// Retrieves the mipmap at the given index.
        ///
        /// @param index [in] The index of the mipmap to retrieve.
        const GTImage::Mipmap & GetMipmap(size_t index) const;

        

        /// Determines whether or not the texture is attached to a framebuffer.
        bool IsAttachedToFramebuffer() const { return this->framebuffers.root != nullptr; }


        /// Deletes the image data, but maintains the other properties.
        void DeleteLocalData();



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


        /// The list of framebuffers that this texture is attached to.
        GTCore::List<Framebuffer*> framebuffers;

        /// The list of shaders that this texture is attached to.
        GTCore::List<Shader*> shaders;


        /// We store a reference count which will be used by the Texture2DLibrary. Initializes to 1.
        mutable int refCount;




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
