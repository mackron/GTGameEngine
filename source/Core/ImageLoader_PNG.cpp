// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.


#ifndef GTLIB_NO_PNG

#include <GTGE/Core/stdlib.hpp>
#include "ImageLoader_PNG.hpp"

#include <GTGE/GTEngine.hpp>
#include <dr_libs/dr_vfs.h>

namespace GT
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
        drvfs_file* pFile;
    };

    static int STBI_Read(void* user, char *data, int size)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);

            unsigned int bytesRead;
            if (!drvfs_read(callbackData->pFile, data, (unsigned int)size, &bytesRead)) {
                return 0;
            }

            return bytesRead;
        }
    }

    static void STBI_Skip(void* user, int n)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);
            drvfs_seek(callbackData->pFile, n, drvfs_origin_current);
        }
    }

    static int STBI_EOF(void* user)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFile != nullptr);
            return drvfs_eof(callbackData->pFile);
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
        //FILE* pFile = IO::Open(this->absolutePath.c_str(), IO::OpenMode::Read);
        drvfs_file* pFile = drvfs_open(g_Context->GetVFS(), this->absolutePath.c_str(), DRVFS_READ, 0);
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
                drvfs_get_file_info(g_Context->GetVFS(), this->absolutePath.c_str(), &m_info);

                m_info.format = GetImageFormatFromSTBChannels(m_channelCount);
                m_info.width  = static_cast<unsigned int>(imageWidth);
                m_info.height = static_cast<unsigned int>(imageHeight);
                m_info.mipmapCount = 1;

                return true;
            }
        }

        return false;
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
        }

        return false;
    }

    bool ImageLoader_PNG::HasFileChanged() const
    {
        // We just need to check the last modified data. If it's different, the file has changed.
        drvfs_file_info tempInfo;
        drvfs_get_file_info(g_Context->GetVFS(), this->absolutePath.c_str(), &tempInfo);

        if (tempInfo.lastModifiedTime != m_info.lastModifiedTime)
        {
            return true;
        }

        return false;
    }
}

#endif
