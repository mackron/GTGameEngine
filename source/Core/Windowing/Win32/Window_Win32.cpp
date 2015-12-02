// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)

#include <GTGE/Core/Window.hpp>
#include <GTGE/Core/Windowing/Win32/Win32.hpp>


namespace GT
{
    extern const char* WindowClassName;     // <-- Defined in WindowManagement.cpp

    // External flags. These are defined in WindowManagement.cpp
    extern const uint32_t WindowCursorHidden;


    /// Defined in WindowManagement.cpp
    HCURSOR GetHCURSOR(Cursor cursor);


    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height)
    {
        DWORD style   = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
        DWORD exStyle = 0;
        HWND hWnd = CreateWindowExA(exStyle, WindowClassName, "", style, x, y, static_cast<int>(width), static_cast<int>(height), nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
        if (hWnd != nullptr)
        {
            // The window was created succcessfully, so now we can create the actual handle.
            auto window = new _Window;
            window->hWnd    = hWnd;
            window->hDC     = GetDC(hWnd);
            window->style   = style;
            window->exStyle = exStyle;

            SetPropA(hWnd, "__gtc_window", reinterpret_cast<HANDLE>(window));


            return window;
        }

        return 0;
    }

    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height, int pixelFormat, const struct tagPIXELFORMATDESCRIPTOR* pfd)
    {
        WindowHandle window = CreateWindow(x, y, width, height);
        if (window != nullptr)
        {
            SetPixelFormat(window->hDC, pixelFormat, pfd);
        }

        return window;
    }

    void DeleteWindow(WindowHandle window)
    {
        DestroyWindow(window->hWnd);
        delete window;
    }


    void ShowWindow(WindowHandle window)
    {
        ShowWindow(window->hWnd, SW_SHOW);
    }

    void HideWindow(WindowHandle window)
    {
        ShowWindow(window->hWnd, SW_HIDE);
    }


    void SetWindowTitle(WindowHandle window, const char* title)
    {
        SetWindowTextA(window->hWnd, title);
    }


    void GetWindowPosition(WindowHandle window, int &xPosOut, int &yPosOut)
    {
        RECT rect;
        GetWindowRect(window->hWnd, &rect);

        xPosOut = rect.left;
        yPosOut = rect.top;
    }

    void SetWindowPosition(WindowHandle window, int xPos, int yPos)
    {
        SetWindowPos(window->hWnd, nullptr, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    
    void GetWindowSize(WindowHandle window, unsigned int &widthOut, unsigned int &heightOut)
    {
        RECT rect;
        GetClientRect(window->hWnd, &rect);

        widthOut  = static_cast<unsigned int>(rect.right);
        heightOut = static_cast<unsigned int>(rect.bottom);
    }

    void SetWindowSize(WindowHandle window, unsigned int width, unsigned int height)
    {
        // TODO: Test this!

        RECT rect;
        rect.right  = width;
        rect.bottom = height;
        AdjustWindowRectEx(&rect, window->style, FALSE, window->exStyle);

        SetWindowPos(window->hWnd, nullptr, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }


    void SetWindowCursor(WindowHandle window, Cursor cursor)
    {
        window->cursor = cursor;

        // If the cursor is currently over the window, we want to switch the cursor immediately.
        if (Win32::IsCursorOverHWND(window->hWnd))
        {
            if (window->cursor != Cursor_None)
            {
                SetCursor(GetHCURSOR(window->cursor));
            }
            else
            {
                Win32::HideCursor();
            }
        }

        PostMessageA(window->hWnd, WM_SETCURSOR, 0, 0);
    }

    void ShowWindowCursor(WindowHandle window)
    {
        if ((window->flags & WindowCursorHidden) != 0)
        {
            window->flags &= ~WindowCursorHidden;

            if (Win32::IsCursorOverHWND(window->hWnd))
            {
                Win32::ShowCursor();
            }
        }
    }

    void HideWindowCursor(WindowHandle window)
    {
        if ((window->flags & WindowCursorHidden) == 0)
        {
            window->flags |= WindowCursorHidden;

            if (Win32::IsCursorOverHWND(window->hWnd))
            {
                Win32::HideCursor();
            }
        }
    }
}


#define THIS_HWND (this->iwo.hWnd)

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunreachable-code"
#endif

namespace GT
{
    Window::Window()
        : iwo()
    {
        this->iwo.hWnd = Win32::CreateHWND();
        this->iwo.hDC  = ::GetDC(THIS_HWND);

        ::SetPropW(THIS_HWND, L"__DeleteHandle", (HANDLE)1);
    }

    Window::Window(const InternalWindowObjects &iwo)
        : iwo(iwo)
    {
        if (iwo.hWnd == 0)
        {
            this->iwo.hWnd = Win32::CreateHWND();
            this->iwo.hDC  = ::GetDC(THIS_HWND);

            ::SetPropW(THIS_HWND, L"__DeleteHandle", (HANDLE)1);
        }
        else
        {
            if (iwo.hDC == 0)
            {
                this->iwo.hDC = ::GetDC(THIS_HWND);
            }
        }
    }

    Window::~Window()
    {
        if (::GetPropW(THIS_HWND, L"__DeleteHandle"))
        {
            Win32::DeleteHWND(THIS_HWND);
        }
    }

    WindowEventHandler & Window::GetEventHandler()
    {
        WindowEventHandler *result = (WindowEventHandler *)::GetPropW(THIS_HWND, L"__eventhandler");
        if (result == nullptr)
        {
            return WindowEventHandler::GetDefault();
        }

        return *result;
    }

    void Window::SetEventHandler(WindowEventHandler &handler)
    {
        ::SetPropW(THIS_HWND, L"__eventhandler", (HANDLE)&handler);
    }

    void Window::GetPosition(int &x, int &y)
    {
        RECT rect;
        GetWindowRect(THIS_HWND, &rect);

        x = rect.left;
        y = rect.top;
    }

    void Window::SetPosition(int x, int y)
    {
        SetWindowPos(THIS_HWND, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Window::GetSize(unsigned int &width, unsigned int &height)
    {
        RECT rect;
        GetClientRect(THIS_HWND, &rect);

        width  = (unsigned int)rect.right;
        height = (unsigned int)rect.bottom;
    }

    void Window::SetSize(unsigned int width, unsigned int height)
    {
        HWND hWnd = THIS_HWND;

        RECT window_rect;
        RECT client_rect;
        int window_width, window_height;

        GetWindowRect(hWnd, &window_rect);
        GetClientRect(hWnd, &client_rect);

        window_width  = (window_rect.right  - window_rect.left);
        window_height = (window_rect.bottom - window_rect.top);

        SetWindowPos(
            hWnd, nullptr, 0, 0,
            int(width)  + (window_width  - client_rect.right),
            int(height) + (window_height - client_rect.bottom),
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Window::SetTitle(const char *title)
    {
        SetWindowTextA(THIS_HWND, title);
    }

    void Window::Show(WindowShowType showType)
    {
        switch (showType)
        {
        case WindowShowType_Normal:    ::ShowWindow(THIS_HWND, SW_SHOWNORMAL);     break;
        case WindowShowType_Maximized: ::ShowWindow(THIS_HWND, SW_SHOWMAXIMIZED);  break;
        case WindowShowType_Minimized: ::ShowWindow(THIS_HWND, SW_MINIMIZE);       break;
        default: break;
        }
    }

    void Window::Redraw(int left, int top, int right, int bottom)
    {
        RECT newRect;
        newRect.left   = left;
        newRect.bottom = bottom;
        newRect.right  = right;
        newRect.top    = top;

        ::InvalidateRect(THIS_HWND, &newRect, true);
    }

    void Window::SetCursor(Cursor cursor)
    {
        Win32::SetCursor(THIS_HWND, cursor);
    }

    void Window::HideCursor()
    {
        Win32::HideCursor(THIS_HWND);
    }

    void Window::ShowCursor()
    {
        Win32::ShowCursor(THIS_HWND);
    }


    void Window::GetMousePosition(int &x, int &y)
    {
        POINT cursorPos;
        ::GetCursorPos(&cursorPos);
        ::ScreenToClient(THIS_HWND, &cursorPos);

        x = cursorPos.x;
        y = cursorPos.y;
    }

    void Window::SetMousePosition(int x, int y)
    {
        POINT cursorPos;
        cursorPos.x = x;
        cursorPos.y = y;

        ::ClientToScreen(THIS_HWND, &cursorPos);
        ::SetCursorPos(cursorPos.x, cursorPos.y);
    }


    void Window::EnableFullscreen(unsigned int width, unsigned int height)
    {
        if (!this->IsFullscreen())
        {
            // Here we are going to retrieve the rectangle of the window and save it as a property. These will be used when restoring the window back from fullscreen.
            int originalX;
            int originalY;
            unsigned int originalWidth;
            unsigned int originalHeight;

            this->GetPosition(originalX, originalY);
            this->GetSize(originalWidth, originalHeight);

            ::SetPropA(THIS_HWND, "__x",      reinterpret_cast<HANDLE>(originalX));
            ::SetPropA(THIS_HWND, "__y",      reinterpret_cast<HANDLE>(originalY));
            ::SetPropA(THIS_HWND, "__width",  reinterpret_cast<HANDLE>(originalWidth));
            ::SetPropA(THIS_HWND, "__height", reinterpret_cast<HANDLE>(originalHeight));


            // We want to set the WS_POPUP and WS_EX_TOPMOST styles.
            LONG style   = ::GetWindowLongA(THIS_HWND, GWL_STYLE);
            LONG exstyle = ::GetWindowLongA(THIS_HWND, GWL_EXSTYLE);

            style   &= ~WS_OVERLAPPEDWINDOW;
            style   |=  (LONG)WS_POPUP;
            exstyle |=  WS_EX_APPWINDOW | WS_EX_TOPMOST;


            DEVMODE fullscreenSettings;
            EnumDisplaySettings(NULL, 0, &fullscreenSettings);
            fullscreenSettings.dmPelsWidth  = static_cast<DWORD>(width);
            fullscreenSettings.dmPelsHeight = static_cast<DWORD>(height);
            fullscreenSettings.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;

            if (ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
            {
                ::SetWindowLongA(THIS_HWND, GWL_STYLE,   style);
                ::SetWindowLongA(THIS_HWND, GWL_EXSTYLE, exstyle);
                ::SetWindowPos(THIS_HWND, HWND_TOPMOST, 0, 0, static_cast<int>(width), static_cast<int>(height), SWP_SHOWWINDOW);

                ::SetPropA(THIS_HWND, "__fullscreen", (HANDLE)1);
            }
        }
    }

    void Window::DisableFullscreen()
    {
        if (this->IsFullscreen())
        {
            // We change the display settings first. This way, we can reset the size and position of the window correctly. If we do this after setting
            // the size/position, it isn't actually set properly.
            ChangeDisplaySettings(nullptr, CDS_RESET);

            // Here we will restore the window.
            LONG style   = ::GetWindowLongA(THIS_HWND, GWL_STYLE);
            LONG exstyle = ::GetWindowLongA(THIS_HWND, GWL_EXSTYLE);

            style   &= ~(LONG)WS_POPUP;
            style   |=  WS_OVERLAPPEDWINDOW;
            exstyle &= ~(WS_EX_APPWINDOW | WS_EX_TOPMOST);

            ::SetWindowLongA(THIS_HWND, GWL_STYLE,   style);
            ::SetWindowLongA(THIS_HWND, GWL_EXSTYLE, exstyle);

            int originalX               = (int)(ptrdiff_t)GetPropA(THIS_HWND, "__x");
            int originalY               = (int)(ptrdiff_t)GetPropA(THIS_HWND, "__y");
            unsigned int originalWidth  = (unsigned int)(size_t)GetPropA(THIS_HWND, "__width");
            unsigned int originalHeight = (unsigned int)(size_t)GetPropA(THIS_HWND, "__height");
            ::SetWindowPos(THIS_HWND, HWND_NOTOPMOST, originalX, originalY, static_cast<int>(originalWidth), static_cast<int>(originalHeight), SWP_SHOWWINDOW);

            ::SetPropA(THIS_HWND, "__fullscreen", (HANDLE)0);
        }
    }

    bool Window::IsFullscreen() const
    {
        return ::GetPropA(THIS_HWND, "__fullscreen") != 0;
    }
}

#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#endif
