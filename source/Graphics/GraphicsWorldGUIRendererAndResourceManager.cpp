// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorldGUIRendererAndResourceManager.hpp>
#include <GTGameEngine/Assets/ImageAsset.hpp>

namespace GT
{
    GraphicsWorldGUIRendererAndResourceManager::GraphicsWorldGUIRendererAndResourceManager()
        : m_pGraphicsResourceManager(nullptr)
    {
    }


    void GraphicsWorldGUIRendererAndResourceManager::SetGraphicsAssetResourceManager(GraphicsAssetResourceManager* pGraphicsResourceManager)
    {
        assert(m_pGraphicsResourceManager == nullptr);
        m_pGraphicsResourceManager = pGraphicsResourceManager;
    }


    void GraphicsWorldGUIRendererAndResourceManager::BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_BeginPaintSurface(gui, hSurface, pInputData);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::EndPaintSurface(GUIContext &gui)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_EndPaintSurface(gui);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::Clear(GUIContext &gui)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_Clear(gui);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::Clear(GUIContext &gui, const GTLib::Rect<int> &rect)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_Clear(gui, rect);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GT::Color colour)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_DrawRectangle(gui, rect, colour);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_SetClippingRect(gui, rect);
        }
    }

    bool GraphicsWorldGUIRendererAndResourceManager::CanDrawText(GUIContext &gui, HGUIFont hFont)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            return m_pGraphicsResourceManager->GetGraphicsWorld().GUI_CanDrawText(gui, hFont);
        }

        return false;
    }

    void GraphicsWorldGUIRendererAndResourceManager::DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_DrawText(gui, textRunDesc);
        }
    }

    void GraphicsWorldGUIRendererAndResourceManager::DrawRawImage(GT::GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->GetGraphicsWorld().GUI_DrawRawImage(gui, xPos, yPos, width, height, pImageData, isTransparent);
        }
    }


    void GraphicsWorldGUIRendererAndResourceManager::InitializeImage(GT::GUIContext &gui, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData)
    {
        // We don't need to do anything here because everything will have been initialized by LoadImage().

        (void)gui;
        (void)hImage;
        (void)width;
        (void)height;
        (void)format;
        (void)pData;
    }

    void GraphicsWorldGUIRendererAndResourceManager::UninitializeImage(GT::GUIContext &gui, HGUIImage hImage)
    {
        // As above, no need to do anything here because it will all be handled by UnloadImage().

        (void)gui;
        (void)hImage;
    }

    void GraphicsWorldGUIRendererAndResourceManager::DrawTexturedRectangle(GT::GUIContext &gui, GTLib::Rect<int> rect, HGUIImage hImage, GT::Color colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        (void)gui;

        if (m_pGraphicsResourceManager != nullptr)
        {
            HGraphicsResource hTextureResource = this->GetTextureResource(hImage);
            if (hTextureResource != NULL)
            {
                m_pGraphicsResourceManager->GetGraphicsWorld().GUI_DrawTexturedRectangle(gui, rect, hTextureResource, colour, subImageOffsetX, subImageOffsetY, subImageWidth, subImageHeight);
            }
        }
    }



    /////////////////////////////////////////////////
    // GUIResourceManager

    HGUIImage GraphicsWorldGUIRendererAndResourceManager::LoadImage(const char* filePath)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            return reinterpret_cast<HGUIImage>(m_pGraphicsResourceManager->LoadTexture(filePath));
        }

        return NULL;
    }

    void GraphicsWorldGUIRendererAndResourceManager::UnloadImage(HGUIImage hImage)
    {
        if (m_pGraphicsResourceManager != nullptr)
        {
            m_pGraphicsResourceManager->Unload(reinterpret_cast<GraphicsAssetResource*>(hImage));
        }
    }

    bool GraphicsWorldGUIRendererAndResourceManager::GetImageSize(HGUIImage hImage, unsigned int &widthOut, unsigned int &heightOut) const
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

    GUIImageFormat GraphicsWorldGUIRendererAndResourceManager::GetImageFormat(HGUIImage hImage) const
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

    const void* GraphicsWorldGUIRendererAndResourceManager::GetImageData(HGUIImage hImage) const
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




    //////////////////////////////////////////////
    // Private

    HGraphicsResource GraphicsWorldGUIRendererAndResourceManager::GetTextureResource(HGUIImage hImage)
    {
        // The input image will be a pointer to a GraphicsAssetResource_Texture object.
        GraphicsAssetResource_Texture* pTextureResource = reinterpret_cast<GraphicsAssetResource_Texture*>(hImage);
        assert(pTextureResource != nullptr);

        return pTextureResource->GetGraphicsResource();
    }

    TextureFormat GraphicsWorldGUIRendererAndResourceManager::GUIImageFormatToGraphicsTextureFormat(GUIImageFormat imageFormat)
    {
        switch (imageFormat)
        {
        case GUIImageFormat::RGBA8:
            {
                return TextureFormat_RGBA8;
            }

        case GUIImageFormat::A8:
            {
                return TextureFormat_R8;
            }

        case GUIImageFormat::None:
        default:
            {
                return TextureFormat_Unknown;
            }
        }
    }
}
