// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "ImageAsset_STB.hpp"
#include <GTGE/Config.hpp>
#include <GTGE/external/stb_image.h>

namespace GT
{
    struct STBICallbackData
    {
        /// A pointer to the virtual file.
        drvfs_file* pFile;
    };

    static int STBI_Read(void* user, char *data, int size)
    {
        STBICallbackData* pCallbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(pCallbackData != nullptr);
        {
            assert(pCallbackData->pFile != nullptr);

            unsigned int bytesRead;
            if (drvfs_read(pCallbackData->pFile, data, static_cast<unsigned int>(size), &bytesRead) == drvfs_success)
            {
                return bytesRead;
            }
            else
            {
                return 0;
            }
        }
    }

    static void STBI_Skip(void* user, int n)
    {
        STBICallbackData* pCallbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(pCallbackData != nullptr);
        {
            assert(pCallbackData->pFile != nullptr);
            drvfs_seek(pCallbackData->pFile, n, drvfs_origin_current);
        }
    }

    static int STBI_EOF(void* user)
    {
        STBICallbackData* pCallbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(pCallbackData != nullptr);
        {
            assert(pCallbackData->pFile != nullptr);
            return drvfs_eof(pCallbackData->pFile);
        }
    }



    ImageAsset_STB::ImageAsset_STB(const char* absolutePathOrIdentifier, AssetType type)
        : ImageAsset(absolutePathOrIdentifier, type),
          m_width(0),
          m_height(0),
          m_format(GT::TextureFormat_RGBA8),
          m_data(nullptr)
    {
    }

    ImageAsset_STB::~ImageAsset_STB()
    {
        stbi_image_free(m_data);
    }


    bool ImageAsset_STB::Load(const char* absolutePath, drvfs_context* pVFS)
    {
        drvfs_file* pFile = drvfs_open(pVFS, absolutePath, DRVFS_READ, 0);
        if (pFile != nullptr)
        {
            bool result = false;

            STBICallbackData callbackData;
            callbackData.pFile = pFile;

            stbi_io_callbacks stbiCallbacks;
            stbiCallbacks.read = STBI_Read;
            stbiCallbacks.skip = STBI_Skip;
            stbiCallbacks.eof  = STBI_EOF;

            int imageWidth;
            int imageHeight;
            auto imageData = stbi_load_from_callbacks(&stbiCallbacks, &callbackData, &imageWidth, &imageHeight, nullptr, 4);
            if (imageData != nullptr)
            {
                m_width  = static_cast<unsigned int>(imageWidth);
                m_height = static_cast<unsigned int>(imageHeight);
                m_format = GT::TextureFormat_RGBA8;
                m_data   = imageData;

                result = true;
            }

            drvfs_close(pFile);
            return result;
        }
        else
        {
            return false;
        }
    }

    unsigned int ImageAsset_STB::GetImageWidth() const
    {
        return m_width;
    }

    unsigned int ImageAsset_STB::GetImageHeight() const
    {
        return m_height;
    }

    GT::TextureFormat ImageAsset_STB::GetImageFormat() const
    {
        return m_format;
    }

    void* ImageAsset_STB::GetImageData() const
    {
        return m_data;
    }
}
