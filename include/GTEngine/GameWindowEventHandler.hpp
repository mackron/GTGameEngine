// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_GameWindowEventHandler_hpp_
#define __GTEngine_GameWindowEventHandler_hpp_

#include <GTLib/Window.hpp>

namespace GTEngine
{
    class Game;

    class GameWindowEventHandler : public GTLib::WindowEventHandler
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
        void OnMouseButtonDown(GTLib::MouseButton button, int x, int y);
        void OnMouseButtonUp(GTLib::MouseButton button, int x, int y);
        void OnMouseButtonDoubleClick(GTLib::MouseButton button, int x, int y);

        void OnKeyPressed(GTLib::Key key);
        void OnKeyReleased(GTLib::Key key);
        void OnKeyDown(GTLib::Key key, unsigned int repeatCount);
        void OnKeyUp(GTLib::Key key);
        
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