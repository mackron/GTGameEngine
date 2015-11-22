// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_ImageLoader_PNG_hpp_
#define __GTLib_ImageLoader_PNG_hpp_
#ifndef GTLIB_NO_PNG

#include <GTLib/ImageLoader.hpp>
#include <GTLib/IO.hpp>
#include <GTEngine/external/stb_image.h>

namespace GTLib
{
    class ImageLoader_PNG : public ImageLoader
    {
    public:

        /// Constructor.
        ImageLoader_PNG(const char *filename);
        
        /// Destructor.
        ~ImageLoader_PNG();

        /// GTLib::ImageLoader::Open().
        bool Open();

        /// GTLib::ImageLoader::GetImageInfo().
        void GetImageInfo(ImageFileInfo &info);

        /// GTLib::ImageLoader::LoadMipmap().
        bool LoadMipmap(unsigned int mipmapIndex, Mipmap &dest);

        /// GTLib::ImageLoader::HasFileChanged().
        bool HasFileChanged() const;

    private:

#if 0
        /**
        *   \brief  Performs all of the initialisation required to start reading the PNG file.
        *
        *   \remarks
        *       Use UninitPNG() when you are finished doing anything with the PNG.
        */
        FILE * InitPNG();

        /**
        *   \brief  Uninitailises the PNG structures from InitPNG().
        */
        void UninitPNG(FILE *file);
#endif


    private:

        // The libpng structures we need for reading.
        //png_structp png;
        //png_infop   pngInfo;
        //png_infop   pngEndInfo;

        


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