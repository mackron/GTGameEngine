// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ImageAsset_STB
#define GT_ImageAsset_STB

#include <GTGameEngine/Assets/ImageAsset.hpp>

namespace GT
{
    /// ImageAsset implementation that uses stb_image for loading.
    class ImageAsset_STB : public ImageAsset
    {
    public:

        /// Constructor.
        ImageAsset_STB(AssetType type);

        /// Destructor.
        ~ImageAsset_STB();


        /// @copydoc GT::Asset::Load()
        bool Load(const char* absolutePath, GT::FileSystem &fileSystem);

        /// @copydoc GT::ImageAsset::GetImageWidth()
        unsigned int GetImageWidth() const;

        /// @copydoc GT::ImageAsset::GetImageHeight()
        unsigned int GetImageHeight() const;

        /// @copydoc GT::ImageAsset::GetImageFormat()
        GT::TextureFormat GetImageFormat() const;

        /// @copydoc GT::ImageAsset::GetImageData()
        void* GetImageData() const;


    private:

        /// The width of the image.
        unsigned int m_width;

        /// The height of the image.
        unsigned int m_height;

        /// The format of the image data.
        GT::TextureFormat m_format;

        /// A pointer to the buffer containing the image data.
        void* m_data;
    };
}

#endif // !__GT_ImageAsset_STB_hpp_
