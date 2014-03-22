// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ThreadCache.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/System.hpp>


// Globals.
namespace GTEngine
{
    /// The map of instantiated threads. The value here is a boolean specifying whether or not the thread is acquired.
    static GTLib::Map<GTLib::Thread*, bool>* Threads;

    /// The map of threads that were acquired forcefully. We keep this separate from the main list so that it's easier to distinguish between the two.
    static GTLib::Map<GTLib::Thread*, bool>* ForcedThreads;

    /// The lock we'll use to guard AcquireThread() and UnacquireThread().
    static GTLib::Mutex AcquireLock;
}

/// Startup/Shutdown.
namespace GTEngine
{
    bool ThreadCache::Startup(size_t minThreadCount)
    {
        Threads       = new GTLib::Map<GTLib::Thread*, bool>;
        ForcedThreads = new GTLib::Map<GTLib::Thread*, bool>;

        unsigned int threadCount = GTLib::Max(minThreadCount, static_cast<size_t>(GTLib::System::GetCPUCount() - 1));
        for (unsigned int i = 0; i < threadCount; ++i)
        {
            Threads->Add(new GTLib::Thread, false);
        }

        return true;
    }

    void ThreadCache::Shutdown()
    {
        if (Threads != nullptr)
        {
            for (size_t i = 0; i < Threads->count; ++i)
            {
                delete Threads->buffer[i]->key;
            }

            for (size_t i = 0; i < ForcedThreads->count; ++i)
            {
                delete ForcedThreads->buffer[i]->key;
            }

            delete Threads;
            delete ForcedThreads;
        }
    }
}

/// Acquire/Unacquire
namespace GTEngine
{
    GTLib::Thread* ThreadCache::AcquireThread(bool force)
    {
        assert(Threads       != nullptr);
        assert(ForcedThreads != nullptr);

        GTLib::Thread* thread = nullptr;

        AcquireLock.Lock();
        {
            // NOTE: I don't like this linear loop. Might do an optimized map later on, depending on how bad this is in practice.
            for (size_t i = 0; i < Threads->count; ++i)
            {
                bool isAcquired = Threads->buffer[i]->value;
                if (!isAcquired)
                {
                    Threads->buffer[i]->value = true;   // <-- mark the thread as acquired.

                    thread = Threads->buffer[i]->key;
                    break;
                }
            }

            // If we still don't have a thread at this point we need check if <force> is set to true. If so, we will force create the thread.
            if (thread == nullptr && force)
            {
                thread = new GTLib::Thread;
                ForcedThreads->Add(thread, true);
            }
        }
        AcquireLock.Unlock();

        return thread;
    }

    void ThreadCache::UnacquireThread(GTLib::Thread* thread)
    {
        AcquireLock.Lock();
        {
            auto iThread = Threads->Find(thread);
            if (iThread != nullptr)
            {
                iThread->value = false; // <-- mark the thread as unacquired.
            }
            else
            {
                iThread = ForcedThreads->Find(thread);
                if (iThread != nullptr)
                {
                    delete iThread->key;
                    ForcedThreads->Remove(thread);
                }
            }
        }
        AcquireLock.Unlock();
    }
}
