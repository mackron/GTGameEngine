// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_WindowManager
#define GT_WindowManager

#include <GTLib/Rect.hpp>
#include <GTLib/Keyboard.hpp>
#include <functional>

#define GT_PRIMARY_MONITOR  -1

namespace GT
{
    enum class WindowType
    {
        PrimaryWindow,              ///< A regular application window with a border and close buttons.
        PrimaryWindow_Borderless,   ///< Same as PrimaryWindow, except without a border.
        ChildWindow,                ///< A normal child window with no background and no border.
        PopupWindow                 ///< A window for things like popup menus, dropdowns, tooltips, etc.
    };

    enum class SystemCursorType
    {
        Arrow,
        IBeam                       // For text editors.
    };


    typedef size_t HWindow;
    typedef size_t HCursor;


    static const int MouseButton_Left   = 1;
    static const int MouseButton_Right  = 2;
    static const int MouseButton_Middle = 3;


    /// Base class for managing windows.
    class WindowManager
    {
    public:

        /// Constructor.
        WindowManager();

        /// Destructor.
        virtual ~WindowManager();



        /// Creates a new window.
        ///
        /// @param hParent [in] A handle to the parent window.
        /// @param type    [in] The window type.
        virtual HWindow CreateWindow(HWindow hParent, WindowType type, int xPos, int yPos, unsigned int width, unsigned int height) = 0;

        /// Deletes the given window.
        ///
        /// @param hWindow [in] A handle to the window.
        virtual void DeleteWindow(HWindow hWindow) = 0;


        /// Sets the title of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose title is being set.
        /// @param title   [in] The new title of the window.
        virtual void SetWindowTitle(HWindow hWindow, const char* title) = 0;

        /// Sets the position of the given window, relative to the parent.
        ///
        /// @param hWindow [in] A handle to the window whose position is being set.
        /// @param xPos    [in] The position of the window on the x axis.
        /// @param yPos    [in] The position of the window on the y axis.
        virtual void SetWindowPosition(HWindow hWindow, int xPos, int yPos) = 0;


        /// Sets the size of the given window.
        ///
        /// @param hWindow [in] A handle to the window whose size is being set.
        /// @param width   [in] The width of the window.
        /// @param height  [in] The height of the window.
        virtual void SetWindowSize(HWindow hWindow, unsigned int width, unsigned int height) = 0;

        /// Retrieves the size of the given window.
        ///
        /// @param hWindow   [in ] A handle to the window whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        ///
        /// @return True if the size of the window is retrieved successfully; false otherwise.
        virtual bool GetWindowSize(HWindow hWindow, unsigned int &widthOut, unsigned int &heightOut) const = 0;


        /// Shows the given window.
        ///
        /// @param hWindow [in] A handle to the window to show.
        virtual void ShowWindow(HWindow hWindow) = 0;

        /// Hides the given window.
        ///
        /// @param hWindow [in] A handle to the window to hide.
        virtual void HideWindow(HWindow hWindow) = 0;


        /// Determines if a window is a descendant of another.
        ///
        /// @param hParentWindow [in] The parent window.
        /// @param hChildWindow  [in] The descendant window.
        virtual bool IsWindowDescendant(HWindow hParentWindow, HWindow hChildWindow) const = 0;


        /// Invalidates a rectangular region of the given window so that it forces a repaint.
        ///
        /// @param hWindow [in] A handle to the window whose region is to be invalidated.
        /// @param rect    [in] The rectangle to invalidate.
        ///
        /// @remarks
        ///     The window is not redrawn immediately. Instead, it posts a message to the window's
        ///     event handler to do a redraw.
        virtual void InvalidateWindowRect(HWindow hWindow, const GTLib::Rect<int> &rect) = 0;


        /// Gives the given window keyboard focus.
        ///
        /// @param hWindow [in] A handle to the window that should receive the keyboard focus.
        virtual void SetFocusedWindow(HWindow hWindow) = 0;

        /// Retrieves a handle to the window that currently has the keyboard focus.
        ///
        /// @return A handle to the window that currently has the keyboard focus.
        virtual HWindow GetFocusedWindow() const = 0;


        /// Sets the cursor to use with the given window.
        virtual void SetWindowCursor(HWindow hWindow, HCursor hCursor) = 0;

        
        /// Loads a custom cursor from a .cur cursor file.
        virtual HCursor CreateCursorFromFile(const char* filePath) = 0;

        /// Retrieves a handle to a system cursor.
        virtual HCursor GetSystemCursor(SystemCursorType cursorType) const = 0;


        /// Shows the cursor at a global level.
        virtual void ShowCursor() = 0;

        /// Hides the cursor at a global level.
        virtual void HideCursor() = 0;

        /// Determines whether or not the cursor is visible at a global level.
        ///
        /// @return True if the cursor is visible at a global level; false otherwise.
        virtual bool IsCursorVisible() const = 0;

        /// Retrieves the position of the mouse cursor relative to the given window.
        ///
        /// @param hWindow      [in]  The window.
        /// @param mousePosXOut [out] A reference to the variable that will receive the position of the mouse on the x axis relative to the top left of the given window.
        /// @param mousePosYOut [out] A reference to the variable that will receive the position of the mouse on the y axis relative to the top left of the given window.
        virtual bool GetCursorPosition(HWindow hWindow, int &mousePosXOut, int &mousePosYOut) const = 0;


        /// Creates a timer.
        virtual bool CreateTimer(HWindow hWindow, size_t timerID, unsigned int milliseconds) = 0;

        /// Deletes a timer.
        virtual void DeleteTimer(HWindow hWindow, size_t timerID) = 0;


        /// Retrieves the amount of time it takes to blink the text cursor, in seconds.
        ///
        /// @remarks
        ///     This should be the sum of both the visible and invisible state.
        virtual float GetTextCursorBlinkTime() = 0;


        /// Posts a message to the window manager's event queue.
        ///
        /// @remarks
        ///     Use this to inject an event into the message queue to notify the application that it should quit.
        virtual void PostQuitMessage(int exitCode) = 0;

        /// Retrieves the exit code that was used to terminate the loop.
        ///
        /// @remarks
        ///     If no exit code has been posted, this will return 0.
        virtual int GetExitCode() const = 0;


        /// Determines whether or not the given key is down.
        ///
        /// @param key [in] The key to check.
        ///
        /// @return True if the key is down; false otherwise.
        virtual bool IsKeyDown(GTLib::Key key) const = 0;


        /// Retrieves the DPI of the given window based on the monitor that the majority of the window's area is contained within.
        ///
        /// @param hWindow [in ] A handle to the window.
        /// @param xDPIOut [out] A reference to the variable that will receive the DPI on the x axis.
        /// @param yDPIOut [out] A reference to the variable that will receive the DPI on the y axis.
        ///
        /// @remarks
        ///     If an error occurs, false will be returned and the returned DPI's will be set to 96.
        virtual bool GetWindowDPI(HWindow hWindow, unsigned int &xDPIOut, unsigned int &yDPIOut) const = 0;

        /// Retrieves the base DPI that can be used to calculate a scaling factor.
        ///
        /// @param xDPIOut [out] A reference to the variable that will receive the DPI on the x axis.
        /// @param yDPIOut [out] A reference to the variable that will receive the DPI on the y axis.
        virtual void GetBaseDPI(unsigned int &xDPIOut, unsigned int &yDPIOut) const = 0;


        /// Retrieves the resolution of the monitor with the given index.
        ///
        /// @param monitor        [in]  The monitor index. Set this to 0 to use the primary monitor.
        /// @param resolutionXOut [out] A reference to the variable that will receive the horizontal resolution.
        /// @param resolutionYOut [out] A reference to the variable that will receive the vertical resolution.
        ///
        /// @remarks
        ///     Set "monitor" to GT_PRIMARY_MONITOR to retrieve information about the primary monitor.
        virtual bool GetMonitorResolution(int monitor, unsigned int &resolutionXOut, unsigned int &resolutionYOut) = 0;

        /// Retrieves the origin (top-left corner) or the given monitor.
        ///
        /// @param monitor    [in]  The monitor index. Set this to 0 to use the primary monitor.
        /// @param originXOut [out] A reference to the variable that will receive the origin on the x axis.
        /// @param originYOut [out] A reference to the variable that will receive the origin on the y axis.
        ///
        /// @remarks
        ///     Set "monitor" to GT_PRIMARY_MONITOR to retrieve information about the primary monitor.
        virtual bool GetMonitorOrigin(int monitor, int &originXOut, int &originYOut) = 0;


        /// Enters into an event-driven loop.
        ///
        /// @param postLoop [in] The function to at the end of every loop iteration.
        ///
        /// @remarks
        ///     When postLoop returns false, the loop will be terminated.
        ///     @par
        ///     In the case of an event-driven loop the post loop function will be called for every event.
        virtual void EventDrivenLoop(std::function<bool ()> postLoop) = 0;

        /// Enters into a real-time loop.
        ///
        /// @param postLoop [in] The function to at the end of every loop iteration.
        ///
        /// @remarks
        ///     When postLoop returns false, the loop will be terminated.
        virtual void RealTimeLoop(std::function<bool ()> postLoop) = 0;
    };
}

#endif
