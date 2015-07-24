// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_FileSystem_hpp_
#define __GT_FileSystem_hpp_

#include <GTLib/Config.hpp>
#include <GTLib/ResultCodes.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Vector.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <wininet.h>
#endif

#include "FileDataTypes.hpp"

namespace GT
{
    typedef size_t HFile;

    /// Structure containing information about a file.
    struct FileInfo
    {
        FileInfo()
            : absolutePath(),
              relativePath(),
              sizeInBytes(0),
              lastModifiedTime(0),
              flags(0)
        {
        }

        /// The absolute path of the file.
        GTLib::String absolutePath;

        /// The relative path of the file.
        GTLib::String relativePath;

        /// The size of the file.
        int64_t sizeInBytes;

        /// The time of the last modification.
        time_t lastModifiedTime;

        /// Flags.
        uint32_t flags;


        //////////////////////////////////////////
        // Flags

        static const uint32_t IsDirectory = (1 << 0);   //< Whether or not the file is a directory.
    };

    /// Class representing the engine's virtual file system.
    ///
    /// The virtual file system is basically just an abstraction layer around the various method of working with files. This abstraction
    /// allows for a common interface between each method.
    ///
    /// Consider two types of files - a simple file on the operating system's file system, and a file sitting on an FTP server. The way
    /// these files are loaded and read is vastly different internally, but we can create an abstraction layer that sits on top and give
    /// them both a common API.
    class FileSystem
    {
    public:

        /// Constructor.
        FileSystem();

        /// Destructor.
        ~FileSystem();


        /// Initializes the file system object.
        ResultCode Startup();

        /// Shuts down the file system object.
        void Shutdown();

        
        /// Adds a base directory to the file system.
        ///
        /// @param absolutePath [in] The absolute path of the base directory.
        /// @param username     [in] The user name to use if authentication is required.
        /// @param password     [in] The password to use if authentication is required.
        ///
        /// @remarks
        ///     There is no notion of current directories in this abstraction, so the notion of base paths are used to handle relative paths. There
        ///     can be any number of base directories. When a relative path is specified, the base paths will be used to resolve the path. The order
        ///     in which the relative is resolved against the base directories is determined by the order in which the base directories are added.
        ///     @par
        ///     The base directory can be a URL for a supported protocol (such as FTP).
        ResultCode AddBaseDirectory(const char* absolutePath);

        /// @copydoc AddBaseDirectory()
        ///
        /// @remarks
        ///     A low-priority base directory will always be checked after the other base directories. This should be used for file systems with
        ///     slow access such as remote servers.
        ResultCode AddLowPriorityBaseDirectory(const char* absolutePath);

        /// Removes a base directory by it's absolute path.
        ///
        /// @param absolutePath [in] The absolute path of the base directory to remove.
        void RemoveBaseDirectory(const char* absolutePath);

        /// Removes all base directories.
        void RemoveAllBaseDirectories();


        /// Retrieves the number of base directories.
        ///
        /// @return
        ///     The returned value includes both normal and low-priority directories.
        ///     @par
        ///     Use the return value to retrieve the base directory at that given index.
        size_t GetBaseDirectoryCount() const;

        /// Retrieves a base directory by it's index.
        ///
        /// @param index [in] The index of the directory to retrieve.
        GTLib::String GetBaseDirectoryByIndex(size_t index) const;


        /// Determines whether or not the given path points to a directory.
        bool IsDirectory(const char* absolutePath);


        /// Iterates over every file and directory in the given directory.
        ///
        /// @param absolutePath [in] The absolute path of the directory whose contents are being iterated.
        /// 
        /// @remarks
        ///     This is not recursive.
        ///     @par
        ///     To iterate over the files and directories in a base directory, do IterateFiles(GetBaseDirectoryByIndex(baseIndex)).
        ///     @par
        ///     Do not delete files during the iteration.
        bool IterateFiles(const char* absolutePath, std::function<bool (const FileInfo &fi)> func) const;


        /// Finds an absolute path from the given path using the base directories.
        ///
        /// @param filePath        [in]  The path whose absolute version being retrieved.
        /// @param absolutePathOut [out] A reference to the string that will receive the absolute path.
        ///
        /// @return True if the absolute path was found; false otherwise.
        ///
        /// @remarks
        ///     If \c filePath is already absolute, this will just check that the path is valid.
        ///     @par
        ///     The priority used by the base directories is determined by the order in which the base directory was added.
        bool FindAbsolutePath(const char* filePath, GTLib::String &absolutePathOut) const;


        /// Attempts to open the given file.
        ///
        /// @param filePath      [in]  The path of the file to open.
        /// @param accessMode    [in]  Whether or not the file should be opened with read/write access.
        /// @param resultCodeOut [out] A reference to the variable that will receive the result code.
        ///
        /// @return A handle to the file, or 0 if an error occurred.
        ///
        /// @remarks
        ///     When opening a file fails, use the result code to determine why and give meaningful feedback.
        ///     @par
        ///     The access mode can be Read OR Write - it cannot be both. The reason for this is to simplify the implementation of the underlying
        ///     file processing for the various types of file sources.
        HFile OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode* resultCodeOut);
        HFile OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode &resultCodeOut) { return this->OpenFile(filePath, accessMode, &resultCodeOut); }
        HFile OpenFile(const char* filePath, FileAccessMode accessMode) { return this->OpenFile(filePath, accessMode, nullptr); }

        /// Closes the given file.
        ///
        /// @param hFile [in] A handle to the file to close.
        ///
        /// @remarks
        ///     This will unmap the file if it is already mapped.
        void CloseFile(HFile hFile);

        /// Reads data from the given file and increments the read/write pointer.
        ///
        /// @param hFile       [in]  A handle to the file to read the data from.
        /// @param bytesToRead [in]  The number of bytes to read.
        /// @param dataOut     [out] A pointer to the buffer that will receive the file data.
        ///
        /// @return The number of bytes actually read.
        ///
        /// @remarks
        ///     If the return value is less than \c bytesToRead than it can be assumed the end of the file has been reached.
        size_t ReadFile(HFile hFile, size_t bytesToRead, void* dataOut);

        /// Writes data to the given file.
        ///
        /// @param hFile        [in] A handle to the file to write the data to.
        /// @param bytesToWrite [in] The number of bytes to write.
        /// @param data         [in] A pointer to the buffer that contains the data to write.
        ///
        /// @return The number of bytes that were written.
        size_t WriteFile(HFile hFile, size_t bytesToWrite, const void* data);

        /// Seeks the read/write pointer of the given file by the given number of bytes.
        ///
        /// @param hFile       [in] A handle to the file to seek.
        /// @param bytesToSeek [in] The number of bytes to seek.
        /// @param origin      [in] Where to seek from (current position, beginning of the file, end of the file).
        ///
        /// @return The number of bytes seeked.
        int64_t SeekFile(HFile hFile, int64_t bytesToSeek, FileSeekOrigin origin);

        /// Retrieves the position of the read/write pointer of the given file.
        ///
        /// @param hFile [in] A handle to the file whose read/write pointer is being retrieved.
        ///
        /// @return The position of the read/write pointer.
        ///
        /// @remarks
        ///     If the file is at the end of the file, this should return the same value as GetSize(). Use this assertion to check if the file's read/write pointer is
        ///     at the end of the file.
        int64_t TellFile(HFile hFile);
        
        /// Retrieves the size of the given file.
        ///
        /// @param hFile [in] A handle to the file whose size is being retrieved.
        ///
        /// @return The size in bytes of the file.
        int64_t GetFileSize(HFile hFile);

        /// Maps the file.
        ///
        /// @param hFile [in] A handle to the file to map.
        ///
        /// @return A pointer to the buffer that represents the contents of the file.
        ///
        /// @remarks
        ///     If the file does not support mapping, this will return null.
        ///     @par
        ///     The file can only be mapped if it was opened in read mode.
        void* MapFile(HFile hFile, size_t length, int64_t offset);

        /// Unmaps the given file.
        ///
        /// @param hFile [in] A handle to the file to map.
        ///
        /// @remarks
        ///     The file will be automatically unmapped when CloseFile is called.
        void UnmapFile(HFile hFile);


    private:

        /// The list of base directories.
        GTLib::Vector<GTLib::String> m_baseDirectories;

        /// The list of low-priority base directories.
        GTLib::Vector<GTLib::String> m_baseDirectoriesLowPriority;
    };
}

#endif
