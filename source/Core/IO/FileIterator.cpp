// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/IO.hpp>
#include <GTLib/Config.hpp>
#include <GTLib/Errors.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Strings/Create.hpp>
#include <GTLib/Strings/Equal.hpp>

#if    defined(GT_PLATFORM_WINDOWS)
// --- Windows ---
#include <GTLib/windows.hpp>
#include <direct.h>
#else
// --- POSIX ---
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace GTLib
{
    namespace IO
    {
        FileIterator::FileIterator()
            : name(nullptr), absolutePath(nullptr), size(0), lastModified(0), isDirectory(false), fileInfoList(new GTLib::List<FileInfo>), count(1)
        {
			this->ctor(nullptr);
        }

		FileIterator::FileIterator(const char *fileQuery)
			: name(nullptr), absolutePath(nullptr), size(0), lastModified(0), isDirectory(false), fileInfoList(new GTLib::List<FileInfo>), count(1)
		{
			this->ctor(fileQuery);
		}

        FileIterator::FileIterator(const FileIterator &i)
			: name(i.name), absolutePath(nullptr), size(i.size), lastModified(i.lastModified), isDirectory(i.isDirectory), fileInfoList(i.fileInfoList), count(1)
        {
            ++this->count;
        }

        FileIterator::~FileIterator()
        {
            // If there are no more instantiations we need to kill everything.
            --this->count;
            if (this->count == 0)
            {
                this->fileInfoList->Clear();
            }

            delete this->fileInfoList;
        }

        FileIterator & FileIterator::Next()
        {
            // If there was a root item before, it needs to be removed.
            if (this->fileInfoList->root != nullptr)
            {
                this->fileInfoList->RemoveRoot();

                // Now if we have a root item we can set the appropriate values. Otherwise we're at the end.
                if (this->fileInfoList->root)
                {
                    auto &fi = this->fileInfoList->root->value;

                    this->name         = fi.path.c_str();
                    this->absolutePath = fi.absolutePath.c_str();
                    this->size         = fi.size;
                    this->lastModified = fi.lastModifiedTime;
                    this->isDirectory  = fi.isDirectory;
                }
                else
                {
                    this->name         = nullptr;
                    this->absolutePath = nullptr;
                    this->size         = 0;
                    this->lastModified = 0;
                    this->isDirectory  = false;
                }
            }

            return *this;
        }

        bool FileIterator::AtEnd() const
        {
            // We're at the end if we don't have any file info left in our list.
            return this->fileInfoList->root == nullptr;
        }

        FileIterator & FileIterator::operator= (const FileIterator &i)
        {
            if (this != &i)
            {
                this->name         = i.name;
                this->absolutePath = i.absolutePath;
                this->size         = i.size;
                this->lastModified = i.lastModified;
                this->isDirectory  = i.isDirectory;
                this->fileInfoList = i.fileInfoList;
                this->count        = i.count;
            }

            return *this;
        }
    }
}



namespace GTLib
{
    namespace IO
    {
#if defined(GT_PLATFORM_WINDOWS)
        // This function does not set the paths!
        void WIN32_FIND_DATAToFileInfo(const WIN32_FIND_DATAA &ffd, FileInfo &fi)
        {
            LARGE_INTEGER size;
            size.LowPart  = ffd.nFileSizeLow;
            size.HighPart = (LONG)ffd.nFileSizeHigh;
            fi.size = size.QuadPart;

            // We're assuming time_t is 64-bit.
            ULARGE_INTEGER ull;
            ull.LowPart  = ffd.ftLastWriteTime.dwLowDateTime;
            ull.HighPart = ffd.ftLastWriteTime.dwHighDateTime;
            fi.lastModifiedTime = static_cast<time_t>(ull.QuadPart / 10000000ULL - 11644473600ULL);

            fi.isDirectory = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        }
#endif

        void FileIterator::ctor(const char *directory)
		{
		    if (directory == nullptr)
		    {
		        directory = "";
		    }

            // === Start of platform-specific code ===
#if defined(GT_PLATFORM_WINDOWS)
            // --- Windows ---
            WIN32_FIND_DATAA ffd;
            const char* currentDirectory = IO::GetCurrentDirectory();

            // We need to put a '*' as the wildcard symbol on Windows. This will find every file in the directory. We then
            // use our own regex matcher for doing matching.
            size_t directoryLength = strlen(directory);
            if (directoryLength < 4096 - 3)
            {
                char directoryQuery[4096];
                memcpy(directoryQuery, directory, directoryLength);

                directoryQuery[directoryLength + 0] = '\\';
                directoryQuery[directoryLength + 1] = '*';
                directoryQuery[directoryLength + 2] = '\0';

                HANDLE hFind = FindFirstFileA(directoryQuery, &ffd);
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        // We need to ignore '.' and '..' directories. In the case of '..', it will actually mess up our base path.
                        if (!GTLib::Strings::Equal(ffd.cFileName, ".") && !GTLib::Strings::Equal(ffd.cFileName, ".."))
                        {
                            FileInfo newFI;
                            WIN32_FIND_DATAToFileInfo(ffd, newFI);

                            // WIN32_FIND_DATAToFileInfo will not set the path. We need to set it here. We don't want to use SetPath() here because it
                            // will unnecessarilly call IO::FindAbsolutePath(). We can get the absolute path from the current directory.
                            //newFI.path = basePath + ffd.cFileName;
                            newFI.path = directory;
                            newFI.path += "/";
                            newFI.path += ffd.cFileName;

                            if (IO::IsPathRelative(newFI.path.c_str()))
                            {
                                newFI.absolutePath = IO::ToAbsolutePath(newFI.path.c_str(), currentDirectory);
                            }
                            else
                            {
                                newFI.absolutePath = newFI.path;
                            }

                            this->fileInfoList->Append(newFI);
                        }

                    } while (FindNextFileA(hFind, &ffd) != 0);


                    // Now we need to close the handle.
                    FindClose(hFind);
                }
            }
#else
            // --- POSIX ---
            basePath += "/";        // <-- Appending a trailing slash now makes it so we don't have to do it for each file.
            
            DIR *dir = opendir(basePath.c_str());
            if (dir != nullptr)
            {
                // We're good, so now we can retrieve all of the matching files.
                struct dirent *tempFileInfo = nullptr;
                do
                {
                    tempFileInfo = readdir(dir);
                    if (tempFileInfo != nullptr)
                    {
                        if (!GTLib::Strings::Equal(tempFileInfo->d_name, ".") && !GTLib::Strings::Equal(tempFileInfo->d_name, ".."))
                        {
                            // The file name must match our input query before it can be added to the list. If it doesn't match,
                            // we just don't bother adding it.
                            if (regex.Match(tempFileInfo->d_name))
                            {
                                // We have a match. First we need to get the file information using stat(). We need the absolute path for this.
                                GTLib::String newFilePath = basePath + tempFileInfo->d_name;
                                
                                struct stat fi;
                                if (stat(newFilePath.c_str(), &fi) == 0)
                                {
                                    // We were able to get the info, so now we can add it to the list. This particular constructor we use here
                                    // will retrieve all of the relevant information using GetFileInfo().
                                    this->fileInfoList->Append(new FileInfo(newFilePath.c_str()));
                                }
                            }
                        }
                    }

                } while (tempFileInfo != nullptr);
            }
            closedir(dir);
#endif

            // We need to apply the details of the first item if we have one.
            if (this->fileInfoList->root != nullptr)
            {
                auto &fi = this->fileInfoList->root->value;

                this->name         = fi.path.c_str();
                this->absolutePath = fi.absolutePath.c_str();
                this->size         = fi.size;
                this->lastModified = fi.lastModifiedTime;
                this->isDirectory  = fi.isDirectory;
            }
		}
    }
}
