// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorldGUIRenderer.hpp>
#include <GTGameEngine/Graphics/GraphicsWorld.hpp>

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
}
