// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/FileSystem.hpp>
#include "FileSystem/File_Native.hpp"

#include <GTLib/IO.hpp>

namespace GT
{
    FileSystem::FileSystem()
        : m_baseDirectories(4),
          m_baseDirectoriesLowPriority(1)
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
        this->RemoveAllBaseDirectories();
    }


    ResultCode FileSystem::AddBaseDirectory(const char* absolutePath)
    {
        m_baseDirectories.PushBack(GTLib::IO::CleanPath(absolutePath).c_str());
        return 0;
    }

    ResultCode FileSystem::AddLowPriorityBaseDirectory(const char* absolutePath)
    {
        m_baseDirectoriesLowPriority.PushBack(GTLib::IO::CleanPath(absolutePath).c_str());
        return 0;
    }

    void FileSystem::RemoveBaseDirectory(const char* absolutePath)
    {
        // Check high-priority first.
        for (size_t iBaseDirectory = 0; iBaseDirectory < m_baseDirectories.GetCount(); ++iBaseDirectory)
        {
            if (m_baseDirectories[iBaseDirectory] == absolutePath)
            {
                m_baseDirectories.Remove(iBaseDirectory);
                break;
            }
        }

        // Low-priority.
        for (size_t iBaseDirectory = 0; iBaseDirectory < m_baseDirectoriesLowPriority.GetCount(); ++iBaseDirectory)
        {
            if (m_baseDirectoriesLowPriority[iBaseDirectory] == absolutePath)
            {
                m_baseDirectoriesLowPriority.Remove(iBaseDirectory);
                break;
            }
        }
    }





    void FileSystem::RemoveAllBaseDirectories()
    {
        m_baseDirectories.Clear();
        m_baseDirectoriesLowPriority.Clear();
    }



    size_t FileSystem::GetBaseDirectoryCount() const
    {
        return m_baseDirectories.GetCount() + m_baseDirectoriesLowPriority.GetCount();
    }

    GTLib::String FileSystem::GetBaseDirectoryByIndex(size_t index) const
    {
        assert(index < this->GetBaseDirectoryCount());

        if (index < m_baseDirectories.GetCount())
        {
            return m_baseDirectories[index];
        }
        else
        {
            return m_baseDirectoriesLowPriority[index - m_baseDirectories.GetCount()];
        }
    }


    bool FileSystem::IterateFiles(const char* absolutePath, std::function<bool (const FileInfo &fi)> func) const
    {
#if defined(GT_PLATFORM_WINDOWS)
        WIN32_FIND_DATAA ffd;

        // We need to put a '*' as the wildcard symbol on Windows. This will find every file in the directory. We then
        // use our own regex matcher for doing matching.
        size_t directoryLength = strlen(absolutePath);
        if (directoryLength < 4096 - 3)
        {
            char directoryQuery[4096];
            memcpy(directoryQuery, absolutePath, directoryLength);

            directoryQuery[directoryLength + 0] = '\\';
            directoryQuery[directoryLength + 1] = '*';
            directoryQuery[directoryLength + 2] = '\0';

            bool wantToTerminate = false;
            HANDLE hFind = FindFirstFileA(directoryQuery, &ffd);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                do
                {
                    // We need to ignore '.' and '..' directories. In the case of '..', it will actually mess up our base path.
                    if (!GTLib::Strings::Equal(ffd.cFileName, ".") && !GTLib::Strings::Equal(ffd.cFileName, ".."))
                    {
                        LARGE_INTEGER liSize;
                        liSize.LowPart  = ffd.nFileSizeLow;
                        liSize.HighPart = ffd.nFileSizeHigh;

                        LARGE_INTEGER liTime;
                        liTime.LowPart  = ffd.ftLastWriteTime.dwLowDateTime;
                        liTime.HighPart = ffd.ftLastWriteTime.dwHighDateTime;


                        FileInfo newFI;
                        newFI.relativePath     = ffd.cFileName;
                        newFI.absolutePath     = absolutePath; newFI.absolutePath += "/"; newFI.absolutePath += ffd.cFileName;
                        newFI.sizeInBytes      = liSize.QuadPart;
                        newFI.lastModifiedTime = liTime.QuadPart;
                        newFI.flags            = 0;

                        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        {
                            newFI.flags |= FileInfo::IsDirectory;
                        }


                        if (!func(newFI))
                        {
                            wantToTerminate = true;
                        }
                    }

                } while (!wantToTerminate && FindNextFileA(hFind, &ffd) != 0);


                // Now we need to close the handle.
                FindClose(hFind);
            }


            return !wantToTerminate;
        }
#endif

        return true;
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
            // High-priority base directories.
            for (size_t iBase = 0; iBase < m_baseDirectories.GetCount(); ++iBase)
            {
                GTLib::String absolutePathIn = m_baseDirectories[iBase] + "/";
                if (GTLib::IO::FileExists((absolutePathIn + filePath).c_str()))
                {
                    absolutePathOut = GTLib::IO::ToAbsolutePath(filePath, absolutePathIn.c_str());
                    return true;
                }
            }

            // Low-priority base directories.
            for (size_t iBase = 0; iBase < m_baseDirectoriesLowPriority.GetCount(); ++iBase)
            {
                GTLib::String absolutePathIn = m_baseDirectoriesLowPriority[iBase] + "/";
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
}
