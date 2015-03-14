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

namespace GT
{
    typedef size_t HFile;

    enum class FileAccessMode
    {
        Read      = 0x01,
        Write     = 0x02,
        ReadWrite = Read | Write
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

        /// Removes a base directory by it's absolute path.
        ///
        /// @param absolutePath [in] The absolute path of the base directory to remove.
        void RemoveBaseDirectory(const char* absolutePath);

        /// Removes all base directories.
        void RemoveAllBaseDirectories();


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
        HFile OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode* resultCodeOut);
        HFile OpenFile(const char* filePath, FileAccessMode accessMode, ResultCode &resultCodeOut) { this->OpenFile(filePath, accessMode, resultCodeOut); }
        HFile OpenFile(const char* filePath, FileAccessMode accessMode) { this->OpenFile(filePath, accessMode, nullptr); }

        /// Closes the given file.
        ///
        /// @param hFile [in] A handle to the file to close.
        void CloseFile(HFile hFile);

        /// Retrieves the size of the given file.
        ///
        /// @param hFile [in] A handle to the file whose size is being retrieved.
        ///
        /// @return The size in bytes of the file.
        int64_t GetFileSize(HFile hFile);





        ///////////////////////////////////////////////////////////////////
        // Static Helpers


    private:

        /// The list of base directories.
        GTLib::Vector<GTLib::String> m_baseDirectories;
    };
}

#endif
