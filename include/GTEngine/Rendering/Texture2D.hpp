// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Texture2D_hpp_
#define __GTEngine_Texture2D_hpp_

#include "TextureFilters.hpp"
#include "TextureWrapModes.hpp"
#include <GTLib/Image.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/List.hpp>

namespace GTEngine
{
    class Framebuffer;
    class Shader;

    /// Class representing a 2D texture.
    ///
    /// Textures should not be constructed directly, but should instead by created with Renderer::CreateTexture2D(). Textures should be deleted
    /// with Renderer::DeleteTexture2D().
    ///
    /// When the data of a texture is changed, it needs to be pushed to the GPU with Renderer::PushTexture2DData() before those changes will
    /// appear during rendering.
    class Texture2D : private GTLib::Image
    {
    protected:

        /// Default constructor.
        Texture2D();

        /// Constructor.
        Texture2D(unsigned int width, unsigned int height, GTLib::ImageFormat format, const void* data = nullptr);



    public:

        /// Destructor.
        virtual ~Texture2D();


        /// Retrieves the relative path of the texture file.
        ///
        /// @return The relative path of the texture, as specified in the constructor.
        ///
        /// @remarks
        ///     This will return an empty string if the image was not loaded from a file.
        const char* GetRelativePath() const;

        /// Sets the relative path of the texture file, if any.
        ///
        /// @param newRelativePath [in] The new relative path.
        ///
        /// @remarks
        ///     This doesn't do any loading. It is a simple setting and should only be used by the texture library.
        void SetRelativePath(const char* newRelativePath);



        /// Sets the base data of the texture.
        ///
        /// @param width  [in] The new width of the image.
        /// @param height [in] The new height of the image.
        /// @param format [in] The new format of the image data.
        /// @param data   [in] The image data.
        ///
        /// @remarks
        ///     If 'data' is null, the storage will be allocated, but will remain undefined. Very useful for framebuffer targets.
        void SetData(unsigned int width, unsigned int height, GTLib::ImageFormat format, const void* data = nullptr);
        
        /// Sets a sub-region of data.
        ///
        /// @param xPos   [in] The x offset of the rectangle region to update.
        /// @param yPos   [in] The y offset of the rectangle region to update.
        /// @param width  [in] The width of the rectangle region to update.
        /// @param height [in] The height of the rectangle region to update.
        /// @param data   [in] A pointer to the new data to use in the rectangle region.
        ///
        /// @remarks
        ///     'data' can not be null.
        void SetSubData(unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, const void* data = nullptr);

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
        GTLib::ImageFormat GetFormat() const;



        /// Retrieves the number of mipmaps making up this texture.
        size_t GetMipmapCount() const;

        /// Retrieves the mipmap at the given index.
        ///
        /// @param index [in] The index of the mipmap to retrieve.
        const GTLib::Mipmap & GetMipmap(size_t index) const;

        

        /// Determines whether or not the texture is attached to a framebuffer.
        bool IsAttachedToFramebuffer() const { return this->framebuffers.root != nullptr; }


        /// Deletes the image data, but maintains the other properties.
        void DeleteLocalData();



        /////////////////////////////////////////////////////////////////////////
        // The methods below should not be called directly. Used internally.

        /// Called when the texture is attached to a framebuffer.
        ///
        /// @remarks
        ///     Do not call this directly. Used internally.
        void OnAttachToFramebuffer(Framebuffer* framebuffer);

        /// Called when the texture is detached from a framebuffer.
        ///
        /// @remarks
        ///     Do not call this directly. Used internally.
        void OnDetachFromFramebuffer(Framebuffer* framebuffer);


        /// Increments the internal reference counter.
        int IncrementReferenceCounter();

        /// Decrements the internal reference counter.
        int DecrementReferenceCounter();
        


    private:

        /// The relative path of the texture.
        GTLib::String relativePath;


        /// The list of framebuffers that this texture is attached to.
        GTLib::List<Framebuffer*> framebuffers;

        /// The list of shaders that this texture is attached to.
        GTLib::List<Shader*> shaders;


        /// We store a reference count which will be used by the Texture2DLibrary. Initializes to 1.
        int refCount;



    private:    // No copying.
        Texture2D(const Texture2D &);
        Texture2D & operator=(const Texture2D &);
    };
}

#endif
