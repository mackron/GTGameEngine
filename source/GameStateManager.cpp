// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/GameStateManager.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    GameStateManager::~GameStateManager()
    {
    }


    void GameStateManager::OnLoadConfigs(Game &)
    {
    }

    bool GameStateManager::OnStartup(Game &, const GTLib::CommandLine &)
    {
        return true;
    }

    void GameStateManager::OnShutdown(Game &)
    {
    }


    void GameStateManager::OnUpdate(Game &, double)
    {
    }

    void GameStateManager::OnStartFrame(Game &)
    {
    }

    void GameStateManager::OnEndFrame(Game &)
    {
    }


    void GameStateManager::OnSize(Game &, unsigned int, unsigned int)
    {
    }


    void GameStateManager::OnMouseMove(Game &, int, int)
    {
    }

    void GameStateManager::OnMouseWheel(Game &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonDown(Game &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonUp(Game &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonDoubleClick(Game &, int, int, int)
    {
    }


    void GameStateManager::OnKeyPress(Game &, GTLib::Key)
    {
    }

    void GameStateManager::OnKeyRelease(Game &, GTLib::Key)
    {
    }

    void GameStateManager::OnKeyDown(Game &, GTLib::Key)
    {
    }

    void GameStateManager::OnKeyUp(Game &, GTLib::Key)
    {
    }


    void GameStateManager::OnReceiveFocus(Game &)
    {
    }

    void GameStateManager::OnLoseFocus(Game &)
    {
    }


    void GameStateManager::OnHandleEvent(Game &, GameEvent &)
    {
    }


    void GameStateManager::OnPause(Game &)
    {
    }

    void GameStateManager::OnResume(Game &)
    {
    }


    bool GameStateManager::OnEditorOpening(Game &)
    {
        return true;
    }

    bool GameStateManager::OnEditorClosing(Game &)
    {
        return true;
    }

    void GameStateManager::OnEditorOpen(Game &)
    {
    }

    void GameStateManager::OnEditorClose(Game &)
    {
    }



    bool GameStateManager::LoadScene(Game &, const char*)
    {
        return false;
    }



    bool GameStateManager::Serialize(GTLib::Serializer &)
    {
        return false;
    }

    
    bool GameStateManager::Deserialize(GTLib::Deserializer &)
    {
        return false;
    }







    /////////////////////////////////////
    // GenericGameStateManager

    GenericGameStateManager::GenericGameStateManager()
        : m_sceneData()
    {
    }

    GenericGameStateManager::~GenericGameStateManager()
    {
        for (size_t i = 0; i < m_sceneData.count; ++i)
        {
            delete m_sceneData.buffer[i]->value;
        }
    }


    bool GenericGameStateManager::UpdateSceneState(const char* relativeFilePath, Scene &scene)
    {
        SceneStateStackRestoreCommands* commands = nullptr;
        
        auto iSceneData = m_sceneData.Find(relativeFilePath);
        if (iSceneData != nullptr)
        {
            commands = iSceneData->value;
        }

        if (commands == nullptr)
        {
            commands = new SceneStateStackRestoreCommands(0, 0);
            m_sceneData.Add(relativeFilePath, commands);
        }


        assert(commands != nullptr);
        {
            // Any previous commands need to be removed.
            commands->Clear();

            // TODO: The way memory management works here is not well defined at all. Consider having an allocator / memory manager as part of the state stack.
            scene.GetStateStackStagingAreaRestoreCommands(*commands);
        }


        return true;
    }


    bool GenericGameStateManager::Serialize(GTLib::Serializer &serializer)
    {
        return true;
    }

    bool GenericGameStateManager::Deserialize(GTLib::Deserializer &deserializer)
    {
        return true;
    }
}