// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Threading/Semaphore.hpp>
#include <GTLib/Errors.hpp>

#if defined(_WIN32)
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
            CloseHandle((HANDLE)this->data);
        }

        void Semaphore::Wait(uint32_t timeoutInMilliseconds)
        {
            WaitForSingleObject((HANDLE)this->data, static_cast<DWORD>(timeoutInMilliseconds));
        }

        void Semaphore::Release()
        {
            ReleaseSemaphore((HANDLE)this->data, 1, nullptr);
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
            else
            {
                GTLib::PostError("Semaphore::Semaphore() - sem_init(%d) returned -1. errno = %d.", value, errno);
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
            if (sem_timedwait((sem_t *)this->data, &ts) == -1)
            {
                GTLib::PostError("Semaphore::Semaphore() - sem_wait() returned -1. errno = %d.", errno);
            }
        }

        void Semaphore::Release()
        {
            if (sem_post((sem_t *)this->data) == -1)
            {
                GTLib::PostError("Semaphore::Semaphore() - sem_post() returned -1. errno = %d.", errno);
            }
        }
    }
}

#endif
