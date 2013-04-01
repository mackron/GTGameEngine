// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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
#include <GTEngine/SceneNodeClassLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTEngine/FontManager.hpp>
#include <GTCore/CommandLine.hpp>
#include <GTCore/IO.hpp>

// Basic globals.
namespace GTEngine
{
    /// A pointer to the global game object.
    Game* GlobalGame = nullptr;

    /// Keeps track of the executable directory.
    static GTCore::String ExecutableDirectory;
}

namespace GTEngine
{
    bool _PreStartup(int argc, char** argv)
    {
        // First this is to more into the applications directory. We get this from the command line.
        GTCore::CommandLine cmdLine(argc, argv);
        GTCore::IO::SetCurrentDirectory(cmdLine.GetApplicationDirectory());

        // We need to keep hold of the executable directory for GetExecutableDirectory().
        ExecutableDirectory = GTCore::IO::ToAbsolutePath(cmdLine.GetApplicationDirectory(), GTCore::IO::GetCurrentDirectory());

        // After moving into the application directory, we need to load up the config file and move into the data directory. From
        // there we can read the user configs and setup the log file.
        //
        // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
        // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
        // the game will use defaults.
        if (ApplicationConfig::Open("config.lua"))
        {
            // The application config will define the data directories where all of the game's data and assets are located. We will
            // move into the directory of the first defined data directory.
            auto &directories = ApplicationConfig::GetDataDirectories();
            if (directories.count > 0)
            {
                GTCore::IO::SetCurrentDirectory(directories[0].c_str());

                // Here we are going to set additional search directories which will make GTCore search these directories if it can not
                // open a file from the current directory. We intentionally don't include the first directory.
                for (size_t i = 1; i < directories.count; ++i)
                {
                    GTCore::IO::AddAdditionalSearchPath(directories[i].c_str());
                }
            }
        }


        // We need to initialise our logging stuff before starting up any major sub-systems, such as the renderer. The log file will be specified
        // as a command line option, else we will use the default value of 'var/logs/engine.html'
        const char** cmdLine_logfile = cmdLine.GetArgument("logfile");
        if (cmdLine_logfile != nullptr)
        {
            Logging::Startup(cmdLine_logfile[0]);
        }
        else
        {
            Logging::Startup("var/logs/engine.html");
        }


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

        Log("Starting Audio Sub-System...");
        if (AudioComposer::Startup())
        {
            // Perhaps allow some cached sounds?
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

        Log("Initializing Scene Node Class Library...");
        SceneNodeClassLibrary::Startup();

        Log("Initializing Script Library...");
        ScriptLibrary::Startup();


        // The font manager.
        Log("Initializing Font Manager...");
        FontManager::Startup();


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


        // Font manager.
        FontManager::Shutdown();


        // We kill our libraries before the major sub-systems.
        ModelLibrary::Shutdown();
        VertexArrayLibrary::Shutdown();
        MaterialLibrary::Shutdown();
        Texture2DLibrary::Shutdown();
        ShaderLibrary::Shutdown();
        SceneNodeClassLibrary::Shutdown();
        ScriptLibrary::Shutdown();
        


        // We shutdown major sub-systems before logging. This allows us to log shutdown info.
        Renderer::Shutdown();
        AudioComposer::Shutdown();


        // Now we can shutdown the minor sub-systems, remembering to do logging last.
        Logging::Shutdown();

        // Thread cache.
        ThreadCache::Shutdown();

        // Application config.
        ApplicationConfig::Close();
    }


    const char* GetExecutableDirectory()
    {
        return ExecutableDirectory.c_str();
    }
}
