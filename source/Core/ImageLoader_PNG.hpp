// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_ImageLoader_PNG
#define GT_ImageLoader_PNG
#ifndef GTLIB_NO_PNG

#include <GTGE/Core/ImageLoader.hpp>
#include <GTGE/external/stb_image.h>

namespace GT
{
    class ImageLoader_PNG : public ImageLoader
    {
    public:

        /// Constructor.
        ImageLoader_PNG(const char *filename);
        
        /// Destructor.
        ~ImageLoader_PNG();

        /// ImageLoader::Open().
        bool Open();

        /// ImageLoader::GetImageInfo().
        void GetImageInfo(ImageFileInfo &info);

        /// ImageLoader::LoadMipmap().
        bool LoadMipmap(unsigned int mipmapIndex, Mipmap &dest);

        /// ImageLoader::HasFileChanged().
        bool HasFileChanged() const;


    private:

        // The channel count.
        int m_channelCount;

        // A pointer to the image data. This is loaded with stb_image.
        void* m_pImageData;

        // Information about the image.
        ImageFileInfo m_info;


    private:    // No copying.
        ImageLoader_PNG(const ImageLoader_PNG &);
        ImageLoader_PNG & operator=(const ImageLoader_PNG &);
    };
}

#endif
#endif