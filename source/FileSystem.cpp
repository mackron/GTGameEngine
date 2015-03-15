// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/FileSystem.hpp>
#include "FileSystem/File_Native.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#endif

#if defined(GT_PLATFORM_LINUX)
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



    HFile FileSystem::OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode* resultCodeOut)
    {
        // NOTE: Only supporting simple files at the moment. This implementation will need to change once other file sources are supported.

        ResultCode result = 0;
        File* pFile = nullptr;

        GTLib::String absolutePath;
        if (this->FindAbsolutePath(filePath, absolutePath))
        {
#if defined(GT_PLATFORM_WINDOWS)
            pFile = new File_Win32;
            result = pFile->Open(absolutePath.c_str(), accessMode);
#elif defined(GT_PLATFORM_LINUX)
            pFile = new File_Unix;
            result = pFile->Open(absolutePath.c_str(), accessMode);
#endif

            if (GT::Failed(result))
            {
                delete pFile;
                pFile = nullptr;
            }
        }
        else
        {
            result = FileNotFound;
        }


        if (resultCodeOut != nullptr)
        {
            *resultCodeOut = result;
        }

        return reinterpret_cast<HFile>(pFile);
    }

    void FileSystem::CloseFile(HFile hFile)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            pFile->Close();
            delete pFile;
        }
    }

    size_t FileSystem::ReadFile(HFile hFile, size_t bytesToRead, void* dataOut)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->Read(bytesToRead, dataOut);
        }

        return 0;
    }

    size_t FileSystem::WriteFile(HFile hFile, size_t bytesToWrite, const void* data)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->Write(bytesToWrite, data);
        }

        return 0;
    }

    int64_t FileSystem::SeekFile(HFile hFile, int64_t bytesToSeek, FileSeekOrigin origin)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->Seek(bytesToSeek, origin);
        }

        return 0;
    }

    int64_t FileSystem::TellFile(HFile hFile)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->Tell();
        }

        return 0;
    }

    int64_t FileSystem::GetFileSize(HFile hFile)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->GetSize();
        }

        return 0;
    }

    void* FileSystem::MapFile(HFile hFile, size_t length, int64_t offset)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            return pFile->Map(length, offset);
        }

        return nullptr;
    }

    void FileSystem::UnmapFile(HFile hFile)
    {
        auto pFile = reinterpret_cast<File*>(hFile);
        if (pFile != nullptr)
        {
            pFile->Unmap();
        }
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
