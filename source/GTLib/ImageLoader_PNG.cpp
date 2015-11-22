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
}

namespace GTLib
{
    ImageFormat GetImageFormatFromSTBChannels(int channels)
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


    struct STBICallbackData
    {
        FILE* pFile;
    };

    int STBI_Read(void* user, char *data, int size)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);

            GTLib::IO::Read(callbackData->pFile, data, size);
            if (ferror(callbackData->pFile)) {
                return 0;
            }

            return 1;
        }
    }

    void STBI_Skip(void* user, int n)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);
            GTLib::IO::Seek(callbackData->pFile, n, SeekOrigin::Current);
        }
    }

    int STBI_EOF(void* user)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);
            return GTLib::IO::AtEnd(callbackData->pFile);
        }
    }


    ImageLoader_PNG::ImageLoader_PNG(const char *filename)
        : ImageLoader(filename), m_channelCount(0), m_pImageData(nullptr), m_info()
    {
        m_info.mipmapCount = 1;               // PNG's will always have 1 mipmap.
    }

    ImageLoader_PNG::~ImageLoader_PNG()
    {
        stbi_image_free(m_pImageData);
    }

    bool ImageLoader_PNG::Open()
    {
        FILE* pFile = GTLib::IO::Open(this->absolutePath.c_str(), GTLib::IO::OpenMode::Read);
        if (pFile != nullptr)
        {
            STBICallbackData callbackData;
            callbackData.pFile = pFile;

            stbi_io_callbacks stbiCallbacks;
            stbiCallbacks.read = STBI_Read;
            stbiCallbacks.skip = STBI_Skip;
            stbiCallbacks.eof  = STBI_EOF;

            int imageWidth;
            int imageHeight;
            m_pImageData = stbi_load_from_callbacks(&stbiCallbacks, &callbackData, &imageWidth, &imageHeight, &m_channelCount, 0);
            if (m_pImageData != nullptr)
            {
                m_info = ImageFileInfo(this->absolutePath.c_str());

                m_info.format = GetImageFormatFromSTBChannels(m_channelCount);
                m_info.width  = static_cast<unsigned int>(imageWidth);
                m_info.height = static_cast<unsigned int>(imageHeight);
                m_info.mipmapCount = 1;

                return true;
            }
        }

        return false;


#if 0
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
#endif
    }

    void ImageLoader_PNG::GetImageInfo(ImageFileInfo &info)
    {
        info = m_info;
    }

    bool ImageLoader_PNG::LoadMipmap(unsigned int mipmapIndex, Mipmap &destMipmap)
    {
        // PNG's don't use the notion of mipmaps, so we only support a mipmap index of 0.
        if (mipmapIndex == 0)
        {
            size_t imageDataSize = m_info.width * m_info.height * m_channelCount;

            destMipmap.DeleteLocalData();
            destMipmap.data = malloc(imageDataSize);
            memcpy(destMipmap.data, m_pImageData, imageDataSize);

            destMipmap.format = m_info.format;
            destMipmap.width  = m_info.width;
            destMipmap.height = m_info.height;

            return true;

#if 0
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
                    dest = ((png_bytep)destMipmap.data) + (i * pitch);

                    png_read_row(this->png, dest, nullptr);
                }

                // At this point everything will be loaded and we can set the Mipmap's new attributes.
                destMipmap.format = GetImageFormatFromPNGChannels(png_get_channels(this->png, this->pngInfo));
                destMipmap.width  = width;
                destMipmap.height = height;

                this->UninitPNG(file);
                return true;
            }
#endif
        }

        return false;
    }

    bool ImageLoader_PNG::HasFileChanged() const
    {
        // We just need to check the last modified data. If it's different, the file has changed.
        GTLib::FileInfo tempInfo(this->absolutePath.c_str());
        if (tempInfo.lastModifiedTime != m_info.lastModifiedTime)
        {
            return true;
        }

        return false;
    }


#if 0
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
#endif
}

#endif
