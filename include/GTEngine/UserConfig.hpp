
#ifndef __GTEngine_UserConfig_hpp_
#define __GTEngine_UserConfig_hpp_

#include <GTCore/Script.hpp>

namespace GTEngine
{
    /// Class representing the user configuration.
    ///
    /// The first function called on this class should be Startup(). This will load the default values of all properties,
    /// which in turn allows a config file to skip properties that it doesn't care about. There are two sets of default
    /// values: engine/server defaults, and client defaults. The engine/server defaults are default values as set by
    /// GTEngine. Client defaults are default values as set by the client application.
    class UserConfig
    {
    // Starup/Shutdown.
    public:

        /// Starts up the user configuration, loading both the engine/server and client defaults.
        /// @param clientDefaultConfig [in] A string containing the default config for the client application. This is NOT a file name, but the actual config script.
        /// @return True if the defaults are loaded successfully; false otherwise.
        ///
        /// @remarks
        ///    This does not load any config files, only the defaults. Use LoadFile() to load the config file.
        static bool Startup(const char* clientDefaultConfig);

        /// Shuts down the user configuration.
        static void Shutdown();


    // Loading.
    public:

        /// Loads a config script.
        /// @param config [in] The config script to load. This is NOT a file name, but the actual config script.
        /// @return True if the script is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///    Use LoadFile() if you need to load from a file instead of a raw script.
        static bool Load(const char* config);

        /// Loads a config file from the current directory.
        /// @param filename [in] The name of the file to load.
        /// @return True if the file is loaded successfully; false otherwise.
        static bool LoadFile(const char* filename);

        
    // Property retrieval.
    public:

        /// Retrieves a property from the config.
        /// @param propertyName [in] The name of the property as a string.
        /// @return The value of the property if it exists, or a logical default if it does not exist.
        static const char* GetString( const char* propertyName);
        static int         GetInteger(const char* propertyName);
        static double      GetDouble( const char* propertyName);
        static float       GetFloat(  const char* propertyName) { return static_cast<float>(GetDouble(propertyName)); }
        static bool        GetBoolean(const char* propertyName);
    };
}

#endif