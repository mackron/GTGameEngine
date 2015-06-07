// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GameContext_Win32_hpp_
#define __GT_GameContext_Win32_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/GameContext.hpp>
#include <GTLib/windows.hpp>

namespace GT
{
    /// The default event handler for game windows.
    LRESULT DefaultWindowProcWin32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

#endif

#endif
