// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ApplicationConfig_hpp_
#define __GTEngine_ApplicationConfig_hpp_

#include <GTLib/Vector.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Script.hpp>
#include <easy_fs/easy_vfs.h>

namespace GT
{
    namespace Engine
    {
        class ApplicationConfig
        {
        public:

            /// Constructor.
            ApplicationConfig();

            /// Destructor.
            ~ApplicationConfig();

    
            /// Opens the application configuration from the given file.
            ///
            /// @param  fileName [in] The name of the config file to load.
            ///
            /// @remarks
            ///     You must call Close() when finished with the configuration.
            bool Open(easyvfs_context* pContext, const char* fileName);
        

            /// Retrieves a reference to the list containing the data directories.
            const GTLib::Vector<GTLib::String> & GetDataDirectories() const;

            /// Copies the data directory strings from the internal list into the given list.
            ///
            /// @param directoriesOut [out] A reference the list that the directories will be copied into.
            ///
            /// @remarks
            ///     The values will be pushed to the back. Existing items will not be cleared.
            void GetDataDirectories(GTLib::Vector<GTLib::String> &directories) const;



        private:

            /// The script object that will be used to read the config file.
            GTLib::Script m_script;

            /// The list of data directories.
            GTLib::Vector<GTLib::String> m_dataDirectories;


        private:    // No copying.
            ApplicationConfig(const ApplicationConfig &);
            ApplicationConfig & operator=(const ApplicationConfig &);
        };
    }
}

#endif
