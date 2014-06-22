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



    bool GameStateManager::Serialize(Game &, GTLib::Serializer &)
    {
        return false;
    }

    
    bool GameStateManager::Deserialize(Game &, GTLib::Deserializer &)
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



    bool GenericGameStateManager::Serialize(Game &game, GTLib::Serializer &serializer)
    {
        // Three chunks:
        //   1 - Header data
        //   2 - Scene data
        //   3 - Global game data

        Serialization::ChunkHeader header;

        // 1 - Header data.
        {
            GTLib::BasicSerializer headerSerializer;
            uint32_t version = this->SerializeHeaderData(game, headerSerializer);

            header.id          = Serialization::ChunkID_GameStateHeader;
            header.sizeInBytes = headerSerializer.GetBufferSizeInBytes();
            header.version     = version;

            serializer.Write(header);
            serializer.Write(headerSerializer.GetBuffer(), header.sizeInBytes);
        }
        

        // 2 - Scene data.
        {
            GTLib::BasicSerializer sceneSerializer;

            // The scene data count.
            sceneSerializer.Write(static_cast<uint32_t>(m_sceneData.count));

            // The actual scene data.
            for (size_t iSceneData = 0; iSceneData < m_sceneData.count; ++iSceneData)
            {
                auto relativePath = m_sceneData.buffer[iSceneData]->key;
                auto sceneData    = m_sceneData.buffer[iSceneData]->value;

                sceneSerializer.WriteString(relativePath);
                sceneData->Serialize(sceneSerializer);
            }


            header.id          = Serialization::ChunkID_GameStateScenes;
            header.sizeInBytes = sceneSerializer.GetBufferSizeInBytes();
            header.version     = 0;

            serializer.Write(header);
            serializer.Write(sceneSerializer.GetBuffer(), header.sizeInBytes);
        }


        // 3 - Global data.
        {
            GTLib::BasicSerializer globalSerializer;
            uint32_t version = this->SerializeGlobalData(game, globalSerializer);

            header.id          = Serialization::ChunkID_GameStateGlobal;
            header.sizeInBytes = globalSerializer.GetBufferSizeInBytes();
            header.version     = version;

            serializer.Write(header);
            serializer.Write(globalSerializer.GetBuffer(), header.sizeInBytes);
        }
        
        


        return true;
    }

    uint32_t GenericGameStateManager::SerializeHeaderData(Game &game, GTLib::Serializer &deserializer)
    {
        return 0;
    }

    uint32_t GenericGameStateManager::SerializeGlobalData(Game &game, GTLib::Serializer &serializer)
    {
        return 0;
    }



    bool GenericGameStateManager::Deserialize(Game &game, GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;


        // 1 - Header data
        {
            if (deserializer.Peek(&header, sizeof(header)) == sizeof(header))
            {
                deserializer.Seek(sizeof(header));

                deserializer.StartChunk(header.sizeInBytes);
                {
                    if (header.id == Serialization::ChunkID_GameStateHeader)
                    {
                        this->DeserializeHeaderData(game, deserializer, header.version);
                    }
                }
                deserializer.EndChunk();
            }
        }


        // 2 - Scene data
        {
            if (deserializer.Peek(&header, sizeof(header)) == sizeof(header))
            {
                deserializer.Seek(sizeof(header));

                deserializer.StartChunk(header.sizeInBytes);
                {
                    if (header.id == Serialization::ChunkID_GameStateScenes)
                    {
                        uint32_t sceneDataCount;
                        deserializer.Read(sceneDataCount);

                        for (uint32_t iSceneData = 0; iSceneData < sceneDataCount; ++iSceneData)
                        {
                            GTLib::String sceneRelativePath;
                            deserializer.Read(sceneRelativePath);

                            auto sceneData = new SceneStateStackRestoreCommands(0, 0);
                            sceneData->Deserialize(deserializer);

                            m_sceneData.Add(sceneRelativePath.c_str(), sceneData);
                        }
                    }
                }
                deserializer.EndChunk();
            }
        }


        // 3 - Global data
        {
            if (deserializer.Peek(&header, sizeof(header)) == sizeof(header))
            {
                deserializer.Seek(sizeof(header));

                deserializer.StartChunk(header.sizeInBytes);
                {
                    if (header.id == Serialization::ChunkID_GameStateGlobal)
                    {
                        this->DeserializeGlobalData(game, deserializer, header.version);
                    }
                }
                deserializer.EndChunk();
            }
        }


        return true;
    }

    bool GenericGameStateManager::DeserializeHeaderData(Game &game, GTLib::Deserializer &deserializer, uint32_t version)
    {
        return true;
    }

    bool GenericGameStateManager::DeserializeGlobalData(Game &game, GTLib::Deserializer &serializer, uint32_t version)
    {
        return true;
    }
}