// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameState.hpp>

namespace GT
{
    GameState::GameState()
    {
    }

    GameState::~GameState()
    {
    }



    void GameState::Serialize(GameContext &, GTLib::Serializer &)
    {
    }

    void GameState::Deserialize(GameContext &, GTLib::Deserializer &)
    {
    }



    unsigned int GameState::GetWindowCount(GameContext &) const
    {
        return 0;
    }

    HWindow GameState::GetWindowByIndex(GameContext &, unsigned int)
    {
        return NULL;
    }


    void GameState::Step(double)
    {
    }



    /////////////////////////////////
    // Events

    bool GameState::OnStarting(GameContext &, const GTLib::CommandLine &)
    {
        return true;
    }

    void GameState::OnClosing(GameContext &)
    {
    }

    bool GameState::OnWantToClose(GameContext &)
    {
        return true;
    }

    void GameState::OnWindowResized(GameContext &, HWindow, unsigned int, unsigned int)
    {
    }

    void GameState::OnWindowMoved(GameContext &, HWindow, int, int)
    {
    }

    void GameState::OnMouseMove(GameContext &, HWindow, int, int)
    {
    }

    void GameState::OnMouseButtonPressed(GameContext &, HWindow, int, int, int)
    {
    }

    void GameState::OnMouseButtonReleased(GameContext &, HWindow, int, int, int)
    {
    }

    void GameState::OnMouseButtonDoubleClicked(GameContext &, HWindow, int, int, int)
    {
    }

    void GameState::OnKeyPressed(GameContext &, HWindow, GTLib::Key)
    {
    }

    void GameState::OnKeyReleased(GameContext &, HWindow, GTLib::Key)
    {
    }

    void GameState::OnPrintableKeyDown(GameContext &, HWindow, char32_t)
    {
    }


    bool GameState::OnWantToOpenEditor(GameContext &)
    {
        return true;
    }

    void GameState::OnEditorOpened(GameContext &)
    {
    }

    void GameState::OnEditorClosed(GameContext &)
    {
    }
}
