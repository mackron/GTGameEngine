// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/WindowEventCallback.hpp>

namespace GTLib
{
    WindowEventCallback::WindowEventCallback()
    {
    }

    WindowEventCallback::~WindowEventCallback()
    {
    }



    ///////////////////////////////////////
    // Event Handlers

    void WindowEventCallback::OnClose(WindowHandle)
    {
    }

    void WindowEventCallback::OnPaint(WindowHandle, int, int, int, int, void*)
    {
    }


    void WindowEventCallback::OnSize(WindowHandle, unsigned int, unsigned int)
    {
    }

    void WindowEventCallback::OnMove(WindowHandle, int, int)
    {
    }


    void WindowEventCallback::OnMouseMove(WindowHandle, int, int)
    {
    }

    void WindowEventCallback::OnMouseButtonDown(WindowHandle, int, int, int)
    {
    }

    void WindowEventCallback::OnMouseButtonUp(WindowHandle, int, int, int)
    {
    }

    void WindowEventCallback::OnMouseButtonDoubleClick(WindowHandle, int, int, int)
    {
    }

    void WindowEventCallback::OnMouseWheel(WindowHandle, int, int, int)
    {
    }

        
    void WindowEventCallback::OnKeyPressed(WindowHandle, Key)
    {
    }

    void WindowEventCallback::OnKeyReleased(WindowHandle, Key)
    {
    }

    void WindowEventCallback::OnKeyDown(WindowHandle, Key)
    {
    }

    void WindowEventCallback::OnKeyUp(WindowHandle, Key)
    {
    }


    void WindowEventCallback::OnReceiveFocus(WindowHandle)
    {
    }

    void WindowEventCallback::OnLoseFocus(WindowHandle)
    {
    }


    void WindowEventCallback::OnTimer(WindowHandle, uint32_t)
    {
    }
}
