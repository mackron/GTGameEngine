// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/System.hpp>
#include <GTLib/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <shellapi.h>

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
