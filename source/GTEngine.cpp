// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Rendering.hpp>
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
#include <GTEngine/Core/WindowManagement.hpp>

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


        // Thread cache.
        ThreadCache::Shutdown();
        
        // GTLib's window management module.
        ShutdownWindowManager();


        /// The engine context.
        delete g_EngineContext;
        g_EngineContext = nullptr;
    }
}
