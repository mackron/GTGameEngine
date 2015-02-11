// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameDisplay_Windowed.hpp>

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
#endif
    }
}