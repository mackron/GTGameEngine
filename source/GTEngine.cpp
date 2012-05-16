
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/UserConfig.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/ThreadCache.hpp>
#include <GTCore/CommandLine.hpp>
#include <GTCore/IO.hpp>

// Basic globals.
namespace GTEngine
{
    /// Keeps track of whether or not a Game object has been created. Currently, only a single game object can be instantiated
    /// for each application. The reason is due to how the Game class uses global/static classes such as GarbageCollector and
    /// Renderer. The use of these static classes may be reassessed later, but for now we will restrict each application to a
    /// single Game object, which shouldn't be much of a problem at all...
    bool IsGameObjectCreated = false;
}

namespace GTEngine
{
    bool Startup(int argc, char** argv, const char* clientDefaultConfig)
    {
        // First this is to more into the applications directory. We get this from the command line.
        GTCore::CommandLine cmdLine(argc, argv);
        GTCore::IO::SetCurrentDirectory(cmdLine.GetApplicationDirectory());

        // After moving into the application directory, we need to load up the config file and move into the data directory. From
        // there we can read the user configs and setup the log file.
        //
        // This is different from a user configuration (which are located in the 'configs' folder). The application configuration
        // usually remains constant. It defines things like directories. We won't return false if we fail to open, in which case
        // the game will use defaults.
        if (ApplicationConfig::Open("config.lua"))
        {
            // The application config will define the Data directory. This is the directory where all of the game's data is
            // located - textures, sounds, levels, user configs, etc. Since everything is now relative to the Data directory,
            // we'll move into it here.
            GTCore::IO::SetCurrentDirectory(ApplicationConfig::Directories::Data());
        }

        // Now that we're in the correct directory, we can startup the user configuration.
        if (UserConfig::Startup(clientDefaultConfig))
        {
            const char** cmdLine_config = cmdLine.GetArgument("config");
            if (cmdLine_config != nullptr)
            {
                UserConfig::LoadFile(cmdLine_config[0]);
            }
        }


        // We need to initialise our logging stuff before starting up any major sub-systems, such as the renderer.
        Logging::Startup(UserConfig::GetString("LogFile"));

        
        // Here we'll startup the thread cache. We will do this before starting the sub-systems so that they themselves can do some
        // multithreaded initialisation if needed.
        ThreadCache::Startup();


        // With the log file created, we can startup all of our other sub-systems.
        Log("Starting Rendering Sub-System...");
        if (Renderer::Startup())
        {
            Log("Loading Shaders...");
            ShaderLibrary::LoadFromDirectory("engine/shaders/cg");
            ShaderLibrary::LoadFromDirectory("shaders/cg");
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

        Log("Initializing Model Library...");
        ModelLibrary::Startup();


        return true;
    }

    void Shutdown()
    {
        // We kill our libraries before the major sub-systems.
        ModelLibrary::Shutdown();
        MaterialLibrary::Shutdown();
        ShaderLibrary::Shutdown();
        Texture2DLibrary::Shutdown();
        

        // We shutdown major sub-systems before logging. This allows us to log shutdown info.
        Renderer::Shutdown();
        AudioComposer::Shutdown();

        // Now we can shutdown the minor sub-systems, remembering to do logging last.
        UserConfig::Shutdown();
        Logging::Shutdown();

        // Thread cache.
        ThreadCache::Shutdown();
    }
}
