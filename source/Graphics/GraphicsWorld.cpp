// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsWorld.hpp>

//#include <cstdio>

namespace GT
{
    GraphicsWorld::GraphicsWorld(GUIContext &gui)
        : m_gui(gui)
    {
    }

    GraphicsWorld::~GraphicsWorld()
    {
    }


    GUIContext & GraphicsWorld::GetGUI() const
    {
        return m_gui;
    }


    void GraphicsWorld::GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData)
    {
        (void)gui;
        (void)hSurface;
        (void)pInputData;
    }

    void GraphicsWorld::GUI_EndPaintSurface(GUIContext &gui)
    {
        (void)gui;
    }

    void GraphicsWorld::GUI_Clear(GUIContext &gui)
    {
        (void)gui;
    }

    void GraphicsWorld::GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect)
    {
        (void)gui;
        (void)rect;
    }

    void GraphicsWorld::GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour)
    {
        (void)gui;
        (void)rect;
        (void)colour;
    }

    void GraphicsWorld::GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect)
    {
        (void)gui;
        (void)rect;
    }

    bool GraphicsWorld::GUI_CanDrawText(GUIContext &gui, HGUIFont hFont)
    {
        (void)gui;
        (void)hFont;

        return false;
    }

    void GraphicsWorld::GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc)
    {
        (void)gui;
        (void)textRunDesc;
    }
}