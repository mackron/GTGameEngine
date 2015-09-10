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


    void GUIEventHandler::OnMove(GUIContext &, HGUIElement, int, int)
    {
    }

    void GUIEventHandler::OnSize(GUIContext &, HGUIElement, unsigned int, unsigned int)
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

    void GUIEventHandler::OnMouseButtonPressed(GUIContext &, HGUIElement, int, int, int)
    {
    }

    void GUIEventHandler::OnMouseButtonReleased(GUIContext &, HGUIElement, int, int, int)
    {
    }

    void GUIEventHandler::OnMouseButtonDoubleClicked(GUIContext &, HGUIElement, int, int, int )
    {
    }

    void GUIEventHandler::OnMouseWheel(GUIContext &, HGUIElement, int, int, int)
    {
    }


    void GUIEventHandler::OnKeyPressed(GUIContext &, HGUIElement, GTLib::Key)
    {
    }

    void GUIEventHandler::OnKeyReleased(GUIContext &, HGUIElement, GTLib::Key)
    {
    }

    void GUIEventHandler::OnPrintableKeyDown(GUIContext &, HGUIElement, char32_t)
    {
    }


    void GUIEventHandler::OnSetMouseEventCapture(GUIContext &, HGUIElement)
    {
    }

    void GUIEventHandler::OnReleaseMouseEventCapture(GUIContext &, HGUIElement)
    {
    }


    void GUIEventHandler::OnSurfaceNeedsRepaint(GUIContext &, HGUISurface, const GTLib::Rect<int> &)
    {
    }

    void GUIEventHandler::OnCursorNeedsToChange(GUIContext &, GUISystemCursor)
    {
    }


    void GUIEventHandler::OnReceiveKeyboardFocus(GUIContext &, HGUIElement)
    {
    }

    void GUIEventHandler::OnLoseKeyboardFocus(GUIContext &, HGUIElement)
    {
    }
}
