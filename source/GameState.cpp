
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

    void GameState::OnMouseButtonDown(GTCore::MouseButton, int, int)
    {
    }

    void GameState::OnMouseButtonUp(GTCore::MouseButton, int, int)
    {
    }

    void GameState::OnMouseButtonDoubleClick(GTCore::MouseButton, int, int)
    {
    }

    void GameState::OnKeyPressed(GTCore::Key)
    {
    }

    void GameState::OnKeyReleased(GTCore::Key)
    {
    }
    
    void GameState::OnPause()
    {
    }

    void GameState::OnResume()
    {
    }
}