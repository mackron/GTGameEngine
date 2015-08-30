// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorldGUIResourceManager.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>

namespace GT
{
    GraphicsWorldGUIResourceManager::GraphicsWorldGUIResourceManager()
        : m_pGraphicsResourceManager(nullptr)
    {
    }

    GraphicsWorldGUIResourceManager::~GraphicsWorldGUIResourceManager()
    {
    }


    void GraphicsWorldGUIResourceManager::SetGraphicsAssetResourceManager(GraphicsAssetResourceManager* pGraphicsResourceManager)
    {
        m_pGraphicsResourceManager = pGraphicsResourceManager;
    }


    HGUIImage GraphicsWorldGUIResourceManager::LoadImage(const char* filePath)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            return reinterpret_cast<HGUIImage>(m_pGraphicsResourceManager->LoadTexture(filePath));
        }

        return NULL;
    }

    void GraphicsWorldGUIResourceManager::UnloadImage(HGUIImage hImage)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->Unload(reinterpret_cast<GraphicsAssetResource*>(hImage));
        }
    }

    bool GraphicsWorldGUIResourceManager::GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const
    {
        GraphicsAssetResource_Texture* pTextureResource = reinterpret_cast<GraphicsAssetResource_Texture*>(hImage);
        assert(pTextureResource != nullptr);

        ImageAsset* pTextureAsset = reinterpret_cast<ImageAsset*>(pTextureResource->GetAsset());
        if (pTextureAsset != nullptr)
        {
            widthOut  = pTextureAsset->GetImageWidth();
            heightOut = pTextureAsset->GetImageHeight();

            return true;
        }

        return false;
    }

    GUIImageFormat GraphicsWorldGUIResourceManager::GetImageFormat(HGUIImage hImage) const
    {
        GraphicsAssetResource_Texture* pTextureResource = reinterpret_cast<GraphicsAssetResource_Texture*>(hImage);
        assert(pTextureResource != nullptr);

        ImageAsset* pTextureAsset = reinterpret_cast<ImageAsset*>(pTextureResource->GetAsset());
        if (pTextureAsset != nullptr)
        {
            TextureFormat format = pTextureAsset->GetImageFormat();
            switch (format)
            {
            case TextureFormat_RGBA8: return GUIImageFormat::RGBA8;
            case TextureFormat_R8:    return GUIImageFormat::A8;

            default: break;
            }
        }

        return GUIImageFormat::Unknown;
    }

    const void* GraphicsWorldGUIResourceManager::GetImageData(HGUIImage hImage) const
    {
        GraphicsAssetResource_Texture* pTextureResource = reinterpret_cast<GraphicsAssetResource_Texture*>(hImage);
        assert(pTextureResource != nullptr);

        ImageAsset* pTextureAsset = reinterpret_cast<ImageAsset*>(pTextureResource->GetAsset());
        if (pTextureAsset != nullptr)
        {
            return pTextureAsset->GetImageData();
        }

        return nullptr;
    }
}
