// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_WindowManager_DefaultWin32_hpp_
#define __GT_WindowManager_DefaultWin32_hpp_

#include "WindowManager_Win32.hpp"

#if defined(GT_PLATFORM_WINDOWS)

namespace GT
{
    class GameContext;

    /// Default Windows implementation of the window manager.
    class WindowManager_DefaultWin32 : public WindowManager_Win32
    {
    public:

        /// Constructor.
        WindowManager_DefaultWin32(GameContext* pGameContext);

        /// Destructor.
        ~WindowManager_DefaultWin32();


        /// @copydoc WindowManager::CreateWindow()
        HWindow CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height);


    private:

        /// A pointer to the game context that will be associated with the window. This is required so that events can be posted to it.
        GameContext* m_pGameContext;
    };
}

#endif

#endif
