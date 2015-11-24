// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ApplicationConfig.hpp>
#include <GTLib/Script.hpp>
#include <GTLib/Errors.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/stdlib.hpp>
#include <easy_path/easy_path.h>

#include <cstring>

namespace GT
{
    namespace Engine
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
                        GTLib::Path absPath(m_script.ToString(-1));
                        absPath.MakeAbsolute(cwd);

                        m_dataDirectories.PushBack(absPath.c_str());
                    }
                    else if (m_script.IsTable(-1))
                    {
                        // We have a list of directories. We need to iterate over each one.
                        for (m_script.PushNil(); m_script.Next(-2); m_script.Pop(1))
                        {
                            if (m_script.IsString(-1))
                            {
                                //GTLib::Path absPath(m_script.ToString(-1));
                                //absPath.MakeAbsolute(cwd);

                                char absPath[EASYVFS_MAX_PATH];
                                easypath_copyandappend(absPath, sizeof(absPath), cwd, m_script.ToString(-1));

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

        const GTLib::Vector<GTLib::String> & ApplicationConfig::GetDataDirectories() const
        {
            return m_dataDirectories;
        }

        void ApplicationConfig::GetDataDirectories(GTLib::Vector<GTLib::String> &directories) const
        {
            for (size_t i = 0; i < m_dataDirectories.count; ++i)
            {
                directories.PushBack(m_dataDirectories[i].c_str());
            }
        }
    }
}
