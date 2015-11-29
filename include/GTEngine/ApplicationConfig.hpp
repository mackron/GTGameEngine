// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ApplicationConfig
#define GT_ApplicationConfig

#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/String.hpp>
#include <GTEngine/Script.hpp>
#include <easy_fs/easy_vfs.h>

namespace GT
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
        const Vector<String> & GetDataDirectories() const;

        /// Copies the data directory strings from the internal list into the given list.
        ///
        /// @param directoriesOut [out] A reference the list that the directories will be copied into.
        ///
        /// @remarks
        ///     The values will be pushed to the back. Existing items will not be cleared.
        void GetDataDirectories(Vector<String> &directories) const;



    private:

        /// The script object that will be used to read the config file.
        GT::Script m_script;

        /// The list of data directories.
        Vector<String> m_dataDirectories;


    private:    // No copying.
        ApplicationConfig(const ApplicationConfig &);
        ApplicationConfig & operator=(const ApplicationConfig &);
    };
}

#endif
