// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GUI/ElementEventHandler.hpp>

namespace GTGUI
{
    ElementEventHandler::~ElementEventHandler()
    {
    }

    void ElementEventHandler::OnSize(Element &)
    {
    }

    void ElementEventHandler::OnMove(Element &)
    {
    }


    void ElementEventHandler::OnDraw(Element &)
    {
    }

    bool ElementEventHandler::ImplementsOnDraw() const
    {
        return false;
    }


    void ElementEventHandler::OnMouseButtonDown(Element &, int, int, int)
    {
    }
    
    void ElementEventHandler::OnMouseButtonUp(Element &, int, int, int)
    {
    }
    
    void ElementEventHandler::OnMouseButtonDoubleClick(Element &, int, int, int)
    {
    }


    void ElementEventHandler::OnMouseEnter(Element &)
    {
    }

    void ElementEventHandler::OnMouseLeave(Element &)
    {
    }

    void ElementEventHandler::OnPush(Element &)
    {
    }

    void ElementEventHandler::OnRelease(Element &)
    {
    }

    void ElementEventHandler::OnPressed(Element &)
    {
    }


    void ElementEventHandler::OnMouseMove(Element &, int, int)
    {
    }

    void ElementEventHandler::OnMouseWheel(Element &, int, int, int)
    {
    }


    void ElementEventHandler::OnKeyPressed(Element &, GTLib::Key)
    {
    }

    bool ElementEventHandler::OnKeyDown(Element &, GTLib::Key)
    {
        return true;
    }
    
    void ElementEventHandler::OnKeyUp(Element &, GTLib::Key)
    {
    }

    void ElementEventHandler::OnTextChanged(Element &)
    {
    }

    void ElementEventHandler::OnFocus(Element &)
    {
    }

    void ElementEventHandler::OnBlur(Element &)
    {
    }

    void ElementEventHandler::OnTear(Element &)
    {
    }

    void ElementEventHandler::OnDrop(Element &, Element &)
    {
    }

    void ElementEventHandler::OnDragAndDropEnter(Element &, Element &)
    {
    }

    void ElementEventHandler::OnDragAndDropLeave(Element &, Element &)
    {
    }

    void ElementEventHandler::OnDragAndDropProxyRemoved(Element &)
    {
    }


    void ElementEventHandler::OnShow(Element &)
    {
    }

    void ElementEventHandler::OnHide(Element &)
    {
    }


    void ElementEventHandler::OnShowTooltip(Element &)
    {
    }

    void ElementEventHandler::OnHideTooltip(Element &)
    {
    }
}
