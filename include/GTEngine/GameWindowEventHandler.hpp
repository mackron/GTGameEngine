// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameWindowEventHandler
#define GT_GameWindowEventHandler

#include <GTEngine/Core/Window.hpp>

namespace GT
{
    class Game;

    class GameWindowEventHandler : public WindowEventHandler
    {
    public:

        void OnClose();
        void OnSize(unsigned int width, unsigned int height);
        
        void OnPaint(int left, int top, int right, int bottom)
        {
            (void)left;
            (void)top;
            (void)right;
            (void)bottom;
        }
        
        void OnUpdate(double dt)
        {
            (void)dt;
        }
        
        void OnMouseMove(int x, int y);
        void OnMouseWheel(int delta, int x, int y);
        void OnMouseButtonDown(MouseButton button, int x, int y);
        void OnMouseButtonUp(MouseButton button, int x, int y);
        void OnMouseButtonDoubleClick(MouseButton button, int x, int y);

        void OnKeyPressed(Key key);
        void OnKeyReleased(Key key);
        void OnKeyDown(Key key, unsigned int repeatCount);
        void OnKeyUp(Key key);
        
        void OnReceiveFocus();
        void OnLoseFocus();


    public:

        /// Constructor.
        GameWindowEventHandler(Game &game);


    private:

        /// The game whose window this event handler is attached to.
        Game &game;


    private:    // No copying.
        GameWindowEventHandler(const GameWindowEventHandler &);
        GameWindowEventHandler & operator=(const GameWindowEventHandler &);

    };
}

#endif