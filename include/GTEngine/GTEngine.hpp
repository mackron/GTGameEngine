
#ifndef __GTEngine_hpp_
#define __GTEngine_hpp_

#include "UserConfig.hpp"

// Startup/Shutdown.
namespace GTEngine
{
    /// Starts up the engine.
    /// @param argc                [in] The argument count.
    /// @param argv                [in] The argument strings.
    /// @param clientDefaultConfig [in] A null-terminated string containing the applications default configuration. See remarks.
    /// @return True if the engine is initialised successfully; false otherwise.
    ///
    /// @remarks
    ///     This will load default configuration script before loading the configuration file. This allows a game
    ///     to define default values for it's configuration.
    bool Startup(int argc, char** argv, const char* clientDefaultConfig = nullptr);

    /// Shuts down the engine.
    void Shutdown();
}


#endif