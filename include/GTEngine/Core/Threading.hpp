// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Threading
#define GT_Threading

#include "Threading/Thread.hpp"
#include "Threading/Mutex.hpp"
#include "Threading/Semaphore.hpp"
#include <cstdint>

namespace GT
{
    namespace Threading
    {
        /**
        *   \brief                    Puts the calling thread to sleep for a given number of milliseconds.
        *   \param  milliseconds [in] The number of milliseconds to put the calling thread to sleep.
        */
        void Sleep(unsigned int milliseconds);
        
        /**
        *   \brief  Terminates the calling thread.
        *
        *   \remarks
        *       It is not recommended to terminate a thread using this function. Instead the thread should run
        *       it's coarse and exit it's function naturally.
        *       \par
        *       The exit code is always 0.
        */
        void Exit();

        /**
        *   \brief  Retrieves the ID of the calling thread.
        */
        uint64_t GetCurrentThreadID();
    }
}

#endif
