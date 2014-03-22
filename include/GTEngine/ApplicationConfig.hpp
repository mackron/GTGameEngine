// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ApplicationConfig_hpp_
#define __GTEngine_ApplicationConfig_hpp_

#include <GTLib/Vector.hpp>
#include <GTLib/String.hpp>

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
        static const GTLib::Vector<GTLib::String> & GetDataDirectories();
        static void GetDataDirectories(GTLib::Vector<const char*> &directories);
    };
}

#endif
