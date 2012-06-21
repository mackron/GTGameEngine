
#ifndef __GTEngine_hpp_
#define __GTEngine_hpp_

// Startup/Shutdown.
namespace GTEngine
{
    /// Starts up the engine.
    ///
    /// @param argc [in] The argument count.
    /// @param argv [in] The argument strings.
    ///
    /// @return True if the engine is initialised successfully; false otherwise.
    bool Startup(int argc, char** argv);

    /// Shuts down the engine.
    void Shutdown();
}


#endif