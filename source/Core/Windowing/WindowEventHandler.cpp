// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Window.hpp>

namespace GTLib
{
    void WindowEventHandler::OnMove(int, int)
    {
    }
    
    void WindowEventHandler::OnSize(unsigned int, unsigned int)
    {
    }
    
    void WindowEventHandler::OnClose()
    {
    }
    
    void WindowEventHandler::OnPaint(int, int, int, int)
    {
    }
    
    void WindowEventHandler::OnUpdate(double)
    {
    }
    
    void WindowEventHandler::OnMouseMove(int, int)
    {
    }

    void WindowEventHandler::OnMouseWheel(int, int, int)
    {
    }
    
    void WindowEventHandler::OnMouseButtonDown(MouseButton, int, int)
    {
    }
    
    void WindowEventHandler::OnMouseButtonUp(MouseButton, int, int)
    {
    }
    
    void WindowEventHandler::OnMouseButtonDoubleClick(MouseButton, int, int)
    {
    }

    void WindowEventHandler::OnKeyPressed(Key)
    {
    }

    void WindowEventHandler::OnKeyReleased(Key)
    {
    }

    void WindowEventHandler::OnKeyDown(Key, unsigned int)
    {
    }

    void WindowEventHandler::OnKeyUp(Key)
    {
    }

    /*
    void WindowEventHandler::OnCharDown(char32_t, unsigned int)
    {
    }

    void WindowEventHandler::OnCharUp(char32_t)
    {
    }
    */
    
    void WindowEventHandler::OnReceiveFocus()
    {
    }
    
    void WindowEventHandler::OnLoseFocus()
    {
    }
    
    
    static WindowEventHandler DefaultWindowEventHandler;
    
    WindowEventHandler & WindowEventHandler::GetDefault()
    {
        return DefaultWindowEventHandler;
    }
}