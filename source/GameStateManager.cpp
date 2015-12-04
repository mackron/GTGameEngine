// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GameStateManager.hpp>
#include <GTGE/Scene.hpp>
#include <GTGE/GTEngine.hpp>

namespace GT
{
    GameStateManager::~GameStateManager()
    {
    }


    void GameStateManager::OnLoadConfigs(Context &)
    {
    }

    bool GameStateManager::OnStartup(Context &)
    {
        return true;
    }

    void GameStateManager::OnShutdown(Context &)
    {
    }


    void GameStateManager::OnUpdate(Context &, double)
    {
    }

    void GameStateManager::OnStartFrame(Context &)
    {
    }

    void GameStateManager::OnEndFrame(Context &)
    {
    }


    void GameStateManager::OnSize(Context &, unsigned int, unsigned int)
    {
    }


    void GameStateManager::OnMouseMove(Context &, int, int)
    {
    }

    void GameStateManager::OnMouseWheel(Context &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonDown(Context &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonUp(Context &, int, int, int)
    {
    }

    void GameStateManager::OnMouseButtonDoubleClick(Context &, int, int, int)
    {
    }


    void GameStateManager::OnKeyPress(Context &, Key)
    {
    }

    void GameStateManager::OnKeyRelease(Context &, Key)
    {
    }

    void GameStateManager::OnKeyDown(Context &, Key)
    {
    }

    void GameStateManager::OnKeyUp(Context &, Key)
    {
    }


    void GameStateManager::OnReceiveFocus(Context &)
    {
    }

    void GameStateManager::OnLoseFocus(Context &)
    {
    }


    void GameStateManager::OnHandleEvent(Context &, GameEvent &)
    {
    }


    void GameStateManager::OnPause(Context &)
    {
    }

    void GameStateManager::OnResume(Context &)
    {
    }


    bool GameStateManager::OnEditorOpening(Context &)
    {
        return true;
    }

    bool GameStateManager::OnEditorClosing(Context &)
    {
        return true;
    }

    void GameStateManager::OnEditorOpen(Context &)
    {
    }

    void GameStateManager::OnEditorClose(Context &)
    {
    }



    bool GameStateManager::LoadScene(Context &, const char*)
    {
        return false;
    }



    bool GameStateManager::Serialize(Context &, Serializer &)
    {
        return false;
    }

    
    bool GameStateManager::Deserialize(Context &, Deserializer &)
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

    SceneStateStackRestoreCommands* GenericGameStateManager::GetSceneData(const char* relativeFilePath)
    {
        auto iSceneData = m_sceneData.Find(relativeFilePath);
        if (iSceneData != nullptr)
        {
            return iSceneData->value;
        }

        return nullptr;
    }



    bool GenericGameStateManager::Serialize(Context &context, Serializer &serializer)
    {
        // Three chunks:
        //   1 - Header data
        //   2 - Scene data
        //   3 - Global game data

        Serialization::ChunkHeader header;

        // 1 - Header data.
        {
            BasicSerializer headerSerializer;
            uint32_t version = this->SerializeHeaderData(context, headerSerializer);

            header.id          = Serialization::ChunkID_GameStateHeader;
            header.sizeInBytes = headerSerializer.GetBufferSizeInBytes();
            header.version     = version;

            serializer.Write(header);
            serializer.Write(headerSerializer.GetBuffer(), header.sizeInBytes);
        }
        

        // 2 - Scene data.
        {
            BasicSerializer sceneSerializer;

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
            BasicSerializer globalSerializer;
            uint32_t version = this->SerializeGlobalData(context, globalSerializer);

            header.id          = Serialization::ChunkID_GameStateGlobal;
            header.sizeInBytes = globalSerializer.GetBufferSizeInBytes();
            header.version     = version;

            serializer.Write(header);
            serializer.Write(globalSerializer.GetBuffer(), header.sizeInBytes);
        }
        
        


        return true;
    }

    uint32_t GenericGameStateManager::SerializeHeaderData(Context &, Serializer &)
    {
        return 0;
    }

    uint32_t GenericGameStateManager::SerializeGlobalData(Context &, Serializer &)
    {
        return 0;
    }



    bool GenericGameStateManager::Deserialize(Context &context, Deserializer &deserializer)
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
                        this->DeserializeHeaderData(context, deserializer, header.version);
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
                            String sceneRelativePath;
                            deserializer.ReadString(sceneRelativePath);

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
                        this->DeserializeGlobalData(context, deserializer, header.version);
                    }
                }
                deserializer.EndChunk();
            }
        }


        return true;
    }

    bool GenericGameStateManager::DeserializeHeaderData(Context &, Deserializer &, uint32_t)
    {
        return true;
    }

    bool GenericGameStateManager::DeserializeGlobalData(Context &, Deserializer &, uint32_t)
    {
        return true;
    }







    /////////////////////////////////////
    // DefaultGameStateManager

    DefaultGameStateManager::DefaultGameStateManager()
        : m_currentScene(nullptr),
          m_currentSceneRelativePath(),
          m_nextScene(nullptr),
          m_nextSceneRelativePath(),
          m_aspectRatio(0.0f),
          m_isDeserializing(false)
    {
    }

    DefaultGameStateManager::~DefaultGameStateManager()
    {
    }


    bool DefaultGameStateManager::OnStartup(Context &game)
    {
        auto &script = game.GetScript();
        script.ExecuteFile(g_Context->GetVFS(), "game-config.lua");


        // First we'll configure the window...
        auto window = game.GetWindow();
        if (window != nullptr)
        {
            auto title = script.GetString("Game.Title");
            if (title != nullptr)
            {
                window->SetTitle(title);
            }
            else
            {
                window->SetTitle("GTGameEngine");
            }

            int width  = script.GetInteger("Game.Width");
            int height = script.GetInteger("Game.Height");

            if (width  == 0) width  = 1280;
            if (height == 0) height = 720;
            window->SetSize(width, height);
        }

        

        if (script.GetBoolean("Game.Fullscreen"))
        {
            game.EnableFullscreen();
        }


        if (script.GetBoolean("Game.EnableVSync"))
        {
            Renderer::SetSwapInterval(1);
        }
        else
        {
            Renderer::SetSwapInterval(0);
        }


        m_aspectRatio = script.GetFloat("Game.AspectRatio");


        // Load the default scene, if any. To do this, we're just going to call the OnLoadScene() event handler which
        // will handle everything for us. Typically this event handler will get called from the scripting environment
        // to give the game a chance to change scenes.
        //
        // If this fails, we want to just start up the editor.
        if (!this->LoadScene(game, script.GetString("Game.StartupScene")))
        {
            game.OpenEditor();
        }


        return true;
    }


    void DefaultGameStateManager::OnShutdown(Context &context)
    {
        (void)context;

        delete m_currentScene;
        m_currentScene = nullptr;

        m_nextScene = nullptr;
    }

    void DefaultGameStateManager::OnUpdate(Context &context, double deltaTimeInSeconds)
    {
        if (!context.IsEditorOpen())      // <-- Don't bother updating the scene if the editor is open.
        {
            if (m_currentScene != nullptr)
            {
                m_currentScene->Update(deltaTimeInSeconds);
            }


            // With the scene drawn, we should draw a fullscreen triangle over the whole window to show the viewport contents.
            if (m_currentScene != nullptr)
            {
                Renderer::SetCurrentFramebuffer(nullptr);
                Renderer::Utils::DrawFullscreenQuad(m_currentScene->GetDefaultViewport().GetColourBuffer());
            }
        }
        
        //CALLGRIND_STOP_INSTRUMENTATION;
    }

    void DefaultGameStateManager::OnStartFrame(Context &context)
    {
        // If a scene needs to be switched, it needs to be done so now.
        if (m_nextScene != nullptr)
        {
            delete m_currentScene;
            m_currentScene             = m_nextScene;
            m_currentSceneRelativePath = m_nextSceneRelativePath;

            // The default viewport needs to have it's size updated. We're going to simulate an OnSize event to handle this.
            unsigned int viewportWidth;
            unsigned int viewportHeight;
            context.GetWindow()->GetSize(viewportWidth, viewportHeight);
            this->OnSize(context, viewportWidth, viewportHeight);

            // In order for scripting to work we need to register it.
            m_currentScene->PostSceneNodeScriptEvent_OnStartup();


            m_nextScene             = nullptr;
            m_nextSceneRelativePath.Clear();
        }
    }


    void DefaultGameStateManager::OnSize(Context &context, unsigned int newWidth, unsigned int newHeight)
    {
        (void)context;

        if (m_currentScene != nullptr)
        {
            m_currentScene->GetDefaultViewport().Resize(newWidth, newHeight);

            auto cameraNode = m_currentScene->GetDefaultViewport().GetCameraNode();
            if (cameraNode != nullptr)
            {
                auto cameraComponent = cameraNode->GetComponent<CameraComponent>();
                if (cameraComponent != nullptr)
                {
                    float fov    = cameraComponent->perspective.fov;
                    float aspect = m_aspectRatio;
                    float znear  = cameraComponent->zNear;
                    float zfar   = cameraComponent->zFar;

                    if (aspect == 0.0f)
                    {
                        aspect = static_cast<float>(newWidth) / static_cast<float>(newHeight);
                    }

                    cameraComponent->Set3DProjection(fov, aspect, znear, zfar);
                }
            }
        }
    }

    bool DefaultGameStateManager::OnEditorClosing(Context &context)
    {
        (void)context;

        return m_currentScene != nullptr;
    }




    bool DefaultGameStateManager::LoadScene(Context &context, const char* sceneRelativePath)
    {
        // Here we'll load the scene, but we don't actually do the switch until the next frame. The reason for this is that the current scene may be in
        // the middle of updating, so we don't want any mixups there. The actual switch will be done in OnStartFrame(), which is a synchronized call.

        auto newScene = new Scene(context);

        // The scene should be registered to the script immediately. No real reason it shouldn't be. Later on, scenes will take an Context object in their
        // constructors which will then allow us to register it with the script in the constructor.
        newScene->RegisterToScript(context.GetScript());
        
        // Only after registering the scene with the script should we do the load.
        if (newScene->LoadFromFile(sceneRelativePath))
        {
            // The scene state needs to be restored appropriately.
            auto sceneRestoreCommands = this->GetSceneData(sceneRelativePath);
            if (sceneRestoreCommands != nullptr)
            {
                sceneRestoreCommands->Execute(*newScene);
            }


            // The state of the previous scene needs to be updated in the game state manager.
            if (m_currentScene != nullptr)
            {
                // We most likely don't want to update the scene state if we're reloading the same scene. Should probably handle this a bit better just in case
                // this is a legitimate use case for a particular game.
                if (m_currentSceneRelativePath != sceneRelativePath)
                {
                    if (!this->IsDeserializing())
                    {
                        this->UpdateSceneState(m_currentSceneRelativePath.c_str(), *m_currentScene);
                    }
                }
            }


            // The scene loaded successfully. The old scene needs to be replaced.
            m_nextScene             = newScene;
            m_nextSceneRelativePath = sceneRelativePath;

            return true;
        }
        else
        {
            // The scene failed to load. Simply return false.
            delete newScene;
            return false;
        }
    }



    ///////////////////////////////////////
    // Serialization

    bool DefaultGameStateManager::Serialize(Context &context, Serializer &serializer)
    {
        // We need to update the current scene state so that it can be restored properly later.
        if (m_currentScene != nullptr)
        {
            this->UpdateSceneState(m_currentSceneRelativePath.c_str(), *m_currentScene);
        }

        // Now we just call the base implementation of Serialize() like normal.
        return GenericGameStateManager::Serialize(context, serializer);
    }

    uint32_t DefaultGameStateManager::SerializeHeaderData(Context &context, Serializer &serializer)
    {
        (void)context;


        // The name of the current scene - we're going to load this when we deserialize.
        serializer.WriteString(m_currentSceneRelativePath);

        return 1;
    }

    uint32_t DefaultGameStateManager::SerializeGlobalData(Context &context, Serializer &serializer)
    {
        (void)context;


        if (m_currentScene != nullptr)
        {
            m_currentScene->PostSceneNodeScriptEvent_OnSerializeGlobalData(serializer);
        }

        return 1;
    }



    ///////////////////////////////////////
    // Deserialization

    bool DefaultGameStateManager::Deserialize(Context &context, Deserializer &deserializer)
    {
        bool result;

        bool wasDeserializing = m_isDeserializing;
        m_isDeserializing = true;
        {
            // Call the base implementation of Deserialize(). This will call DeserializeHeaderData() and DeserializeGlobalData().
            result = GenericGameStateManager::Deserialize(context, deserializer);
        }
        m_isDeserializing = wasDeserializing;


        return result;
    }

    bool DefaultGameStateManager::DeserializeHeaderData(Context &context, Deserializer &deserializer, uint32_t version)
    {
        (void)context;

        if (version == 1)
        {
            deserializer.ReadString(m_nextSceneRelativePath);

            return true;
        }
        
        return false;
    }

    bool DefaultGameStateManager::DeserializeGlobalData(Context &context, Deserializer &deserializer, uint32_t version)
    {
        (void)context;
        (void)deserializer;

        if (version == 1)
        {
            // In order to deserialize the global game data, there needs to be a scene ready to go. We do this by loading the scene here.
            if (this->LoadScene(context, m_nextSceneRelativePath.c_str()))
            {
                assert(m_nextScene != nullptr);
                {
                    m_nextScene->PostSceneNodeScriptEvent_OnDeserializeGlobalData(deserializer);
                }

                return true;
            }
        }

        return false;
    }
}