// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ThreadCache_hpp_
#define __GTEngine_ThreadCache_hpp_

#include <GTEngine/Core/Threading.hpp>
#include <cstddef>

namespace GT
{
    /// Static class representing the engine's global thread pool
    ///
    /// Whenever a thread is needed it should be retrieved from the thread pool.
    class ThreadCache
    {
    /// Startup/Shutdown.
    public:

        /// Starts up the thread pool.
        ///
        /// The initial number of threads will be equal to the number of CPU threads, which will be 1 at a minimum.
        static bool Startup(size_t minThreadCount = 1);

        /// Shuts down the thread pool.
        static void Shutdown();



    // Retrievals.
    public:

        /// Retrieves the next available thread.
        ///
        /// @param force [in] Whether or not to force the creation of a new thread is none are available. Defaults to false.
        ///
        /// @remarks
        ///     If <force> is false, this will return null if no threads are available. When <force> is set to true, it's possible, and highly likely, that
        ///     a new Thread object will be instantiated. Be aware of this overhead.
        ///     @par
        ///     Unacquire the thread with UnacquireThread().
        static Thread* AcquireThread(bool force = false);

        /// Unacquires a thread that was acquired with AcquireThread().
        ///
        /// @param thread [in] A pointer to the thread to unacquire.
        static void UnacquireThread(Thread* thread);
    };
}

#endif
