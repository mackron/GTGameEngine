// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameDisplay_Windowed.hpp>
#include <GTLib/windows.hpp>

namespace GT
{
    namespace GE
    {
        GameDisplay_Windowed::~GameDisplay_Windowed()
        {
        }


        GameDisplayType GameDisplay_Windowed::GetType() const
        {
            return GameDisplayType_Windowed;
        }


        ///////////////////////////////////////
        // Platform-specific

#if defined(GT_PLATFORM_WINDOWS)
        GameDisplay_Windowed::GameDisplay_Windowed(GPURenderingDevice &renderingDevice, HWND hWnd)
            : GameDisplay(renderingDevice),
              m_hWnd(hWnd)
        {
        }

        HWND GameDisplay_Windowed::GetWindow() const
        {
            return m_hWnd;
        }

        void GameDisplay_Windowed::GetSize(unsigned int &widthOut, unsigned int &heightOut) const
        {
            RECT clientRect;
            GetClientRect(m_hWnd, &clientRect);

            widthOut  = clientRect.right - clientRect.left;
            heightOut = clientRect.bottom - clientRect.top;
        }
#endif
    }
}