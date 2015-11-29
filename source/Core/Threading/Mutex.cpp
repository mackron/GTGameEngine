// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Threading/Mutex.hpp>

#if defined(_WIN32)
#include <GTEngine/Core/windows.hpp>

namespace GTLib
{
    namespace Threading
    {
        Mutex::Mutex()
            : data(nullptr)
        {
            this->data = CreateEvent(nullptr, FALSE, TRUE, nullptr);
        }

        Mutex::~Mutex()
        {
            CloseHandle((HANDLE)this->data);
        }

        void Mutex::Lock()
        {
            WaitForSingleObject((HANDLE)this->data, INFINITE);
        }

        void Mutex::Unlock()
        {
            SetEvent((HANDLE)this->data);
        }
    }
}

#else
#include <pthread.h>

namespace GTLib
{
    namespace Threading
    {
        Mutex::Mutex()
            : data(nullptr)
        {
            pthread_mutex_t *internalMutex = new pthread_mutex_t;
            pthread_mutex_init(internalMutex, nullptr);

            this->data = internalMutex;
        }

        Mutex::~Mutex()
        {
            pthread_mutex_destroy((pthread_mutex_t *)this->data);
        }

        void Mutex::Lock()
        {
            pthread_mutex_lock((pthread_mutex_t *)this->data);
        }

        void Mutex::Unlock()
        {
            pthread_mutex_unlock((pthread_mutex_t *)this->data);
        }
    }
}

#endif
