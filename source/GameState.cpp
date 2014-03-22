// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/GameState.hpp>

namespace GTEngine
{
    GameState::GameState()
        : isInitialised(false)
    {
    }

    GameState::~GameState()
    {
    }

    void GameState::Activate()
    {
        if (!this->isInitialised)
        {
            this->OnStartup();
            this->isInitialised = true;
        }

        this->OnActivate();
    }

    void GameState::Deactivate()
    {
        this->OnDeactivate();
    }



    /////////////////////////////////////////////////
    // Default Event Implementations.

    void GameState::OnStartup()
    {
    }

    void GameState::OnActivate()
    {
    }

    void GameState::OnDeactivate()
    {
    }

    
    void GameState::OnUpdate(double)
    {
    }

    void GameState::OnMouseMove(int, int)
    {
    }

    void GameState::OnMouseWheel(int, int, int)
    {
    }

    void GameState::OnMouseButtonDown(GTLib::MouseButton, int, int)
    {
    }

    void GameState::OnMouseButtonUp(GTLib::MouseButton, int, int)
    {
    }

    void GameState::OnMouseButtonDoubleClick(GTLib::MouseButton, int, int)
    {
    }

    void GameState::OnKeyPressed(GTLib::Key)
    {
    }

    void GameState::OnKeyReleased(GTLib::Key)
    {
    }
    
    void GameState::OnPause()
    {
    }

    void GameState::OnResume()
    {
    }

    void GameState::OnSize(unsigned int, unsigned int)
    {
    }
}