// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/WindowManager_DefaultWin32.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GameContext.hpp>

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

namespace GT
{
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

    LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        GameContext* pGameContext = reinterpret_cast<GameContext*>(GetWindowLongPtrW(hWnd, 0));
        if (pGameContext != nullptr)
        {
            switch (msg)
            {
            case WM_ERASEBKGND:
                {
                    return 1;       //< Never draw the background of the window - always leave that the engine's native GUI system.
                }


            case WM_CLOSE:
                {
                    pGameContext->OnWantToClose();
                    break;
                }

            case WM_MOVE:
                {
                    pGameContext->OnWindowMoved(reinterpret_cast<HWindow>(hWnd), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }

            case WM_SIZE:
                {
                    pGameContext->OnWindowResized(reinterpret_cast<HWindow>(hWnd), LOWORD(lParam), HIWORD(lParam));
                    break;
                }

            case WM_MOUSEMOVE:
                {
                    pGameContext->OnMouseMove(reinterpret_cast<HWindow>(hWnd), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }


            case WM_LBUTTONDOWN:
                {
                    pGameContext->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_LBUTTONUP:
                {
                    pGameContext->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_LBUTTONDBLCLK:
                {
                    pGameContext->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }


            case WM_RBUTTONDOWN:
                {
                    pGameContext->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_RBUTTONUP:
                {
                    pGameContext->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_RBUTTONDBLCLK:
                {
                    pGameContext->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Right, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }


            case WM_MBUTTONDOWN:
                {
                    pGameContext->OnMouseButtonPressed(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_MBUTTONUP:
                {
                    pGameContext->OnMouseButtonReleased(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }
            case WM_MBUTTONDBLCLK:
                {
                    pGameContext->OnMouseButtonDoubleClicked(reinterpret_cast<HWindow>(hWnd), MouseButton_Middle, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    break;
                }


            case WM_KEYDOWN:
                {
                    if ((lParam & (1 << 30)) == 0)  // No auto-repeat.
                    {
                        pGameContext->OnKeyPressed(reinterpret_cast<HWindow>(hWnd), FromWin32VirtualKey(wParam));
                    }
                    
                    break;
                }

            case WM_KEYUP:
                {
                    pGameContext->OnKeyReleased(reinterpret_cast<HWindow>(hWnd), FromWin32VirtualKey(wParam));
                    break;
                }

            case WM_UNICHAR:
                {
                    if ((lParam & (1 << 31)) != 0)
                    {
                        int repeatCount = lParam & 0x0000FFFF;
                        for (int i = 0; i < repeatCount; ++i)
                        {
                            pGameContext->OnPrintableKeyDown(reinterpret_cast<HWindow>(hWnd), static_cast<char32_t>(wParam));
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
                        pGameContext->OnPaintWindow(reinterpret_cast<HWindow>(hWnd), rect);
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



    WindowManager_DefaultWin32::WindowManager_DefaultWin32(GameContext* pGameContext)
        : WindowManager_Win32(sizeof(void*), reinterpret_cast<WNDPROC>(DefaultWindowProcWin32)),
          m_pGameContext(pGameContext)
    {
    }

    WindowManager_DefaultWin32::~WindowManager_DefaultWin32()
    {
    }


    HWindow WindowManager_DefaultWin32::CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = WindowManager_Win32::CreateWindow(hParent, type, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            ::SetWindowLongPtrW(reinterpret_cast<HWND>(hWindow), 0, reinterpret_cast<LONG_PTR>(m_pGameContext));
        }

        return hWindow;
    }
}

#endif
