// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_IO_hpp_
#define __GTLib_IO_hpp_

#include "IO/FileIterator.hpp"
#include "IO/cstdio.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <cstdio>

// The following #undef's are just in case this file is included after windows.h.
#undef DeleteFile
#undef CopyFile
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef CreateDirectory

namespace GTLib
{
    namespace IO
    {
        /// Retrieves the current directory.
        ///
        /// @return A pointer to the current directory.
        GTLib::String GetCurrentDirectory();

        /// Sets the current directory.
        ///
        /// @param  directory [in] The new directory that should be made current.
        /// @param  create    [in] Specifies whether or not to create the directories if required.
        ///
        /// @return                True if the directory is successfully changed; false otherwise.
        ///
        /// @remarks
        ///     The directory can be relative or absolute.
        ///     @par
        ///     This function can be used to create a directory.
        bool SetCurrentDirectory(const char* directory, bool create = false);

        /// Pushes the current directory onto the directory stack so that it can be restored later.
        void PushCurrentDirectory();

        /// Removes the top directory from the stack and sets it as the current directory.
        void PopCurrentDirectory();


        /// Adds an additional search directory for relative paths.
        ///
        /// @param absolutePath [in] The absolute path to use for finding files with relative paths
        void AddAdditionalSearchPath(const char* absolutePath);

        /// Removes a path that was added with AddAdditionalSearchPath().
        ///
        /// @param absolutePath [in] The path to remove from the additional search paths.
        void RemoveAdditionalSearchPath(const char* absolutePath);

        /// Retrieves the number of additional search paths.
        size_t GetAdditionalSearchPathCount();

        /// Retrieves an additional search path by it's index. Returns null if the index is invalid.
        GTLib::String GetAdditionalSearchPath(size_t index);

        /// Finds the full absolute file path of the given relative file name.
        ///
        /// @param relativePath    [in ] The relative path whose absolute path is being retrieved.
        /// @param absolutePathOut [out] A reference to the string object that will receive the absolute path.
        ///
        /// @return True if the file was found; false otherwise.
        ///
        /// @remarks
        ///     This will include the current directory.
        ///     @par
        ///     If 'relativePath' is already absolute, this will return true if the file exists.
        bool FindAbsolutePath(const char* relativePath, GTLib::String &absolutePathOut);

        /// Converts a relative path to an absolute path.
        ///
        /// @param relativePath [in] The relative path that is being made absolute.
        /// @param basePath     [in] The base path.
        ///
        /// @remarks
        ///     If the base path is null, the current directory will be used.
        GTLib::String ToAbsolutePath(const char* relativePath, const char* basePath);
        GTLib::String ToAbsolutePath(const char* relativePath);

        /// Converts an absolute path to a relative path based on the given base path.
        ///
        /// @param absolutePath [in] The absolute path that is being made relative.
        /// @param basePath     [in] The base path to make 'absolutePath' relative to.
        ///
        /// @remarks
        ///     If the base path is null, the current directory will be used.
        ///     @par
        ///     This function does not evaluate ".." segments. These segments will be treated as a normal segment. Therefore, this
        ///     function assumes the input paths have been cleaned before hand.
        GTLib::String ToRelativePath(const char* absolutePath, const char* basePath);
        GTLib::String ToRelativePath(const char* absolutePath);
        
        
        /// Appends two paths together and cleans them at the same time.
        ///
        /// @param part1 [in] The first (left side) part of the path.
        /// @param part2 [in] The second (right side) part of the path.
        ///
        /// @return A string containing the concatendated and cleaned path.
        ///
        /// @remarks
        ///     This function will ensure there is a slash dividing the two parts.
        GTLib::String AppendAndCleanPaths(const char* part1, const char* part2);
        
        
        /// Splits a path into it's segments and places it into the given list.
        ///
        /// @param path     [in]  The path to split.
        /// @param partsOut [out] A reference to the container that will receive the segments.
        ///
        /// @remarks
        ///     The length of the buffer required to store the path as a string, including the null terminator. This does not account for a trailing slash.
        size_t SplitAndCleanPath(const char* path, GTLib::Vector<StringSegmentUTF<const char>> &partsOut);


        /// Cleans the given path by resolving the "." and ".." segments.
        ///
        /// @param path [in] The path to clean.
        ///
        /// @return A string containing the cleaned path.
        GTLib::String CleanPath(const char* path);

        /// Cleans the given path by resolving the "." and ".." segments, and placing the result in the same string.
        ///
        /// @param path [in, out] The path to clean. This must be non-const because the function will be modifying it's data.
        ///
        /// @return The new length of the path string.
        size_t CleanPathInPlace(char* path);


        /// Builds a path from the given parts and writes them to the given string buffer.
        ///
        /// @param parts   [in]  The parts to build the path from.
        /// @param pathOut [out] A pointer to the string buffer that will receive the string. This will be null terminated.
        ///
        /// @remarks
        ///     This function assumes the buffer is large enough to store the path.
        void BuildPath(const GTLib::Vector<StringSegmentUTF<const char>> &parts, char* pathOut);

        
        
        /// Determines whether or not the path is absolute.
        bool IsPathAbsolute(const char* path);
        
        /// Determines whether or not the path is relative.
        bool IsPathRelative(const char* path);

        
        /// Removes the file name from the given path.
        GTLib::String RemoveFileNameFromPath(const char* path);
        inline GTLib::String RemoveFileNameFromPath(const GTLib::String &path) { return RemoveFileNameFromPath(path.c_str()); }
        
        /// Retrieves the parent directory of the given path.
        GTLib::String GetParentDirectoryPath(const char* path);
        inline GTLib::String GetParentDirectoryPath(const GTLib::String &path) { return GetParentDirectoryPath(path.c_str()); }
        
        
        /// Splits a path into is base and file name sections.
        ///
        /// @param path [in] The path to split.
        /// @param basePathOut [out] A reference to the string that will receive the base part.
        /// @param fileNameOut [out] A reference to the string that will receive the file name part.
        void SplitPath(const char* path, GTLib::StringSegmentUTF<const char> &basePathOut, GTLib::StringSegmentUTF<const char> &fileNameOut);
        void SplitPath(const char* path, GTLib::String &basePathOut, GTLib::String &fileNameOut);


        /**
        *   \brief                            Creates a directory, and it's parent's.
        *   \param  directory            [in] The path of the directory to create. Can be absolute or relative to the current directory.
        *   \param  directorySizeInBytes [in] The size in bytes of the directory string. Use this if the directory can not easily be null terminated. Can be -1; see remarks.
        *   \return                           True if the directory was created successfully; false otherwise.
        *
        *   \remarks
        *       If the directory can not be null terminated, set \c directorySizeInBytes to the size in bytes of the directory string.
        */
        bool MakeDirectory(const char* directory, ptrdiff_t directorySizeInBytes = -1);
        inline bool CreateDirectory(const char* directory, ptrdiff_t directorySizeInBytes = -1) { return GTLib::IO::MakeDirectory(directory, directorySizeInBytes); }


        /// Deletes the directory at the given path.
        ///
        /// @param directoryPath [in] The path of the directory to delete.
        ///
        /// @remarks
        ///     This will recursively delete the contents of the directory.
        ///     @par
        ///     If 'directoryPath' is relative, it will be assumed that it is relative to the current directory.
        void DeleteDirectory(const char* directoryPath);


        /// Determines whether or not the given directory exists.
        bool DirectoryExists(const char* directory);


        /**
        *   \brief            Retrieves a pointer to the first character in the file name portion of a path.
        *   \param  path [in] The path whose file name is being retrieved.
        *   \return           A pointer to the first character in the file name.
        *
        *   \remarks
        *       The returned pointer will be just an offset of \c path.
        */
        const char* FileName(const char* path);

        /// Removes the extension from the given path.
        ///
        /// @param path [in] The path whose extension is being removed.
        GTLib::String RemoveExtension(const char* path);
        
        /// Retrieves a pointer to the extension of the given path.
        ///
        /// @param path [in] The path whose extension is being retrieved.
        const char* GetExtension(const char* path);
    }
}


// Functions for working with files. The purpose of these file is to provide a cross-compiler and cross-platform
// set of functions for working with low-level C files.
#include <sys/types.h>
#include <sys/stat.h>
namespace GTLib
{
    typedef size_t FileHandle;
    
    /// Enumerator for the different movement origins when seeking a file.
    enum class SeekOrigin : int
    {
        Start   = SEEK_SET,
        Current = SEEK_CUR,
        End     = SEEK_END
    };
    
    /// Structure containing information about a file.
    struct FileInfo
    {
        /// Default constructor.
        FileInfo();

        /// Copy constructor.
        FileInfo(const FileInfo &other);

        /// Constructor.
        FileInfo(const char* path);

        /// Destructor.
        virtual ~FileInfo();                // <-- Intentionally virtual.


        /// Sets both the regular and absolute path of the file.
        ///
        /// @param path [in] The regular path of the file. The absolute path will be derived from this based on the current directory.
        bool SetPath(const char* path);


        /////////////////////////////////////////////////////////////
        // Operators.

        /// Assignment.
        FileInfo & operator=(const FileInfo &other);

        

        /////////////////////////////////////////////////////////////
        // Attributes

        /// The path of the file. Can be relative or absolute.
        GTLib::String path;

        /// The absolute path of the file.
        GTLib::String absolutePath;

        /// The size of the file.
        int64_t size;

        /// The time of the last modification.
        time_t lastModifiedTime;

        /// Whether or not the file is a directory.
        bool isDirectory;

        /// Whether or not the file actually exists.
        bool exists;
    };

    
    /// Opens a file.
    ///
    /// @param filePath    [in]  The path of the file to load.
    /// @param openMode    [in]  A combination of OpenMode flags specifying how the file should be opened. Files will be opened in binary mode by default.
    /// @param fileInfoOut [out] A reference to the FileInfo structure that will receive information about the file.
    ///
    /// @remarks
    ///     Files are always opened in binary mode.
    ///     @par
    ///     The returned file needs to support memory mapping.
    ///     @par
    ///     If using both read and write, consider using MapFile/UnmapFile instead of ReadFile/WriteFile. When using ReadFile/WriteFile, there is only
    ///     a single, shared file pointer. You can set the pointer with SeekFile().
    FileHandle OpenFile(const char* filePath, unsigned int openMode);
    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo &fileInfoOut);
    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo* fileInfoOut);
    
    /// Closes a file.
    ///
    /// @param file [in] The file to close.
    ///
    /// @remarks
    ///     If the file is mapped, it will be unmapped.
    void CloseFile(FileHandle file);
    
    
    /// Retrieves the size of the given file.
    ///
    /// @param file [in] The file whose size is being retrieved.
    int64_t GetFileSize(FileHandle file);
    
    
    /// Memory maps a file.
    ///
    /// @param file   [in] The file to memory map.
    /// @param length [in] The size of the memory block in bytes.
    /// @param offset [in] The offset in the file where the returned pointer should begin. This will need to be aligned.
    ///
    /// @remarks
    ///     If a file has already been mapped, this will just return a pointer to the already-mapped region. If you want a different
    ///     sized mapping, unmap first.
    ///     @par
    ///     For reading a file, you can use GetFileSize() to determine the mapping size. Note how the return value from GetFileSize()
    ///     is an int64_t, wherease the 'length' argument in MapFile() is a size_t.
    ///     @par
    ///     On Windows, it appears that file mapping must support reading (let me know if that's incorrect and how to resolve). Thus,
    ///     this function will fail if OpenMode::Read was not specified when opening the file with OpenFile(). For consistency, and
    ///     to help prevent cross-platform bugs, this restriction is also placed on the Linux build.
    void* MapFile(FileHandle file, size_t length, int64_t offset = 0);
    
    /// Unmaps the given file.
    ///
    /// @param file [in] The file to unmap.
    ///
    /// @remarks
    ///     If the file is not mapped, this will do nothing an return immediately. A file will be unmapped when it is closed.
    void UnmapFile(FileHandle file);
    
    /// Retrieves the alignment file offsets need to be set at when calling MapFile().
    ///
    /// @remarks
    ///     On Windows, this is set to dwAllocationGranularity from the SYSTEM_INFO structure returned by GetSystemInfo().
    ///     @par
    ///     On Linux, this the value returned by sysconf(_SC_PAGE_SIZE).
    int64_t GetFileMappingOffsetAlignment();
    
    
    /// Reads data from the given file, starting from the current file pointer.
    ///
    /// @param file        [in]  The file to read the data from.
    /// @param bufferOut   [out] A pointer to the buffer that will receive the data.
    /// @param bytesToRead [in]  The number of bytes to read.
    ///
    /// @return The number of bytes that were read. Returns 0 if the file is at the end of the file. Returns -1 if an error occurs.
    ///
    /// @remarks
    ///     The return value can be less than that specified by bytesToRead, which usually means the end of the file has been reached.
    ///     @par
    ///     Consider using file mapping with MapFile/UnmapFile for massive files.
    ///     @par
    ///     The  file pointer used by ReadFile() is shared with WriteFile(). Use SeekFile() to change the position of the file pointer.
    intptr_t ReadFile(FileHandle file, void* bufferOut, size_t bytesToRead);
    
    /// Writes data to the given file, starting from the current file pointer.
    ///
    /// @param file         [in] The file to write the data to.
    /// @param buffer       [in] A pointer to the buffer that contains the data.
    /// @param bytesToWrite [in] The number of bytes to write.
    ///
    /// @return The number of bytes that were written. Returns 0 if nothing was written. Returns -1 if an error occurs.
    ///
    /// @remarks
    ///     Consider using file mapping with MapFile/UnmapFile for massive files.
    ///     @par
    ///     The file pointer used by WriteFile() is shared with ReadFile(). Use SeekFile() to change the position of the file pointer.
    intptr_t WriteFile(FileHandle file, const void* buffer, size_t bytesToWrite);
    
    /// Moves the file pointer for the given file.
    ///
    /// @param file   [in] The file whose file pointer is being moved.
    /// @param offset [in] The amount to move the file pointer. Can be negative, in which case it will be moved backwards.
    /// @param origin [in] The position to start seeking from.
    ///
    /// @return The new position of the file pointer.
    int64_t SeekFile(FileHandle file, int64_t offset, SeekOrigin origin);

    /// Retrieves the current position of the file pointer.
    ///
    /// @param file [in] The file whose file pointer is being retrieved.
    int64_t GetFilePointer(FileHandle file);
    
    
    
    namespace IO
    {
		/**
		*	\brief	Namespace for file opening modes.
		*
		*	We put these in a namespace so that you need to use OpenMode::* syntax. Also, we don't use an enum
		*	because the opening API uses bitwise-or operations to specify multiple modes.
		*/
		namespace OpenMode
		{
			static const unsigned int Binary       = 0;
			static const unsigned int Text         = (1 << 0);
			static const unsigned int Read         = (1 << 1);
			static const unsigned int Write        = (1 << 2);
			static const unsigned int Append       = (1 << 3);
            static const unsigned int CreateDirs   = (1 << 4);
            static const unsigned int NoSearchDirs = (1 << 5);
		}

        /**
        *   \brief                Cross compiler/platform function for opening a C file.
        *   \param  filename [in] The name and path of the file to open. Can be absolute or relative.
        *   \param  openMode [in] A combination of OpenMode flags specifying how the file should be opened. Files will be opened in binary mode by default.
        *   \return               A pointer to the new FILE object, or nullptr if an error occurs.
        *
        *   \remarks
        *       One or both OpenMode_Read/OpenMode_Write must be specified, otherwise the function will fail.
        *       \par
        *       The file will be opened in binary mode by default.
        *       \par
        *       The returned file can be used in the stdio functions (fread, fseek, etc) if needed. The file will be opened in 64-bit mode, but other functions
        *       will always take a size_t type. The file can still be used just fine in 32-bit mode, but you will not be able to pass 64-bit types to other
        *       functions.
        *       \par
        *       Note that when OpenMode_Write is set in \c openMode, but not OpenMode_Append, the contents of an existing file is erased.
        */
        FILE* Open(const char *filename, unsigned int openMode, bool useSearchDirectories = true);

        /**
        *   \brief  Closes a file previously opened with Open().
        */
        void Close(FILE *file);

        /**
        *   \brief                Reads data from a file.
        *   \param  file     [in] A pointer to the file to read from.
        *   \param  dest     [in] A pointer to the buffer that will receive the data. Must be at least \c numBytes in size.
        *   \param  numBytes [in] The number of bytes to read from the file.
        *   \return               The number of bytes that were read.
        *
        *   \remarks
        *       If the return value is different to \c numBytes, it means either there was an error or the end of the file was reached. Use AtEnd() to determine
        *       if the end of the file was reached.
        */
        size_t Read(FILE *file, void *dest, size_t numBytes);

        /**
        *   \brief                Writes data to a file.
        *   \param  file     [in] A pointer to the C file to write to.
        *   \param  source   [in] A pointer to the buffer that contains the data to write.
        *   \param  numBytes [in] The number of bytes to write to the file.
        *   \return               The number of bytes written to the file.
        */
        size_t Write(FILE *file, const void *source, size_t numBytes);

        /**
        *   \brief              Writes a null-terminated string to the file, but does not write the null-terminator itself.
        *   \param  file   [in] A pointer to the C file to write to.
        *   \param  source [in] The source string to write.
        *   \return             True if the string will written successfully; false otherwise.
        */
        bool WriteString(FILE *file, const char *source);

        /**
        *   \brief              Cross compiler/platform function for seeking of a C file.
        *   \param  file   [in] A pointer to the file that is beeing seeked.
        *   \param  offset [in] The amount to seek.
        *   \param  origin [in] The origin of the seek.
        *   \return             True if the seek was successful; false otherwise.
        *
        *   \remarks
        *       Take note of the return value - it is not the same as fseek().
        */
        template <typename T>
        bool Seek(FILE* file, T offset, SeekOrigin origin)
        {
        #if defined(GT_COMPILER_VC)
            return _fseeki64(file, static_cast<int64_t>(offset), static_cast<int>(origin)) == 0;
        #else
            return fseeko64(file, static_cast<int64_t>(offset), static_cast<int>(origin)) == 0;
        #endif
        }

        /**
        *   \brief            Retrieves the value of the position indicator of the given file.
        *   \param  file [in] The file whose position indicator is being retrieved.
        *   \return           The value of the position indicator of the given file.
        */
        int64_t Tell(FILE *file);

        /**
        *   \brief            Determines if a file is at the end.
        *   \param  file [in] A pointer to the file whose EOF flag is being checked.
        *   \return           True if the file has it's EOF flag set; false otherwise.
        */
        bool AtEnd(FILE *file);

        /**
        *   \brief            Writes any cached data to the file.
        *   \param  file [in] A pointer to the file that's being flushed.
        *   \return           True if the file was flushed successfully; false otherwise.
        */
        bool Flush(FILE *file);


        /**
        *   \brief            Helper function for retrieving the size of the given file. Only works for files opened in binary mode.
        *   \param  file [in] The file whose size is being retrieved.
        *   \return           The size of the file.
        */
        int64_t Size(FILE *file);

        /**
        *   \brief                Determines whether or not a file exists.
        *   \param  filename [in] The name and path of the file to check.
        *   \return               True if the file exists; false otherwise.
        */
        bool FileExists(const char *filename);

        /**
        *   \brief              Copies a file.
        *   \param  source [in] The file name and path of the file that is being copied.
        *   \param  dest   [in] The destination file.
        *   \return             True if the file is copied successfully; false otherwise.
        *
        *   \remarks
        *       This function will overwrite the destination file if it alread exists.
        */
        bool CopyFile(const char *source, const char *dest);

		/**
		*	\brief	Deletes the given file.
		*	\return	True if the file was deleted; false otherwise.
		*/
		bool DeleteFile(const char *filename);


        /// Retrieves information about a file.
        ///
        /// @param filename [in]  The name of the file whose info is being retrieved.
        /// @param dest     [out] A reference to the FileInfo structure that will receive the information.
        ///
        /// @remarks
        ///     This will search the directory search paths.
        bool GetFileInfo(const char* filename, FileInfo &dest);

        /// Retrieves information about a file from an absolute path.
        ///
        /// @param absolutePath [in] The absolute path of the file whose info is being retrieved.
        /// @param dest         [out] A reference to the FileInfo structure that will receive the information.
        ///
        /// @remarks
        ///     This will NOT search the directory search paths.
        ///     @par
        ///     This is more efficient than GetFileInfo() because it does not need to search the directory search paths. If the absolute path
        ///     of the file is known, this function should be used instead.
        bool GetFileInfoFromAbsolutePath(const char* absolutePath, FileInfo &dest);




        ////////////////////////////////////////////////////
        // IO Helpers.

        /// A helper function for reading the entire contents of a text file.
        ///
        /// @param fileName [in]  The name of the file to load.
        /// @param content  [out] A reference to the string that will receive the file contents.
        ///
        /// @return True if the file was read successfully; false otherwise.
        bool OpenAndReadTextFile(const char* fileName, GTLib::String &content);

        /// A helper function for writing the given string to a text file.
        ///
        /// @param fileName           [in] The name of the file to save.
        /// @param content            [in] The string to write to the file.
        /// @param contentSizeInBytes [in] The size in bytes of 'content'.
        ///
        /// @return True if the file was written successfully; false otherwise.
        ///
        /// @remarks
        ///     This will overwrite any existing file.
        bool OpenAndWriteTextFile(const char* fileName, const char* content, ptrdiff_t contentSizeInBytes = -1);
    }
}

#endif
