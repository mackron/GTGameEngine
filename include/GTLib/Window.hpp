// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Window_hpp_
#define __GTLib_Window_hpp_

#include "Windowing/InternalWindowObjects.hpp"
#include "Windowing/WindowEventHandler.hpp"
#include "Cursors.hpp"
#include "Vector.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include "windows.hpp"
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // nonstandard extension.
#endif

#include <cassert>


namespace GTLib
{
    enum WindowShowType
    {
        WindowShowType_Normal,
        WindowShowType_Minimized,
        WindowShowType_Maximized,
    };

    /**
    *   \brief  The main window class for GTLib.
    *
    *   The implementation of the Window class is platform-specific.
    *
    *   \section X11-Specific
    *   With X11, GTLib does not use the notion of a Display. It will create a global display object when the first window
    *   is constructed. You can specify your own display by calling GTLib::X11::SetGlobalDisplay() before constructing the
    *   first window. GTLib does not support multiple displays. Specifying InternalWindowObjects::display does not do anything
    *   when constructing a window from an InternalWindowObjects structure.
    */
    class Window
    {
    public:     // Platform-specific.

        /**
        *   \brief  Default constructor.
        *
        *   \remarks
        *       With X11, the global display object will be used as the display. If this is the first window to be constructed,
        *       the global display will be created, unless it was explicatly set with GTLib::X11::SetGlobalDisplay().
        */
        Window();

        /**
        *   \brief  Constructor for an already created window.
        *
        *   \remarks
        *       With X11, the global display will be used as the display. If this is the first window to be constructed, the global
        *       display will be created, unless it was explicatly set with GTLib::X11::SetGlobalDisplay().
        */
        Window(const InternalWindowObjects &iwo);

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       After the last window is destructed, the global display is also destroyed.
        */
        ~Window();


        /**
        *   \brief               Sets the event handler.
        *   \param  handler [in] A reference to the handler to use with the window.
        */
        void SetEventHandler(WindowEventHandler &handler);
        void SetEventHandler(WindowEventHandler* handler) { assert(handler != nullptr); this->SetEventHandler(*handler); }
        
        /**
        *   \brief  Retrieves the event handler being used by the window.
        *   \return A reference to the event handler of the window.
        *
        *   \remarks
        *       If an event handler has not yet been set, it returns the default event handler.
        */
        WindowEventHandler & GetEventHandler();


        /**
        *   \brief  Retrieves the position of the window relative to the parent.
        *
        *   \remarks
        *       For the moment the parent will always be the desktop. This may change in the future.
        */
        void GetPosition(int &x, int &y);
        
        /**
        *   \brief  Sets the position of the window.
        */
        void SetPosition(int x, int y);
        
        
        /**
        *   \brief  Retrieves the dimensions of the window, not including the frame.
        *
        *   \remarks
        *       If the size of the window including the frame is required, use GetSizeWithFrame().
        */
        void GetSize(unsigned int &width, unsigned int &height);
        
        /**
        *   \brief  Sets the size of the window, not including the frame.
        */
        void SetSize(unsigned int width, unsigned int height);
        
        
        /**
        *   \brief  Sets the title of the window.
        */
        void SetTitle(const char *title);
        
        
        /**
        *   \brief  Shows the window.
        */
        void Show(WindowShowType showType = WindowShowType_Normal);
        
        
        /**
        *   \brief  Redraws a part of the window.
        */
        void Redraw(int left, int top, int right, int bottom);


        /**
        *   \brief              Sets the cursor to a system cursor.
        *   \param  cursor [in] The type of cursor to set.
        */
        void SetCursor(Cursor cursor);

        /// Hides the cursor.
        void HideCursor();

        /// Shows the cursor.
        void ShowCursor();


        /**
        *   \brief  Retrieves the position of the mouse relative to this window.
        */
        void GetMousePosition(int &x, int &y);

        /**
        *   \brief  Sets the position of the mouse relative to this window.
        */
        void SetMousePosition(int x, int y);


        /// Switches to fullscreen mode.
        void EnableFullscreen(unsigned int width, unsigned int height);

        /// Disables fullscreen mode, resetting the winodw back to it's previous state.
        void DisableFullscreen();

        /// Determines whether or not the window is fullscreen.
        bool IsFullscreen() const;



    public:     // Cross-platform.

        /**
        *   \brief  Retrieves a constant reference to the internal window objects structure.
        */
        const InternalWindowObjects & GetInternalObjects() const;


    private:

        /// The internal window objects.
        InternalWindowObjects iwo;


    private:    // No copying.
        Window(const Window &);
        Window & operator=(const Window &);

    };
}

namespace GTLib
{
    /// Structure representing a window.
    ///
    /// @remarks
    ///     This will have different properties depending on the platform. This system is less politically correct, but makes things much, much
    ///     simpler in practice.
    struct _Window
    {
#if defined(GT_PLATFORM_WINDOWS)
        /// The Win32 window handle.
        HWND hWnd;
        
        /// The Win32 device context.
        HDC hDC;

        /// The style the window was created with. This is used in calculating the size of a window based on the size of the client area.
        DWORD style;

        /// The extended style the window was created with. This is used in calculating the size of a window based on the size of the client area.
        DWORD exStyle;

        /// The cursor to use when the mouse is over the window.
        Cursor cursor;

        /// Various flags for things like keeping track of whether or not the control key is down or a cursor is visible.
        uint32_t flags;


        /// Constructor.
        _Window()
            : hWnd(nullptr), hDC(nullptr), style(0), exStyle(0),
              cursor(Cursor_Arrow), flags(0)
        {
        }
#else
        /// The X11 window handle.
        ::Window window;
        
        /// The X11 display this window is part of.
        Display* display;
        
        /// A pointer to the XVisualInfo structure that can be used when creating the window.
        XVisualInfo *vi;

        /// The colour map to use with the window.
        Colormap colormap;
        
        /// Flags for miscellaneous things with the window.
        uint32_t flags;
        
        
        // The previous positions. This is used to help split ConfigureNotify events into separate position and size events.
        int prevPosX;
        int prevPosY;
        
        // The previous size. This is used to help split ConfigureNotofy events into separate position and size events.
        int prevWidth;
        int prevHeight;
        
        
        /// A vector containing the last time each mouse button was pressed. We need this in order to check for double-clicks. We use
        /// a vector here because there can be an arbitrary number of mouse buttons, and I don't like hard-coded sized arrays. Each entry
        /// in this list corresponds to a button index.
        GTLib::Vector<Time> lastMouseButtonClickTimes;
        
        
        /// Constructor.
        _Window()
            : window(0), display(nullptr), vi(nullptr), colormap(0), flags(0),
              prevPosX(0), prevPosY(0), prevWidth(0), prevHeight(0),
              lastMouseButtonClickTimes()
        {
        }
        
    private:    // No copying.
        _Window(const _Window &);
        _Window & operator=(const _Window &);
#endif
    };
    typedef _Window* WindowHandle;
    
    
    /// Creates a window.
    ///
    /// @param x           [in] The x position of the window.
    /// @param y           [in] The y position of the window.
    /// @param width       [in] The width of the window.
    /// @param height      [in] The height of the window.
    /// @param pixelFormat [in] (Windows only) The pixel format to use with the window for OpenGL rendering.
    /// @param pfd         [in] (Windows only) The pixel format descriptor to use with the window for OpenGL rendering.
    /// @param visualInfo  [in] (X11 only) A pointer to the XVisualInfo structure to use when creating the window. Required with OpenGL.
    /// @param colormap    [in] (X11 only) The colour map to use when creating the window. Can be 0, in which case it will be created based on the XVisualInfo structure.
    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height);

#if defined(GT_PLATFORM_WINDOWS)
    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height, int pixelFormat, const struct tagPIXELFORMATDESCRIPTOR* pfd);
#endif

#if defined(GT_PLATFORM_LINUX)
    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height, XVisualInfo* visualInfo, Colormap colormap = 0);
#endif
    
    /// Deletes a window.
    ///
    /// @param window [in] A handle to the window to delete.
    void DeleteWindow(WindowHandle window);
    
    
    /// Shows the given window.
    ///
    /// @param window [in] A handle to the window to show.
    void ShowWindow(WindowHandle window);
    
    /// Hides the given windiow.
    ///
    /// @param window [in] A handle to the window to hide.
    void HideWindow(WindowHandle window);
    
    
    /// Sets the title of the window.
    ///
    /// @param window [in] The window whose title is getting set.
    /// @param title  [in] The new title of the window.
    void SetWindowTitle(WindowHandle window, const char* title);
    
    
    /// Retrieves the position of the window.
    ///
    /// @param window [in]  The window whose position is being retrieved.
    /// @param xOut   [out] A reference to the variable that will receive the x position.
    /// @param yOut   [out] A reference to teh variable that will receive the y position.
    void GetWindowPosition(WindowHandle window, int &xOut, int &yOut);
    
    /// Sets the position of the given window.
    ///
    /// @param window [in] The window whose position is being set.
    /// @param x      [in] The new x position.
    /// @param y      [in] The new y position.
    void SetWindowPosition(WindowHandle window, int x, int y);
    
    
    /// Retrieves the size of the client area of the given window.
    ///
    /// @param window    [in]  The window whose size is being retrieved.
    /// @param widthOut  [out] A reference to the variable that will receive the width.
    /// @param heightOut [out] A reference to the variable that will receive the height.
    void GetWindowSize(WindowHandle window, unsigned int &widthOut, unsigned int &heightOut);
    
    /// Sets the size of the client area of the given window.
    ///
    /// @param window [in] The window whose size is being set.
    /// @param width  [in] The new width.
    /// @param height [in] The new height.
    ///
    /// @remarks
    ///     This sets the size of the client area. The final size of the window including the decoration will be larger than the given coordinates.
    void SetWindowSize(WindowHandle window, unsigned int width, unsigned int height);


    /// Sets the cursor to use with the given window.
    ///
    /// @param window [in] The window whose cursor is being set.
    /// @param cursor [in] The system cursor to apply to the window.
    void SetWindowCursor(WindowHandle window, Cursor cursor);

    /// Shows the cursor for the given window.
    ///
    /// @param window [in] The window whose cursor is being shown.
    void ShowWindowCursor(WindowHandle window);

    /// Hides the cursor for the given window.
    ///
    /// @param window [in] The window whose cursor is being hidden.
    void HideWindowCursor(WindowHandle window);
    
    
    /**
    *   \brief            Processes all of the messages for each window.
    *   \param  wait [in] Whether or not the function should block until the next event arrives.
    *   \return           True if there are more messages ready to be pumped; false otherwise.
    *
    *   \remarks
    *       The return value is only really useful if 'wait' is set to false. It allows the application to loop through
    *       any pending messages before moving forward.
    */
    bool PumpNextWindowEvent(bool wait);
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
