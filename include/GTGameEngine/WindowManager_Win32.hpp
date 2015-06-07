// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_WindowManager_Win32_hpp_
#define __GT_WindowManager_Win32_hpp_

#include "Config.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include "WindowManager.hpp"
#include <GTLib/windows.hpp>
#include <GTLib/Rect.hpp>

namespace GT
{
    /// The Windows implementation of the window manager.
    class WindowManager_Win32 : public WindowManager
    {
    public:

        /// Constructor.
        WindowManager_Win32();

        /// Destructor.
        ~WindowManager_Win32();


        /// @copydoc WindowManager::CreateWindow()
        HWindow CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height);

        /// @copydoc WindowManager::DeleteWindow()
        void DeleteWindow(HWindow hWindow);


        /// @copydoc WindowManager::SetWindowTitle()
        void SetWindowTitle(HWindow hWindow, const char* title);

        /// @copydoc WindowManager::SetWindowPosition()
        void SetWindowPosition(HWindow hWindow, int xPos, int yPos);

        /// @copydoc WindowManager::SetWindowSize()
        void SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height);


        /// @copydoc WindowManager::ShowWindow()
        void ShowWindow(HWindow hWindow);

        /// @copydoc WindowManager::HideWindow()
        void HideWindow(HWindow hWindow);


        /// @copydoc WindowManager::InvalidateWindowRect()
        void InvalidateWindowRect(HWindow hWindow, const GTLib::Rect<int> &rect);


        /// @copydoc WindowManager::SetFocusedWindow()
        void SetFocusedWindow(HWindow hWindow);

        /// @copydoc WindowManager::GetFocusedWindow()
        HWindow GetFocusedWindow() const;



        ////////////////////////////////////
        // Win32 Specific

        /// Sets the window handler for the given window.
        ///
        /// @param hWindow [in] A handle to the window whose window procedure is being set.
        /// @param wndProc [in] The window procedure function.
        void SetWindowProc(HWindow hWindow, WNDPROC wndProc);


    private:


    };
}

#endif

#endif // !__GT_WindowManager_Win32_hpp_
