// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_WindowEventHandler
#define GT_WindowEventHandler

#include "MouseButtons.hpp"
#include "Keys.hpp"

namespace GT
{
    /**
    *   \brief  Base class for the window event listener.
    *
    *   This class has default implementations of each event which just do nothing.
    */
    class WindowEventHandler
    {
    public:
    
        /**
        *   \brief         Called after the window is moved.
        *   \param  x [in] The new X position of the window relative to the parent.
        *   \param  y [in] The new Y position of the window relative to the parent.
        */
        virtual void OnMove(int x, int y);
    
        /**
        *   \brief              Called after the window is resized.
        *   \param  width  [in] The new width of the window.
        *   \param  height [in] The new height of the window.
        */
        virtual void OnSize(unsigned int width, unsigned int height);
        
        /**
        *   \brief  Called when the window has received a close command.
        */
        virtual void OnClose();
        
        /**
        *   \brief  Called when the window needs to be redrawn.
        */
        virtual void OnPaint(int left, int top, int right, int bottom);
        
        /**
        *   \brief  Called when various things need to be updated.
        *
        *   \remarks
        *       This event is called roughly once each millisecond, but may vary. 'dt' will specify the amount
        *       of time in seconds since the last time OnUpdate() was called.
        */
        virtual void OnUpdate(double dt);
        
        /**
        *   \brief  Called when the system mouse is moved over the window.
        *
        *   \remarks
        *       This function will be called when Window::PostMouseMoveEvent() is called. By default this function
        *       should be called automatically when the mouse is moved while over the window. Applications can disable
        *       this functionality with Window::DisableAutoMouseEvents().
        *       \par
        *       The coordinates are always relative to the bottom-left of the window.
        */
        virtual void OnMouseMove(int x, int y);

        /**
        *   \brief             Called when the mouse wheel is moved.
        *   \param  delta [in] The distance the mouse wheel has moved. A positive value means the wheel was move forward away from the user.
        *   \param  x     [in] The x position of the mouse.
        *   \param  y     [in] The y position of the mouse.
        */
        virtual void OnMouseWheel(int delta, int x, int y);
        
        /**
        *   \brief  Called when the mouse has one of it's buttons pressed.
        */
        virtual void OnMouseButtonDown(MouseButton button, int x, int y);
        
        /**
        *   \brief  Called when the mouse has one of it's buttons released.
        */
        virtual void OnMouseButtonUp(MouseButton button, int x, int y);
        
        /**
        *   \brief  Called when a mouse button is double clicked.
        */
        virtual void OnMouseButtonDoubleClick(MouseButton button, int x, int y);
        

        /**
        *   \brief  Called when a key is pressed. This does not repeat as the key is held down.
        */
        virtual void OnKeyPressed(Key key);

        /**
        *   \brief  Called when a key is released. This is the opposite of OnKeyPressed().
        *
        *   \remarks
        *       This is very similar to OnKeyUp(), but 'key' always corresponds to 'key' from OnKeyPressed().
        */
        virtual void OnKeyReleased(Key key);

        /**
        *   \brief  Called when a key is down. This is a repeating message.
        */
        virtual void OnKeyDown(Key key, unsigned int repeatCount);

        /**
        *   \brief  Called when a key is released.
        */
        virtual void OnKeyUp(Key key);

        
        /**
        *   \brief  Called when the window receives focus.
        */
        virtual void OnReceiveFocus();
        
        /**
        *   \brief  Called when the window loses focus.
        */
        virtual void OnLoseFocus();
        
                
    public:
    
        /**
        *   \brief  Retrieves the default window event listener instantiation.
        */
        static WindowEventHandler & GetDefault();
        
        
    public:
    
        virtual ~WindowEventHandler() {}
    };
}



#endif
