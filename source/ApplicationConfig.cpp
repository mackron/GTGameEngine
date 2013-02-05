// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ApplicationConfig.hpp>
#include <GTCore/Script.hpp>
#include <GTCore/Errors.hpp>
#include <GTCore/String.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/stdlib.hpp>

#include <cstring>

namespace GTEngine
{
    /// Helper function for retrieving the 'Data' directory from the config script.
    void ApplicationConfig_ReadDataDirectories();


    /// The script for the config file. If this is null, we know the configuration is not yet loaded.
    static GTCore::Script* ConfigScript = nullptr;

    /// The list of data directories.
    static GTCore::Vector<GTCore::String> DataDirectories;


    bool ApplicationConfig::Open(const char* fileName)
    {
        if (!ConfigScript)
        {
            ConfigScript = new GTCore::Script;
            ConfigScript->LoadAndExecute
            (
                "Directories = \n"
                "{\n"
                "    Data = {};\n"
                "}\n"
            );

            if (ConfigScript->LoadFile(fileName) && ConfigScript->Execute())
            {
                ApplicationConfig_ReadDataDirectories();
            }
            else
            {
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

    const GTCore::Vector<GTCore::String> & ApplicationConfig::GetDataDirectories()
    {
        return DataDirectories;
    }

    void ApplicationConfig::GetDataDirectories(GTCore::Vector<const char*> &directories)
    {
        for (size_t i = 0; i < DataDirectories.count; ++i)
        {
            directories.PushBack(DataDirectories[i].c_str());
        }
    }

    void ApplicationConfig_ReadDataDirectories()
    {
        if (ConfigScript)
        {
            ConfigScript->GetGlobal("Directories");
            if (ConfigScript->IsTable(-1))
            {
                ConfigScript->Push("Data");
                ConfigScript->GetTableValue(-2);
                if (ConfigScript->IsString(-1))
                {
                    GTCore::Path absPath(ConfigScript->ToString(-1));
                    absPath.MakeAbsolute();

                    DataDirectories.PushBack(absPath.c_str());
                }
                else if (ConfigScript->IsTable(-1))
                {
                    // We have a list of directories. We need to iterate over each one.
                    for (ConfigScript->PushNil(); ConfigScript->Next(-2); ConfigScript->Pop(1))
                    {
                        if (ConfigScript->IsString(-1))
                        {
                            GTCore::Path absPath(ConfigScript->ToString(-1));
                            absPath.MakeAbsolute();

                            DataDirectories.PushBack(absPath.c_str());
                        }
                    }
                }
                ConfigScript->Pop(1);
            }
            ConfigScript->Pop(1);
        }
    }
}
