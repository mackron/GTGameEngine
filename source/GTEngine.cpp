// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/GTEngine.hpp>
#include <GTGE/Rendering.hpp>
#include <GTGE/ShaderLibrary.hpp>
#include <GTGE/Texture2DLibrary.hpp>
#include <GTGE/MaterialLibrary.hpp>
#include <GTGE/ModelLibrary.hpp>
#include <GTGE/VertexArrayLibrary.hpp>
#include <GTGE/PrefabLibrary.hpp>
#include <GTGE/ParticleSystemLibrary.hpp>
#include <GTGE/ScriptLibrary.hpp>
#include <GTGE/MessageDispatcher.hpp>
#include <GTGE/Core/WindowManagement.hpp>


namespace GT
{
    /// The global engine context. TODO: Remove this and have applications create this themselves. This global object is only used during the transition phase.
    GT::Context* g_Context = nullptr;


    bool _PreStartup(Context* pContext)
    {
        // Before we can do any windowing operations we will need to initialise the window management module of GTLib.
        StartupWindowManager();


        // With the log file created, we can startup all of our other sub-systems.
        g_Context->Logf("Starting Rendering Sub-System...");
        if (Renderer::Startup())
        {
            g_Context->Logf("Renderer Caps:");
            g_Context->Logf("    Max Colour Attachments: %d", Renderer::GetMaxColourAttachments());
            g_Context->Logf("    Max Draw Buffers:       %d", Renderer::GetMaxDrawBuffers());
            g_Context->Logf("    Max Texture Units:      %d", Renderer::GetMaxTextureUnits());

            g_Context->Logf("Loading Shaders...");
            ShaderLibrary::LoadFromDirectory("engine/shaders/glsl");
            ShaderLibrary::LoadFromDirectory("shaders/glsl");
        }
        else
        {
            return false;
        }


        // With sub-systems started up, we can startup our resource libraries.
        g_Context->Logf("Initializing Texture Library...");
        Texture2DLibrary::Startup();

        g_Context->Logf("Initializing Material Library...");
        MaterialLibrary::Startup();

        g_Context->Logf("Initializing Vertex Array Library...");
        VertexArrayLibrary::Startup();

        g_Context->Logf("Initializing Model Library...");
        ModelLibrary::Startup();

        g_Context->Logf("Initializing Prefab Library...");
        PrefabLibrary::Startup();

        g_Context->Logf("Initializing Particle System Library...");
        ParticleSystemLibrary::Startup();

        g_Context->Logf("Initializing Script Library...");
        ScriptLibrary::Startup(pContext);


        return true;
    }


    void Shutdown(Context* pContext)
    {
        if (pContext != nullptr)
        {
            pContext->Shutdown();
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


        // GTLib's window management module.
        ShutdownWindowManager();
        

        /// Delete the engine context for real.
        delete g_Context;
        g_Context = nullptr;
    }
}
