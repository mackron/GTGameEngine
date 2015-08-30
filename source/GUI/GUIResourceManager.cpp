// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIResourceManager.hpp>

namespace GT
{
    GUIResourceManager::GUIResourceManager()
        : m_loadedImages(), m_encodedResourcePaths()
    {
    }

    GUIResourceManager::~GUIResourceManager()
    {
    }


    HGUIImage GUIResourceManager::AcquireImage(const char* filePath, unsigned int &referenceCountOut)
    {
        // First we need to check if the image is already loaded.
        auto hImage = this->AcquireLoadedImage(filePath, referenceCountOut);
        if (hImage == 0)
        {
            // The image is not already loaded, so it needs to be loaded now.
            hImage = this->LoadImage(filePath);
            if (hImage != 0)
            {
                referenceCountOut = 1;
                m_loadedImages.PushBack(LoadedImage(hImage, filePath));
            }
        }

        return hImage;
    }

    void GUIResourceManager::UnacquireImage(HGUIImage hImage, unsigned int &referenceCountOut)
    {
        // We simply decrement the reference counter. If the new counter is at 0, we want to delete it completely.
        size_t loadedImageIndex;
        LoadedImage* pLoadedImage = this->FindLoadedImage(hImage, loadedImageIndex);
        if (pLoadedImage != nullptr)
        {
            assert(pLoadedImage->referenceCount > 0);
            {
                pLoadedImage->referenceCount -= 1;
                if (pLoadedImage->referenceCount == 0)
                {
                    this->UnloadImage(pLoadedImage->hImage);
                    m_loadedImages.Remove(loadedImageIndex);
                }

                referenceCountOut = pLoadedImage->referenceCount;
            }
        }
    }


    uint32_t GUIResourceManager::EncodeFilePath(const char* filePath)
    {
        uint32_t encodedValue = GTLib::Hash(filePath);
        m_encodedResourcePaths.Add(encodedValue, filePath);

        return encodedValue;
    }

    const char* GUIResourceManager::DecodeFilePath(uint32_t encodedFilePath)
    {
        auto iDecodedValue = m_encodedResourcePaths.Find(encodedFilePath);
        if (iDecodedValue != nullptr)
        {
            return iDecodedValue->value.c_str();
        }

        return nullptr;
    }


    void GUIResourceManager::UnloadAllResources()
    {
        for (size_t iLoadedResource = 0; iLoadedResource < m_loadedImages.count; ++iLoadedResource)
        {
            this->UnloadImage(m_loadedImages[iLoadedResource].hImage);
        }

        m_loadedImages.Clear();
    }



    /////////////////////////////////////////
    // Private

    HGUIImage GUIResourceManager::AcquireLoadedImage(const char* filePath, unsigned int &referenceCountOut)
    {
        for (size_t iLoadedImage = 0; iLoadedImage < m_loadedImages.count; ++iLoadedImage)
        {
            LoadedImage &loadedImage = m_loadedImages[iLoadedImage];

            if (strcmp(loadedImage.filePath, filePath) == 0)
            {
                loadedImage.referenceCount += 1;

                referenceCountOut = loadedImage.referenceCount;
                return loadedImage.hImage;
            }
        }

        return 0;
    }

    GUIResourceManager::LoadedImage* GUIResourceManager::FindLoadedImage(HGUIImage hImage, size_t &indexOut)
    {
        for (size_t iLoadedImage = 0; iLoadedImage < m_loadedImages.count; ++iLoadedImage)
        {
            auto &loadedImage = m_loadedImages[iLoadedImage];

            if (loadedImage.hImage == hImage)
            {
                indexOut = iLoadedImage;
                return &loadedImage;
            }
        }

        return nullptr;
    }
}
