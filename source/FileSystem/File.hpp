// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_File_hpp_
#define __GT_File_hpp_

#include <GTGameEngine/FileDataTypes.hpp>
#include <GTLib/ResultCodes.hpp>
#include <cstdint>

namespace GT
{
    /// Base class for the various sources of files.
    ///
    /// Inherited from this class will be an implementation for Win32 native files, Unix native files, Zip files, etc.
    class File
    {
    public:

        /// Constructor.
        File();

        /// Destructor.
        virtual ~File();


        /// Opens the file from the given path.
        ///
        /// @param absolutePath [in] The absolute path of the file to open.
        ///
        /// @return The result code of the Open operation.
        virtual ResultCode Open(const char* absolutePath, FileAccessMode accessMode) = 0;

        /// Closes the file.
        virtual void Close() = 0;


        /// Reads data from the file and increments the read/write pointer.
        ///
        /// @param bytesToRead [in]  The number of bytes to read.
        /// @param dataOut     [out] A pointer to the buffer that will receive the file data.
        ///
        /// @return The number of bytes actually written.
        ///
        /// @remarks
        ///     If the return value is less than \c bytesToRead than it can be assumed the end of the file has been reached.
        virtual size_t Read(size_t bytesToRead, void* dataOut) = 0;

        /// Writes data to the file.
        ///
        /// @param bytesToWrite [in] The number of bytes to write.
        /// @param data         [in] A pointer to the buffer that contains the data to write.
        ///
        /// @return The number of bytes that were written.
        virtual size_t Write(size_t bytesToWrite, const void* data) = 0;

        /// Seeks the read/write pointer by the given number of bytes.
        ///
        /// @param bytesToSeek [in] The number of bytes to seek.
        /// @param origin      [in] Where to seek from (current position, beginning of the file, end of the file).
        ///
        /// @return The number of bytes seeked.
        virtual int64_t Seek(int64_t bytesToSeek, FileSeekOrigin origin) = 0;

        /// Retrieves the position of the read/write pointer.
        ///
        /// @return The position of the read/write pointer.
        ///
        /// @remarks
        ///     If the file is at the end of the file, this should return the same value as GetSize(). Use this assertion to check if the file's read/write pointer is
        ///     at the end of the file.
        virtual int64_t Tell() const = 0;

        /// Retrieves the size of the file in bytes.
        ///
        /// @return The size in bytes of the file.
        virtual int64_t GetSize() const = 0;

        /// Maps the file.
        ///
        /// @return A pointer to the buffer that represents the contents of the file.
        ///
        /// @remarks
        ///     If the file does not support mapping, this will return null.
        ///     @par
        ///     The file can only be mapped if it was opened in read mode.
        virtual void* Map(size_t length, int64_t offset) = 0;

        /// Unmaps the file.
        virtual void Unmap() = 0;
    };
}

#endif
