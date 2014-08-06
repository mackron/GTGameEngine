// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/PrefabLibrary.hpp>
#include <GTEngine/ParticleSystemLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTEngine/MessageDispatcher.hpp>
#include <GTLib/CommandLine.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/WindowManagement.hpp>

//#include "Audio/OpenAL/AudioEngine_OpenAL.hpp"

// Basic globals.
namespace GTEngine
{
    /// A pointer to the global game object.
    Game* GlobalGame = nullptr;

    /// The global engine context. TODO: Remove this and have applications create this themselves. This global object is only used during the transition phase.
    GT::Engine::EngineContext* g_EngineContext = nullptr;
}

namespace GTEngine
{
    bool _PreStartup(const GTLib::CommandLine &commandLine)
    {
        (void)commandLine;

#if 0
        AudioEngine_OpenAL audioEngine;
        if (audioEngine.Startup())
        {
            PlaybackDeviceHandle device = audioEngine.OpenPlaybackDevice(0);
            if (device != 0)
            {
                audioEngine.ClosePlaybackDevice(device);
            }
            else
            {
                printf("Failed to open playback device.\n");
            }

            audioEngine.Shutdown();
        }
        else
        {
            printf("Failed to initialize OpenAL audio engine.\n");
        }
#endif
        
        
        // Before we can do any windowing operations we will need to initialise the window management module of GTLib.
        GTLib::StartupWindowManager();


        // Here we'll startup the thread cache. We will do this before starting the sub-systems so that they themselves can do some
        // multithreaded initialisation if needed.
        ThreadCache::Startup();


        // With the log file created, we can startup all of our other sub-systems.
        Log("Starting Rendering Sub-System...");
        if (Renderer::Startup())
        {
            Log("Renderer Caps:");
            Log("    Max Colour Attachments: %d", Renderer::GetMaxColourAttachments());
            Log("    Max Draw Buffers:       %d", Renderer::GetMaxDrawBuffers());
            Log("    Max Texture Units:      %d", Renderer::GetMaxTextureUnits());

            Log("Loading Shaders...");
            ShaderLibrary::LoadFromDirectory("engine/shaders/glsl");
            ShaderLibrary::LoadFromDirectory("shaders/glsl");
        }
        else
        {
            return false;
        }

#if 0
        Log("Starting Audio Sub-System...");
        if (AudioComposer::Startup())
        {
            // Perhaps allow some cached sounds?
        }
        else
        {
            return false;
        }
#endif


        // With sub-systems started up, we can startup our resource libraries.
        Log("Initializing Texture Library...");
        Texture2DLibrary::Startup();

        Log("Initializing Material Library...");
        MaterialLibrary::Startup();

        Log("Initializing Vertex Array Library...");
        VertexArrayLibrary::Startup();

        Log("Initializing Model Library...");
        ModelLibrary::Startup();

        Log("Initializing Prefab Library...");
        PrefabLibrary::Startup();

        Log("Initializing Particle System Library...");
        ParticleSystemLibrary::Startup();

        Log("Initializing Script Library...");
        ScriptLibrary::Startup();



        return true;
    }

    void Shutdown(Game* game)
    {
        if (game != nullptr)
        {
            assert(game == GlobalGame);

            game->Shutdown();
            delete game;

            GlobalGame = nullptr;
        }



        // We kill our libraries before the major sub-systems.
        ModelLibrary::Shutdown();
        MaterialLibrary::Shutdown();
        ShaderLibrary::Shutdown();
        Texture2DLibrary::Shutdown();
        VertexArrayLibrary::Shutdown();
        PrefabLibrary::Shutdown();
        ParticleSystemLibrary::Shutdown();
        ScriptLibrary::Shutdown();

        // We shutdown major sub-systems before logging. This allows us to log shutdown info.
        Renderer::Shutdown();
        //AudioComposer::Shutdown();


        // Thread cache.
        ThreadCache::Shutdown();
        
        // GTLib's window management module.
        GTLib::ShutdownWindowManager();


        /// The engine context.
        delete g_EngineContext;
        g_EngineContext = nullptr;
    }


    const char* GetExecutableDirectory()
    {
        if (g_EngineContext != nullptr)
        {
            return g_EngineContext->GetExecutableDirectoryAbsolutePath();
        }

        return nullptr;
    }
}
