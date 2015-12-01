// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GUIElementEventHandler.hpp>

namespace GT
{
    GUIElementEventHandler::~GUIElementEventHandler()
    {
    }

    void GUIElementEventHandler::OnSize(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnMove(GUIElement &)
    {
    }


    void GUIElementEventHandler::OnDraw(GUIElement &)
    {
    }

    bool GUIElementEventHandler::ImplementsOnDraw() const
    {
        return false;
    }


    void GUIElementEventHandler::OnMouseButtonDown(GUIElement &, int, int, int)
    {
    }
    
    void GUIElementEventHandler::OnMouseButtonUp(GUIElement &, int, int, int)
    {
    }
    
    void GUIElementEventHandler::OnMouseButtonDoubleClick(GUIElement &, int, int, int)
    {
    }


    void GUIElementEventHandler::OnMouseEnter(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnMouseLeave(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnPush(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnRelease(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnPressed(GUIElement &)
    {
    }


    void GUIElementEventHandler::OnMouseMove(GUIElement &, int, int)
    {
    }

    void GUIElementEventHandler::OnMouseWheel(GUIElement &, int, int, int)
    {
    }


    void GUIElementEventHandler::OnKeyPressed(GUIElement &, GT::Key)
    {
    }

    bool GUIElementEventHandler::OnKeyDown(GUIElement &, GT::Key)
    {
        return true;
    }
    
    void GUIElementEventHandler::OnKeyUp(GUIElement &, GT::Key)
    {
    }

    void GUIElementEventHandler::OnTextChanged(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnFocus(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnBlur(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnTear(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnDrop(GUIElement &, GUIElement &)
    {
    }

    void GUIElementEventHandler::OnDragAndDropEnter(GUIElement &, GUIElement &)
    {
    }

    void GUIElementEventHandler::OnDragAndDropLeave(GUIElement &, GUIElement &)
    {
    }

    void GUIElementEventHandler::OnDragAndDropProxyRemoved(GUIElement &)
    {
    }


    void GUIElementEventHandler::OnShow(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnHide(GUIElement &)
    {
    }


    void GUIElementEventHandler::OnShowTooltip(GUIElement &)
    {
    }

    void GUIElementEventHandler::OnHideTooltip(GUIElement &)
    {
    }
}
