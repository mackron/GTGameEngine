// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ApplicationConfig.hpp>
#include <GTEngine/Script.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/stdlib.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    ApplicationConfig::ApplicationConfig()
        : m_script(),
            m_dataDirectories()
    {
    }

    ApplicationConfig::~ApplicationConfig()
    {
    }


    bool ApplicationConfig::Open(easyvfs_context* pVFS, const char* fileName)
    {
        // Reset everything just to be sure.
        m_dataDirectories.Clear();

        // Just need a bit of setup in the script before doing anything:
        //   Directories =
        //   {
        //       Data = {};
        //   }
        m_script.PushNewTable();
        {
            m_script.Push("Data");
            m_script.PushNewTable();
            m_script.SetTableValue(-3);
        }
        m_script.SetGlobal("Directories");



        // Everything is reset, so now we load the script.
        if (m_script.LoadFileAndExecute(pVFS, fileName))
        {
            char cwd[EASYVFS_MAX_PATH];
            _getcwd(cwd, sizeof(cwd));

            m_script.GetGlobal("Directories");
            assert(m_script.IsTable(-1));
            {
                m_script.Push("Data");
                m_script.GetTableValue(-2);
                if (m_script.IsString(-1))
                {
                    char absPath[EASYVFS_MAX_PATH];
                    easypath_to_absolute(m_script.ToString(-1), cwd, absPath, sizeof(absPath));

                    m_dataDirectories.PushBack(absPath);
                }
                else if (m_script.IsTable(-1))
                {
                    // We have a list of directories. We need to iterate over each one.
                    for (m_script.PushNil(); m_script.Next(-2); m_script.Pop(1))
                    {
                        if (m_script.IsString(-1))
                        {
                            char absPath[EASYVFS_MAX_PATH];
                            easypath_copy_and_append(absPath, sizeof(absPath), cwd, m_script.ToString(-1));

                            m_dataDirectories.PushBack(absPath);
                        }
                    }
                }
                m_script.Pop(1);
            }
            m_script.Pop(1);

            return true;
        }
        else
        {
            return false;
        }
    }

    const Vector<String> & ApplicationConfig::GetDataDirectories() const
    {
        return m_dataDirectories;
    }

    void ApplicationConfig::GetDataDirectories(Vector<String> &directories) const
    {
        for (size_t i = 0; i < m_dataDirectories.count; ++i)
        {
            directories.PushBack(m_dataDirectories[i].c_str());
        }
    }
}
