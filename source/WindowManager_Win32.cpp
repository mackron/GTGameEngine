// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/WindowManager_Win32.hpp>
#include <cassert>

#define CURSOR_SUPPRESSED           0x00000002

namespace GT
{
    const wchar_t* g_WindowClass = L"GT_WndClass";

    WindowManager_Win32::WindowManager_Win32(unsigned int extraWindowBytes, WNDPROC wndProc)
        : WindowManager()
    {
        // Window class need to be registered.
        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = extraWindowBytes;
        wc.lpfnWndProc   = wndProc;
        wc.lpszClassName = g_WindowClass;
        wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        if (!::RegisterClassExW(&wc))
        {
            // Failed to register window class.
            assert(false);
        }
    }

    WindowManager_Win32::~WindowManager_Win32()
    {
        UnregisterClassW(g_WindowClass, GetModuleHandleW(NULL));
    }


    HWindow WindowManager_Win32::CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        DWORD dwExStyle = 0;
        DWORD dwStyle   = 0;

        switch (type)
        {
        case WindowType::PrimaryWindow:
            {
                dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
                break;
            }

        case WindowType::PrimaryWindow_Borderless:
            {
                dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
                break;
            }

        case WindowType::ChildWindow:
            {
                assert(hParent != 0);

                dwStyle |= WS_CHILD;
                break;
            }

        case WindowType::PopupWindow:
            {
                dwStyle |= WS_POPUP;
                break;
            }

        default:
            {
                assert(false);
            }
        }


        HWND hWnd = ::CreateWindowExW(dwExStyle, g_WindowClass, L"GTGameEngine", dwStyle, xPos, yPos, width, height, reinterpret_cast<HWND>(hParent), NULL, NULL, nullptr);
        if (hWnd != NULL)
        {
        }

        return reinterpret_cast<HWindow>(hWnd);
    }

    void WindowManager_Win32::DeleteWindow(HWindow hWindow)
    {
        ::DestroyWindow(reinterpret_cast<HWND>(hWindow));
    }


    void WindowManager_Win32::SetWindowTitle(HWindow hWindow, const char* title)
    {
        if (hWindow != 0)
        {
            ::SetWindowTextA(reinterpret_cast<HWND>(hWindow), title);
        }
    }

    void WindowManager_Win32::SetWindowPosition(HWindow hWindow, int xPos, int yPos)
    {
        if (hWindow != 0)
        {
            ::SetWindowPos(reinterpret_cast<HWND>(hWindow), NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
    }

    void WindowManager_Win32::SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height)
    {
        if (hWindow != 0)
        {
            ::SetWindowPos(reinterpret_cast<HWND>(hWindow), NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
        }
    }


    void WindowManager_Win32::ShowWindow(HWindow hWindow)
    {
        if (hWindow != 0)
        {
            ::ShowWindow(reinterpret_cast<HWND>(hWindow), SW_SHOW);
        }
    }

    void WindowManager_Win32::HideWindow(HWindow hWindow)
    {
        if (hWindow != 0)
        {
            ::ShowWindow(reinterpret_cast<HWND>(hWindow), SW_HIDE);
        }
    }


    void WindowManager_Win32::InvalidateWindowRect(HWindow hWindow, const GTLib::Rect<int> &rect)
    {
        if (hWindow != 0)
        {
            RECT rectWin32;
            rectWin32.left   = rect.left;
            rectWin32.top    = rect.top;
            rectWin32.right  = rect.right;
            rectWin32.bottom = rect.bottom;
            ::InvalidateRect(reinterpret_cast<HWND>(hWindow), &rectWin32, true);
        }
    }


    void WindowManager_Win32::SetFocusedWindow(HWindow hWindow)
    {
        ::SetFocus(reinterpret_cast<HWND>(hWindow));
    }

    HWindow WindowManager_Win32::GetFocusedWindow() const
    {
        return reinterpret_cast<HWindow>(::GetFocus());
    }


    void WindowManager_Win32::ShowCursor()
    {
        if (!this->IsCursorVisible())
        {
            while (::ShowCursor(TRUE) < 0);
        }
    }

    void WindowManager_Win32::HideCursor()
    {
        if (this->IsCursorVisible())
        {
            while (::ShowCursor(FALSE) >= 0);
        }
    }

    bool WindowManager_Win32::IsCursorVisible() const
    {
        CURSORINFO ci;
        ci.cbSize = sizeof(ci);

        if (GetCursorInfo(&ci))
        {
            return ci.flags == CURSOR_SHOWING || ci.flags == CURSOR_SUPPRESSED;
        }

        return true;
    }


    void WindowManager_Win32::PostQuitMessage(int exitCode)
    {
        ::PostQuitMessage(exitCode);
    }

    int WindowManager_Win32::GetExitCode() const
    {
        return static_cast<int>(m_wExitCode);
    }



    void WindowManager_Win32::EventDrivenLoop(std::function<bool ()> postLoop)
    {
        MSG msg;
        BOOL bRet;
        while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                // Unknown error.
                break;
            }
                
            TranslateMessage(&msg);
            DispatchMessageA(&msg);


            if (!postLoop())
            {
                break;
            }
        }


        // If we get here we received a WM_QUIT message.
        m_wExitCode = msg.wParam;
    }

    void WindowManager_Win32::RealTimeLoop(std::function<bool ()> postLoop)
    {
        for (;;)
        {
            // Handle window events.
            MSG msg;
            if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    // Received a quit message.
                    m_wExitCode = msg.wParam;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }


            if (!postLoop())
            {
                break;
            }
        }
    }
}
