// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Threading/Semaphore.hpp>
#include <GTLib/Config.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <limits.h>

namespace GTLib
{
    namespace Threading
    {
        Semaphore::Semaphore(int value)
            : data(nullptr)
        {
            this->data = CreateSemaphore(nullptr, value, LONG_MAX, nullptr);
        }

        Semaphore::~Semaphore()
        {
            CloseHandle(reinterpret_cast<HANDLE>(this->data));
        }

        void Semaphore::Wait(uint32_t timeoutInMilliseconds)
        {
            WaitForSingleObject(reinterpret_cast<HANDLE>(this->data), static_cast<DWORD>(timeoutInMilliseconds));
        }

        void Semaphore::Release()
        {
            ReleaseSemaphore(reinterpret_cast<HANDLE>(this->data), 1, nullptr);
        }
    }
}

#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <errno.h>
#include <cstdio>

// NOTE: OSX doesn't support unnamed semaphores. Will need to handle that later.

namespace GTLib
{
    namespace Threading
    {
        Semaphore::Semaphore(int value)
            : data(nullptr)
        {
            sem_t *internalSemaphore = new sem_t;
            if (sem_init(internalSemaphore, 0, (unsigned int)value) != -1)
            {
                this->data = internalSemaphore;
            }
        }

        Semaphore::~Semaphore()
        {
            sem_close((sem_t *)this->data);
        }

        void Semaphore::Wait(uint32_t timeoutInMilliseconds)
        {
            struct timespec ts;
            ts.tv_sec  = static_cast<time_t>(timeoutInMilliseconds / 1000);
            ts.tv_nsec = static_cast<long>((timeoutInMilliseconds - (ts.tv_sec * 1000)) * 1000);
            sem_timedwait((sem_t *)this->data, &ts);
        }

        void Semaphore::Release()
        {
            sem_post((sem_t *)this->data);
        }
    }
}

#endif
