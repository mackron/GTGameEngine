// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GTEngine.hpp>
#include <GTGE/Game.hpp>
#include <GTGE/ApplicationConfig.hpp>
#include <GTGE/Rendering.hpp>
#include <GTGE/ShaderLibrary.hpp>
#include <GTGE/Texture2DLibrary.hpp>
#include <GTGE/MaterialLibrary.hpp>
#include <GTGE/ModelLibrary.hpp>
#include <GTGE/VertexArrayLibrary.hpp>
#include <GTGE/PrefabLibrary.hpp>
#include <GTGE/ParticleSystemLibrary.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/ThreadCache.hpp>
#include <GTGE/MessageDispatcher.hpp>
#include <GTGE/Core/WindowManagement.hpp>

// Basic globals.
namespace GT
{
    /// A pointer to the global game object.
    Game* GlobalGame = nullptr;

    /// The global engine context. TODO: Remove this and have applications create this themselves. This global object is only used during the transition phase.
    GT::EngineContext* g_EngineContext = nullptr;
}

namespace GT
{
    bool _PreStartup()
    {
        // Before we can do any windowing operations we will need to initialise the window management module of GTLib.
        StartupWindowManager();


        // Here we'll startup the thread cache. We will do this before starting the sub-systems so that they themselves can do some
        // multithreaded initialisation if needed.
        ThreadCache::Startup();


        // With the log file created, we can startup all of our other sub-systems.
        g_EngineContext->Logf("Starting Rendering Sub-System...");
        if (Renderer::Startup())
        {
            g_EngineContext->Logf("Renderer Caps:");
            g_EngineContext->Logf("    Max Colour Attachments: %d", Renderer::GetMaxColourAttachments());
            g_EngineContext->Logf("    Max Draw Buffers:       %d", Renderer::GetMaxDrawBuffers());
            g_EngineContext->Logf("    Max Texture Units:      %d", Renderer::GetMaxTextureUnits());

            g_EngineContext->Logf("Loading Shaders...");
            ShaderLibrary::LoadFromDirectory("engine/shaders/glsl");
            ShaderLibrary::LoadFromDirectory("shaders/glsl");
        }
        else
        {
            return false;
        }


        // With sub-systems started up, we can startup our resource libraries.
        g_EngineContext->Logf("Initializing Texture Library...");
        Texture2DLibrary::Startup();

        g_EngineContext->Logf("Initializing Material Library...");
        MaterialLibrary::Startup();

        g_EngineContext->Logf("Initializing Vertex Array Library...");
        VertexArrayLibrary::Startup();

        g_EngineContext->Logf("Initializing Model Library...");
        ModelLibrary::Startup();

        g_EngineContext->Logf("Initializing Prefab Library...");
        PrefabLibrary::Startup();

        g_EngineContext->Logf("Initializing Particle System Library...");
        ParticleSystemLibrary::Startup();

        g_EngineContext->Logf("Initializing Script Library...");
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


        // Thread cache.
        ThreadCache::Shutdown();
        
        // GTLib's window management module.
        ShutdownWindowManager();


        /// The engine context.
        delete g_EngineContext;
        g_EngineContext = nullptr;
    }
}
