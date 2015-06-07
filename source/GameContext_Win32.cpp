// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GameContext_Win32.hpp"

#if defined(GT_PLATFORM_WINDOWS)

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

namespace GT
{
    LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        GameContext* pGameContext = reinterpret_cast<GameContext*>(GetWindowLongPtrW(hWnd, sizeof(void*)*1));
        if (pGameContext != nullptr)
        {
            switch (msg)
            {
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
                        pGameContext->OnKeyPressed(reinterpret_cast<HWindow>(hWnd), static_cast<GTLib::Key>(wParam));
                    }
                    
                    break;
                }

            case WM_KEYUP:
                {
                    pGameContext->OnKeyReleased(reinterpret_cast<HWindow>(hWnd), static_cast<GTLib::Key>(wParam));
                    break;
                }

            case WM_UNICHAR:
                {
                    if ((lParam & (1 << 31)) != 0)
                    {
                        int repeatCount = lParam & 0x0000FFFF;
                        for (int i = 0; i < repeatCount; ++i)
                        {
                            pGameContext->OnPrintableKeyDown(reinterpret_cast<HWindow>(hWnd), wParam);
                        }
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
}

#endif
