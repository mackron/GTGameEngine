// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_WindowManager_Win32
#define GT_WindowManager_Win32

#include "Config.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include "WindowManager.hpp"
#include <GTLib/windows.hpp>
#include <GTLib/Rect.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    GTLib::Key FromWin32VirtualKey(WPARAM key);
    int ToWin32VirtualKey(GTLib::Key key);

    /// The Windows implementation of the window manager.
    class WindowManager_Win32 : public WindowManager
    {
    public:

        /// Constructor.
        WindowManager_Win32();

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


        /// @copydoc WindowManager::IsWindowDescendant()
        virtual bool IsWindowDescendant(HWindow hParentWindow, HWindow hChildWindow) const;


        /// @copydoc WindowManager::InvalidateWindowRect()
        virtual void InvalidateWindowRect(HWindow hWindow, const GTLib::Rect<int> &rect);


        /// @copydoc WindowManager::SetFocusedWindow()
        virtual void SetFocusedWindow(HWindow hWindow);

        /// @copydoc WindowManager::GetFocusedWindow()
        virtual HWindow GetFocusedWindow() const;


        /// @copydoc WindowManager::SetWindowCursor()
        virtual void SetWindowCursor(HWindow hWindow, HCursor hCursor);


        /// @copydoc WindowManager::CreateCursorFromFile()
        virtual HCursor CreateCursorFromFile(const char* filePath);

        /// @copydoc WindowManager::GetSystemCursor()
        virtual HCursor GetSystemCursor(SystemCursorType cursorType) const;


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
        virtual bool IsKeyDown(GTLib::Key key) const;


        /// @copydoc WindowManager::GetMonitorDPI()
        virtual bool GetWindowDPI(HWindow hWindow, unsigned int &xDPIOut, unsigned int &yDPIOut) const;

        /// @copydoc WindowManager::GetBaseDPI()
        virtual void GetBaseDPI(unsigned int &xDPIOut, unsigned int &yDPIOut) const;


        /// @copydoc WindowManager::GetMonitorResolution()
        virtual bool GetMonitorResolution(int monitor, unsigned int &resolutionXOut, unsigned int &resolutionYOut);

        /// @copydoc WindowManager::GetMonitorOrigin()
        virtual bool GetMonitorOrigin(int monitor, int &originXOut, int &originYOut);


        /// @copydoc WindowManager::EventDrivenLoop()
        virtual void EventDrivenLoop(std::function<bool ()> postLoop);

        /// @copydoc WindowManager::RealTimeLoop()
        virtual void RealTimeLoop(std::function<bool ()> postLoop);


        /// Retrieves a reference to the list of popup windows.
        GTLib::Vector<HWindow> & _GetWindows() { return m_windows; }

        /// Refreshes the position of the given popup window.
        void _RefreshPopupPosition(HWindow hPopupWindow);



        /////////////////////////////////////////////
        // To Be Implemented by Child Classes

        /// Called after a window is created.
        //virtual void OnWindowCreated(HWindow hWindow) = 0;

        /// Called just before a window is deleted.
        //virtual void OnWindowDeleting(HWindow hWindow) = 0;

        /// Called when the WM_CLOSE event is received.
        virtual void OnClose() = 0;

        /// Called when a primary window is activated.
        virtual void OnWindowActivated(HWindow hWindow) = 0;

        /// Called when a primary window is deactivated.
        virtual void OnWindowDeactivated(HWindow hWindow) = 0;

        /// Called when ths WM_MOVE event is received.
        virtual void OnMove(HWindow hWindow, int xPos, int yPos) = 0;

        /// Called when the WM_SIZE event is received.
        virtual void OnSize(HWindow hWindow, unsigned int width, unsigned int height) = 0;

        /// Called when the mouse enters the given window.
        virtual void OnMouseEnter(HWindow hWindow) = 0;

        /// Called when the mouse leaves the given window.
        virtual void OnMouseLeave(HWindow hWindow) = 0;

        /// Called when the WM_MOUSEMOVE event is called.
        virtual void OnMouseMove(HWindow hWindow, int mousePosX, int mousePosY) = 0;

        /// Called when a mouse button is pressed.
        virtual void OnMouseButtonPressed(HWindow hWinodw, int button, int mousePosX, int mousePosY) = 0;

        /// Called when a mouse button is released.
        virtual void OnMouseButtonReleased(HWindow hWindow, int button, int mousePosX, int mousePosY) = 0;

        /// Called when a mouse button is double-clicked.
        virtual void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int mousePosX, int mousePosY) = 0;

        /// Called when the mouse wheel is turned.
        virtual void OnMouseWheel(HWindow hWindow, int delta, int mousePosX, int mousePosY) = 0;

        /// Called when a key is pressed.
        virtual void OnKeyPressed(HWindow hWnd, GTLib::Key key) = 0;

        /// Called when a key is released.
        virtual void OnKeyReleased(HWindow hWnd, GTLib::Key key) = 0;

        /// Called when a printable key is pressed, including auto-repeat.
        virtual void OnPrintableKeyDown(HWindow hWnd, char32_t character) = 0;

        /// Called when the WM_PAINT message is received.
        virtual void OnPaintWindow(HWindow hWnd, const GTLib::Rect<int> &rect) = 0;


    public:

        // Structure containing information about a window. This is mostly to help work around Win32 API issues.
        struct WindowData
        {
            /// A pointer to the window manager that owns the window.
            WindowManager_Win32* pWindowManager;

            /// The position of the window relative to the parent on the y axis.
            int relativePosX;

            /// The position of the window relative to the parent on the y axis.
            int relativePosY;

            /// The window type. This is used so that certain event handlers can be handled differently depending on the window type. This is
            /// mostly used to handle popup windows.
            WindowType type;

            /// Some flags to help with handling some events.
            uint32_t flags;

            /// The cursor to use with the window. This should be the arrow cursor by default.
            HCURSOR hCursor;

            /// The high-surrogate from a WM_CHAR message. This is used in order to build a surrogate pair from a couple of WM_CHAR messages. When
            /// a WM_CHAR message is received when code point is not a high surrogate, this is set to 0.
            char16_t utf16HighSurrogate;
        };


        /// Determines whether or not the given window is owned by this window manager.
        bool IsWindowOwnedByThis(HWND hWnd) const;

        /// Finds the top-level primary window that is either the given window itself, or an ancestor.
        HWND GetTopLevelPrimaryWindow(HWND hWnd) const;



    private:

        /// A handle to the user32.dll module. We need this to get a handle to SetProcessDPIAware().
        HMODULE m_hUser32DLL;

        /// A handle to the shcore.dll module.
        HMODULE m_hSHCoreDLL;

        /// The list of windows that currently exist. We use this to handle activation state changes.
        GTLib::Vector<HWindow> m_windows;

        /// The exit code that was used to exit the process.
        WPARAM m_wExitCode;
    };
}

#endif

#endif // !__GT_WindowManager_Win32_hpp_
