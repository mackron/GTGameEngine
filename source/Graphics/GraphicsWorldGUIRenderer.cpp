// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorldGUIRenderer.hpp>
#include <GTGameEngine/Graphics/GraphicsWorldGUIResourceManager.hpp>

namespace GT
{
    GraphicsWorldGUIRenderer::GraphicsWorldGUIRenderer()
        : m_pGraphicsWorld(nullptr)
    {
    }


    void GraphicsWorldGUIRenderer::SetGraphicsWorld(GraphicsWorld* pGraphicsWorld)
    {
        assert(m_pGraphicsWorld == nullptr);
        m_pGraphicsWorld = pGraphicsWorld;
    }


    void GraphicsWorldGUIRenderer::BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_BeginPaintSurface(gui, hSurface, pInputData);
        }
    }

    void GraphicsWorldGUIRenderer::EndPaintSurface(GUIContext &gui)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_EndPaintSurface(gui);
        }
    }

    void GraphicsWorldGUIRenderer::Clear(GUIContext &gui)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_Clear(gui);
        }
    }

    void GraphicsWorldGUIRenderer::Clear(GUIContext &gui, const GTLib::Rect<int> &rect)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_Clear(gui, rect);
        }
    }

    void GraphicsWorldGUIRenderer::DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_DrawRectangle(gui, rect, colour);
        }
    }

    void GraphicsWorldGUIRenderer::SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_SetClippingRect(gui, rect);
        }
    }

    bool GraphicsWorldGUIRenderer::CanDrawText(GUIContext &gui, HGUIFont hFont)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            return m_pGraphicsWorld->GUI_CanDrawText(gui, hFont);
        }

        return false;
    }

    void GraphicsWorldGUIRenderer::DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_DrawText(gui, textRunDesc);
        }
    }

    void GraphicsWorldGUIRenderer::DrawRawImage(GT::GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent)
    {
        if (m_pGraphicsWorld != nullptr)
        {
            m_pGraphicsWorld->GUI_DrawRawImage(gui, xPos, yPos, width, height, pImageData, isTransparent);
        }
    }


    void GraphicsWorldGUIRenderer::InitializeImage(GT::GUIContext &gui, HGUIImage hImage, unsigned int width, unsigned int height, GUIImageFormat format, const void* pData)
    {
        // We don't need to do anything here because this will be used in conjunction with GraphicsWorldGUIResourceManager which is where
        // the texture resource will be created.

        (void)gui;
        (void)hImage;
        (void)width;
        (void)height;
        (void)format;
        (void)pData;
    }

    void GraphicsWorldGUIRenderer::UninitializeImage(GT::GUIContext &gui, HGUIImage hImage)
    {
        // As above, no need to do anything here because it will all be handled by GraphicsWorldGUIResourceManager.

        (void)gui;
        (void)hImage;
    }

    void GraphicsWorldGUIRenderer::DrawTexturedRectangle(GT::GUIContext &gui, GTLib::Rect<int> rect, HGUIImage hImage, GTLib::Colour colour, unsigned int subImageOffsetX, unsigned int subImageOffsetY, unsigned int subImageWidth, unsigned int subImageHeight)
    {
        (void)gui;

        HGraphicsResource hTextureResource = this->GetTextureResource(hImage);
        if (hTextureResource != NULL)
        {
            m_pGraphicsWorld->GUI_DrawTexturedRectangle(gui, rect, hTextureResource, colour, subImageOffsetX, subImageOffsetY, subImageWidth, subImageHeight);
        }
    }



    //////////////////////////////////////////////
    // Private

    HGraphicsResource GraphicsWorldGUIRenderer::GetTextureResource(HGUIImage hImage)
    {
        // The input image will be a pointer to a GraphicsAssetResource_Texture object because it will be used in conjunction with GraphicsWorldGUIResourceManager.

        GraphicsAssetResource_Texture* pTextureResource = reinterpret_cast<GraphicsAssetResource_Texture*>(hImage);
        assert(pTextureResource != nullptr);

        return pTextureResource->GetGraphicsResource();
    }

    TextureFormat GraphicsWorldGUIRenderer::GUIImageFormatToGraphicsTextureFormat(GUIImageFormat imageFormat)
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
