// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/FileSystem.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#include <GTLib/IO.hpp>

namespace GT
{
    /////////////////////////////////////////////
    // FileSystem

    FileSystem::FileSystem()
        : m_baseDirectories(4)
    {
    }

    FileSystem::~FileSystem()
    {
        this->RemoveAllBaseDirectories();
    }


    ResultCode FileSystem::Startup()
    {
        return 0;
    }

    void FileSystem::Shutdown()
    {
    }


    ResultCode FileSystem::AddBaseDirectory(const char* absolutePath)
    {
        m_baseDirectories.PushBack(absolutePath);
        return 0;
    }

    void FileSystem::RemoveBaseDirectory(const char* absolutePath)
    {
        for (size_t iBaseDirectory = 0; iBaseDirectory < m_baseDirectories.GetCount(); ++iBaseDirectory)
        {
            if (m_baseDirectories[iBaseDirectory] == absolutePath)
            {
                m_baseDirectories.Remove(iBaseDirectory);
                break;
            }
        }
    }

    void FileSystem::RemoveAllBaseDirectories()
    {
        m_baseDirectories.Clear();
    }



    bool FileSystem::FindAbsolutePath(const char* filePath, GTLib::String &absolutePathOut) const
    {
        // TODO: Handle zip files properly.

        if (!GTLib::IO::IsPathAbsolute(filePath))
        {
            for (size_t iBase = 0; iBase < m_baseDirectories.GetCount(); ++iBase)
            {
                GTLib::String absolutePathIn = m_baseDirectories[iBase] + "/";
                if (GTLib::IO::FileExists((absolutePathIn + filePath).c_str()))
                {
                    absolutePathOut = GTLib::IO::ToAbsolutePath(filePath, absolutePathIn.c_str());
                    return true;
                }
            }
        }
        else
        {
            // 'filePath' is an absolute path if we make it to this branch.
            if (GTLib::IO::FileExists(filePath))
            {
                absolutePathOut = filePath;
                return true;
            }
        }
            
        return false;
    }


    ///////////////////////////////////////////////////////////////////
    // Static Helpers

}
