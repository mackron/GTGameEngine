// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "ImageAsset_STB.hpp"
#include <GTGameEngine/Config.hpp>
#include <GTGameEngine/FileSystem.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_BMP
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#if !defined(GT_BUILD_PNG)
#define STBI_NO_PNG
#endif
#if !defined(GT_BUILD_TGA)
#define STBI_NO_TGA
#endif
#if !defined(GT_BUILD_JPG)
#define STBI_NO_JPEG
#endif
#if !defined(GT_BUILD_PSD)
#define STBI_NO_PSD
#endif

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wreserved-id-macro"
    #pragma GCC diagnostic ignored "-Wshadow"
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wcast-align"
    #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
    #pragma GCC diagnostic ignored "-Wdisabled-macro-expansion"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wold-style-cast"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4100)
    #pragma warning(disable:4456)
    #pragma warning(disable:4457)
    #pragma warning(disable:4505)
#endif
#include "../external/stb/stb_image.h"
#if defined(__clang__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif

namespace GT
{
    struct STBICallbackData
    {
        GT::FileSystem* pFileSystem;
        GT::HFile hFile;
    };

    int STBI_Read(void* user, char *data, int size)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFileSystem != nullptr);
            assert(callbackData->hFile       != 0);

            return static_cast<int>(callbackData->pFileSystem->ReadFile(callbackData->hFile, static_cast<unsigned int>(size), data));
        }
    }

    void STBI_Skip(void* user, int n)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFileSystem != nullptr);
            assert(callbackData->hFile       != 0);

            callbackData->pFileSystem->SeekFile(callbackData->hFile, n, GT::FileSeekOrigin::Current);
        }
    }

    int STBI_EOF(void* user)
    {
        auto callbackData = reinterpret_cast<STBICallbackData*>(user);
        assert(callbackData != nullptr);
        {
            assert(callbackData->pFileSystem != nullptr);
            assert(callbackData->hFile       != 0);

            return callbackData->pFileSystem->TellFile(callbackData->hFile) == callbackData->pFileSystem->GetFileSize(callbackData->hFile);
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


    bool ImageAsset_STB::Load(const char* absolutePath, GT::FileSystem &fileSystem)
    {
        GT::HFile hFile = fileSystem.OpenFile(absolutePath, GT::FileAccessMode::Read);
        if (hFile != 0)
        {
            bool result = false;

            STBICallbackData callbackData;
            callbackData.pFileSystem = &fileSystem;
            callbackData.hFile       = hFile;

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

            fileSystem.CloseFile(hFile);
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
