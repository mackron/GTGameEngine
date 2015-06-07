// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_WindowManager_hpp_
#define __GT_WindowManager_hpp_

#include <GTLib/Rect.hpp>

namespace GT
{
    enum class WindowType
    {
        PrimaryWindow,              //< A regular application window with a border and close buttons.
        PrimaryWindow_Borderless,   //< Same as PrimaryWindow, except without a border.
        ChildWindow,                //< A normal child window with no background and no border.
        PopupWindow                 //< A window for things like popup menus, dropdowns, tooltips, etc.
    };

    typedef size_t HWindow;


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


        /// Shows the given window.
        ///
        /// @param hWindow [in] A handle to the window to show.
        virtual void ShowWindow(HWindow hWindow) = 0;

        /// Hides the given window.
        ///
        /// @param hWindow [in] A handle to the window to hide.
        virtual void HideWindow(HWindow hWindow) = 0;


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
    };
}

#endif
