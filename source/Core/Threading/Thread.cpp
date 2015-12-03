// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/Threading/Thread.hpp>
#include <easy_util/easy_util.h>

namespace GT
{
    // Base structure for the thread data sent to the thread precedure.
    struct _ThreadData
    {
        _ThreadData()
            : entryProc(nullptr), entryData(nullptr), busy(false), alive(true), wantsToStop(false), entrySemaphore(0), endMutex()
        {
            entrySemaphore = easyutil_create_semaphore(0);
            endMutex = easyutil_create_mutex();
        }

        virtual ~_ThreadData()
        {
            easyutil_delete_semaphore(this->entrySemaphore);
            easyutil_delete_mutex(this->endMutex);
        }

        // The entry point to execute.
        ThreadEntryProc entryProc;

        // The data sent with the entry point.
        void *entryData;

        // Keeps track of whether or not the thead is busy.
        bool busy;

        // Keeps track of whether or not the thread is still alive.
        bool alive;

        // Keeps track of whether or not the thread wants to stop.
        bool wantsToStop;

        // The semaphore to wait on before executing the function. Used by the internal thread procedure.
        easyutil_semaphore entrySemaphore;

        // The mutex that controls when we can start another function.
        easyutil_mutex endMutex;

    private:    // No copying.
        _ThreadData(const _ThreadData &);
        _ThreadData & operator=(const _ThreadData &);
    };

    /**
    *   \brief  The main threading loop executed by all threads.
    */
    void _ThreadLoop(_ThreadData *data)
    {
        if (data)
        {
            // If we have a function waiting to be executed, we need to make sure it is called before terminating.
            while (data->entryProc != nullptr || data->alive)
            {
                // We need to wait for a semaphore before executing the function. We don't just want to keep looping. Once
                // we have passed the semaphore gate we can assume the start procedure is valid and data is valid.
                easyutil_wait_semaphore(data->entrySemaphore);

                // The thread is busy.
                data->busy = true;

                // We can now call the entry point function.
                if (data->entryProc)
                {
                    data->entryProc(data->entryData);
                }

                // The thread is no longer busy.
                data->entryProc = nullptr;
                data->busy      = false;

                // We are no longer running the function.
                easyutil_unlock_mutex(data->endMutex);
            }
        }
    }

    /**
    *   \brief  Thread procedure for executing a job. 'data' is a Job object.
    */
#if 0
    void JobThreadProc(void *data)
    {
        if (data)
        {
            ((ThreadJob *)data)->Run();
        }
    }
#endif


    Thread::Thread()
        : data(nullptr)
    {
        this->ctor();
    }

    Thread::Thread(ThreadEntryProc entryProc, void *entryData)
        : data(nullptr)
    {
        this->ctor();
        this->Start(entryProc, entryData);
    }

#if 0
    Thread::Thread(ThreadJob &job)
        : data(nullptr)
    {
        this->ctor();
        this->Start(job);
    }
#endif

    Thread::~Thread()
    {
        // We don't delete the data here - it is done in _ThreadProc. Instead we just mark it as dead.
        if (data)
        {
            // First, just let the thread know that we want it to stop executing.
            this->Stop();

            reinterpret_cast<_ThreadData *>(this->data)->alive = false;

            // There is a situation where we'll be stuck waiting in the entrySemaphore lock. In this case,
            // we'll never actually get passed it in order to delete the thread data. To solve, we just
            // do a non-blocking call to a null function which will get us passed that lock. We do a
            // non-blocking call because 1) we don't want the calling thread to stall and 2) if the thread
            // is already running a function, we won't actually encounter the semaphore lock problem.
            this->Start(nullptr, nullptr, false);
        }
    }

    bool Thread::Start(ThreadEntryProc entryProc, void *entryData, bool block)
    {
        // If we're not blocking and we're busy, we need to return false.
        if (!block)
        {
            if (this->Busy())
            {
                return false;
            }
        }

        // If we're not busy we can set the entry point and data and release the semaphore.
        auto selfData = reinterpret_cast<_ThreadData *>(this->data);

        // We always need to do a lock since we're now running the function. If we get here in non-blocking mode (block == false),
        // this lock will return immediately.
        easyutil_lock_mutex(selfData->endMutex);

        selfData->entryProc   = entryProc;
        selfData->entryData   = entryData;
        selfData->busy        = true;
        selfData->wantsToStop = false;

        easyutil_release_semaphore(selfData->entrySemaphore);

        return true;
    }

    //bool Thread::Start(ThreadJob &job, bool block)
    //{
    //    return this->Start(JobThreadProc, &job, block);
    //}

    void Thread::Stop()
    {
        reinterpret_cast<_ThreadData *>(this->data)->wantsToStop = true;
    }

    bool Thread::WantsToStop() const
    {
        return reinterpret_cast<_ThreadData *>(this->data)->wantsToStop;
    }

    void Thread::Wait()
    {
        // We only want to wait if we're busy.
        if (this->Busy())
        {
            // We wait, and then release straight away.
            easyutil_lock_mutex(reinterpret_cast<_ThreadData*>(this->data)->endMutex);
            easyutil_unlock_mutex(reinterpret_cast<_ThreadData*>(this->data)->endMutex);
        }
    }

    bool Thread::Busy()
    {
        return reinterpret_cast<_ThreadData*>(this->data)->busy;
    }
}


#if defined(_WIN32)
#include <GTGE/Core/windows.hpp>

namespace GT
{
    struct _ThreadDataWin32 : public _ThreadData
    {
        _ThreadDataWin32()
            : hThread(nullptr)
        {
        }

        // The Windows thread handle.
        HANDLE hThread;

    private:    // No copying.
        _ThreadDataWin32(const _ThreadDataWin32 &);
        _ThreadDataWin32 & operator=(const _ThreadDataWin32 &);
    };

    // The thread procedure.
    DWORD WINAPI _ThreadProc(_ThreadDataWin32 *input)
    {
        if (input)
        {
            // Now we need to do the main thread loop.
            _ThreadLoop(input);

            // If we have made it here the thread has died and we need to delete the input data.
            CloseHandle(input->hThread);
            input->hThread = nullptr;

            delete input;
        }

        return 0;
    }


    void Thread::SetPriority(ThreadPriority priority)
    {
        HANDLE hThread = ((_ThreadDataWin32 *)this->data)->hThread;

        switch (priority)
        {
            case ThreadPriority_Lowest:
            {
                SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
                break;
            }

            case ThreadPriority_Low:
            {
                SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
                break;
            }

            case ThreadPriority_High:
            {
                SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
                break;
            }

            case ThreadPriority_Highest:
            {
                SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
                break;
            }

            // Normal by default.
            case ThreadPriority_Normal:
            default:
            {
                SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
            }
        }
    }

    void Thread::ctor()
    {
        // We need to create the thread. The data structure is deleted by the thread procedure (_ThreadProc). Do
        // not delete it in the destructor.
        _ThreadDataWin32 *threadData = new _ThreadDataWin32;
        this->data = threadData;

        // Now we need a thread object.
        threadData->hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)_ThreadProc, threadData, 0, nullptr);
    }
}

#else
#include <pthread.h>

namespace GT
{
    struct _ThreadDataPOSIX : public _ThreadData
    {
        _ThreadDataPOSIX()
            : pthread(0)
        {
        }

        // The Windows thread handle.
        pthread_t pthread;
    };

    // The thread procedure.
    void * _ThreadProc(void *input)
    {
        if (input)
        {
            // Now we need to do the main thread loop.
            _ThreadLoop(reinterpret_cast<_ThreadDataPOSIX*>(input));

            // If we have made it here the thread has died and we need to delete the input data.
            delete reinterpret_cast<_ThreadDataPOSIX*>(input);
            pthread_exit(nullptr);
        }

        return nullptr;
    }

    void Thread::SetPriority(ThreadPriority)
    {
    }

    void Thread::ctor()
    {
        auto threadData = new _ThreadDataPOSIX;
        this->data = threadData;

        pthread_create(&threadData->pthread, nullptr, _ThreadProc, (void *)threadData);
    }
}

#endif
