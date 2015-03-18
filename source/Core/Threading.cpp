// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Threading.hpp>
#include <GTLib/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>

namespace GTLib
{
    namespace Threading
    {
        void Sleep(unsigned int milliseconds)
        {
            ::Sleep(milliseconds);
        }

        void Exit()
        {
            ::ExitThread(0);
        }

        uint64_t GetCurrentThreadID()
        {
            return (uint64_t)::GetCurrentThreadId();
        }
    }
}

#else
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>

namespace GTLib
{
    namespace Threading
    {
        void Sleep(unsigned int milliseconds)
        {
            usleep(milliseconds * 1000);    // <-- usleep is in microseconds.
        }

        void Exit()
        {
            pthread_exit(nullptr);
        }

        uint64_t GetCurrentThreadID()
        {
            return (uint64_t)syscall(SYS_gettid);
        }
    }
}

#endif
