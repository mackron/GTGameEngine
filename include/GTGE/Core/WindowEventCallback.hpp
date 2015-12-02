// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_WindowEventCallback
#define GT_WindowEventCallback

#include "WindowEvent.hpp"

namespace GT
{
    /// Callback passed to NextWindowEvent().
    ///
    /// @remarks
    ///     This class is how events should be handled.
    class WindowEventCallback
    {
    public:

        /// Constructor.
        WindowEventCallback();

        /// Destructor.
        virtual ~WindowEventCallback();



        ///////////////////////////////////////
        // Event Handlers

        /// Called when the OnClose event needs to be handled.
        ///
        /// @param window [in] The window that's closing.
        virtual void OnClose(WindowHandle window);

        /// Called when the OnPaint event needs to be handled.
        ///
        /// @param window  [in] The window that needs to be painted.
        /// @param left    [in] The position of the left side of the rectangle that needs to be painted.
        /// @param top     [in] The position of the top side of the rectangle that needs to be painted.
        /// @param right   [in] The position of the right side of the rectangle that needs to be painted.
        /// @param bottom  [in] The position of the bottom side of the rectangle that needs to be painted.
        /// @param auxData [in] On Windows, this is a pointer to the PAINTSTRUCT from BeginPaint().
        ///
        /// @remarks
        ///     On Windows, this will be called in between BeginPaint() And EndPaint(). EndPaint() will be called straight after OnPaint() returns. 'auxData' will be a pointer
        ///     to the PAINTSTRUCT created by BeginPaint(). A simple cast is safe, after which you can retrieve the HDC for drawing with GDI, if desired.
        virtual void OnPaint(WindowHandle window, int left, int top, int right, int bottom, void* auxData);


        /// Called when the OnSize event needs to be handled.
        ///
        /// @param window    [in] The window that's changing size.
        /// @param newWidth  [in] The width of the client area of the window.
        /// @param newHeight [in] The new height of the client area of the window.
        virtual void OnSize(WindowHandle window, unsigned int newWidth, unsigned int newHeight);

        /// Called when the OnMove event needs to be handled.
        ///
        /// @param window  [in] The window that's changing position.
        /// @param newXPos [in] The new x position of the window.
        /// @param newYPos [in] The new y position of the window.
        virtual void OnMove(WindowHandle window, int newXPos, int newYPos);


        /// Called when the OnMouseMove event needs to be handled.
        ///
        /// @param window    [in] The window that has received the event.
        /// @param mousePosX [in] The position of the mouse on the x axis.
        /// @param mousePosY [in] The position of the mouse on the y axis.
        virtual void OnMouseMove(WindowHandle window, int mousePosX, int mousePosY);

        /// Called when the OnMouseButtonDown event needs to be handled.
        ///
        /// @param window    [in] The window that has received the event.
        /// @param button    [in] The code of the mouse button that was pressed.
        /// @param mousePosX [in] The position of the mouse on the x axis.
        /// @param mousePosY [in] The position of the mouse on the y axis.
        ///
        /// @remarks
        ///     For the left, right and middle mouse buttons, 'button' will equal 1, 2 and 3 respectively.
        virtual void OnMouseButtonDown(WindowHandle window, int button, int mousePosX, int mousePosY);

        /// Called when the OnMouseButtonUp event needs to be handled.
        ///
        /// @param window    [in] The window that has received the event.
        /// @param button    [in] The code of the mouse button that was raised.
        /// @param mousePosX [in] The position of the mouse on the x axis.
        /// @param mousePosY [in] The position of the mouse on the y axis.
        ///
        /// @remarks
        ///     For the left, right and middle mouse buttons, 'button' will equal 1, 2 and 3 respectively.
        virtual void OnMouseButtonUp(WindowHandle window, int button, int mousePosX, int mousePosY);

        /// Called when the OnMouseButtonDoubleClick event needs to be handled.
        ///
        /// @param window    [in] The window that has received the event.
        /// @param button    [in] The code of the mouse button that was double-clicked.
        /// @param mousePosX [in] The position of the mouse on the x axis.
        /// @param mousePosY [in] The position of the mouse on the y axis.
        ///
        /// @remarks
        ///     For the left, right and middle mouse buttons, 'button' will equal 1, 2 and 3 respectively.
        ///     @par
        ///     This event will always be preceeded by OnMouseButtonDown.
        virtual void OnMouseButtonDoubleClick(WindowHandle window, int button, int mousePosX, int mousePosY);

        /// Called when the MouseWheel event needs to be handled.
        ///
        /// @param window [in] The window that has received the event.
        /// @param delta  [in] The mouse wheel delta (how far it has moved).
        /// @param mousePosX [in] The position of the mouse on the x axis.
        /// @param mousePosY [in] The position of the mouse on the y axis.
        ///
        /// @remarks
        ///     When the mouse wheel is scrolled up, 'delta' will be greater than 0; otherwise 'delta' wil be less than 0.
        virtual void OnMouseWheel(WindowHandle window, int delta, int mousePosX, int mousePosY);

        
        /// Called when a key is pressed on the keyboard.
        ///
        /// @param window [in] The window that has received the event.
        /// @param key    [in] The key code of the key that was pressed.
        ///
        /// @remarks
        ///     The difference between OnKeyPressed and OnKeyDown is that OnKeyPressed does not auto-repeat and ASCII alphabetical characters are always upper-case.
        virtual void OnKeyPressed(WindowHandle window, Key key);

        /// Called when a key is released.
        ///
        /// @param window [in] The window that has received the event.
        /// @param key    [in] the key code of the key that was released.
        ///
        /// @remarks
        ///     The difference between OnKeyReleased and OnKeyDown is that OnKeyReleased is intended to be paired with OnKeyPressed, whereas OnKeyUp should be paired with OnKeyDown.
        virtual void OnKeyReleased(WindowHandle window, Key key);

        /// Called when a key is pressed on the keyboard, including auto-repeating.
        ///
        /// @param window [in] The window that has received the event.
        /// @param key    [in] The key code of the key that was pressed or auto-repeated.
        ///
        /// @remarks
        ///     The difference between OnKeyPressed and OnKeyDown is that OnKeyPressed does not auto-repeat and ASCII alphabetical characters are always upper-case.
        virtual void OnKeyDown(WindowHandle window, Key key);

        /// Called when a key is released from the keyboard.
        ///
        /// @param window [in] The window that has received the event.
        /// @param key    [in] The key code of the key that was released.
        ///
        /// @remarks
        ///     The difference between OnKeyReleased and OnKeyDown is that OnKeyReleased is intended to be paired with OnKeyPressed, whereas OnKeyUp should be paired with OnKeyDown.
        virtual void OnKeyUp(WindowHandle window, Key key);


        /// Called when the window received focus.
        ///
        /// @param window [in] The window that has received the event.
        virtual void OnReceiveFocus(WindowHandle window);

        /// Called when the window loses focus.
        ///
        /// @param window [in] The window that has received the event.
        virtual void OnLoseFocus(WindowHandle window);


        /// Called when a timer is triggered.
        ///
        /// @param window  [in] The window that has received the event.
        /// @param timerID [in] The ID of the timer that was triggered.
        virtual void OnTimer(WindowHandle window, uint32_t timerID);
    };
}

#endif // !GT_WindowEventCallback
