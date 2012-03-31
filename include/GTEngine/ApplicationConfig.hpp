
#ifndef __GTEngine_ApplicationConfig_hpp_
#define __GTEngine_ApplicationConfig_hpp_

namespace GTEngine
{
    class ApplicationConfig
    {
    public:
    
        /**
        *   \brief                Opens the application configuration from the given file.
        *   \param  fileName [in] The name of the config file to load.
        *
        *   \remarks
        *       You must call Close() when finished with the configuration.
        */
        static bool Open(const char* fileName);
        
        /**
        *   \brief  Closes the application configuration.
        */
        static void Close();

    
        /**
        *   \brief  Structure for retrieving directories.
        */
        struct Directories
        {
            static const char * Data();
        };
    };
}

#endif
