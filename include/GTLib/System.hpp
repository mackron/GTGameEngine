// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_System_hpp_
#define __GTLib_System_hpp_

#include <cstddef>
#include "Vector.hpp"

namespace GTLib
{
    namespace System
    {
        /// Static class for retrieving system directories. In some cases, such as fonts, there will multiple directories. In these cases the retrieval
        /// functions will return a Vector containing the directory strings.
        class Directories
        {
        public:
        
            /**
            *   \brief  Retrieves a pointer to the string containing the home directory path.
            */
            static const char* Home();

            /// Retrieves the list of root-level font directories. Note that this does NOT include sub-directories. For example, on Linux it will include 
            /// "/usr/share/fonts", but will not include "/usr/share/fonts/freetype". 
            static Vector<const char*> & Fonts();
        };
        
        /**
        *   \brief  Retrieves the number of logical CPU's attached to the machine.
        */
        unsigned int GetCPUCount();
        
        /**
        *   \brief  Retrieves the page size.
        */
        size_t GetPageSize();


        /// Opens a file or URL in the preferred application.
        ///
        /// @param filePath [in] The file path to open.
        bool OpenFileOrURL(const char* url);
    }
}


#endif
