
#ifndef __GTEngine_ApplicationConfig_hpp_
#define __GTEngine_ApplicationConfig_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/String.hpp>

namespace GTEngine
{
    class ApplicationConfig
    {
    public:
    
        /// Opens the application configuration from the given file.
        ///
        /// @param  fileName [in] The name of the config file to load.
        ///
        /// @remarks
        ///     You must call Close() when finished with the configuration.
        static bool Open(const char* fileName);
        
        /// Closes the application configuration.
        static void Close();


        /// Retrieves a reference to the list containing the data directories.
        static const GTCore::Vector<GTCore::String> & GetDataDirectories();
        static void GetDataDirectories(GTCore::Vector<const char*> &directories);
    };
}

#endif
