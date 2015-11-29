// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ImageLoader_PNG_hpp_
#define __GTLib_ImageLoader_PNG_hpp_
#ifndef GTLIB_NO_PNG

#include <GTEngine/Core/ImageLoader.hpp>
#include <GTEngine/external/stb_image.h>

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