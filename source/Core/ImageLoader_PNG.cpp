// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.


#ifndef GTLIB_NO_PNG

#include <GTLib/stdlib.hpp>
#include "ImageLoader_PNG.hpp"

#ifdef _MSC_VER
#pragma warning (disable:4611)      // interaction between '_setjmp' and C++ object destruction is non-portable
#endif

// Helper functions.
namespace GTLib
{
    /**
    *   \brief  Retrieves the image format from the given PNG channel count.
    */
    ImageFormat GetImageFormatFromPNGChannels(png_byte channels)
    {
        switch (channels)
        {
        case 1:  return ImageFormat_R8;
        case 3:  return ImageFormat_RGB8;
        case 4:  return ImageFormat_RGBA8;
        default: break;
        }

        return ImageFormat_Auto;
    }
}

namespace GTLib
{
    ImageLoader_PNG::ImageLoader_PNG(const char *filename)
        : ImageLoader(filename), loadUpsideDown(false), png(nullptr), pngInfo(nullptr), pngEndInfo(nullptr),
          info()
    {
        info.mipmapCount = 1;   // PNG's will always have 1 mipmap.
    }

    ImageLoader_PNG::~ImageLoader_PNG()
    {
    }

    bool ImageLoader_PNG::Open()
    {
        // First we need a file...
        FILE *file = this->InitPNG();
        if (file != nullptr)
        {
            // With the libpng stuff initialised, we can now fill our 'info' structure.
            this->info = ImageFileInfo(this->absolutePath.c_str());

            this->info.format      = GetImageFormatFromPNGChannels(png_get_channels(this->png, this->pngInfo));
            this->info.width       = png_get_image_width(this->png, this->pngInfo);
            this->info.height      = png_get_image_height(this->png, this->pngInfo);
            this->info.mipmapCount = 1;

            this->UninitPNG(file);
            return true;
        }

        return false;
    }

    void ImageLoader_PNG::GetImageInfo(ImageFileInfo &info)
    {
        info = this->info;
    }

    bool ImageLoader_PNG::LoadMipmap(unsigned int mipmapIndex, Mipmap &destMipmap)
    {
        // PNG's don't use the notion of mipmaps, so we only support a mipmap index of 0.
        if (mipmapIndex == 0)
        {
            FILE *file = this->InitPNG();
            if (file != nullptr)
            {
                // We'll first need to grab the size of a row. From there we can calculate the sizes of everything.
                size_t pitch = png_get_rowbytes(this->png, this->pngInfo);

                // We also need the width and height.
                png_uint_32 width  = png_get_image_width(this->png, this->pngInfo);
                png_uint_32 height = png_get_image_height(this->png, this->pngInfo);

                // We'll be reading directly into the destination mipmap's buffer. Therefore, we need to clean
                // any existing data.
                destMipmap.DeleteLocalData();
                destMipmap.data = malloc(pitch * height);

                // Now we can start reading. The destination will depend on this->loadUpsideDown.
                for (unsigned int i = 0; i < height; ++i)
                {
                    png_bytep dest = nullptr;
                    if (!this->loadUpsideDown)
                    {
                        dest = ((png_bytep)destMipmap.data) + (i * pitch);
                    }
                    else
                    {
                        dest = ((png_bytep)destMipmap.data) + ((height - i - 1) * pitch);
                    }

                    png_read_row(this->png, dest, nullptr);
                }

                // At this point everything will be loaded and we can set the Mipmap's new attributes.
                destMipmap.format = GetImageFormatFromPNGChannels(png_get_channels(this->png, this->pngInfo));
                destMipmap.width  = width;
                destMipmap.height = height;

                this->UninitPNG(file);
                return true;
            }
        }

        return false;
    }

    void ImageLoader_PNG::SetLoadUpsideDown(bool loadUpsideDown)
    {
        this->loadUpsideDown = loadUpsideDown;
    }

    bool ImageLoader_PNG::HasFileChanged() const
    {
        // We just need to check the last modified data. If it's different, the file has changed.
        GTLib::FileInfo tempInfo(this->absolutePath.c_str());
        if (tempInfo.lastModifiedTime != this->info.lastModifiedTime)
        {
            return true;
        }

        return false;
    }


    FILE * ImageLoader_PNG::InitPNG()
    {
        volatile auto file = GTLib::IO::Open(this->absolutePath.c_str(), GTLib::IO::OpenMode::Read);
        if (file != nullptr)
        {
            // First we want to make sure we have a PNG. We do this by reading the first 8 bytes and then let libpng check those bytes for us.
            png_byte header[8];
            GTLib::IO::Read((FILE*)file, header, 8);

            if (png_sig_cmp(header, 0, 8) == 0)
            {
                // Now we need to create our structures.
                this->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
                if (png != nullptr)
                {
                    this->pngInfo = png_create_info_struct(this->png);
                    if (this->pngInfo != nullptr)
                    {
                        this->pngEndInfo = png_create_info_struct(this->png);
                        if (pngEndInfo != nullptr)
                        {
                            // If we have made it here, everything has initialised successfully. What we need to do now is
                            // read some up to the point where we can start reading actual image data. Here are the steps:
                            //    1) Have libpng use 'file' as the source of the PNG.
                            //    2) Let libpng know that we've already read the header (the first 8 bytes).
                            //    3) Read the PNG info. This must be done before starting to read the image data.
                            png_init_io(this->png, (FILE *)file);               // 1
                            png_set_sig_bytes(this->png, 8);                    // 2
                            png_read_info(this->png, this->pngInfo);            // 3

                            return (FILE *)file;
                        }
                        else
                        {
                            png_destroy_read_struct(&this->png, &this->pngInfo, nullptr);
                        }
                    }
                    else
                    {
                        png_destroy_read_struct(&this->png, nullptr, nullptr);
                    }
                }
            }

            // Something went wrong, so we need to close the file and fall through to a nullptr return.
            GTLib::IO::Close((FILE *)file);
        }

        // Something went wrong.
        return nullptr;
    }

    void ImageLoader_PNG::UninitPNG(FILE *file)
    {
        // Any previous libpng structures need to be destroyed.
        png_destroy_read_struct(&this->png, &this->pngInfo, &this->pngEndInfo);

        // The file also needs to be closed.
        GTLib::IO::Close(file);
    }
}

#endif
