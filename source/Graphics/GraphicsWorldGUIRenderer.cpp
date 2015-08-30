// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorldGUIRenderer.hpp>
#include <GTGameEngine/Graphics/GraphicsWorld.hpp>

namespace GT
{
    GraphicsWorldGUIRenderer::GraphicsWorldGUIRenderer()
        : m_pGraphicsWorld(nullptr), m_textures()
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
        (void)gui;

        assert(hImage != NULL);

        if (m_pGraphicsWorld != nullptr)
        {
            GraphicsTextureResourceDesc textureDesc;
            textureDesc.width  = width;
            textureDesc.height = height;
            textureDesc.depth  = 1;
            textureDesc.format = GUIImageFormatToGraphicsTextureFormat(format);
            textureDesc.pData  = pData;
            HGraphicsResource hTextureResource = m_pGraphicsWorld->CreateTextureResource(textureDesc);
            if (hTextureResource != NULL)
            {
                m_textures.Add(hImage, hTextureResource);
            }
        }
    }

    void GraphicsWorldGUIRenderer::UninitializeImage(GT::GUIContext &gui, HGUIImage hImage)
    {
        (void)gui;

        HGraphicsResource hTextureResource = this->GetTextureResource(hImage);
        if (hTextureResource != NULL)
        {
            if (m_pGraphicsWorld != nullptr)
            {
                m_pGraphicsWorld->DeleteResource(hTextureResource);
            }

            m_textures.RemoveByKey(hImage);
        }
    }



    //////////////////////////////////////////////
    // Private

    HGraphicsResource GraphicsWorldGUIRenderer::GetTextureResource(HGUIImage hImage)
    {
        auto iTextureResource = m_textures.Find(hImage);
        if (iTextureResource != nullptr)
        {
            return iTextureResource->value;
        }

        return NULL;
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
