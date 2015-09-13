// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/WindowManager_DefaultWin32.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GameContext.hpp>



namespace GT
{
    WindowManager_DefaultWin32::WindowManager_DefaultWin32(GameContext* pGameContext)
        : WindowManager_Win32(),
          m_pGameContext(pGameContext)
    {
    }

    WindowManager_DefaultWin32::~WindowManager_DefaultWin32()
    {
    }



    void WindowManager_DefaultWin32::OnClose()
    {
        m_pGameContext->OnWantToClose();
    }
    
    void WindowManager_DefaultWin32::OnWindowActivated(HWindow hWindow)
    {
        m_pGameContext->OnWindowActivated(hWindow);
    }

    void WindowManager_DefaultWin32::OnWindowDeactivated(HWindow hWindow)
    {
        m_pGameContext->OnWindowDeactivated(hWindow);
    }

    void WindowManager_DefaultWin32::OnMove(HWindow hWindow, int xPos, int yPos)
    {
        m_pGameContext->OnWindowMoved(hWindow, xPos, yPos);
    }

    void WindowManager_DefaultWin32::OnSize(HWindow hWindow, unsigned int width, unsigned int height)
    {
        m_pGameContext->OnWindowResized(hWindow, width, height);
    }

    void WindowManager_DefaultWin32::OnMouseEnter(HWindow hWindow)
    {
        m_pGameContext->OnMouseEnter(hWindow);
    }

    void WindowManager_DefaultWin32::OnMouseLeave(HWindow hWindow)
    {
        m_pGameContext->OnMouseLeave(hWindow);
    }

    void WindowManager_DefaultWin32::OnMouseMove(HWindow hWindow, int mousePosX, int mousePosY)
    {
        m_pGameContext->OnMouseMove(hWindow, mousePosX, mousePosY);
    }

    void WindowManager_DefaultWin32::OnMouseButtonPressed(HWindow hWindow, int button, int mousePosX, int mousePosY)
    {
        m_pGameContext->OnMouseButtonPressed(hWindow, button, mousePosX, mousePosY);
    }

    void WindowManager_DefaultWin32::OnMouseButtonReleased(HWindow hWindow, int button, int mousePosX, int mousePosY)
    {
        m_pGameContext->OnMouseButtonReleased(hWindow, button, mousePosX, mousePosY);
    }

    void WindowManager_DefaultWin32::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int mousePosX, int mousePosY)
    {
        m_pGameContext->OnMouseButtonDoubleClicked(hWindow, button, mousePosX, mousePosY);
    }

    void WindowManager_DefaultWin32::OnMouseWheel(HWindow hWindow, int delta, int mousePosX, int mousePosY)
    {
        m_pGameContext->OnMouseWheel(hWindow, delta, mousePosX, mousePosY);
    }

    void WindowManager_DefaultWin32::OnKeyPressed(HWindow hWindow, GTLib::Key key, bool autoRepeat)
    {
        if (!autoRepeat)
        {
            m_pGameContext->OnKeyPressed(hWindow, key);
        }
        
        m_pGameContext->OnKeyPressedAutoRepeat(hWindow, key);
    }

    void WindowManager_DefaultWin32::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
        m_pGameContext->OnKeyReleased(hWindow, key);
    }

    void WindowManager_DefaultWin32::OnPrintableKeyDown(HWindow hWindow, char32_t character)
    {
        m_pGameContext->OnPrintableKeyDown(hWindow, character);
    }

    void WindowManager_DefaultWin32::OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect)
    {
        m_pGameContext->OnPaintWindow(hWindow, rect);
    }
}

#endif
