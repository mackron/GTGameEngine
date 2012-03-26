
#ifndef __GTEngine_GameWindowEventHandler_hpp_
#define __GTEngine_GameWindowEventHandler_hpp_

#include <GTWindow/WindowEventHandler.hpp>

namespace GTEngine
{
    class Game;

    class GameWindowEventHandler : public GTWindow::WindowEventHandler
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
        void OnMouseButtonDown(GTWindow::MouseButton button, int x, int y);
        void OnMouseButtonUp(GTWindow::MouseButton button, int x, int y);
        void OnMouseButtonDoubleClick(GTWindow::MouseButton button, int x, int y);

        void OnKeyPressed(GTWindow::Key key);
        void OnKeyReleased(GTWindow::Key key);
        void OnKeyDown(GTWindow::Key key, unsigned int repeatCount);
        void OnKeyUp(GTWindow::Key key);
        
        
        void OnReceiveFocus()   // OnFocus
        {
        }
        
        void OnLoseFocus()      // OnBlur
        {
        }


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