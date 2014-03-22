// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTGameEngine_GameEventFilter_hpp_
#define __GTGameEngine_GameEventFilter_hpp_

#include <GTLib/Windowing/MouseButtons.hpp>
#include <GTLib/Windowing/Keys.hpp>

namespace GTEngine
{
    /// Base class for filtering events posted by the Game class.
    ///
    /// The event filtering system is designed to allow different parts of the engine and game to change the input values of events
    /// and prevent events from being dispatched.
    ///
    /// Every input to every method in this class is an in/out parameter. Every method will return a boolean to indicate whether or
    /// not the event should be dispatched. When true is returned, the event will be dispatched; otherwise, the event will not be
    /// dispatched.
    ///
    /// The default implementation of every method in this class will leave the input parameters as-is and return true.
    class GameEventFilter
    {
    public:
        
        /// Constructor.
        GameEventFilter();
        
        /// Destructor.
        virtual ~GameEventFilter();
        
        
        /// Called when the game wants to post the OnSize event.
        ///
        /// @param width  [in, out] The new width of the game window.
        /// @param height [in, out] The new height of the game window.
        ///
        /// @return True if the event should be dispatched; false otherwise.
        virtual bool OnSize(unsigned int &width, unsigned int &height);
        
        /// Called when the mouse is moved within the game viewport.
        ///
        /// @param mousePosX [in, out] The position of the mouse on the X axis, relative to the left of the game viewport.
        /// @param mousePosY [in, out] The position of the mouse on the Y axis, relative to the top of the game viewport.
        ///
        /// @return True if the event should be dispatched; false otherwise.
        virtual bool OnMouseMove(int &mousePosX, int &mousePosY);
        
        /// Called when the mouse wheel it turned.
        ///
        /// @param delta     [in, out] The amount the mouse wheel has turned.
        /// @param mousePosX [in, out] The position of the mouse on the X axis, relative to the left of the game viewport.
        /// @param mousePosY [in, out] The position of the mouse on the Y axis, relative to the top of the game viewport.
        virtual bool OnMouseWheel(int &delta, int &mousePosX, int &mousePosY);
        
        /// Called when a mouse button is pressed.
        ///
        /// @param button    [in, out] The button code for the button that was pressed.
        /// @param mousePosX [in, out] The position of the mouse on the X axis, relative to the left of the game viewport.
        /// @param mousePosY [in, out] The position of the mouse on the Y axis, relative to the top of the game viewport.
        virtual bool OnMouseButtonDown(GTLib::MouseButton &button, int &mousePosX, int &mousePosY);
        
        /// Called when a mouse button is released.
        ///
        /// @param button    [in, out] The button code for the button that was released.
        /// @param mousePosX [in, out] The position of the mouse on the X axis, relative to the left of the game viewport.
        /// @param mousePosY [in, out] The position of the mouse on the Y axis, relative to the top of the game viewport.
        virtual bool OnMouseButtonUp(GTLib::MouseButton &button, int &mousePosX, int &mousePosY);
        
        /// Called when a mouse button is double-clicked.
        ///
        /// @param button    [in, out] The button code for the button that was double-clicked.
        /// @param mousePosX [in, out] The position of the mouse on the X axis, relative to the left of the game viewport.
        /// @param mousePosY [in, out] The position of the mouse on the Y axis, relative to the top of the game viewport.
        virtual bool OnMouseButtonDoubleClick(GTLib::MouseButton &button, int &mousePosX, int &mousePosY);
        
        
        /// Called when a key is pressed.
        ///
        /// @param key [in, out] The key code for the key that was just pressed.
        ///
        /// @remarks
        ///     The difference between this and OnKeyDown() is that OnKeyPressed() does not handle auto-repeat.
        virtual bool OnKeyPressed(GTLib::Key &key);
        
        /// Called when a key is released.
        ///
        /// @param key [in, out] The key code for the key that was just released.
        virtual bool OnKeyReleased(GTLib::Key &key);
        
        /// Called when a key is pressed, taking auto-repeat into account.
        ///
        /// @param key [in, out] The key code for the key that was just pressed or auto-repeated.
        ///
        /// @remarks
        ///     The difference between this and OnKeyDown() is that OnKeyPressed() does not handle auto-repeat.
        virtual bool OnKeyDown(GTLib::Key &key);
        
        /// Called when a key is released.
        ///
        /// @param key [in, out] The key code for the key that was just released.
        virtual bool OnKeyUp(GTLib::Key &key);
        
        
        /// Called when the game window receives focus.
        virtual bool OnReceiveFocus();
        
        /// Called when the game window loses focus.
        virtual bool OnLoseFocus();
    };
}

#endif
