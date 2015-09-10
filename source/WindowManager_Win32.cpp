// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/WindowManager_Win32.hpp>
#include <cassert>

// Normally we would include the two files below, but of course they are not in the latest build
// of MinGW at the time of writing. Thus, we need to just copy the structures from the MSDN
// documentation until MinGW adds these files.
#if 0
#include <VersionHelpers.h>
#include <ShellScalingApi.h>
#else
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
typedef enum PROCESS_DPI_AWARENESS {
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

#define VERSIONHELPERAPI inline bool

VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
        VerSetConditionMask(
            0, VER_MAJORVERSION, VER_GREATER_EQUAL),
               VER_MINORVERSION, VER_GREATER_EQUAL),
               VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

VERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0);
}
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif
#endif


#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
#endif


#define CURSOR_SUPPRESSED           0x00000002

namespace GT
{
    static const wchar_t* g_WindowClass      = L"GT_WndClass";
    static const wchar_t* g_WindowClassPopup = L"GT_WndClass_Popup";

    static const size_t WindowFlag_IsCursorInside = (1 << 0);

    #define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
    #define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

    typedef BOOL    (__stdcall * PFN_SetProcessDPIAware)     (void);
    typedef HRESULT (__stdcall * PFN_SetProcessDpiAwareness) (PROCESS_DPI_AWARENESS);
    typedef HRESULT (__stdcall * PFN_GetDpiForMonitor)       (HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);


    /// Structure for passing information to EnumDisplayMonitors()
    struct WindowEnumData
    {
        WindowEnumData()
            : targetIndex(0),
              currentIndex(0),
              hMonitor(NULL)
        {
        }

        /// The target iteration index.
        int targetIndex;

        /// The current iteration index.
        int currentIndex;

        /// A handle to the monitor.
        HMONITOR hMonitor;
    };

    BOOL CALLBACK GetMonitorByIndexCallback(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData)
    {
        WindowEnumData* pData = reinterpret_cast<WindowEnumData*>(dwData);
        if (pData != nullptr)
        {
            if (pData->targetIndex == pData->currentIndex)
            {
                pData->hMonitor = hMonitor;
                return false;
            }


            pData->currentIndex += 1;
            return true;
        }

        return false;
    }

    HMONITOR GetMonitorByIndex(int monitorIndex)
    {
        WindowEnumData data;
        data.targetIndex = monitorIndex;
        EnumDisplayMonitors(NULL, NULL, GetMonitorByIndexCallback, reinterpret_cast<LPARAM>(&data));

        return data.hMonitor;
    }


    void TrackMouseLeaveEvent(HWND hWnd)
    {
        TRACKMOUSEEVENT tme;
        ZeroMemory(&tme, sizeof(tme));
        tme.cbSize    = sizeof(tme);
        tme.dwFlags   = TME_LEAVE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
    }


    GTLib::Key FromWin32VirtualKey(WPARAM key)
    {
        // Easy cases for letters and numbers...
        if (key >= 'A' && key <= 'Z')
        {
            return static_cast<GTLib::Key>(key);
        }
        if (key >= '0' && key <= '9')
        {
            return static_cast<GTLib::Key>(key);
        }

        switch (key)
        {
        case VK_PAUSE:      return GTLib::Keys::Pause;
        case VK_SCROLL:     return GTLib::Keys::ScrollLock;

        case VK_BACK:       return GTLib::Keys::Backspace;
        case VK_TAB:        return GTLib::Keys::Tab;
        case VK_DELETE:     return GTLib::Keys::Delete;
        case VK_RETURN:     return GTLib::Keys::Return;
        case VK_SPACE:      return GTLib::Keys::Space;
        case VK_ESCAPE:     return GTLib::Keys::Escape;

        case VK_SHIFT:      return GTLib::Keys::Shift;
        case VK_CONTROL:    return GTLib::Keys::Ctrl;
        case VK_MENU:       return GTLib::Keys::Alt;
        case VK_LWIN:       return GTLib::Keys::Super;
        case VK_RWIN:       return GTLib::Keys::Super;
        case VK_CAPITAL:    return GTLib::Keys::CapsLock;

            /*
        case VK_LSHIFT:     return GTLib::Keys::LeftShift;
        case VK_RSHIFT:     return GTLib::Keys::RightShift;
        case VK_LCONTROL:   return GTLib::Keys::LeftCtrl;
        case VK_RCONTROL:   return GTLib::Keys::RightCtrl;
        case VK_LMENU:      return GTLib::Keys::LeftAlt;
        case VK_RMENU:      return GTLib::Keys::RightAlt;
            */

        case VK_LEFT:       return GTLib::Keys::ArrowLeft;
        case VK_RIGHT:      return GTLib::Keys::ArrowRight;
        case VK_UP:         return GTLib::Keys::ArrowUp;
        case VK_DOWN:       return GTLib::Keys::ArrowDown;

        case VK_F1:         return GTLib::Keys::F1;
        case VK_F2:         return GTLib::Keys::F2;
        case VK_F3:         return GTLib::Keys::F3;
        case VK_F4:         return GTLib::Keys::F4;
        case VK_F5:         return GTLib::Keys::F5;
        case VK_F6:         return GTLib::Keys::F6;
        case VK_F7:         return GTLib::Keys::F7;
        case VK_F8:         return GTLib::Keys::F8;
        case VK_F9:         return GTLib::Keys::F9;
        case VK_F10:        return GTLib::Keys::F10;
        case VK_F11:        return GTLib::Keys::F11;
        case VK_F12:        return GTLib::Keys::F12;
        case VK_F13:        return GTLib::Keys::F13;
        case VK_F14:        return GTLib::Keys::F14;
        case VK_F15:        return GTLib::Keys::F15;
        case VK_F16:        return GTLib::Keys::F16;
        case VK_F17:        return GTLib::Keys::F17;
        case VK_F18:        return GTLib::Keys::F18;
        case VK_F19:        return GTLib::Keys::F19;
        case VK_F20:        return GTLib::Keys::F20;
        case VK_F21:        return GTLib::Keys::F21;
        case VK_F22:        return GTLib::Keys::F22;
        case VK_F23:        return GTLib::Keys::F23;
        case VK_F24:        return GTLib::Keys::F24;


        case VK_END:        return GTLib::Keys::End;
        case VK_HOME:       return GTLib::Keys::Home;
        case VK_PRIOR:      return GTLib::Keys::PageUp;
        case VK_NEXT:       return GTLib::Keys::PageDown;


        case VK_SELECT:     return GTLib::Keys::Select;
        case VK_PRINT:      return GTLib::Keys::Print;
        case VK_EXECUTE:    return GTLib::Keys::Execute;
        case VK_INSERT:     return GTLib::Keys::Insert;
        // GTLib::Keys::Undo
        // GTLib::Keys::Redo
        // GTLib::Keys::Menu
        // GTLib::Keys::Find
        // GTLib::Keys::Cancel
        case VK_HELP:       return GTLib::Keys::Help;
        case VK_CANCEL:     return GTLib::Keys::Break;
        // GTLib::Keys::ModeSwitch
        case VK_NUMLOCK:    return GTLib::Keys::NumLock;



        default: break;
        }

        return '\0';
    }


    int ToWin32VirtualKey(GTLib::Key key)
    {
        // Easy cases for letters and numbers...
        if (key >= GTLib::Keys::A && key <= GTLib::Keys::Z)
        {
            return static_cast<int>(key);
        }
        if (key >= '0' && key <= '9')
        {
            return static_cast<int>(key);
        }

        switch (key)
        {
        case GTLib::Keys::Pause:    return VK_PAUSE;
        case GTLib::Keys::ScrollLock:return VK_SCROLL;

        case GTLib::Keys::Backspace:return VK_BACK;
        case GTLib::Keys::Tab:return VK_TAB;
        case GTLib::Keys::Delete:return VK_DELETE;
        case GTLib::Keys::Return:return VK_RETURN;
        case GTLib::Keys::Space:return VK_SPACE;
        case GTLib::Keys::Escape:return VK_ESCAPE;

        case GTLib::Keys::Shift:return VK_SHIFT;
        case GTLib::Keys::Ctrl:return VK_CONTROL;
        case GTLib::Keys::Alt:return VK_MENU;
        case GTLib::Keys::Super:return VK_LWIN;
        case GTLib::Keys::CapsLock:return VK_CAPITAL;

            /*
        case GTLib::Keys::LeftShift:return VK_LSHIFT;
        case GTLib::Keys::RightShift:return VK_RSHIFT;
        case GTLib::Keys::LeftCtrl:return VK_LCONTROL;
        case GTLib::Keys::RightCtrl:return VK_RCONTROL;
        case GTLib::Keys::LeftAlt:return VK_LMENU;
        case GTLib::Keys::RightAlt:return VK_RMENU;
            */

        case GTLib::Keys::ArrowLeft:return VK_LEFT;
        case GTLib::Keys::ArrowRight:return VK_RIGHT;
        case GTLib::Keys::ArrowUp:return VK_UP;
        case GTLib::Keys::ArrowDown:return VK_DOWN;

        case GTLib::Keys::F1:return VK_F1;
        case GTLib::Keys::F2:return VK_F2;
        case GTLib::Keys::F3:return VK_F3;
        case GTLib::Keys::F4:return VK_F4;
        case GTLib::Keys::F5:return VK_F5;
        case GTLib::Keys::F6:return VK_F6;
        case GTLib::Keys::F7:return VK_F7;
        case GTLib::Keys::F8:return VK_F8;
        case GTLib::Keys::F9:return VK_F9;
        case GTLib::Keys::F10:return VK_F10;
        case GTLib::Keys::F11:return VK_F11;
        case GTLib::Keys::F12:return VK_F12;
        case GTLib::Keys::F13:return VK_F13;
        case GTLib::Keys::F14:return VK_F14;
        case GTLib::Keys::F15:return VK_F15;
        case GTLib::Keys::F16:return VK_F16;
        case GTLib::Keys::F17:return VK_F17;
        case GTLib::Keys::F18:return VK_F18;
        case GTLib::Keys::F19:return VK_F19;
        case GTLib::Keys::F20:return VK_F20;
        case GTLib::Keys::F21:return VK_F21;
        case GTLib::Keys::F22:return VK_F22;
        case GTLib::Keys::F23:return VK_F23;
        case GTLib::Keys::F24:return VK_F24;


        case GTLib::Keys::End:return VK_END;
        case GTLib::Keys::Home:return VK_HOME;
        case GTLib::Keys::PageUp:return VK_PRIOR;
        case GTLib::Keys::PageDown:return VK_NEXT;


        case GTLib::Keys::Select:return VK_SELECT;
        case GTLib::Keys::Print:return VK_PRINT;
        case GTLib::Keys::Execute:return VK_EXECUTE;
        case GTLib::Keys::Insert:return VK_INSERT;
        // GTLib::Keys::Undo
        // GTLib::Keys::Redo
        // GTLib::Keys::Menu
        // GTLib::Keys::Find
        // GTLib::Keys::Cancel
        case GTLib::Keys::Help:return VK_HELP;
        case GTLib::Keys::Break:return VK_CANCEL;
        // GTLib::Keys::ModeSwitch
        case GTLib::Keys::NumLock:return VK_NUMLOCK;



        default: break;
        }

        return 0;
    }


    bool IsWin32MouseButtonKeyCode(WPARAM wParam)
    {
        return wParam == VK_LBUTTON || wParam == VK_RBUTTON || wParam == VK_MBUTTON || wParam == VK_XBUTTON1 || wParam == VK_XBUTTON2;
    }



    LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WindowManager_Win32::WindowData* pWindowData = reinterpret_cast<WindowManager_Win32::WindowData*>(GetWindowLongPtr(hWnd, 0));
        if (pWindowData != nullptr)
        {
            WindowManager_Win32* pWindowManager = pWindowData->pWindowManager;

            switch (msg)
            {
            case WM_CREATE:
                {
                    TrackMouseLeaveEvent(hWnd);
                    return 0;
                }


            case WM_ERASEBKGND:
                {
                    return 1;       //< Never draw the background of the window - always leave that the engine's native GUI system.
                }


            case WM_CLOSE:
                {
                    pWindowManager->OnClose();
                    return 0;
                }

            case WM_MOVE:
                {
                    // All child popup windows need to be repositions so that their positions stay relative.
                    GTLib::Vector<HWindow> &windows = pWindowManager->_GetWindows();
                    for (size_t i = 0; i < windows.GetCount(); ++i)
                    {
                        WindowManager_Win32::WindowData* pPopupWindowData = reinterpret_cast<WindowManager_Win32::WindowData*>(::GetWindowLongPtr(reinterpret_cast<HWND>(windows[i]), 0));
                        if (pPopupWindowData != nullptr)
                        {
                            if (pPopupWindowData->type == WindowType::PopupWindow)
                            {
                                pWindowManager->_RefreshPopupPosition(windows[i]);
                            }
                        }
                    }

                    pWindowManager->OnMove(reinterpret_cast<HWindow>(hWnd), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }

            case WM_SIZE:
                {
                    pWindowManager->OnSize(reinterpret_cast<HWindow>(hWnd), LOWORD(lParam), HIWORD(lParam));
                    break;
                }


            case WM_MOUSEMOVE:
                {
                    if ((pWindowData->flags & WindowFlag_IsCursorInside) == 0)
                    {
                        TrackMouseLeaveEvent(hWnd);
                        pWindowData->flags |= WindowFlag_IsCursorInside;

                        pWindowManager->OnMouseEnter(reinterpret_cast<HWindow>(hWnd));
                    }

                    pWindowManager->OnMouseMove(reinterpret_cast<HWindow>(hWnd), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }

            case WM_MOUSELEAVE:
                {
                    pWindowData->flags &= ~WindowFlag_IsCursorInside;

                    pWindowManager->OnMouseLeave(reinterpret_cast<HWindow>(hWnd));
                    break;
                }



            case WM_NCLBUTTONDOWN:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, p.x, p.y);
                    break;
                }

            case WM_NCLBUTTONUP:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, p.x, p.y);
                    break;
                }

            case WM_NCLBUTTONDBLCLK:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, p.x, p.y);
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, p.x, p.y);
                    break;
                }

            case WM_LBUTTONDOWN:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_LBUTTONUP:
                {
                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_LBUTTONDBLCLK:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }



            case WM_NCRBUTTONDOWN:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, p.x, p.y);
                    break;
                }

            case WM_NCRBUTTONUP:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, p.x, p.y);
                    break;
                }

            case WM_NCRBUTTONDBLCLK:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, p.x, p.y);
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, p.x, p.y);
                    break;
                }

            case WM_RBUTTONDOWN:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_RBUTTONUP:
                {
                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_RBUTTONDBLCLK:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }


            case WM_NCMBUTTONDOWN:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, p.x, p.y);
                    break;
                }

            case WM_NCMBUTTONUP:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, p.x, p.y);
                    break;
                }

            case WM_NCMBUTTONDBLCLK:
                {
                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, p.x, p.y);
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, p.x, p.y);
                    break;
                }

            case WM_MBUTTONDOWN:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_MBUTTONUP:
                {
                    pWindowManager->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_MBUTTONDBLCLK:
                {
                    pWindowManager->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    pWindowManager->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }



            case WM_MOUSEWHEEL:
                {
                    int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

                    POINT p;
                    p.x = GET_X_LPARAM(lParam);
                    p.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &p);

                    pWindowManager->OnMouseWheel(reinterpret_cast<HWindow>(hWnd), delta, p.x, p.y);
                    break;
                }



            case WM_KEYDOWN:
                {
                    if (!IsWin32MouseButtonKeyCode(wParam))
                    {
                        if ((lParam & (1 << 30)) == 0)  // No auto-repeat.
                        {
                            pWindowManager->OnKeyPressed(reinterpret_cast<HWindow>(hWnd), FromWin32VirtualKey(wParam));
                        }
                    }

                    break;
                }

            case WM_KEYUP:
                {
                    if (!IsWin32MouseButtonKeyCode(wParam))
                    {
                        pWindowManager->OnKeyReleased(reinterpret_cast<HWindow>(hWnd), FromWin32VirtualKey(wParam));
                    }

                    break;
                }

                // NOTE: WM_UNICHAR is not posted by Windows itself, but rather intended to be posted by applications. Thus, we need to use WM_CHAR. WM_CHAR
                //       posts events as UTF-16 code points. When the code point is a surrogate pair, we need to store it and wait for the next WM_CHAR event
                //       which will contain the other half of the pair.
            case WM_CHAR:
                {
                    if ((lParam & (1U << 31)) == 0)     // Bit 31 will be 1 if the key was pressed, 0 if it was released.
                    {
                        if (IS_HIGH_SURROGATE(wParam))
                        {
                            assert(pWindowData->utf16HighSurrogate == 0);
                            pWindowData->utf16HighSurrogate = char16_t(wParam);
                        }
                        else
                        {
                            char32_t character = char32_t(wParam);
                            if (IS_LOW_SURROGATE(wParam))
                            {
                                assert(IS_HIGH_SURROGATE(pWindowData->utf16HighSurrogate) != 0);
                                character = GTLib::Strings::UTF16PairToUTF32(pWindowData->utf16HighSurrogate, char16_t(wParam));
                            }

                            pWindowData->utf16HighSurrogate = 0;


                            int repeatCount = lParam & 0x0000FFFF;
                            for (int i = 0; i < repeatCount; ++i)
                            {
                                pWindowManager->OnPrintableKeyDown(reinterpret_cast<HWindow>(hWnd), character);
                            }
                        }
                    }

                    break;
                }


            case WM_PAINT:
                {
                    RECT rectWin32;
                    if (GetUpdateRect(hWnd, &rectWin32, FALSE))
                    {
                        GTLib::Rect<int> rect;
                        rect.left   = rectWin32.left;
                        rect.right  = rectWin32.right;
                        rect.top    = rectWin32.top;
                        rect.bottom = rectWin32.bottom;
                        pWindowManager->OnPaintWindow(reinterpret_cast<HWindow>(hWnd), rect);
                    }

                    break;
                }

            case WM_NCACTIVATE:
                {
                    GTLib::Vector<HWindow> &windows = pWindowManager->_GetWindows();
                    BOOL keepActive = static_cast<BOOL>(wParam);
                    BOOL syncOthers = TRUE;

#if 1
                    for (size_t i = 0; i < windows.GetCount(); ++i)
                    {
                        if (windows[i] == static_cast<HWindow>(lParam))
                        {
                            keepActive = TRUE;
                            syncOthers = FALSE;

                            break;
                        }
                    }
#endif

                    if (lParam == -1)
                    {
                        return DefWindowProc(hWnd, msg, keepActive, 0);
                    }

                    if (syncOthers)
                    {
                        for (size_t i = 0; i < windows.GetCount(); ++i)
                        {
                            if (reinterpret_cast<HWindow>(hWnd) != windows[i] && hWnd != reinterpret_cast<HWND>(lParam))
                            {
                                SendMessage(reinterpret_cast<HWND>(windows[i]), msg, keepActive, -1);
                            }
                        }
                    }

                    return DefWindowProc(hWnd, msg, keepActive, lParam);
                }

            case WM_ACTIVATE:
                {
                    HWND hActivatedWnd   = NULL;
                    HWND hDeactivatedWnd = NULL;

                    if (LOWORD(wParam) != WA_INACTIVE)
                    {
                        // Activated.
                        hActivatedWnd   = hWnd;
                        hDeactivatedWnd = reinterpret_cast<HWND>(lParam);
                    }
                    else
                    {
                        // Deactivated.
                        hActivatedWnd   = reinterpret_cast<HWND>(lParam);
                        hDeactivatedWnd = hWnd;
                    }

                    bool isActivatedWindowOwnedByThis   = pWindowManager->IsWindowOwnedByThis(hActivatedWnd);
                    bool isDeactivatedWindowOwnedByThis = pWindowManager->IsWindowOwnedByThis(hDeactivatedWnd);

                    if (isActivatedWindowOwnedByThis && isDeactivatedWindowOwnedByThis)
                    {
                        // Both windows are owned the by application.

                        if (LOWORD(wParam) != WA_INACTIVE)
                        {
                            hActivatedWnd   = pWindowManager->GetTopLevelPrimaryWindow(hActivatedWnd);
                            hDeactivatedWnd = pWindowManager->GetTopLevelPrimaryWindow(hDeactivatedWnd);

                            if (hActivatedWnd != hDeactivatedWnd)
                            {
                                if (hDeactivatedWnd != NULL)
                                {
                                    pWindowManager->OnWindowDeactivated(reinterpret_cast<HWindow>(hDeactivatedWnd));
                                }

                                if (hActivatedWnd != NULL)
                                {
                                    pWindowManager->OnWindowActivated(reinterpret_cast<HWindow>(hActivatedWnd));
                                }
                            }
                        }
                    }
                    else
                    {
                        // The windows are not both owned by this manager.

                        if (isDeactivatedWindowOwnedByThis)
                        {
                            hDeactivatedWnd = pWindowManager->GetTopLevelPrimaryWindow(hDeactivatedWnd);
                            if (hDeactivatedWnd != NULL)
                            {
                                pWindowManager->OnWindowDeactivated(reinterpret_cast<HWindow>(hDeactivatedWnd));
                            }
                        }

                        if (isActivatedWindowOwnedByThis)
                        {
                            hActivatedWnd = pWindowManager->GetTopLevelPrimaryWindow(hActivatedWnd);
                            if (hActivatedWnd != NULL)
                            {
                                pWindowManager->OnWindowActivated(reinterpret_cast<HWindow>(hActivatedWnd));
                            }
                        }
                    }


                    break;
                }


            case WM_SETCURSOR:
                {
                    if (LOWORD(lParam) == HTCLIENT)
                    {
                        SetCursor(pWindowData->hCursor);
                        return TRUE;
                    }

                    break;
                }


            default:
                {
                    break;
                }
            }
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }



    WindowManager_Win32::WindowManager_Win32()
        : WindowManager(),
          m_hUser32DLL(NULL),
          m_hSHCoreDLL(NULL)
    {
        bool fallBackToDiscourageDPIAPI = false;

        // The application should be DPI aware.
        if (IsWindows8Point1OrGreater())
        {
            // We can't call SetProcessDpiAwareness() directly because otherwise on versions of Windows < 8.1 we'll get an error at load time about
            // a missing DLL.
            m_hSHCoreDLL = ::LoadLibraryW(L"shcore.dll");   // <-- This is freed in the destructor.
            if (m_hSHCoreDLL != nullptr)
            {
                PFN_SetProcessDpiAwareness _SetProcessDpiAwareness = reinterpret_cast<PFN_SetProcessDpiAwareness>(::GetProcAddress(m_hSHCoreDLL, "SetProcessDpiAwareness"));
                if (_SetProcessDpiAwareness != nullptr)
                {
                    _SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
                }
                else
                {
                    // Couldn't find SetProcessDpiAwareness() so fall back to the discouraged way.
                    fallBackToDiscourageDPIAPI = true;
                }
            }
            else
            {
                // Couldn't find shcore.dll so fall back to the discouraged way.
                fallBackToDiscourageDPIAPI = true;
            }
        }
        else
        {
            // Not running at least Windows 8.1 so fall back to the discouraged way.
            fallBackToDiscourageDPIAPI = true;
        }


        if (fallBackToDiscourageDPIAPI)
        {
            m_hUser32DLL = ::LoadLibraryW(L"user32.dll");   // <-- This is freed in the destructor.
            if (m_hUser32DLL != NULL)
            {
                PFN_SetProcessDPIAware _SetProcessDPIAware = reinterpret_cast<PFN_SetProcessDPIAware>(::GetProcAddress(m_hUser32DLL, "SetProcessDPIAware"));
                if (_SetProcessDPIAware != NULL) {
                    _SetProcessDPIAware();
                }
            }
        }



        // Window classes need to be registered.
        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = sizeof(void*);
        wc.lpfnWndProc   = reinterpret_cast<WNDPROC>(DefaultWindowProcWin32);
        wc.lpszClassName = g_WindowClass;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.style         = CS_OWNDC | CS_DBLCLKS;
        if (!::RegisterClassExW(&wc))
        {
            // Failed to register window class.
            assert(false);
        }


        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = sizeof(void*);
        wc.lpfnWndProc   = reinterpret_cast<WNDPROC>(DefaultWindowProcWin32);
        wc.lpszClassName = g_WindowClassPopup;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.style         = CS_OWNDC | CS_DBLCLKS | CS_DROPSHADOW;
        if (!::RegisterClassExW(&wc))
        {
            // Failed to register window class.
            assert(false);
        }
    }

    WindowManager_Win32::~WindowManager_Win32()
    {
        ::UnregisterClassW(g_WindowClass, GetModuleHandleW(NULL));

        if (m_hSHCoreDLL != NULL) {
            ::FreeLibrary(m_hSHCoreDLL);
        }

        if (m_hUser32DLL != NULL) {
            ::FreeLibrary(m_hUser32DLL);
        }
    }


    HWindow WindowManager_Win32::CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        const wchar_t* windowClass = g_WindowClass;
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
                windowClass = g_WindowClassPopup;
                dwStyle |= WS_POPUP;
                break;
            }

        default:
            {
                assert(false);
            }
        }


        HWND hWnd = ::CreateWindowExW(dwExStyle, windowClass, L"GTGameEngine", dwStyle, xPos, yPos, width, height, reinterpret_cast<HWND>(hParent), NULL, NULL, nullptr);
        if (hWnd != NULL)
        {
            // Set the auxilliary window data to help in processing some window events.
            WindowData* pWindowData = new WindowData;
            pWindowData->pWindowManager     = this;
            pWindowData->relativePosX       = xPos;
            pWindowData->relativePosY       = yPos;
            pWindowData->type               = type;
            pWindowData->flags              = 0;
            pWindowData->hCursor            = LoadCursor(NULL, IDC_ARROW);
            pWindowData->utf16HighSurrogate = 0;
            ::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pWindowData));


            // The size of the window needs to be adjusted so that the client area is set to the width and height. We also need to adjust for DPI scaling.
            unsigned int xDPIBase;
            unsigned int yDPIBase;
            this->GetBaseDPI(xDPIBase, yDPIBase);

            unsigned int windowXDPI;
            unsigned int windowYDPI;
            this->GetWindowDPI(reinterpret_cast<HWindow>(hWnd), windowXDPI, windowYDPI);

            double scaleX = static_cast<double>(windowXDPI) / static_cast<double>(xDPIBase);
            double scaleY = static_cast<double>(windowYDPI) / static_cast<double>(yDPIBase);

            int scaledWidth  = static_cast<int>(width  * scaleX);
            int scaledHeight = static_cast<int>(height * scaleY);

            if (type == WindowType::PrimaryWindow)
            {
                RECT windowRect;
                RECT clientRect;
                ::GetWindowRect(hWnd, &windowRect);
                ::GetClientRect(hWnd, &clientRect);

                int windowFrameX = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
                int windowFrameY = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

                scaledWidth  += windowFrameX;
                scaledHeight += windowFrameY;
            }

            SetWindowPos(hWnd, NULL, 0, 0, scaledWidth, scaledHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);


            // If the window is a popup it will be positioned relative to the window rectangle, but we want it relative to the client area. We need to convert.
            if (type == WindowType::PopupWindow && hParent != 0)
            {
                this->_RefreshPopupPosition(reinterpret_cast<HWindow>(hWnd));
            }




            m_windows.PushBack(reinterpret_cast<HWindow>(hWnd));
        }

        return reinterpret_cast<HWindow>(hWnd);
    }

    void WindowManager_Win32::DeleteWindow(HWindow hWindow)
    {
        // Delete the window data.
        WindowData* pWindowData = reinterpret_cast<WindowData*>(::GetWindowLongPtr(reinterpret_cast<HWND>(hWindow), 0));
        if (pWindowData != nullptr)
        {
            SetWindowLongPtr(reinterpret_cast<HWND>(hWindow), 0, 0);
            delete pWindowData;
        }

        // Remove the window from the main list.
        m_windows.RemoveFirstOccuranceOf(hWindow);


        // Destroy the window last.
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
            WindowData* pWindowData = reinterpret_cast<WindowData*>(::GetWindowLongPtr(reinterpret_cast<HWND>(hWindow), 0));
            if (pWindowData != nullptr)
            {
                pWindowData->relativePosX = xPos;
                pWindowData->relativePosY = yPos;

                if (pWindowData->type == WindowType::PopupWindow)
                {
                    this->_RefreshPopupPosition(hWindow);
                }
                else
                {
                    ::SetWindowPos(reinterpret_cast<HWND>(hWindow), NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
                }
            }
            else
            {
                ::SetWindowPos(reinterpret_cast<HWND>(hWindow), NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
        }
    }

    void WindowManager_Win32::SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height)
    {
        if (hWindow != 0)
        {
            ::SetWindowPos(reinterpret_cast<HWND>(hWindow), NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
        }
    }

    bool WindowManager_Win32::GetWindowSize(HWindow hWindow, unsigned int &widthOut, unsigned int &heightOut) const
    {
        if (hWindow != 0)
        {
            RECT clientRect;
            if (GetClientRect(reinterpret_cast<HWND>(hWindow), &clientRect))
            {
                widthOut  = static_cast<unsigned int>(clientRect.right - clientRect.left);
                heightOut = static_cast<unsigned int>(clientRect.bottom - clientRect.top);

                return true;
            }
        }

        return false;
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


    bool WindowManager_Win32::IsWindowDescendant(HWindow hParentWindow, HWindow hChildWindow) const
    {
        if (hParentWindow != 0 && hChildWindow != 0)
        {
            return ::IsChild(reinterpret_cast<HWND>(hParentWindow), reinterpret_cast<HWND>(hChildWindow)) != 0;
        }

        return false;
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


    void WindowManager_Win32::SetWindowCursor(HWindow hWindow, HCursor hCursor)
    {
        if (hWindow != 0)
        {
            WindowData* pWindowData = reinterpret_cast<WindowData*>(::GetWindowLongPtr(reinterpret_cast<HWND>(hWindow), 0));
            if (pWindowData != nullptr)
            {
                pWindowData->hCursor = reinterpret_cast<HCURSOR>(hCursor);

                // If the cursor is over the window, change it now.
                if ((pWindowData->flags & WindowFlag_IsCursorInside) != 0)
                {
                    SetCursor(reinterpret_cast<HCURSOR>(pWindowData->hCursor));
                }
            }
        }
    }


    HCursor WindowManager_Win32::CreateCursorFromFile(const char* filePath)
    {
        return reinterpret_cast<HCursor>(LoadCursorFromFileA(filePath));
    }

    HCursor WindowManager_Win32::GetSystemCursor(SystemCursorType cursorType) const
    {
        switch (cursorType)
        {
        case SystemCursorType::Arrow:
            {
                return reinterpret_cast<HCursor>(LoadCursor(NULL, IDC_ARROW));
            }

        case SystemCursorType::IBeam:
            {
                return reinterpret_cast<HCursor>(LoadCursor(NULL, IDC_IBEAM));
            }


        default: break;
        }

        return 0;
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

    bool WindowManager_Win32::GetCursorPosition(HWindow hWindow, int &mousePosXOut, int &mousePosYOut) const
    {
        POINT p;
        if (GetCursorPos(&p))
        {
            if (ScreenToClient(reinterpret_cast<HWND>(hWindow), &p))
            {
                mousePosXOut = p.x;
                mousePosYOut = p.y;

                return true;
            }
        }

        return false;
    }


    void WindowManager_Win32::PostQuitMessage(int exitCode)
    {
        ::PostQuitMessage(exitCode);
    }

    int WindowManager_Win32::GetExitCode() const
    {
        return static_cast<int>(m_wExitCode);
    }


    bool WindowManager_Win32::IsKeyDown(GTLib::Key key) const
    {
        return (GetAsyncKeyState(ToWin32VirtualKey(key)) & 0x8000) != 0;
    }


    bool WindowManager_Win32::GetWindowDPI(HWindow hWindow, unsigned int &xDPIOut, unsigned int &yDPIOut) const
    {
        if (hWindow != NULL)
        {
            if (IsWindows8Point1OrGreater())
            {
                // We're running on a version of Windows that is either Windows 8.1 or newer which means it supports per-monitor DPI.

                PFN_GetDpiForMonitor _GetDpiForMonitor = reinterpret_cast<PFN_GetDpiForMonitor>(::GetProcAddress(m_hSHCoreDLL, "GetDpiForMonitor"));
                if (_GetDpiForMonitor)
                {
                    HMONITOR hMonitor = MonitorFromWindow(reinterpret_cast<HWND>(hWindow), MONITOR_DEFAULTTOPRIMARY);
                    if (hMonitor != NULL)
                    {
                        if (_GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &xDPIOut, &yDPIOut) == S_OK)
                        {
                            return true;
                        }
                    }
                }
            }
            else
            {
                // We're running on a version of Windows that is older than Windows 8.1 which means there is no per-monitor DPI support.

                HDC hDC = GetDC(NULL);
                xDPIOut = static_cast<unsigned int>(::GetDeviceCaps(hDC, LOGPIXELSX));
                yDPIOut = static_cast<unsigned int>(::GetDeviceCaps(hDC, LOGPIXELSY));
                ReleaseDC(NULL, hDC);

                return true;
            }
        }


        xDPIOut = 96;
        yDPIOut = 96;

        return false;
    }

    void WindowManager_Win32::GetBaseDPI(unsigned int &xDPIOut, unsigned int &yDPIOut) const
    {
        xDPIOut = 96;
        yDPIOut = 96;
    }


    bool WindowManager_Win32::GetMonitorResolution(int monitor, unsigned int &resolutionXOut, unsigned int &resolutionYOut)
    {
        if (monitor == GT_PRIMARY_MONITOR)
        {
            resolutionXOut = static_cast<unsigned int>(::GetSystemMetrics(SM_CXSCREEN));
            resolutionYOut = static_cast<unsigned int>(::GetSystemMetrics(SM_CYSCREEN));

            return true;
        }
        else
        {
            HMONITOR hMonitor = GetMonitorByIndex(monitor);
            if (hMonitor != NULL)
            {
                MONITORINFO mi;
                ZeroMemory(&mi, sizeof(mi));
                mi.cbSize = sizeof(mi);

                if (GetMonitorInfoW(hMonitor, &mi))
                {
                    resolutionXOut = mi.rcMonitor.right  - mi.rcMonitor.left;
                    resolutionYOut = mi.rcMonitor.bottom - mi.rcMonitor.top;

                    return true;
                }
            }

            return false;
        }
    }

    bool WindowManager_Win32::GetMonitorOrigin(int monitor, int &originXOut, int &originYOut)
    {
        if (monitor == GT_PRIMARY_MONITOR)
        {
            originXOut = 0;
            originYOut = 0;

            return true;
        }
        else
        {
            HMONITOR hMonitor = GetMonitorByIndex(monitor);
            if (hMonitor != NULL)
            {
                MONITORINFO mi;
                ZeroMemory(&mi, sizeof(mi));
                mi.cbSize = sizeof(mi);

                if (GetMonitorInfoW(hMonitor, &mi))
                {
                    originXOut = mi.rcMonitor.left;
                    originYOut = mi.rcMonitor.top;

                    return true;
                }
            }

            return false;
        }
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



    void WindowManager_Win32::_RefreshPopupPosition(HWindow hPopupWindow)
    {
        WindowData* pPopupWindowData = reinterpret_cast<WindowData*>(::GetWindowLongPtr(reinterpret_cast<HWND>(hPopupWindow), 0));
        assert(pPopupWindowData != nullptr);
        {
            assert(pPopupWindowData->type == WindowType::PopupWindow);

            HWND hOwnerWnd = ::GetWindow(reinterpret_cast<HWND>(hPopupWindow), GW_OWNER);
            if (hOwnerWnd != NULL)
            {
                RECT ownerRect;
                ::GetWindowRect(hOwnerWnd, &ownerRect);

                POINT p;
                p.x = 0;
                p.y = 0;
                if (::ClientToScreen(hOwnerWnd, &p))
                {
                    RECT parentRect;
                    ::GetWindowRect(hOwnerWnd, &parentRect);

                    int offsetX = p.x - parentRect.left;
                    int offsetY = p.y - parentRect.top;

                    ::SetWindowPos(reinterpret_cast<HWND>(hPopupWindow), NULL, ownerRect.left + pPopupWindowData->relativePosX + offsetX, ownerRect.top + pPopupWindowData->relativePosY + offsetY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
        }
    }



    ///////////////////////////////////
    // Private

    bool WindowManager_Win32::IsWindowOwnedByThis(HWND hWnd) const
    {
        for (size_t i = 0; i < m_windows.GetCount(); ++i)
        {
            if (m_windows[i] == reinterpret_cast<HWindow>(hWnd))
            {
                return true;
            }
        }

        return false;
    }

    HWND WindowManager_Win32::GetTopLevelPrimaryWindow(HWND hWnd) const
    {
        assert(this->IsWindowOwnedByThis(hWnd));

        HWND hTopLevelWindow = hWnd;
        do
        {
            WindowData* pWindowData = reinterpret_cast<WindowData*>(::GetWindowLongPtr(hTopLevelWindow, 0));
            if (pWindowData != nullptr)
            {
                if (pWindowData->type == WindowType::PrimaryWindow || pWindowData->type == WindowType::PrimaryWindow_Borderless)
                {
                    return hTopLevelWindow;
                }
            }

            hTopLevelWindow = ::GetWindow(hTopLevelWindow, GW_OWNER);
        } while (hTopLevelWindow != 0);


        return hTopLevelWindow;
    }
}

#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif
