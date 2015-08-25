// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_System
#define GTLib_System

#include <cstddef>

namespace GTLib
{
    namespace System
    {
        /// Retrieves the number of logical CPU's attached to the machine.
        unsigned int GetCPUCount();

        /// Retrieves the page size.
        size_t GetPageSize();


        /// Opens a file or URL in the preferred application.
        ///
        /// @param url [in] The file path to open.
        bool OpenFileOrURL(const char* url);
    }
}


#endif
