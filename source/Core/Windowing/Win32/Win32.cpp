// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#include <GTEngine/Core/Windowing/Win32/Win32.hpp>
#include <GTEngine/Core/Window.hpp>
#include <GTEngine/Core/Vector.hpp>

#include <cstdio>

namespace GT
{
    namespace Win32
    {
        /// The list of handles for every created window.
        static Vector<HWND> WindowHandles;


        /// The class name of the window.
        static const wchar_t* WindowClass = L"GT_DefaultWindow_OLD";        // <-- TODO: What was this again? Why is it "old"? Need to look at this...

        /// The system cursor handles. These are initialised and uninitialised with the main window class.
        static HCURSOR Win32SystemCursors[Cursor_Count] = {0};

        /// Keeps track of whether or not the mouse cursor is visible.
        static bool IsCursorVisible = true;





        /**
        *   \brief  Converts a Windows mouse button event code to a GalickGUI mouse button.
        */
        MouseButton FromWin32MouseButtonMSGCode(UINT msg)
        {
            switch (msg)
            {
            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                {
                    return MouseButton_Left;
                }

            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                {
                    return MouseButton_Right;
                }

            case WM_MBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                {
                    return MouseButton_Middle;
                }

            default: break;
            }

            return MouseButton_Unknown;
        }

        Key FromWin32VirtualKey(WPARAM key)
        {
            // Easy cases for letters and numbers...
            if (key >= 'A' && key <= 'Z')
            {
                return (Key)key;
            }
            if (key >= '0' && key <= '9')
            {
                return (Key)key;
            }

            switch (key)
            {
            case VK_PAUSE:      return Keys::Pause;
            case VK_SCROLL:     return Keys::ScrollLock;

            case VK_BACK:       return Keys::Backspace;
            case VK_TAB:        return Keys::Tab;
            case VK_DELETE:     return Keys::Delete;
            case VK_RETURN:     return Keys::Return;
            case VK_SPACE:      return Keys::Space;
            case VK_ESCAPE:     return Keys::Escape;

            case VK_SHIFT:      return Keys::Shift;
            case VK_CONTROL:    return Keys::Ctrl;
            case VK_MENU:       return Keys::Alt;
            case VK_LWIN:       return Keys::Super;
            case VK_RWIN:       return Keys::Super;
            case VK_CAPITAL:    return Keys::CapsLock;

                /*
            case VK_LSHIFT:     return Keys::LeftShift;
            case VK_RSHIFT:     return Keys::RightShift;
            case VK_LCONTROL:   return Keys::LeftCtrl;
            case VK_RCONTROL:   return Keys::RightCtrl;
            case VK_LMENU:      return Keys::LeftAlt;
            case VK_RMENU:      return Keys::RightAlt;
                */

            case VK_LEFT:       return Keys::ArrowLeft;
            case VK_RIGHT:      return Keys::ArrowRight;
            case VK_UP:         return Keys::ArrowUp;
            case VK_DOWN:       return Keys::ArrowDown;

            case VK_F1:         return Keys::F1;
            case VK_F2:         return Keys::F2;
            case VK_F3:         return Keys::F3;
            case VK_F4:         return Keys::F4;
            case VK_F5:         return Keys::F5;
            case VK_F6:         return Keys::F6;
            case VK_F7:         return Keys::F7;
            case VK_F8:         return Keys::F8;
            case VK_F9:         return Keys::F9;
            case VK_F10:        return Keys::F10;
            case VK_F11:        return Keys::F11;
            case VK_F12:        return Keys::F12;
            case VK_F13:        return Keys::F13;
            case VK_F14:        return Keys::F14;
            case VK_F15:        return Keys::F15;
            case VK_F16:        return Keys::F16;
            case VK_F17:        return Keys::F17;
            case VK_F18:        return Keys::F18;
            case VK_F19:        return Keys::F19;
            case VK_F20:        return Keys::F20;
            case VK_F21:        return Keys::F21;
            case VK_F22:        return Keys::F22;
            case VK_F23:        return Keys::F23;
            case VK_F24:        return Keys::F24;


            case VK_END:        return Keys::End;
            case VK_HOME:       return Keys::Home;
            case VK_PRIOR:      return Keys::PageUp;
            case VK_NEXT:       return Keys::PageDown;


            case VK_SELECT:     return Keys::Select;
            case VK_PRINT:      return Keys::Print;
            case VK_EXECUTE:    return Keys::Execute;
            case VK_INSERT:     return Keys::Insert;
            // Keys::Undo
            // Keys::Redo
            // Keys::Menu
            // Keys::Find
            // Keys::Cancel
            case VK_HELP:       return Keys::Help;
            case VK_CANCEL:     return Keys::Break;
            // Keys::ModeSwitch
            case VK_NUMLOCK:    return Keys::NumLock;



            default: break;
            }

            return '\0';
        }

        /// Helper function for hiding the cursor at a global level.
        void HideCursor()
        {
            if (Win32::IsCursorVisible)
            {
                Win32::IsCursorVisible = false;
                while (::ShowCursor(FALSE) >= 0);
            }
        }

        /// Helper function for showing the cursor at a global level.
        void ShowCursor()
        {
            if (!Win32::IsCursorVisible)
            {
                Win32::IsCursorVisible = true;
                while (::ShowCursor(TRUE) < 0);
            }
        }

        /// The window procedure for the window.
        LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            WindowEventHandler *eventHandler = (WindowEventHandler *)::GetPropW(hWnd, L"__eventhandler");
            if (eventHandler == nullptr)
            {
                eventHandler = &WindowEventHandler::GetDefault();
            }

            switch (msg)
            {
            case WM_CLOSE:
                {
                    eventHandler->OnClose();
                    return 0;
                }

            case WM_SIZE:
                {
                    eventHandler->OnSize(LOWORD(lParam), HIWORD(lParam));
                    break;
                }

            case WM_MOVE:
                {
                    eventHandler->OnMove(LOWORD(lParam), HIWORD(lParam));
                    break;
                }

            case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hWnd, &ps);

                    eventHandler->OnPaint(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);

                    EndPaint(hWnd, &ps);
                    return 0;
                }

            case WM_NCMOUSEMOVE:
                {
                    POINT temp;
                    temp.x = LOWORD(lParam);
                    temp.y = HIWORD(lParam);
                    ScreenToClient(hWnd, &temp);

                    eventHandler->OnMouseMove(temp.x, temp.y);

                    return 0;
                }

            case WM_MOUSEMOVE:
                {
                    // The casting to signed 16-bit integers is important here! When the mouse is captured and then moves onto the non-client area, it will loop
                    // around to 65535. If we don't cast, we will lose the sign.
                    int16_t x = static_cast<int16_t>(LOWORD(lParam));
                    int16_t y = static_cast<int16_t>(HIWORD(lParam));
                    eventHandler->OnMouseMove(x, y);

                    return 0;
                }

            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
                {
                    SetCapture(hWnd);
                    eventHandler->OnMouseButtonDown(FromWin32MouseButtonMSGCode(msg), LOWORD(lParam), HIWORD(lParam));
                    break;
                }

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
                {
                    ReleaseCapture();
                    eventHandler->OnMouseButtonUp(FromWin32MouseButtonMSGCode(msg), LOWORD(lParam), HIWORD(lParam));
                    break;
                }

            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK:
                {
                    // Windows does not post a button down event for the second click when a double click event is posted. We want this.
                    MouseButton button = FromWin32MouseButtonMSGCode(msg);
                    WORD xPos = LOWORD(lParam);
                    WORD yPos = HIWORD(lParam);

                    eventHandler->OnMouseButtonDown(button, xPos, yPos);
                    eventHandler->OnMouseButtonDoubleClick(button, xPos, yPos);

                    break;
                }


            // Keyboard messages are a little complicated here because WM_KEYDOWN won't produce Unicode code points. Instead, unicode
            // characters are received in WM_CHAR. To work around this we will delay posting events for printable characters until WM_CHAR.
            //
            // Another issue here is that holding down the Ctrl key will cause the character codes for A, B, C, etc to change. It will change
            // the codes so that A = 1, B = 2, etc. To resolve this, we need to make sure those key codes are handled in WM_KEYDOWN instead
            // of WM_CHAR.

            case WM_KEYDOWN:
                {
                    // OnKeyDown() is a repeating message, whereas OnKeyPressed() is non-repeating. We actually post both of
                    // these messages from this event.

                    // First we need to convert the key.
                    Key key = FromWin32VirtualKey(wParam);

                    if (key == Keys::Ctrl)
                    {
                        ::SetPropW(hWnd, L"__ctrldown", (HANDLE)1);
                    }

                    if (key != Keys::Null)
                    {
                        // Don't post printable characters if the Ctrl key is up.
                        if (!IsKeyPrintable(key) || ::GetPropW(hWnd, L"__ctrldown") != 0)
                        {
                            // Only if the key was previously up do we want to post this message...
                            if ((lParam & (1 << 30)) == 0)
                            {
                                // The key was previously up - we'll post.
                                eventHandler->OnKeyPressed(key);
                            }

                            eventHandler->OnKeyDown(key, (lParam & 0xFFFF));
                        }
                    }

                    return 0;
                }

            case WM_KEYUP:
                {
                    Key key = FromWin32VirtualKey(wParam);

                    if (key == Keys::Ctrl)
                    {
                        ::SetPropW(hWnd, L"__ctrldown", 0);
                    }

                    eventHandler->OnKeyReleased(key);
                    eventHandler->OnKeyUp(key);

                    return 0;
                }

            case WM_CHAR:
                {
                    // Some keys will fall-through to this message even if they've been handled by WM_KEYDOWN. We absolutely do not
                    // want to post duplicate messages. We will filter them out here.
                    if (wParam != Keys::Backspace &&
                        wParam != Keys::Return    &&
                        wParam != Keys::Delete    &&
                        wParam != Keys::Escape    &&
                        wParam != Keys::Tab)
                    {
                        bool isCTRLDown = ::GetPropW(hWnd, L"__ctrldown") != 0;
                        if (!(isCTRLDown && wParam < 0x20))       // <-- This filters those annoying translations as a result of the CTRL key being pressed while on an ascii character.
                        {
                            if ((lParam & (1 << 31)) != 0)
                            {
                                eventHandler->OnKeyUp(static_cast<Key>(wParam));
                            }
                            else
                            {
                                // We need to check for a key press event. We're going to convert lower case characters to upper case.
                                if ((lParam & (1 << 30)) == 0)
                                {
                                    // The key was previously up - we'll post.
                                    WPARAM wParam_OnKeyPress = wParam;
                                    if (wParam >= 'a' && wParam <= 'z')
                                    {
                                        wParam_OnKeyPress -= 0x20;
                                    }

                                    eventHandler->OnKeyPressed(static_cast<Key>(wParam_OnKeyPress));
                                }

                                eventHandler->OnKeyDown(static_cast<Key>(wParam), (lParam & 0xFFFF));
                            }
                        }
                    }

                    return 0;
                }


            case WM_SETFOCUS:
                {
                    eventHandler->OnReceiveFocus();
                    break;
                }

            case WM_KILLFOCUS:
                {
                    eventHandler->OnLoseFocus();
                    break;
                }

            case WM_SETCURSOR:
                {
                    bool visible = ::GetPropW(hWnd, L"__cursorvisible") != 0;
                    if (visible)
                    {
                        HCURSOR cursor = (HCURSOR)::GetPropW(hWnd, L"__cursor");
                        if (cursor == nullptr)
                        {
                            Win32::HideCursor();
                        }
                        else
                        {
                            ::SetCursor(cursor);
                            Win32::ShowCursor();
                        }
                    }
                    else
                    {
                        Win32::HideCursor();
                    }

                    break;
                }

            case WM_MOUSEWHEEL:
                {
                    POINT temp;
                    temp.x = LOWORD(lParam);
                    temp.y = HIWORD(lParam);
                    ScreenToClient(hWnd, &temp);

                    eventHandler->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / 120, temp.x, temp.y);
                    return 0;
                }

            default: break;
            }

            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        HWND CreateHWND()
        {
            // If the window count is zero we need to register the window class.
            if (WindowHandles.count == 0)
            {
                // We need cursors.
                Win32SystemCursors[Cursor_Arrow]                = ::LoadCursorA(nullptr, IDC_ARROW);
                Win32SystemCursors[Cursor_Beam]                 = ::LoadCursorA(nullptr, IDC_IBEAM);
                Win32SystemCursors[Cursor_Hand]                 = ::LoadCursorA(nullptr, IDC_HAND);
                Win32SystemCursors[Cursor_Cross]                = ::LoadCursorA(nullptr, IDC_CROSS);

                Win32SystemCursors[Cursor_VertDoubleArrow]      = ::LoadCursorA(nullptr, IDC_SIZENS);
                Win32SystemCursors[Cursor_HorzDoubleArrow]      = ::LoadCursorA(nullptr, IDC_SIZEWE);

                Win32SystemCursors[Cursor_SizeArrowTop]         = ::LoadCursorA(nullptr, IDC_SIZENS);
                Win32SystemCursors[Cursor_SizeArrowBottom]      = ::LoadCursorA(nullptr, IDC_SIZENS);
                Win32SystemCursors[Cursor_SizeArrowLeft]        = ::LoadCursorA(nullptr, IDC_SIZEWE);
                Win32SystemCursors[Cursor_SizeArrowRight]       = ::LoadCursorA(nullptr, IDC_SIZEWE);
                Win32SystemCursors[Cursor_SizeArrowTopLeft]     = ::LoadCursorA(nullptr, IDC_SIZENWSE);
                Win32SystemCursors[Cursor_SizeArrowTopRight]    = ::LoadCursorA(nullptr, IDC_SIZENESW);
                Win32SystemCursors[Cursor_SizeArrowBottomLeft]  = ::LoadCursorA(nullptr, IDC_SIZENWSE);
                Win32SystemCursors[Cursor_SizeArrowBottomRight] = ::LoadCursorA(nullptr, IDC_SIZENESW);


                //Win32SystemCursors[SystemCursor_SizeNS  ] = ::LoadCursorA(nullptr, IDC_SIZENS);
                //Win32SystemCursors[SystemCursor_SizeWE  ] = ::LoadCursorA(nullptr, IDC_SIZEWE);
                //Win32SystemCursors[SystemCursor_SizeNESW] = ::LoadCursorA(nullptr, IDC_SIZENESW);
                //Win32SystemCursors[SystemCursor_SizeNWSE] = ::LoadCursorA(nullptr, IDC_SIZENWSE);


                WNDCLASSEXW wc;
                memset(&wc, 0, sizeof(wc));
                wc.cbSize        = sizeof(wc);
                wc.cbWndExtra    = sizeof(void *);
                wc.lpfnWndProc   = (WNDPROC)WindowProc;
                wc.lpszClassName = WindowClass;
                wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
                if (!RegisterClassExW(&wc))
                {
                    return nullptr;
                }
            }

            // We can create the window when we have a class.
            HWND hWnd = CreateWindowExW(0, WindowClass, L"GTGUI Application",
                                        WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
                                        0, 0, 0, 0, nullptr, nullptr, ::GetModuleHandle(nullptr), nullptr);

            Win32::SetCursor(hWnd, Cursor_Arrow);
            Win32::ShowCursor(hWnd);

            WindowHandles.PushBack(hWnd);

            return hWnd;
        }

        void DeleteHWND(HWND hWnd)
        {
            // Before destroying our window, we need to remove all of our properties.
            ::RemovePropW(hWnd, L"__eventhandler");
            ::RemovePropW(hWnd, L"__cursorvisible");
            ::RemovePropW(hWnd, L"__cursor");
            ::RemovePropW(hWnd, L"__ctrldown");

            ::DestroyWindow(hWnd);

            WindowHandles.RemoveFirstOccuranceOf(hWnd);

            // Here we decrement our window count and unregister the window class, if necessary.
            if (WindowHandles.count == 0)
            {
                UnregisterClassW(WindowClass, ::GetModuleHandle(nullptr));
            }
        }

        void SetCursor(HWND hWnd, Cursor cursor)
        {
            // We are going to check if the cursor is over the given window and do an immediate switch.
            if (IsCursorOverHWND(hWnd))
            {
                if (cursor == Cursor_None)
                {
                    Win32::HideCursor();
                }
                else
                {
                    ::SetCursor(Win32SystemCursors[cursor]);
                    Win32::ShowCursor();
                }
            }

            ::SetPropW(hWnd, L"__cursor", (HANDLE)Win32SystemCursors[cursor]);
            ::PostMessageA(hWnd, WM_SETCURSOR, 0, 0);
        }

        void HideCursor(HWND hWnd)
        {
            if (IsCursorOverHWND(hWnd))
            {
                Win32::HideCursor();
            }

            ::SetPropW(hWnd, L"__cursorvisible", (HANDLE)0);
            ::PostMessageA(hWnd, WM_SETCURSOR, 0, 0);
        }

        void ShowCursor(HWND hWnd)
        {
            if (IsCursorOverHWND(hWnd))
            {
                Win32::ShowCursor();
            }

            ::SetPropW(hWnd, L"__cursorvisible", (HANDLE)1);
            ::PostMessageA(hWnd, WM_SETCURSOR, 0, 0);
        }

        bool IsCursorOverHWND(HWND hWnd)
        {
            RECT windowRect;
            ::GetWindowRect(hWnd, &windowRect);

            POINT cursorPos;
            ::GetCursorPos(&cursorPos);

            return windowRect.left <= cursorPos.x && windowRect.right  > cursorPos.x &&
                   windowRect.top  <= cursorPos.y && windowRect.bottom > cursorPos.y;
        }

        HWND GetFirstHWND()
        {
            if (WindowHandles.count > 0)
            {
                return WindowHandles[0];
            }

            return 0;
        }
    }
}

namespace GT
{
    extern MSG g_MSG;
    bool PumpNextWindowEvent(bool wait)
    {
        if (wait)
        {
            if (GetMessageA(&g_MSG, nullptr, 0, 0) != 0)
            {
                TranslateMessage(&g_MSG);
                DispatchMessageA(&g_MSG);
            }
        }
        else
        {
            if (PeekMessageA(&g_MSG, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&g_MSG);
                DispatchMessageA(&g_MSG);
            }
        }

        // We need to return true if there is another event waiting for us.
        return PeekMessageA(&g_MSG, nullptr, 0, 0, PM_NOREMOVE) != 0;
    }
}

#endif
