
#include <GTEngine/GameWindowEventHandler.hpp>
#include <GTEngine/Game.hpp>

namespace GTEngine
{
    GameWindowEventHandler::GameWindowEventHandler(Game &game)
        : game(game)
    {
    }

    void GameWindowEventHandler::OnClose()
    {
        GameEvent e;
        e.code = EventCodes::OnClose;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnSize(unsigned int width, unsigned int height)
    {
        GameEvent e;
        e.code = EventCodes::OnSize;
        e.size.width  = width;
        e.size.height = height;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseMove(int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseMove;
        e.mousemove.x = x;
        e.mousemove.y = y;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseWheel(int delta, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseWheel;
        e.mousewheel.delta = delta;
        e.mousewheel.x     = x;
        e.mousewheel.y     = y;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonDown(GTCore::MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonDown;
        e.mousedown.button = button;
        e.mousedown.x      = x;
        e.mousedown.y      = y;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonUp(GTCore::MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonUp;
        e.mouseup.button = button;
        e.mouseup.x      = x;
        e.mouseup.y      = y;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnMouseButtonDoubleClick(GTCore::MouseButton button, int x, int y)
    {
        GameEvent e;
        e.code = EventCodes::OnMouseButtonDoubleClick;
        e.mousedoubleclick.button = button;
        e.mousedoubleclick.x      = x;
        e.mousedoubleclick.y      = y;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyPressed(GTCore::Key key)
    {
        GameEvent e;
        e.code           = EventCodes::OnKeyPressed;
        e.keypressed.key = key;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyReleased(GTCore::Key key)
    {
        GameEvent e;
        e.code           = EventCodes::OnKeyReleased;
        e.keyreleased.key = key;

        this->game.SendEvent(e);
    }

    void GameWindowEventHandler::OnKeyDown(GTCore::Key key, unsigned int repeatCount)
    {
        GameEvent e;
        e.code        = EventCodes::OnKeyDown;
        e.keydown.key = key;

        for (unsigned int i = 0; i < repeatCount; ++i)
        {
            this->game.SendEvent(e);
        }
    }

    void GameWindowEventHandler::OnKeyUp(GTCore::Key key)
    {
        GameEvent e;
        e.code = EventCodes::OnKeyUp;
        e.keyup.key = key;

        this->game.SendEvent(e);
    }
}