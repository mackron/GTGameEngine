// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIEventHandler.hpp>

namespace GT
{
    GUIEventHandler::GUIEventHandler()
    {
    }

    GUIEventHandler::~GUIEventHandler()
    {
    }


    void GUIEventHandler::OnMove(GUIContext &, HGUIElement, unsigned int, unsigned int)
    {
    }

    void GUIEventHandler::OnSize(GUIContext &, HGUIElement, int, int)
    {
    }


    void GUIEventHandler::OnMouseEnter(GUIContext &, HGUIElement)
    {
    }

    void GUIEventHandler::OnMouseLeave(GUIContext &, HGUIElement)
    {
    }

    void GUIEventHandler::OnMouseMove(GUIContext &, HGUIElement, int, int)
    {
    }


    void GUIEventHandler::OnSurfaceNeedsRepaint(GUIContext &, HGUISurface, const GTLib::Rect<int> &)
    {
    }
}