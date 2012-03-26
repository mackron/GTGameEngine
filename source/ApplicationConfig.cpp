/**
*/
#include <GTEngine/ApplicationConfig.hpp>
#include <GTCore/Script.hpp>
#include <GTCore/Errors.hpp>
#include <GTCore/Strings/Create.hpp>

#include <cstdlib>
#include <cstring>

namespace GTEngine
{
    /// Helper function for retrieving the 'Data' directory from teh config script.
    const char * ApplicationConfig_GetDataDirectory();

    /// The script for the config file. If this is null, we know the configuration is not yet loaded.
    static GTCore::Script *ConfigScript = nullptr;
    
    /**
    *   \brief  Structure for storing the different directories.
    */
    struct _ConfigDirectories
    {
        _ConfigDirectories()
            : data(nullptr)
        {
        }
        
        ~_ConfigDirectories()
        {
            GTCore::Strings::Delete((char *)this->data);
        }
        
        const char *data;
        
    private:    // No copying.
        _ConfigDirectories(const _ConfigDirectories &);
        _ConfigDirectories & operator=(const _ConfigDirectories &);
        
    }ConfigDirectories;
    
    bool ApplicationConfig::Open(const char *fileName)
    {
        if (!ConfigScript)
        {
            ConfigScript = new GTCore::Script;
            if (ConfigScript->LoadFile(fileName) && ConfigScript->Execute())
            {
                ConfigDirectories.data = GTCore::Strings::Create(ApplicationConfig_GetDataDirectory());
            }
            else
            {
                // We should post an error.
                GTCore::PostError("%s: %s", fileName, ConfigScript->ToString(-1));

                delete ConfigScript;
                ConfigScript = nullptr;
            }
        }
        
        return ConfigScript != nullptr;
    }

    void ApplicationConfig::Close()
    {
        delete ConfigScript;
        ConfigScript = nullptr;
    }
    

    const char * ApplicationConfig::Directories::Data()
    {
        return ConfigDirectories.data;
    }


    const char * ApplicationConfig_GetDataDirectory()
    {
        const char *result = nullptr;
        
        if (ConfigScript)
        {
            ConfigScript->GetGlobal("Directories");
            if (ConfigScript->IsTable(-1))
            {
                ConfigScript->Push("Data");
                ConfigScript->GetTableValue(-2);
                if (ConfigScript->IsString(-1))
                {
                    result = ConfigScript->ToString(-1);
                }
                ConfigScript->Pop(1);
            }
            ConfigScript->Pop(1);
        }
        
        return result;
    }
}