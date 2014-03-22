// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_GameState_hpp_
#define __GTEngine_GameState_hpp_

#include <GTLib/Windowing/Keys.hpp>
#include <GTLib/Windowing/MouseButtons.hpp>

namespace GTEngine
{
    /// Base class for doing game states.
    class GameState
    {
    public:

        /// Constructor.
        GameState();

        /// Destructor.
        virtual ~GameState();


        /// Activates the game state.
        ///
        /// @remarks
        ///     If this is the first time the game state is activated, OnStartup() will be called before OnActivate().
        void Activate();

        /// Deactivates the game state.
        ///
        /// @remarks
        ///     This will call OnDeactivate().
        void Deactivate();


        ///////////////////////////////////////////////////////
        // Events.

        /// Called when the game state needs to start up.
        virtual void OnStartup();

        /// Called when the game state is activated.
        virtual void OnActivate();

        /// Called when the game state is deactivated.
        virtual void OnDeactivate();


        /// Called when the game is being updated.
        ///
        /// @param deltaTimeInSeconds [in] The time since the last frame, in seconds.
        virtual void OnUpdate(double deltaTimeInSeconds);

        /// Called when the mouse has moved.
        ///
        /// @param x [in] The x position of the mouse.
        /// @param y [in] The y position of the mouse.
        virtual void OnMouseMove(int x, int y);

        /// Called when mouse wheel has been turned.
        ///
        /// @param delta [in] The mouse wheel movement.
        /// @param x     [in] The x position of the mouse.
        /// @param y     [in] The y position of the mouse.
        virtual void OnMouseWheel(int delta, int x, int y);

        /// Called when a mouse button has been pressed.
        ///
        /// @param button [in] The button that was pressed.
        /// @param x      [in] The x position of the mouse.
        /// @param y      [in] The y position of the mouse.
        virtual void OnMouseButtonDown(GTLib::MouseButton button, int x, int y);

        /// Called when a mouse button has been released.
        ///
        /// @param button [in] The button that was released.
        /// @param x      [in] The x position of the mouse.
        /// @param y      [in] The y position of the mouse.
        virtual void OnMouseButtonUp(GTLib::MouseButton button, int x, int y);

        /// Called when a mouse button has been double-clicked.
        ///
        /// @param button [in] The button that was double-clicked.
        /// @param x      [in] The x position of the mouse.
        /// @param y      [in] The y position of the mouse.
        virtual void OnMouseButtonDoubleClick(GTLib::MouseButton button, int x, int y);

        /// Called when a key is pressed.
        ///
        /// @param key [in] The key that was pressed.
        virtual void OnKeyPressed(GTLib::Key key);

        /// Called when a key is released.
        ///
        /// @param key [in] The key that was released.
        virtual void OnKeyReleased(GTLib::Key key);

        /// Called when the game is paused.
        virtual void OnPause();

        /// Called when the game is resumed.
        virtual void OnResume();


        /// Called when the game window has been resized.
        ///
        /// @param width  [in] The new width of the game window.
        /// @param height [in] The new height of the game window.
        virtual void OnSize(unsigned int width, unsigned int height);


    private:

        /// Keeps track of whether or not the game state has been initialised.
        bool isInitialised;
    };
}

#endif