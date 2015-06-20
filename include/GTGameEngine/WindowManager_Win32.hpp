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
    GTLib::Key FromWin32VirtualKey(WPARAM key);
    int ToWin32VirtualKey(GTLib::Key key);

    /// The Windows implementation of the window manager.
    class WindowManager_Win32 : public WindowManager
    {
    public:

        /// Constructor.
        WindowManager_Win32(unsigned int extraWindowBytes, WNDPROC wndProc);

        /// Destructor.
        virtual ~WindowManager_Win32();


        /// @copydoc WindowManager::CreateWindow()
        virtual HWindow CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height);

        /// @copydoc WindowManager::DeleteWindow()
        virtual void DeleteWindow(HWindow hWindow);


        /// @copydoc WindowManager::SetWindowTitle()
        virtual void SetWindowTitle(HWindow hWindow, const char* title);


        /// @copydoc WindowManager::SetWindowPosition()
        virtual void SetWindowPosition(HWindow hWindow, int xPos, int yPos);


        /// @copydoc WindowManager::SetWindowSize()
        virtual void SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height);

        /// @copydoc WindowManager::GetWindowSize()
        virtual bool GetWindowSize(HWindow hWindow, unsigned int &widthOut, unsigned int &heightOut) const;



        /// @copydoc WindowManager::ShowWindow()
        virtual void ShowWindow(HWindow hWindow);

        /// @copydoc WindowManager::HideWindow()
        virtual void HideWindow(HWindow hWindow);


        /// @copydoc WindowManager::InvalidateWindowRect()
        virtual void InvalidateWindowRect(HWindow hWindow, const GTLib::Rect<int> &rect);


        /// @copydoc WindowManager::SetFocusedWindow()
        virtual void SetFocusedWindow(HWindow hWindow);

        /// @copydoc WindowManager::GetFocusedWindow()
        virtual HWindow GetFocusedWindow() const;


        /// @copydoc WindowManager::ShowCursor()
        virtual void ShowCursor();

        /// @copydoc WindowManager::HideCursor()
        virtual void HideCursor();

        /// @copydoc WindowManager::IsCursorVisible()
        virtual bool IsCursorVisible() const;

        /// @copydoc WindowManager::GetMousePosition()
        virtual bool GetCursorPosition(HWindow hWindow, int &mousePosXOut, int &mousePosYOut) const;


        /// @copydoc WindowManager::PostQuitMessage()
        virtual void PostQuitMessage(int exitCode);

        /// @copydoc WindowManager::GetExitCode()
        virtual int GetExitCode() const;


        /// @copydoc WindowManager::IsKeyDown()
        bool IsKeyDown(GTLib::Key key) const;


        /// @copydoc WindowManager::EventDrivenLoop()
        virtual void EventDrivenLoop(std::function<bool ()> postLoop);

        /// @copydoc WindowManager::RealTimeLoop()
        virtual void RealTimeLoop(std::function<bool ()> postLoop);


    private:

        /// The exit code that was used to exit the process.
        WPARAM m_wExitCode;
    };
}

#endif

#endif // !__GT_WindowManager_Win32_hpp_
