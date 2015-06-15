// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/WindowManager_DefaultWin32.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GameContext.hpp>

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

namespace GT
{
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
