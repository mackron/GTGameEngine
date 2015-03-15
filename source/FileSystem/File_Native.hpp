// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_File_Native_hpp_
#define __GT_File_Native_hpp_

#include <GTLib/Config.hpp>
#include "File.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#if defined(GT_PLATFORM_LINUX)
#endif

namespace GT
{
#if defined(GT_PLATFORM_WINDOWS)
    class File_Win32 : public File
    {
    public:

        /// Constructor.
        File_Win32();

        /// Destructor.
        ~File_Win32();


        /// @copydoc File::Open()
        ResultCode Open(const char* absolutePath, FileAccessMode accessMode);

        /// @copydoc File::Close()
        void Close();


        /// @copydoc File::Open()
        size_t Read(size_t bytesToRead, void* dataOut);

        /// @copydoc File::Seek()
        int64_t Seek(int64_t bytesToSeek, FileSeekOrigin origin);

        /// @copydoc File::Tell()
        int64_t Tell() const;

        /// @copydoc File::GetSize()
        int64_t GetSize() const;

        /// @copydoc File::Map()
        void* Map(size_t length, int64_t offset);

        /// @copydoc File::Unmap()
        void Unmap();



    private:

        /// The Win32 file handle.
        HANDLE m_hWin32File;

        /// The Win32 file mapping handle.
        HANDLE m_hWin32FileMapping;

        /// The file mapping pointer. We keep track of this so we can return it from Map() when it is called while already being mapped.
        void* m_pMappingBuffer;

        /// The open mode. This needs to be tracked in order to use CreateFileMapping() correctly.
        FileAccessMode m_accessMode;


    private:    // No copying.
        File_Win32(const File_Win32 &);
        File_Win32 & operator=(const File_Win32 &);
    };
#endif



#if defined(GT_PLATFORM_LINUX)
    class File_Unix : public File
    {
    public:

        /// Constructor.
        File_Unix();

        /// Destructor.
        ~File_Unix();


        /// @copydoc File::Open()
        size_t Read(size_t bytesToRead, void* dataOut);

        /// @copydoc File::Seek()
        int64_t Seek(int64_t bytesToSeek, FileSeekOrigin origin);

        /// @copydoc File::Tell()
        int64_t Tell() const;

        /// @copydoc File::GetSize()
        int64_t GetSize() const;

        /// @copydoc File::Map()
        void* Map();

        /// @copydoc File::Unmap()
        void Unmap();


    private:    // No copying.
        File_Unix(const File_Unix &);
        File_Unix & operator=(const File_Unix &);
    };
#endif
}

#endif
