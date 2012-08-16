
#ifndef __GTEngine_Logging_hpp_
#define __GTEngine_Logging_hpp_

namespace GTEngine
{
    namespace Logging
    {
        /// Starts up the logging sub-system.
        void Startup(const char* filename);

        /// Shuts down the logging component of the engine. There is no need for a Startup function since Log() will do
        /// any initialisation when required.
        ///
        /// This function will be called by GTEngine::Shutdown().
        void Shutdown();
    }

    /// Logs a message.
    void LogString(const char* value);
    void Log(const char* format, ...);
}

#endif