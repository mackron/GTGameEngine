// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/DefaultGameState.hpp>

namespace GT
{
    DefaultGameState::DefaultGameState()
        : GameState(),
          m_pEngineContext(nullptr), m_pGameContext(nullptr)
    {
    }

    DefaultGameState::~DefaultGameState()
    {
        this->Shutdown();
    }



    bool DefaultGameState::Startup(int argc, char** argv)
    {
        if (m_pEngineContext == nullptr)
        {
            m_pEngineContext = new GT::EngineContext();
            if (GT::Succeeded(m_pEngineContext->Startup(argc, argv)))
            {
                m_pGameContext = new GT::GameContext(*m_pEngineContext, *this);
                if (GT::Succeeded(m_pGameContext->Startup()))
                {
                    return true;
                }
                else
                {
                    this->Shutdown();
                }
            }
            else
            {
                this->Shutdown();
            }
        }

        return m_pEngineContext != nullptr;
    }

    void DefaultGameState::Shutdown()
    {
        if (m_pGameContext != nullptr) {
            delete m_pGameContext;
            m_pGameContext = nullptr;
        }

        if (m_pEngineContext != nullptr) {
            delete m_pEngineContext;
            m_pEngineContext = nullptr;
        }
    }

    int DefaultGameState::Run()
    {
        return m_pGameContext->Run();
    }



    ///////////////////////////////////////////
    // Virtual Methods

    bool DefaultGameState::OnStartup(const GTLib::CommandLine &)
    {
        return true;
    }

    void DefaultGameState::OnShutdown()
    {
    }

    void DefaultGameState::OnStep(double)
    {
    }





    ///////////////////////////////////////////
    // Inherited from GT::GameState

    void DefaultGameState::Step(GT::GameContext &, double deltaTimeInSeconds)
    {
        this->OnStep(deltaTimeInSeconds);
    }

    bool DefaultGameState::OnStarting(GT::GameContext &, const GTLib::CommandLine &commandLine)
    {


        return this->OnStartup(commandLine);
    }

    void DefaultGameState::OnClosing(GT::GameContext &)
    {
        this->OnShutdown();
    }
}