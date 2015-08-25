// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/System.hpp>
#include <GTLib/Config.hpp>
#include <GTLib/Strings/Create.hpp>
#include <cstring>

/*
With system directories, we don't use std::string objects. Instead we allocate the directory once (when it is first retrieved) and
leave it in memory until it is cleared. This way we just refer to the memory directly and return a 'const char *' instead of constructing
a std::string.

The GetDirectory_*() functions need to be implemented for each different implementation. Each of these functions need to allocate their
string onto the heap, copy the contents of the path into that string, and then return a pointer to the memory. The memory must be allocated
with malloc(). It will be free with free().
*/

#if defined(GT_PLATFORM_WINDOWS)

#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-value"
    #pragma GCC diagnostic ignored "-Wunknown-pragmas"
    #pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
#include <GTLib/windows.hpp>
#include <shlobj.h>
#include <shellapi.h>
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

namespace GTLib
{
    namespace System
    {
        const char * GetDirectory_Home()
        {
            char path[MAX_PATH];
            SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path);

            // Any back-slashes must be converted to forward slashes.
            for (size_t i = 0; i < MAX_PATH && path[i] != 0; ++i)
            {
                if (path[i] == '\\')
                {
                    path[i] = '/';
                }
            }

            return GTLib::Strings::Create(path);
        }

        Vector<const char*>* CreateFontDirectories()
        {
            auto directories = new Vector<const char*>;

            char path[MAX_PATH];
            SHGetFolderPathA(nullptr, CSIDL_WINDOWS, nullptr, 0, path);

            // Any back-slashes must be converted to forward slashes. After this we need to append the "Fonts" directory.
            size_t i = 0;
            for (; i < MAX_PATH && path[i] != 0; ++i)
            {
                if (path[i] == '\\')
                {
                    path[i] = '/';
                }
            }

            // 'i' will be at the null terminator. We must have enough room to store the "/Fonts" string.
            if (i + 6 < MAX_PATH)
            {
                path[i++] = '/';
                path[i++] = 'F';
                path[i++] = 'o';
                path[i++] = 'n';
                path[i++] = 't';
                path[i++] = 's';
                path[i]   = '\0';

                directories->PushBack(GTLib::Strings::Create(path));
            }

            return directories;
        }

        void DeleteFontDirectories(Vector<const char*>* directories)
        {
            if (directories != nullptr)
            {
                for (size_t i = 0; i < directories->count; ++i)
                {
                    GTLib::Strings::Delete(const_cast<char*>((*directories)[i]));
                }

                delete directories;
            }
        }
    }
}

namespace GTLib
{
    namespace System
    {
        unsigned int GetCPUCount()
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);

            return si.dwNumberOfProcessors;
        }

        size_t GetPageSize()
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);

            return si.dwPageSize;
        }

        bool OpenFileOrURL(const char* url)
        {
            return ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL) > reinterpret_cast<HINSTANCE>(32);
        }
    }
}


#elif defined(GT_PLATFORM_LINUX)
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <GTLib/Strings/List.hpp>

namespace GTLib
{
    namespace System
    {
        const char * GetDirectory_Home()
        {
            return ::strdup(getenv("HOME"));
        }

        const char * GetDirectory_Fonts()
        {
            return nullptr;
        }

        Vector<const char*>* CreateFontDirectories()
        {
            auto directories = new Vector<const char*>;

            // We'll manually fill our list with common values.
            directories->PushBack(GTLib::Strings::Create("/usr/share/fonts"));
            directories->PushBack(GTLib::Strings::Create("/usr/local/share/fonts"));

            // There is also the "~/.fonts" directory...
            Strings::List<char> homeFontDir;
            homeFontDir.Append(System::Directories::Home());
            homeFontDir.Append("/.fonts");

            directories->PushBack(GTLib::Strings::Create(homeFontDir.c_str()));

            return directories;
        }

        void DeleteFontDirectories(Vector<const char*>* directories)
        {
            if (directories != nullptr)
            {
                for (size_t i = 0; i < directories->count; ++i)
                {
                    GTLib::Strings::Delete(const_cast<char*>((*directories)[i]));
                }

                delete directories;
            }
        }
    }
}

namespace GTLib
{
    namespace System
    {
        unsigned int GetCPUCount()
        {
            return sysconf(_SC_NPROCESSORS_ONLN);
        }

        size_t GetPageSize()
        {
            return (size_t)sysconf(_SC_PAGESIZE);
        }

        bool OpenFileOrURL(const char* url)
        {
            auto pid = fork();
            if (pid == 0)
            {
                execl("/usr/bin/xdg-open", "/usr/bin/xdg-open", url, nullptr);
                exit(0);
            }
            else if (pid == -1)
            {
                return false;
            }

            return true;
        }
    }
}

#endif

namespace GTLib
{
    namespace System
    {
        /**
        *   \brief  Structure containing the pointers to each system directory.
        *
        *   \remarks
        *       When this structure is destructed, it will deallocate the memory.
        */
        struct _DirectoryStrings
        {
            _DirectoryStrings()
                : home(nullptr), fonts(nullptr)
            {
            }

            ~_DirectoryStrings()
            {
                this->ClearMemory();
            }

            const char * Home()
            {
                if (!this->home)
                {
                    this->home = GetDirectory_Home();
                }

                return this->home;
            }

            Vector<const char*> & Fonts()
            {
                if (this->fonts == nullptr)
                {
                    this->fonts = CreateFontDirectories();
                }

                return *this->fonts;
            }

            /**
            *   \brief  Clears the internal memory being used to store the paths. This can used publicly if the caller
            *           doesn't want to store the memory over the life of the entire application.
            */
            void ClearMemory()
            {
                GTLib::Strings::Delete(const_cast<char*>(reinterpret_cast<const char *>(this->home)));
                this->home = nullptr;

                DeleteFontDirectories(fonts);
            }

        private:

            /// The home directory.
            const char *home;

            /// The font directories.
            Vector<const char*>* fonts;


        private:    // No copying.
            _DirectoryStrings(const _DirectoryStrings &);
            _DirectoryStrings & operator=(const _DirectoryStrings &);

        }DirectoryStrings;


        const char * Directories::Home()
        {
            return DirectoryStrings.Home();
        }

        Vector<const char*> & Directories::Fonts()
        {
            return DirectoryStrings.Fonts();
        }
    }
}
