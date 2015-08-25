// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/FileSystem.hpp>
#include "external/easy_vfs/easy_vfs.h"
#include "external/easy_vfs/extras/easy_vfs_mtl.h"
#include "external/easy_vfs/extras/easy_vfs_zip.h"
#include "external/easy_path/easy_path.h"

#include <GTLib/IO.hpp>

namespace GT
{
    FileSystem::FileSystem()
        : m_pVFS(nullptr)
    {
    }

    FileSystem::~FileSystem()
    {
        this->RemoveAllBaseDirectories();
    }


    ResultCode FileSystem::Startup()
    {
        m_pVFS = easyvfs_createcontext();
        if (m_pVFS != nullptr)
        {
            easyvfs_registerarchivecallbacks_zip(m_pVFS);
            easyvfs_registerarchivecallbacks_mtl(m_pVFS);   // Add support for Wavefront MTL files.

            return 0;
        }
        else
        {
            // Failed to create the virtual file system.
            return 1;
        }
    }

    void FileSystem::Shutdown()
    {
        easyvfs_deletecontext(m_pVFS);
        m_pVFS = nullptr;

        this->RemoveAllBaseDirectories();
    }


    ResultCode FileSystem::AddBaseDirectory(const char* absolutePath)
    {
        easyvfs_addbasedirectory(m_pVFS, GTLib::IO::CleanPath(absolutePath).c_str());
        return 0;
    }

    ResultCode FileSystem::InsertBaseDirectory(const char* absolutePath, unsigned int index)
    {
        easyvfs_insertbasedirectory(m_pVFS, absolutePath, index);
        return 0;
    }

    void FileSystem::RemoveBaseDirectory(const char* absolutePath)
    {
        easyvfs_removebasedirectory(m_pVFS, absolutePath);
    }





    void FileSystem::RemoveAllBaseDirectories()
    {
        easyvfs_removeallbasedirectories(m_pVFS);
    }



    size_t FileSystem::GetBaseDirectoryCount() const
    {
        return easyvfs_basedirectorycount(m_pVFS);
    }

    GTLib::String FileSystem::GetBaseDirectoryByIndex(size_t index) const
    {
        assert(index < this->GetBaseDirectoryCount());

        char absolutePath[EASYVFS_MAX_PATH];
        easyvfs_getbasedirectorybyindex(m_pVFS, static_cast<unsigned int>(index), absolutePath, EASYVFS_MAX_PATH);

        return absolutePath;
    }


    bool FileSystem::IsDirectory(const char* absoluteOrRelativePath)
    {
        easyvfs_fileinfo fi;
        if (easyvfs_getfileinfo(m_pVFS, absoluteOrRelativePath, &fi))
        {
            return (fi.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0;
        }

        return false;
    }


    bool FileSystem::IterateFiles(const char* absoluteOrRelativePath, std::function<bool (const FileInfo &fi)> func) const
    {
        bool wantToTerminate = false;

        easyvfs_iterator i;
        if (easyvfs_beginiteration(m_pVFS, absoluteOrRelativePath, &i))
        {
            easyvfs_fileinfo fi;
            while (!wantToTerminate && easyvfs_nextiteration(m_pVFS, &i, &fi))
            {
                FileInfo fi2;
                fi2.absolutePath     = fi.absolutePath;
                fi2.relativePath     = easypath_filename(fi.absolutePath);
                fi2.sizeInBytes      = fi.sizeInBytes;
                fi2.lastModifiedTime = fi.lastModifiedTime;

                if ((fi.attributes & EASYVFS_FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    fi2.flags |= FileInfo::IsDirectory;
                }
                if ((fi.attributes & EASYVFS_FILE_ATTRIBUTE_READONLY) != 0)
                {
                    fi2.flags |= FileInfo::IsReadOnly;
                }

                if (!func(fi2))
                {
                    wantToTerminate = true;
                }
            }

            easyvfs_enditeration(m_pVFS, &i);
        }


        return !wantToTerminate;
    }



    HFile FileSystem::OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode* resultCodeOut)
    {
        easyvfs_accessmode accessModeInternal;
        switch (accessMode)
        {
        case FileAccessMode::Read:
            {
                accessModeInternal = easyvfs_read;
                break;
            }

        case FileAccessMode::Write:
            {
                accessModeInternal = easyvfs_write;
                break;
            }

        default:
            {
                return 0;
            }
        }

        if (resultCodeOut != nullptr)
        {
            *resultCodeOut = 0;
        }

        return reinterpret_cast<HFile>(easyvfs_openfile(m_pVFS, filePath, accessModeInternal));
    }

    void FileSystem::CloseFile(HFile hFile)
    {
        easyvfs_closefile(reinterpret_cast<easyvfs_file*>(hFile));
    }

    unsigned int FileSystem::ReadFile(HFile hFile, unsigned int bytesToRead, void* dataOut)
    {
        unsigned int bytesRead;
        if (easyvfs_readfile(reinterpret_cast<easyvfs_file*>(hFile), dataOut, bytesToRead, &bytesRead))
        {
            return bytesRead;
        }

        return 0;
    }

    unsigned int FileSystem::WriteFile(HFile hFile, unsigned int bytesToWrite, const void* data)
    {
        unsigned int bytesWritten;
        if (easyvfs_writefile(reinterpret_cast<easyvfs_file*>(hFile), data, bytesToWrite, &bytesWritten))
        {
            return bytesWritten;
        }

        return 0;
    }

    int64_t FileSystem::SeekFile(HFile hFile, int64_t bytesToSeek, FileSeekOrigin origin)
    {
        easyvfs_seekorigin originInternal;
        switch (origin)
        {
        case FileSeekOrigin::Current:
            {
                originInternal = easyvfs_current;
                break;
            }

        case FileSeekOrigin::Start:
            {
                originInternal = easyvfs_start;
                break;
            }

        case FileSeekOrigin::End:
            {
                originInternal = easyvfs_start;
                break;
            }

        default:
            {
                return 0;
            }
        }

        return easyvfs_seekfile(reinterpret_cast<easyvfs_file*>(hFile), bytesToSeek, originInternal);
    }

    int64_t FileSystem::TellFile(HFile hFile)
    {
        return easyvfs_tellfile(reinterpret_cast<easyvfs_file*>(hFile));
    }

    int64_t FileSystem::GetFileSize(HFile hFile)
    {
        return easyvfs_filesize(reinterpret_cast<easyvfs_file*>(hFile));
    }


    bool FileSystem::DeleteFile(const char* filePath)
    {
        return easyvfs_deletefile(m_pVFS, filePath) != 0;
    }

    bool FileSystem::RenameFile(const char* filePathOld, const char* filePathNew)
    {
        return easyvfs_renamefile(m_pVFS, filePathOld, filePathNew) != 0;
    }

    bool FileSystem::CreateDirectory(const char* filePath)
    {
        return easyvfs_mkdir(m_pVFS, filePath) != 0;
    }

    



    bool FileSystem::FindAbsolutePath(const char* filePath, GTLib::String &absolutePathOut) const
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_findabsolutepath(m_pVFS, filePath, absolutePath, EASYVFS_MAX_PATH))
        {
            absolutePathOut = absolutePath;
            return true;
        }

        return false;
    }

    bool FileSystem::FindAbsolutePath(const char* filePath, char* absolutePathOut, unsigned int absolutePathOutSizeInBytes)
    {
        return easyvfs_findabsolutepath(m_pVFS, filePath, absolutePathOut, absolutePathOutSizeInBytes) != 0;
    }


    bool FileSystem::FindAbsolutePath(const char* filePath, const char* highestPriorityBasePath, GTLib::String &absolutePathOut) const
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_findabsolutepath_explicitbase(m_pVFS, filePath, highestPriorityBasePath, absolutePath, EASYVFS_MAX_PATH))
        {
            absolutePathOut = absolutePath;
            return true;
        }

        return false;
    }

    bool FileSystem::FindAbsolutePath(const char* filePath, const char* highestPriorityBasePath, char* absolutePathOut, unsigned int absolutePathOutSizeInBytes)
    {
        return easyvfs_findabsolutepath_explicitbase(m_pVFS, filePath, highestPriorityBasePath, absolutePathOut, absolutePathOutSizeInBytes) != 0;
    }


    bool FileSystem::FindAbsolutePathForWriting(const char* filePath, const char* basePath, char* absolutePathOut, unsigned int absolutePathOutSizeInBytes)
    {
        // All we do is append the paths.
        if (basePath == nullptr)
        {
            if (easyvfs_basedirectorycount(m_pVFS) > 0)
            {
                easyvfs_getbasedirectorybyindex(m_pVFS, 0, absolutePathOut, absolutePathOutSizeInBytes);
                easypath_append(absolutePathOut, absolutePathOutSizeInBytes, filePath);
            }
            else
            {
                return false;
            }
        }
        else
        {
            easypath_copyandappend(absolutePathOut, absolutePathOutSizeInBytes, basePath, filePath);
        }

        return true;
    }
}
