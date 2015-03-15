// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "File_Native.hpp"

namespace GT
{
#if defined(GT_PLATFORM_WINDOWS)
    File_Win32::File_Win32()
        : m_hWin32File(0),
          m_hWin32FileMapping(0),
          m_pMappingBuffer(nullptr),
          m_accessMode(FileAccessMode::None)
    {
    }

    File_Win32::~File_Win32()
    {
    }


    ResultCode File_Win32::Open(const char* absolutePath, FileAccessMode accessMode)
    {
        DWORD dwDesiredAccess;
        DWORD dwShareMode;
        DWORD dwCreationDisposition;

        switch (accessMode)
        {
        case FileAccessMode::Read:
            {
                dwDesiredAccess       = FILE_GENERIC_READ;
                dwShareMode           = FILE_SHARE_READ;
                dwCreationDisposition = OPEN_EXISTING;

                break;
            }

        case FileAccessMode::Write:
            {
                dwDesiredAccess       = FILE_GENERIC_WRITE;
                dwShareMode           = 0;
                dwCreationDisposition = OPEN_ALWAYS;

                break;
            }

        case FileAccessMode::None:
        default:
            {
                return AccessDenied;
            }
        }


        m_hWin32File = CreateFileA(absolutePath, dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
        if (m_hWin32File != INVALID_HANDLE_VALUE)
        {
            m_accessMode = accessMode;

            return NoError;
        }
        else
        {
            switch (GetLastError())
            {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
                {
                    return FileNotFound;
                }

            case ERROR_TOO_MANY_OPEN_FILES:
                {
                    return TooManyOpenFiles;
                }

            case ERROR_ACCESS_DENIED:
                {
                    return AccessDenied;
                }

            default:
                {
                    return UnknownError;
                }
            }
        }
    }

    void File_Win32::Close()
    {
        if (m_hWin32File != 0)
        {
            // Make sure the file is unmapped.
            this->Unmap();

            // Now just close the handle and set everything to 0.
            CloseHandle(m_hWin32File);
            m_hWin32File = 0;

            // Reset the access mode.
            m_accessMode = FileAccessMode::None;
        }
    }


    size_t File_Win32::Read(size_t bytesToRead, void* dataOut)
    {
        DWORD bytesRead;
        if (ReadFile(m_hWin32File, dataOut, static_cast<DWORD>(bytesToRead), &bytesRead, nullptr))
        {
            return bytesRead;
        }
        else
        {
            // There was an error reading the file data.
            return 0;
        }
    }

    size_t File_Win32::Write(size_t bytesToWrite, const void* data)
    {
        DWORD bytesWritten;
        if (WriteFile(m_hWin32File, data, static_cast<DWORD>(bytesToWrite), &bytesWritten, nullptr))
        {
            return bytesWritten;
        }
        else
        {
            // There was an error writing the file data.
            return 0;
        }
    }

    int64_t File_Win32::Seek(int64_t bytesToSeek, FileSeekOrigin origin)
    {
        LARGE_INTEGER lNewFilePointer;
        LARGE_INTEGER lDistanceToMove;
        lDistanceToMove.QuadPart = bytesToSeek;

        DWORD dwMoveMethod;
        switch (origin)
        {
        case FileSeekOrigin::Current:
            {
                dwMoveMethod = FILE_CURRENT;
                break;
            }

        case FileSeekOrigin::Start:
            {
                dwMoveMethod = FILE_BEGIN;
                break;
            }

        case FileSeekOrigin::End:
            {
                dwMoveMethod = FILE_END;
                break;
            }

        default:
            {
                return 0;
            }
        }


        if (SetFilePointerEx(m_hWin32File, lDistanceToMove, &lNewFilePointer, dwMoveMethod))
        {
            return lNewFilePointer.QuadPart;
        }
        else
        {
            return 0;
        }
    }

    int64_t File_Win32::Tell() const
    {
        LARGE_INTEGER lNewFilePointer;
        LARGE_INTEGER lDistanceToMove;
        lDistanceToMove.QuadPart = 0;

        if (SetFilePointerEx(m_hWin32File, lDistanceToMove, &lNewFilePointer, FILE_CURRENT))
        {
            return lNewFilePointer.QuadPart;
        }
        else
        {
            return 0;
        }
    }

    int64_t File_Win32::GetSize() const
    {
        LARGE_INTEGER fileSize;
        GetFileSizeEx(m_hWin32File, &fileSize);

        return fileSize.QuadPart;
    }

    void* File_Win32::Map(size_t length, int64_t offset)
    {
        if (m_hWin32FileMapping != nullptr)
        {
            return m_pMappingBuffer;
        }

        if (m_accessMode == FileAccessMode::Read)
        {
            DWORD flProtect       = PAGE_READONLY;
            DWORD dwDesiredAccess = FILE_MAP_READ;

            LARGE_INTEGER dwMaximumSize;
            dwMaximumSize.QuadPart = offset + length;

            LARGE_INTEGER dwFileOffset;
            dwFileOffset.QuadPart = offset;


            m_hWin32FileMapping = CreateFileMappingA(m_hWin32File, nullptr, flProtect, dwMaximumSize.HighPart, dwMaximumSize.LowPart, nullptr);
            if (m_hWin32FileMapping != nullptr)
            {
                m_pMappingBuffer = MapViewOfFile(m_hWin32FileMapping, dwDesiredAccess, dwFileOffset.HighPart, dwFileOffset.LowPart, length);

                if (m_pMappingBuffer == nullptr)
                {
                    CloseHandle(m_hWin32FileMapping);
                    m_hWin32FileMapping = 0;
                }

                return m_pMappingBuffer;
            }
            else
            {
                // Error mapping file.
                return nullptr;
            }
        }
        else
        {
            // Cannot map which in write mode.
            return nullptr;
        }
    }

    void File_Win32::Unmap()
    {
        if (m_hWin32FileMapping != 0)
        {
            UnmapViewOfFile(m_pMappingBuffer);
            m_pMappingBuffer = nullptr;

            CloseHandle(m_hWin32FileMapping);
            m_hWin32FileMapping = 0;
        }
    }


#endif

#if defined(GT_PLATFORM_LINUX)
#endif
}
