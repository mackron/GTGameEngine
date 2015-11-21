// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/IO.hpp>
#include <GTLib/Strings/Tokenizer.hpp>
#include <GTLib/String.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/stdlib.hpp>
#include <cstring>
#include <cerrno>


#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <direct.h>
	#if defined(GT_COMPILER_VC)
	#include <io.h>
	#endif
#elif defined(GT_PLATFORM_LINUX)
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <cstdio>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // non-standard extension.
#endif

// Directory stuff.
namespace GTLib
{
    namespace IO
    {
        /// This contains the current directory.
        static GTLib::String CurrentDirectory;

        /// A helper function for placing the current directory into the given string.
        void __GetCurrentDirectory(GTLib::String &dest)
        {
            char buffer[1024];
            buffer[1023] = '\0';

        #if defined(GT_PLATFORM_WINDOWS)
            _getcwd(buffer, 1024);
            GTLib::Path::FormatSlashes(buffer, '/');     // <-- The resulting path will use back-slashes. We need to convert to forward-slashes.
        #else
            char *silencer = getcwd(buffer, 1024);          // <-- 'silencer' is just used to silence a warning with GCC.
            (void)silencer;
        #endif

            dest = buffer;
        }


        const char* GetCurrentDirectory()
        {
            if (CurrentDirectory.GetLength() == 0)
            {
                __GetCurrentDirectory(CurrentDirectory);
            }

            return CurrentDirectory.c_str();
        }


        bool SetCurrentDirectory(const char *directory, bool create)
        {
			if (directory == nullptr || *directory == '\0')	    // <-- don't go anywhere on an empty string...
			{
				return true;
			}

        #if defined(_MSC_VER)
            int chdir_result = _chdir(directory);
        #else
            int chdir_result = chdir(directory);
        #endif
            if (chdir_result == -1)
            {
                // We have an error so try creating the directory if applicable.
                if (errno == ENOENT && create == true)
                {
                    if (IO::MakeDirectory(directory))
                    {
                        bool result = IO::SetCurrentDirectory(directory, false);
                        if (result)
                        {
                            // We need to updated 'CurrentDirectory' to contain the new absolute directory.
                            __GetCurrentDirectory(CurrentDirectory);
                        }

                        return result;
                    }
                    else
                    {
                        printf("Error making directory with IO::MakeDirectory(\"%s\")\n", directory);
                        return false;
                    }
                }

                return false;
            }

            // We need to updated 'CurrentDirectory' to contain the new absolute directory.
            __GetCurrentDirectory(CurrentDirectory);
            return true;
        }

        bool MakeDirectory(const char *directory, ptrdiff_t directorySize)
        {
            if (directory == nullptr || directorySize == 0)
            {
                return true;
            }

            bool result = true;

            // We're going to be modifying the current directory. We'll want to restore it later.
            PushCurrentDirectory();

            // On an absolute directory that starts with a slash (/), the tokenizer will actually skip that root directory.
            // this will cause the directories to be created relative to the current directory rather than absolutely. Therefore,
            // we need to check if the directory is absolute, and if so, move into the root directory first.
            if (*directory == '/')
            {
                SetCurrentDirectory("/", false);
            }

            // We are going to split the entire directory and try to move into each one. If we can't, we try creating it
            // and then try creating again. If that fails, we return false. It's important that we don't try to create the
            // directories when calling SetCurrentDirectory() since that calls this function.
            GT::BasicBuffer buffer;
            Strings::Tokenizer iPath(directory, directorySize, "/");
            while (iPath)
            {
                // We need to copy the path section into the temporary buffer because we need to make it null terminated. On
                // Windows we will also attach a slash to the end to account for cases such as "C:".
#if defined(GT_PLATFORM_WINDOWS)
                buffer.Allocate((size_t)iPath.GetSizeInTs() + 2);    // +1 for null terminator; +1 for slash.
#else
                buffer.Allocate((size_t)iPath.GetSizeInTs() + 1);    // +1 for null terminator.
#endif

                char* pathSection = static_cast<char *>(buffer.GetDataPointer());
                std::memcpy(pathSection, iPath.start, static_cast<size_t>(iPath.GetSizeInTs()));

#if defined(GT_PLATFORM_WINDOWS)
                pathSection[iPath.GetSizeInTs() + 0] = '/';
                pathSection[iPath.GetSizeInTs() + 1] = '\0';
#else
                pathSection[iPath.GetSizeInTs() + 0] = '\0';
#endif

                if (!SetCurrentDirectory(pathSection, false))
                {
                    // We need to try creating. We can check if it worked by trying to move into it again.
                    if (IO::mkdir(pathSection) == -1)
                    {
                        if (errno != EEXIST)
                        {
                            // There was an error.
                            result = false;
                            break;
                        }
                    }
                    else
                    {
                        // The directory was created successfully, so now we will enter it.
                        if (!SetCurrentDirectory(pathSection, false))
                        {
                            // We still can't move into the directory for some reason.
                            result = false;
                            break;
                        }
                    }
                }

                ++iPath;
            }

            PopCurrentDirectory();
            return result;
        }


        void DeleteDirectory(const char* directoryPath)
        {
            // We need to do this recursively.
            FileIterator iFile((GTLib::String(directoryPath) + "/.*").c_str());
            while (iFile)
            {
                if (iFile.isDirectory)
                {
                    DeleteDirectory(iFile.absolutePath);
                }
                else
                {
                    IO::DeleteFile(iFile.absolutePath);
                }

                ++iFile;
            }

            // The directory should now be empty, so now we can delete the directory itself.
        #if defined(GT_PLATFORM_WINDOWS)
            RemoveDirectoryA(directoryPath);
        #else
            rmdir(directoryPath);
        #endif
        }


        bool DirectoryExists(const char* directory)
        {
        #if defined(GT_PLATFORM_WINDOWS)
            DWORD attributes = GetFileAttributes(directory);
            return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
        #else
            if (access(directory, F_OK) == 0)
            {
                struct stat64 status;
                stat64(directory, &status);

                return (status.st_mode & S_IFDIR) != 0;
            }

            return false;
        #endif
        }


        //--- Directory stack stuff. ---

        /**
        *   \brief  Structure for the directory stack item.
        */
        struct _DirectoryStackItem
        {
            _DirectoryStackItem(const char *valueIn, _DirectoryStackItem *below)
                : value(valueIn), below(below)
            {
            }


            /// The value of the stack item.
            GTLib::String value;

            /// The stack item below this one.
            _DirectoryStackItem* below;


        private:    // No copying.
            _DirectoryStackItem(const _DirectoryStackItem &);
            _DirectoryStackItem & operator=(const _DirectoryStackItem &);
        };

        /// The top stack item.
        _DirectoryStackItem *DirectoryStack_Top = nullptr;


        void PushCurrentDirectory()
        {
            // We need a new top item.
            auto newTop = new _DirectoryStackItem(IO::GetCurrentDirectory(), DirectoryStack_Top);
            DirectoryStack_Top = newTop;
        }

        void PopCurrentDirectory()
        {
            if (DirectoryStack_Top)
            {
                IO::SetCurrentDirectory(DirectoryStack_Top->value.c_str());

                auto oldTop = DirectoryStack_Top;
                DirectoryStack_Top = oldTop->below;

                delete oldTop;
            }
        }



        /// The list of additional search paths for relative files.
        static GTLib::Vector<GTLib::String> AdditionalSearchDirectories;

        void AddAdditionalSearchPath(const char* absolutePath)
        {
            if (!AdditionalSearchDirectories.Exists(absolutePath))
            {
                AdditionalSearchDirectories.PushBack(absolutePath);
            }
        }

        void RemoveAdditionalSearchPath(const char* absolutePath)
        {
            AdditionalSearchDirectories.RemoveFirstOccuranceOf(absolutePath);
        }

        size_t GetAdditionalSearchPathCount()
        {
            return AdditionalSearchDirectories.count;
        }

        GTLib::String GetAdditionalSearchPath(size_t index)
        {
            if (index < AdditionalSearchDirectories.count)
            {
                return AdditionalSearchDirectories[index];
            }

            return nullptr;
        }

        bool FindAbsolutePath(const char* relativePath, GTLib::String &absolutePath)
        {
            if (!IO::IsPathAbsolute(relativePath))
            {
                GTLib::String absolutePathIn(IO::GetCurrentDirectory());
                absolutePathIn += "/";

                if (GTLib::IO::FileExists((absolutePathIn + relativePath).c_str()))
                {
                    absolutePath = ToAbsolutePath(relativePath, absolutePathIn.c_str());        // <-- Force a clean path.
                    return true;
                }
                else
                {
                    // We need to check each of the additional search directories.
                    for (size_t i = 0; i < AdditionalSearchDirectories.count; ++i)
                    {
                        absolutePathIn = AdditionalSearchDirectories[i] + "/";
                        if (GTLib::IO::FileExists((absolutePathIn + relativePath).c_str()))
                        {
                            absolutePath = ToAbsolutePath(relativePath, absolutePathIn.c_str());
                            return true;
                        }
                    }
                }
            }
            else
            {
                if (GTLib::IO::FileExists(relativePath))       // <-- 'relativePath' is actually an absolute path if we make it to this branch.
                {
                    absolutePath = relativePath;
                    return true;
                }
            }
            
            return false;
        }

        GTLib::String ToAbsolutePath(const char* relativePath, const char* basePath)
        {
            if (IO::IsPathRelative(relativePath) && IO::IsPathAbsolute(basePath))
            {
                return IO::AppendAndCleanPaths(basePath, relativePath);
            }
            
            
            // We'll get here if one of the paths is not the correct type.
            return relativePath;
        }

        GTLib::String ToAbsolutePath(const char* relativePath)
        {
            return ToAbsolutePath(relativePath, IO::GetCurrentDirectory());
        }


        GTLib::String ToRelativePath(const char* absolutePath, const char* basePath)
        {
            if (IO::IsPathAbsolute(absolutePath) && IO::IsPathAbsolute(basePath))
            {
                assert(absolutePath != nullptr);
                assert(basePath     != nullptr);
                
                
                // We do this in two phases. The first phase simply removes everything from the absolute path until we find
                // a non-matching character. The second phase is where the ".." parts of the path are added.
                size_t endOfLastPartAbsolute = 0;
                size_t endOfLastPartBase     = 0;
                size_t iCharAbsolute = 0;
                size_t iCharBase     = 0;
                
                char c1;
                char c2;
                do
                {
                    c1 = absolutePath[iCharAbsolute];
                    c2 = basePath[iCharBase];
                    
                    
                    if (c1 == '/' || c1 == '\\')
                    {
                        do
                        {
                            endOfLastPartAbsolute = iCharAbsolute;
                            c1 = absolutePath[++iCharAbsolute];
                            
                            if (c1 == '.')
                            {
                                c1 = absolutePath[++iCharAbsolute];
                            }
                            
                        } while (c1 == '/' || c1 == '\\');
                    }
                    else
                    {
                        iCharAbsolute += 1;
                    }
                    
                    
                    if (c2 == '/' || c1 == '\\')
                    {
                        do
                        {
                            endOfLastPartBase = iCharBase;
                            c2 = basePath[++iCharBase];
                            
                            if (c2 == '.')
                            {
                                c2 = basePath[++iCharBase];
                            }
                            
                        } while (c2 == '/' || c2 == '\\');
                    }
                    else
                    {
                        iCharBase += 1;
                    }
                    
                } while (c1 != '\0' && c2 != '\0' && c1 == c2);
                
                // If both strings are at the end it means they are equal and the result will just be an empty path.
                if (c1 == '\0' && c2 == '\0')
                {
                    return "";
                }




                // Now we need to do the ".." phase.
                GTLib::String result;
                
                if (c2 != '\0')
                {
                    iCharBase = endOfLastPartBase;
                    while ((c2 = basePath[iCharBase]) != '\0')
                    {
                        if (c2 == '/' || c2 == '\\')
                        {
                            c2 = basePath[iCharBase + 1];
                            if (c2 != '/' && c2 != '\\' && c2 != '\0')
                            {
                                result     += "../";
                                iCharBase  += 1;        // <-- Don't need to check that character again.
                            }
                        }
                        
                        iCharBase += 1;
                    }
                }
                
                
                result += absolutePath + endOfLastPartAbsolute + 1;
                return result;
            }
            
            return "";
        }

        GTLib::String ToRelativePath(const char* absolutePath)
        {
            return ToRelativePath(absolutePath, IO::GetCurrentDirectory());
        }
        
        
        GTLib::String AppendAndCleanPaths(const char* part1, const char* part2)
        {
            size_t pathLength = 0;

            // We need to find the start and end positions of each section in the first part so we can handle the ".." segments in the second part.
            GTLib::Vector<StringSegmentUTF<const char>> pathSegments(32);
            pathLength += IO::SplitAndCleanPath(part1, pathSegments);
            pathLength += IO::SplitAndCleanPath(part2, pathSegments);




            
            // We have the parts, so now we just need to build the string.
            GTLib::String result;
            for (size_t iSegment = 0; iSegment < pathSegments.GetCount(); ++iSegment)
            {
                auto &segment = pathSegments[iSegment];
                
                result.Append(segment.strBegin, segment.strEnd - segment.strBegin);
                
                if (iSegment + 1 < pathSegments.GetCount())
                {
                    result.Append("/", 1);
                }
            }
            
            return result;
        }
        
        
        inline int SplitAndCleanPath_Append(const char* path, uint32_t index0, uint32_t index1, GTLib::Vector<StringSegmentUTF<const char>> &partsOut)
        {
            int length = static_cast<int>(index1 - index0);         // <-- Initially will never be negative, but can be mutated to make it negative in one case.
            if (length > 2)
            {
                partsOut.PushBack(StringSegmentUTF<const char>(path + index0, path + index1));
            }
            else
            {
                if (length == 2)
                {
                    if (path[index0] != '.' || path[index0 + 1] != '.')
                    {
                        partsOut.PushBack(StringSegmentUTF<const char>(path + index0, path + index1));
                    }
                    else
                    {
                        if (partsOut.GetCount() > 0)
                        {
                            length = -static_cast<int>(partsOut.GetBack().strEnd - partsOut.GetBack().strBegin);
                            partsOut.PopBack();
                        }
                        else
                        {
                            length = 0;
                        }
                    }
                }
                else
                {
                    if (length == 1)
                    {
                        if (path[index0] != '.')
                        {
                            partsOut.PushBack(StringSegmentUTF<const char>(path + index0, path + index1));
                        }
                        else
                        {
                            length = 0;
                        }
                    }
                }
            }
            
            
            // Add or subtract one to compensate for the slash separator or null terminator.
            if (length > 0)
            {
                return length + 1;
            }
            else if (length < 0)
            {
                return length - 1;
            }


            assert(length == 0);
            return length;
        }
        
        size_t SplitAndCleanPath(const char* path, GTLib::Vector<StringSegmentUTF<const char>> &partsOut)
        {
            if (path != nullptr)
            {
                size_t resultLength = 0;

                uint32_t currentIndex0 = 0;
                uint32_t currentIndex1 = 0;
            
                // We add an empty part for the initial slash, if we have one. This is the only place where an empty part is allowed.
                if (path[currentIndex0] == '/')
                {
                    partsOut.PushBack(StringSegmentUTF<const char>(path + currentIndex0, path + currentIndex0));
                
                    currentIndex0 = 1;
                    currentIndex1 = 1;

                    resultLength  = 1;   // <-- Take the slash into account.
                }
            
                char c;
                do
                {
                    c = path[currentIndex1];
                    if (c == '/' || c == '\\')
                    {
                        resultLength += SplitAndCleanPath_Append(path, currentIndex0, currentIndex1, partsOut);
                    

                        currentIndex0 = currentIndex1;
                        {
                            do { c = path[++currentIndex0]; } while (c == '/' || c == '\\');
                        }
                        currentIndex1 = currentIndex0;
                    }
                    else
                    {
                        ++currentIndex1;
                    }
                
                } while (c != '\0');
            
            
                // The last part needs to be added in case the path did not end with a slash and thus wasn't hit by the loop above.
                if (currentIndex0 != currentIndex1)
                {
                    resultLength += SplitAndCleanPath_Append(path, currentIndex0, currentIndex1 - 1, partsOut);     // <-- Subtract 1 because currentIndex1 will be just beyond the null terminator.
                }


                return resultLength;
            }

            return 0;
        }


        GTLib::String CleanPath(const char* path)
        {
            GTLib::Vector<StringSegmentUTF<const char>> pathSegments(32);
            IO::SplitAndCleanPath(path, pathSegments);

            
            // We have the parts, so now we just need to build the string.
            GTLib::String result;
            for (size_t iSegment = 0; iSegment < pathSegments.GetCount(); ++iSegment)
            {
                auto &segment = pathSegments[iSegment];
                
                result.Append(segment.strBegin, segment.strEnd - segment.strBegin);
                
                if (iSegment + 1 < pathSegments.GetCount())
                {
                    result.Append("/", 1);
                }
            }
            
            return result;
        }

        size_t CleanPathInPlace(char* path)
        {
            GTLib::Vector<StringSegmentUTF<const char>> pathSegments(32);
            size_t pathLength = IO::SplitAndCleanPath(path, pathSegments);

            IO::BuildPath(pathSegments, path);

            return pathLength;
        }


        void BuildPath(const GTLib::Vector<StringSegmentUTF<const char>> &segments, char* pathOut)
        {
            for (size_t iSegment = 0; iSegment < segments.GetCount(); ++iSegment)
            {
                auto &segment = segments[iSegment];

                size_t segmentLength = static_cast<size_t>(segment.strEnd - segment.strBegin);
                
                memmove(pathOut, segment.strBegin, segmentLength);
                pathOut += segmentLength;

                if (iSegment + 1 < segments.GetCount())
                {
                    pathOut[0] = '/';
                    pathOut += 1;
                }
            }

            pathOut[0] = '\0';  // <-- Ensure we are null terminated.
        }
        
        
        bool IsPathAbsolute(const char* path)
        {
            return Path::IsAbsolute(path);
        }
        
        bool IsPathRelative(const char* path)
        {
            return Path::IsRelative(path);
        }


        GTLib::String RemoveFileNameFromPath(const char* path)
        {
            GTLib::StringSegmentUTF<const char> basePath;
            GTLib::StringSegmentUTF<const char> fileName;
            SplitPath(path, basePath, fileName);
            
            return GTLib::String(basePath.strBegin, basePath.strEnd - basePath.strBegin);
        }
        
        GTLib::String GetParentDirectoryPath(const char* path)
        {
            size_t pathEnd = 0;
            size_t iChar   = 0;
            
            char c;
            do
            {
                c = path[iChar];
                if (c == '/' || c == '\\')
                {
                    size_t sectionEnd = iChar;
                    
                    do { c = path[++iChar]; } while (c == '/' || c == '\\');
                    
                    
                    if (c == '\0')
                    {
                        break;
                    }
                    else
                    {
                        pathEnd = sectionEnd;
                    }
                    
                }
                else
                {
                    ++iChar;
                }
                
            } while (c != '\0');
            
            return GTLib::String(path, pathEnd);
        }
        
        
        void SplitPath(const char* path, GTLib::StringSegmentUTF<const char> &basePathOut, GTLib::StringSegmentUTF<const char> &fileNameOut)
        {
            size_t lastSlashRegionStart = 0;
            size_t lastSlashRegionEnd   = 0;
            
            size_t iChar = 0;
            
            char c;
            do
            {
                c = path[iChar];
                if (c == '/' || c == '\\')
                {
                    lastSlashRegionStart = iChar;
                    {
                        do { c = path[++iChar]; } while (c == '/' || c == '\\');
                    }
                    lastSlashRegionEnd = iChar;
                }
                else
                {
                    ++iChar;
                }
                
            } while (c != '\0');
            
            
            basePathOut.strBegin = path;
            basePathOut.strEnd   = path + lastSlashRegionStart;
            
            fileNameOut.strBegin = path + lastSlashRegionEnd;
            fileNameOut.strEnd   = path + iChar - 1;
        }
        
        void SplitPath(const char* path, GTLib::String &basePathOut, GTLib::String &fileNameOut)
        {
            GTLib::StringSegmentUTF<const char> basePath;
            GTLib::StringSegmentUTF<const char> fileName;
            SplitPath(path, basePath, fileName);
            
            basePathOut = GTLib::String(basePath.strBegin, basePath.strEnd - basePath.strBegin);
            fileNameOut = GTLib::String(fileName.strBegin, fileName.strEnd - fileName.strBegin);
        }
        
        
        const char* FileName(const char *path)
        {
            // We just iterate over the string until we find the last occurance of a slash character. Since the slash characters are just
            // ASCII characters, we can do a standard iteration.
            auto result = path;
            
            char c;
            do
            {
                c = *path;
                if (c == '/' || c == '\\')
                {
                    result = path + 1;      // <-- Take it past the slash.
                }
                
                ++path;
                
            } while (c != '\0');
            
            
            return result;
        }


        GTLib::String RemoveExtension(const char* path)
        {
            auto extension = IO::GetExtension(path);
            if (extension > path && *(extension - 1) == '.')
            {
                extension -= 1;
            }

            return GTLib::String(path, extension - path);
        }

        
        const char* GetExtension(const char* path)
        {
            size_t lastDotPos = static_cast<size_t>(-1);
            size_t iChar      = 0;
            
            char c;
            do
            {
                c = path[iChar++];
                if (c == '.')
                {
                    do
                    {
                        c = path[iChar++];
                    } while (c == '.');
                 
                    
                    if (c != '/' && c != '\\')
                    {
                        lastDotPos = iChar - 1;
                    }
                    else
                    {
                        ++iChar;
                    }
                }
                
            } while (c != '\0');
            
            
            if (lastDotPos == static_cast<size_t>(-1))
            {
                return path + iChar - 1;
            }
            
            return path + lastDotPos;
        }
    }
}


// Files implementation.
namespace GTLib
{
#if defined(GT_PLATFORM_WINDOWS)
    struct _FileHandle
    {
        _FileHandle()
            : win32File(INVALID_HANDLE_VALUE), win32MappingHandle(nullptr), mappingPtr(nullptr), openMode(0), absolutePath()
        {
        }


        /// The Win32 file handle.
        HANDLE win32File;

        /// The Win32 file mapping handle.
        HANDLE win32MappingHandle;

        /// A pointer to the current file mapping.
        void* mappingPtr;

        /// The open mode that the file was created with. This is set the 'openMode' from OpenFile() verbatim. We need to keep track of this
        /// so we can correctly use CreateFileMapping().
        unsigned int openMode;

        /// The absolute path of the file. We need this so we can fill a FileInfo structure correctly.
        GTLib::String absolutePath;


    private:    // No copying.
        _FileHandle(const _FileHandle &);
        _FileHandle & operator=(const _FileHandle &);
    };



    FileHandle OpenFile(const char* filePath, unsigned int openMode)
    {
        return OpenFile(filePath, openMode, nullptr);
    }
    
    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo &fileInfoOut)
    {
        return OpenFile(filePath, openMode, &fileInfoOut);
    }

    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo* fileInfoOut)
    {
        GTLib::String filePathToLoad;
        if ((openMode & IO::OpenMode::NoSearchDirs) == 0 && (openMode & IO::OpenMode::Read) != 0 && (openMode & IO::OpenMode::Write) == 0 && !GTLib::Path::IsAbsolute(filePath))
        {
            if (!IO::FindAbsolutePath(filePath, filePathToLoad))
            {
                return 0;
            }
        }
        else
        {
            filePathToLoad = filePath;
        }


        DWORD dwDesiredAccess       = 0;
        DWORD dwCreationDisposition = OPEN_EXISTING;

        if ((openMode & IO::OpenMode::Write) != 0)
        {
            dwDesiredAccess |= FILE_GENERIC_WRITE;

            if ((openMode & IO::OpenMode::Append) == 0)
            {
                dwDesiredAccess &= ~FILE_APPEND_DATA;
            }


            dwCreationDisposition = OPEN_ALWAYS;
        }
        if ((openMode & IO::OpenMode::Read) != 0)
        {
            dwDesiredAccess |= FILE_GENERIC_READ;
        }


        HANDLE win32File = CreateFileA(filePathToLoad.c_str(), dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (win32File == INVALID_HANDLE_VALUE)
        {
            if ((openMode & IO::OpenMode::Write) != 0 && (openMode & IO::OpenMode::CreateDirs) != 0)
            {
                // We failed to create the file, which is most likely because the directory hierarchy doesn't exist yet. We'll create the directory
                // and try again.
                GTLib::String directory;
                GTLib::String devnull;
                IO::SplitPath(filePathToLoad.c_str(), directory, devnull);
                IO::MakeDirectory(directory.c_str());
                
                win32File = CreateFileA(filePathToLoad.c_str(), dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
            }
        }

        if (win32File != INVALID_HANDLE_VALUE)
        {
            auto file = new _FileHandle;
            file->win32File          = win32File;
            file->win32MappingHandle = nullptr;
            file->openMode           = openMode;
            file->absolutePath       = IO::ToAbsolutePath(filePathToLoad.c_str());

            if (fileInfoOut != nullptr)
            {
                LARGE_INTEGER fileSize;
                GetFileSizeEx(win32File, &fileSize);

                FILETIME lastModifiedTime;
                GetFileTime(win32File, nullptr, nullptr, &lastModifiedTime);

                ULARGE_INTEGER lastModifiedTime64;
                lastModifiedTime64.LowPart  = lastModifiedTime.dwLowDateTime;
                lastModifiedTime64.HighPart = lastModifiedTime.dwHighDateTime;
                

                fileInfoOut->path             = filePath;
                fileInfoOut->absolutePath     = file->absolutePath;
                fileInfoOut->size             = fileSize.QuadPart;
                fileInfoOut->lastModifiedTime = lastModifiedTime64.QuadPart;
                fileInfoOut->isDirectory      = false;
                fileInfoOut->exists           = true;
            }

            return reinterpret_cast<size_t>(file);
        }

        return 0;
    }

    void CloseFile(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            UnmapFile(fileIn);
            
            CloseHandle(file->win32File);
            delete file;
        }
    }


    int64_t GetFileSize(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            LARGE_INTEGER fileSize;
            GetFileSizeEx(file->win32File, &fileSize);

            return fileSize.QuadPart;
        }
        
        return 0;
    }


    void* MapFile(FileHandle fileIn, size_t length, int64_t offset)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            if (file->win32MappingHandle != nullptr)
            {
                return file->mappingPtr;
            }

            if ((file->openMode & IO::OpenMode::Read) != 0)
            {
                DWORD flProtect       = PAGE_READONLY;
                DWORD dwDesiredAccess = FILE_MAP_READ;

                if ((file->openMode & IO::OpenMode::Write) != 0)
                {
                    flProtect       = PAGE_READWRITE;
                    dwDesiredAccess = FILE_MAP_READ | FILE_MAP_WRITE;
                }


                LARGE_INTEGER dwMaximumSize;
                dwMaximumSize.QuadPart = offset + length;

                LARGE_INTEGER dwFileOffset;
                dwFileOffset.QuadPart = offset;


                file->win32MappingHandle = CreateFileMappingA(file->win32File, nullptr, flProtect, dwMaximumSize.HighPart, dwMaximumSize.LowPart, nullptr);
                if (file->win32MappingHandle != nullptr)
                {
                    file->mappingPtr = MapViewOfFile(file->win32MappingHandle, dwDesiredAccess, dwFileOffset.HighPart, dwFileOffset.LowPart, length);

                    if (file->mappingPtr == nullptr)
                    {
                        CloseHandle(file->win32MappingHandle);
                        file->win32MappingHandle = nullptr;
                    }

                    return file->mappingPtr;
                }
            }
        }

        return nullptr;
    }

    void UnmapFile(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            if (file->win32MappingHandle != nullptr)
            {
                UnmapViewOfFile(file->mappingPtr);
                file->mappingPtr = nullptr;

                CloseHandle(file->win32MappingHandle);
                file->win32MappingHandle = nullptr;
            }
        }
    }

    int64_t GetFileMappingOffsetAlignment()
    {
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);

        return systemInfo.dwAllocationGranularity;
    }


    intptr_t ReadFile(FileHandle fileIn, void* bufferOut, size_t bytesToRead)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            DWORD bytesRead;
            if (::ReadFile(file->win32File, bufferOut, static_cast<DWORD>(bytesToRead), &bytesRead, nullptr))
            {
                return bytesRead;
            }
        }

        return -1;
    }

    intptr_t WriteFile(FileHandle fileIn, const void* buffer, size_t bytesToWrite)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            DWORD bytesWritten;
            if (::WriteFile(file->win32File, buffer, static_cast<DWORD>(bytesToWrite), &bytesWritten, nullptr))
            {
                return bytesWritten;
            }
        }

        return -1;
    }

    int64_t SeekFile(FileHandle fileIn, int64_t offset, SeekOrigin origin)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            LARGE_INTEGER lNewFilePointer;
            LARGE_INTEGER lDistanceToMove;
            lDistanceToMove.QuadPart = offset;


            DWORD dwMoveMethod = FILE_CURRENT;
            if (origin == SeekOrigin::Start)
            {
                dwMoveMethod = FILE_BEGIN;
            }
            else if (origin == SeekOrigin::End)
            {
                dwMoveMethod = FILE_END;
            }


            if (SetFilePointerEx(file->win32File, lDistanceToMove, &lNewFilePointer, dwMoveMethod))
            {
                return lNewFilePointer.QuadPart;
            }
        }

        return -1;
    }

    int64_t GetFilePointer(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            LARGE_INTEGER lNewFilePointer;
            LARGE_INTEGER lDistanceToMove;
            lDistanceToMove.QuadPart = 0;

            if (SetFilePointerEx(file->win32File, lDistanceToMove, &lNewFilePointer, FILE_CURRENT))
            {
                return lNewFilePointer.QuadPart;
            }
        }

        return 0;
    }
#endif

#if defined(GT_PLATFORM_LINUX)
    struct _FileHandle
    {
        _FileHandle()
            : fd(-1), openMode(0), sizeInBytes(0), mmapPtr(nullptr), absolutePath()
        {
        }
        
        
        /// The POSIX file descriptor.
        int fd;
        
        /// The open mode that the file was created with. This is set the 'openMode' from OpenFile() verbatim. We need to keep track of this
        /// so we can correctly use mmap().
        unsigned int openMode;
        
        /// The size of the file.
        int64_t sizeInBytes;
        
        /// The address of the current memory mapping. If the file has not currently mapped, this is set to false.
        void* mmapPtr;
        
        /// The absolute path of the file. We need this so we can fill a FileInfo structure correctly.
        GTLib::String absolutePath;
        
        
    private:    // No copying.
        _FileHandle(const _FileHandle &);
        _FileHandle & operator=(const _FileHandle &);
    };
    
    
    FileHandle OpenFile(const char* filePath, unsigned int openMode)
    {
        return OpenFile(filePath, openMode, nullptr);
    }
    
    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo &fileInfoOut)
    {
        return OpenFile(filePath, openMode, &fileInfoOut);
    }
    
    FileHandle OpenFile(const char* filePath, unsigned int openMode, FileInfo* fileInfoOut)
    {
        GTLib::String filePathToLoad;
        if ((openMode & IO::OpenMode::NoSearchDirs) == 0 && (openMode & IO::OpenMode::Read) != 0 && (openMode & IO::OpenMode::Write) == 0 && !GTLib::Path::IsAbsolute(filePath))
        {
            if (!IO::FindAbsolutePath(filePath, filePathToLoad))
            {
                return 0;
            }
        }
        else
        {
            filePathToLoad = filePath;
        }
        
        
        
        int mode = O_RDONLY;
        if ((openMode & IO::OpenMode::Write) != 0)
        {
            if ((openMode & IO::OpenMode::Read) != 0)
            {
                mode = O_RDWR;
            }
            else
            {
                mode = O_WRONLY;
            }
            
            mode |= O_CREAT;
            mode |= O_TRUNC;
        }
        
        if ((openMode & IO::OpenMode::Append) != 0)
        {
            mode |= O_APPEND;
        }
        
        
        
        int fd = open(filePathToLoad.c_str(), mode);
        if (fd == -1)
        {
            if (errno == ENOENT && (openMode & IO::OpenMode::Write) != 0 && (openMode & IO::OpenMode::CreateDirs) != 0)
            {
                // We failed to create the file, which is most likely because the directory hierarchy doesn't exist yet. We'll create the directory
                // and try again.
                GTLib::String directory;
                GTLib::String devnull;
                IO::SplitPath(filePathToLoad.c_str(), directory, devnull);
                IO::MakeDirectory(directory.c_str());
                
                fd = open(filePathToLoad.c_str(), mode);
            }
        }
        
        if (fd != -1)
        {
            auto file = new _FileHandle;
            file->fd           = fd;
            file->openMode     = openMode;
            file->sizeInBytes  = 0;
            file->mmapPtr      = nullptr;
            file->absolutePath = IO::ToAbsolutePath(filePathToLoad.c_str());
            
            struct stat64 st;
            if (fstat64(fd, &st) != -1)
            {
                file->sizeInBytes = st.st_size;
                
                if (fileInfoOut != nullptr)
                {
                    fileInfoOut->path             = filePath;
                    fileInfoOut->absolutePath     = file->absolutePath;
                    fileInfoOut->size             = st.st_size;
                    fileInfoOut->lastModifiedTime = st.st_mtime;
                    fileInfoOut->isDirectory      = (st.st_mode & S_IFDIR) != 0;
                    fileInfoOut->exists           = true;
                }
            }
            else
            {
                assert(false);      // <-- We should never get here if the file failed to open, so fstat() should always work, I think...
            }
            
            return reinterpret_cast<size_t>(file);
        }
        
        
        return 0;
    }
    
    void CloseFile(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            UnmapFile(fileIn);
            
            close(file->fd);
            delete file;
        }
    }
    
    
    int64_t GetFileSize(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            // If the file was opened in write mode, we can't depend on file->sizeInBytes. Instead we need to use fstat(). If the file
            // is in read-only mode, we can just use file->sizeInBytes, which should be a tiny bit faster.
            if ((file->openMode & IO::OpenMode::Write) != 0)
            {
                struct stat64 st;
                if (fstat64(file->fd, &st) != -1)
                {
                    return st.st_size;
                }
                
                return 0;
            }
            
            if ((file->openMode & IO::OpenMode::Read) != 0)
            {
                return file->sizeInBytes;
            }
        }
        
        return 0;
    }
    

    void* MapFile(FileHandle fileIn, size_t length, int64_t offset)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            // For compatibility with windows, don't allow mapping if the file doesn't support reading.
            if ((file->openMode & IO::OpenMode::Read) == 0)
            {
                return nullptr;
            }


            if (file->mmapPtr != nullptr)
            {
                return file->mmapPtr;
            }
            
            
            int prot = PROT_NONE;
            if ((file->openMode & IO::OpenMode::Read) != 0)
            {
                prot |= PROT_READ;
            }
            if ((file->openMode & IO::OpenMode::Write) != 0)
            {
                prot |= PROT_WRITE;
            }
            
            int flags = MAP_PRIVATE;
            
            file->mmapPtr = mmap(NULL, length, prot, flags, file->fd, offset);
            if (file->mmapPtr == MAP_FAILED)
            {
                file->mmapPtr = nullptr;
            }
            
            
            // If the file is write-only, we want to set the size of the file to the mapped buffer size.
            if (file->mmapPtr != nullptr && (file->openMode & IO::OpenMode::Read) == 0)
            {
                file->sizeInBytes = length;
            }
            
            
            return file->mmapPtr;
        }
        
        return nullptr;
    }
    
    void UnmapFile(FileHandle fileIn)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            if (file->mmapPtr != nullptr)
            {
                munmap(file->mmapPtr, file->sizeInBytes);
            }
        }
    }
    
    int64_t GetFileMappingOffsetAlignment()
    {
        return sysconf(_SC_PAGE_SIZE);
    }
    
    
    intptr_t ReadFile(FileHandle fileIn, void* bufferOut, size_t bytesToRead)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            return read(file->fd, bufferOut, bytesToRead);
        }
        
        return -1;
    }
    
    intptr_t WriteFile(FileHandle fileIn, void* bufferOut, size_t bytesToWrite)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            return write(file->fd, bufferOut, bytesToWrite);
        }
        
        return -1;
    }
    
    int64_t SeekFile(FileHandle fileIn, int64_t offset, SeekOrigin origin)
    {
        auto file = reinterpret_cast<_FileHandle*>(fileIn);
        if (file != nullptr)
        {
            return lseek64(file->fd, offset, static_cast<int>(origin));
        }
        
        return -1;
    }
#endif
    
    
    
    namespace IO
    {
        /**
        *   \brief  Helper function for converting OpenMode flags to the stdio string-style open mode.
        *   \return nullptr if an error occurs.
        */
        const char* GetOpenModeString(unsigned int openMode)
        {
            // We must have OpenMode_Read or OpenMode_Write set.
            if (openMode & OpenMode::Read || openMode & OpenMode::Write)
            {
                if (openMode & OpenMode::Text)           // Text mode.
                {
                    if (openMode & OpenMode::Read)
                    {
                        if (openMode & OpenMode::Write)
                        {
                            if (openMode & OpenMode::Append)
                            {
                                return "a+";                // Text/Read/Write/Append
                            }
                            else
                            {
                                return "w+";                // Text/Read/Write/Clear
                            }
                        }
                        else
                        {
                            return "r";                     // Text/Read Only
                        }
                    }
                    else
                    {
                        if (openMode & OpenMode::Append)
                        {
                            return "a";                     // Text/Write Only/Append
                        }
                        else
                        {
                            return "w";                     // Text/Write Only/Clear
                        }
                    }
                }
                else                                    // Binary mode.
                {
                    if (openMode & OpenMode::Read)
                    {
                        if (openMode & OpenMode::Write)
                        {
                            if (openMode & OpenMode::Append)
                            {
                                return "a+b";                // Binary/Read/Write/Append
                            }
                            else
                            {
                                return "w+b";                // Binary/Read/Write/Clear
                            }
                        }
                        else
                        {
                            return "rb";                     // Binary/Read Only
                        }
                    }
                    else
                    {
                        if (openMode & OpenMode::Append)
                        {
                            return "ab";                     // Binary/Write Only/Append
                        }
                        else
                        {
                            return "wb";                     // Binary/Write Only/Clear
                        }
                    }
                }
            }

            return nullptr;
        }

        FILE* Open(const char* fileNameIn, unsigned int openMode, bool useSearchDirectories)
        {
            GTLib::String fileName;

            if (!GTLib::Path::IsAbsolute(fileNameIn) && useSearchDirectories && (openMode & OpenMode::Read) != 0 && (openMode & OpenMode::Write) == 0)
            {
                if (!FindAbsolutePath(fileNameIn, fileName))
                {
                    return nullptr;
                }
            }
            else
            {
                fileName = fileNameIn;
            }


            // At this point we will have a file name. We can now attempt to open it.
            FILE* result = nullptr;

            auto strOpenMode = GetOpenModeString(openMode);
            if (strOpenMode)
            {
                // If we want to create directories (only valid with Write mode), we need to open the file slightly differently. We want to first move into
                // the directory defined by fileNameIn and then create the file using only the file name.
                if ((openMode & OpenMode::Write) != 0 && (openMode & OpenMode::CreateDirs) != 0)
                {
                    GTLib::Path path(fileName.c_str());
                    path.RemoveLast();

                    IO::PushCurrentDirectory();
                    IO::SetCurrentDirectory(path.c_str(), true);

                    fileName = IO::FileName(fileNameIn);
                }


                // If we're on Windows we will convert any forward slashes to back slashes. For some stupid reason,
                // if a path is absolute it won't open if the slashes are forward...
            #if defined(GT_PLATFORM_WINDOWS)
                // TODO: Implement and use String::Replace() on fileName directory for this instead.

                auto newFileName = GTLib::Strings::Create(fileName.c_str());
                GTLib::Path::FormatSlashes(newFileName, '\\');

                #if defined(GT_COMPILER_VC)
                ::fopen_s(&result, newFileName, strOpenMode);
                #else
                result = ::fopen64(newFileName, strOpenMode);
                #endif

                GTLib::Strings::Delete(newFileName);
            #else
                result = ::fopen64(fileName.c_str(), strOpenMode);
            #endif


                // We need to restore the previous directory if we changed it.
                if ((openMode & OpenMode::Write) != 0 && (openMode & OpenMode::CreateDirs) != 0)
                {
                    IO::PopCurrentDirectory();
                }
            }

            return result;
        }

        void Close(FILE *file)
        {
            if (file != nullptr)
            {
                fclose(file);
            }
        }

        size_t Read(FILE *file, void *dest, size_t numBytes)
        {
            return fread(dest, 1, numBytes, file);
        }

        size_t Write(FILE *file, const void *source, size_t numBytes)
        {
            return fwrite(source, 1, numBytes, file);
        }

        bool WriteString(FILE *file, const char *source)
        {
            return fputs(source, file) >= 0;
        }

        int64_t Tell(FILE *file)
        {
        #if defined(GT_COMPILER_VC)
            return _ftelli64(file);
        #else
            return ftello64(file);
        #endif
        }

        bool AtEnd(FILE *file)
        {
            return feof(file) != 0;
        }

        bool Flush(FILE *file)
        {
            return fflush(file) != 0;
        }

        int64_t Size(FILE *file)
        {
            // We'll be moving back to this position.
            int64_t startPos = Tell(file);

            Seek(file, 0, SeekOrigin::End);
            int64_t size = Tell(file);

            // Now we need to move back to the original position.
            Seek(file, startPos, SeekOrigin::Start);

            return size;
        }

        bool FileExists(const char *filename)
        {
			// We'll use the
		#if defined(GT_COMPILER_VC)
			return _access(filename, 0) == 0;
		#else
			return access(filename, F_OK) == 0;
		#endif
        }


        bool CopyFile(const char *source, const char *dest)
        {
            // We need to make the directory first.
            MakeDirectory(dest, FileName(dest) - dest);		// <-- The result from FileName() will never be smaller than dest. No worries about negatives.

		#if defined(GT_PLATFORM_WINDOWS)
			return ::CopyFileA(source, dest, false) != 0;
		#else

			// NOTE: This technique is about 10-20x slower than the Win32-specific version. Really need to look into this.

			// Now we can try opening the files. If any fail to open, we will return false.
            FILE *sourceFile = Open(source, OpenMode::Read);
            FILE *destFile   = Open(dest, OpenMode::Write);

            if (sourceFile != nullptr && destFile != nullptr)
            {
				const size_t chunkSize = 4096 * 4096;
				char *tempData = (char *)malloc(chunkSize);

				// We just keep reading chunks until the end of the file is reached.
				while (!AtEnd(sourceFile))
				{
					size_t bytesRead = Read(sourceFile, tempData, chunkSize);
					Write(destFile, tempData, bytesRead);
				}

                free(tempData);

                Close(sourceFile);
                Close(destFile);

                return true;
            }
            else
            {
                Close(sourceFile);
                Close(destFile);

                return false;
            }
		#endif
        }



		bool DeleteFile(const char *filename)
		{
			return remove(filename) == 0;
		}
    }
}

// File info stuff.
namespace GTLib
{
    FileInfo::FileInfo()
        : path(), absolutePath(), size(0), lastModifiedTime(0), isDirectory(false), exists(false)
    {
    }

    FileInfo::FileInfo(const FileInfo &other)
        : path(other.path), absolutePath(other.absolutePath),
          size(other.size), lastModifiedTime(other.lastModifiedTime), isDirectory(other.isDirectory),
          exists(other.exists)
    {
    }

    FileInfo::FileInfo(const char* path)
        : path(), absolutePath(), size(0), lastModifiedTime(0), isDirectory(false), exists(false)
    {
        GTLib::IO::GetFileInfo(path, *this);
    }

    FileInfo::~FileInfo()
    {
    }

    FileInfo & FileInfo::operator=(const FileInfo &other)
    {
        this->path             = other.path;
        this->absolutePath     = other.absolutePath;
        this->size             = other.size;
        this->lastModifiedTime = other.lastModifiedTime;
        this->isDirectory      = other.isDirectory;
        this->exists           = other.exists;

        return *this;
    }

    bool FileInfo::SetPath(const char* pathIn)
    {
        this->path = pathIn;

        // FindAbsolutePath() will attempt to look for files that actually exist and use the search directories for determining
        // the absolute path.
        if (!GTLib::IO::FindAbsolutePath(pathIn, this->absolutePath))
        {
            // If we get here, it means the file doesn't actually exist. This is OK, in which case we just use the current directory
            // to determine the absolute path.
            this->absolutePath = GTLib::IO::ToAbsolutePath(pathIn, GTLib::IO::GetCurrentDirectory());
            this->exists       = false;
        }
        else
        {
            this->exists = true;
        }

        return this->exists;
    }


    namespace IO
    {
        bool GetFileInfo(const char* filename, FileInfo &fi)
        {
            if (GTLib::IO::FindAbsolutePath(filename, fi.absolutePath))
            {
    #if defined(GT_PLATFORM_WINDOWS)
                struct _stat64 result;
                if (_stat64(fi.absolutePath.c_str(), &result) == 0)
    #else
                struct stat64 result;
                if (stat64(fi.absolutePath.c_str(), &result) == 0)
    #endif
                {
                    fi.path             = filename;
                    fi.size             = result.st_size;
                    fi.lastModifiedTime = result.st_mtime;
                    fi.isDirectory      = (result.st_mode & S_IFDIR) != 0;
                    fi.exists           = true;
                }
                else
                {
                    fi.exists = false;
                }
            }
            else
            {
                fi.path             = filename;
                fi.absolutePath     = GTLib::IO::ToAbsolutePath(filename, GTLib::IO::GetCurrentDirectory());
                fi.size             = 0;
                fi.lastModifiedTime = 0;
                fi.isDirectory      = false;
                fi.exists           = false;
            }

            return fi.exists;
        }

        bool GetFileInfoFromAbsolutePath(const char* absolutePath, FileInfo &fi)
        {
            assert(IO::IsPathAbsolute(absolutePath));

        #if defined(GT_PLATFORM_WINDOWS)
            struct _stat64 result;
            if (_stat64(absolutePath, &result) == 0)
        #else
            struct stat64 result;
            if (stat64(absolutePath, &result) == 0)
        #endif
            {
                fi.path             = absolutePath;
                fi.absolutePath     = absolutePath;
                fi.size             = result.st_size;
                fi.lastModifiedTime = result.st_mtime;
                fi.isDirectory      = (result.st_mode & S_IFDIR) != 0;
                fi.exists           = true;
            }
            else
            {
                fi.exists = false;
            }

            return fi.exists;
        }



        /////////////////////////////////////////////////
        // IO Helpers.

        bool OpenAndReadTextFile(const char* fileName, GTLib::String &content)
        {
            auto file = IO::Open(fileName, OpenMode::Read);
            if (file != nullptr)
            {
                size_t sizeInBytes = static_cast<size_t>(IO::Size(file));

                auto tempContent = static_cast<char*>(malloc(sizeInBytes));
                IO::Read(file, tempContent, sizeInBytes);

                content.Assign(tempContent, sizeInBytes);

                free(tempContent);
                IO::Close(file);
                return true;
            }

            return false;
        }

        bool OpenAndWriteTextFile(const char* fileName, const char* content, ptrdiff_t contentSizeInBytes)
        {
            auto file = IO::Open(fileName, OpenMode::Write);
            if (file != nullptr)
            {
                if (contentSizeInBytes == -1)
                {
                    contentSizeInBytes = GTLib::Strings::SizeInBytes(content);
                }

                IO::Write(file, content, static_cast<size_t>(contentSizeInBytes));
                IO::Close(file);

                return true;
            }

            return false;
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
