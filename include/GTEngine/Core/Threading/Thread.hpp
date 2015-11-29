// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Threading_Thread
#define GT_Threading_Thread

#include "Job.hpp"

namespace GT
{
    /// Delegate for the entry point of a thread. Contains only a single argument - a void pointer. Returns nothing.
    typedef void (* ThreadEntryProc)(void *);
        
    /**
    *   \brief  Enumerator for the different thread priorities.
    */
    enum ThreadPriority
    {
        ThreadPriority_Lowest,
        ThreadPriority_Low,
        ThreadPriority_Normal,
        ThreadPriority_High,
        ThreadPriority_Highest,
            
        ThreadPriority_Default = ThreadPriority_Normal
    };

    /**
    *   \brief  Class representing a thread.
    *
    *   This class is not actually thread-safe. This is done due to performance - we don't want to sync on such
    *   a low-level class. You can use the thread object across multiple threads, but you must ensure you do the
    *   appropriate synchronizations. In general, you will only want, and need, to use the thread object in the
    *   thread that created it.
    */
    class Thread
    {
    public:
        
        /**
        *   \brief  Constructor.
        *
        *   A thread does not begin executing straight after construction. It does, however, create the internal
        *   thread which will be placed in a waiting state. To start the thread, call thread::start().
        */
        Thread();
            
        /**
        *   \brief  Constructor for automatically starting the execution of a thread.
        *
        *   \remarks
        *       This constructor is the same as the default constructor execpt that it executes the given procedure
        *       straight away rather than requiring a call to thread::start(). It's more for convenience than
        *       anything else.
        *       \par
        *       This constructor will also allow for syntax like the following.
        *       \code
        *       Thread(EntryProc, EntryData);
        *       \endcode
        *       Syntax like the above would simply create a thread, execute the given procedure and then kill the
        *       thread once it's finished.
        */
        Thread(ThreadEntryProc entryProc, void *entryData);
            
        /**
        *   \brief           Constructor for automatically starting the execution of a thread.
        *   \param  job [in] A reference to the job that should be executed.
        */
        Thread(Job &job);
            
        /**
        *   \brief  Destructor.
        *
        *   Destructing a thread object does not stop the execution of the procedure which is currently running in
        *   the thread. However, once the the procedure has finished executing, the internal thread will be stopped
        *   and any resources released.
        */
        ~Thread();
            
        /**
        *   \brief                 Begins executing the thread using the given function and input data.
        *   \param  entryProc [in] The entry point to the thread.
        *   \param  entryData [in] A pointer to some data to pass to the entry point. Can be null.
        *   \return                True if the thread was successfully started. If the thread is busy, false is returned.
        *
        *   \remarks
        *       A thread can not execute a procedure if it is already executing another one. Therefore, this function will
        *       block until the current procedure is finished, unless 'block' is set to false (it is true by default). You
        *       can use Wait() to wait until the thread becomes available. Do not do 'while(thread.Busy());' to block
        *       since it may starve other threads.
        *       \par
        *       This method should not be called on the same object across multiple threads. There is a possibility that
        *       the entry point procedure and data will be overwritten by the other thread, resulting in Start() returning
        *       true, but the function not actually executing. Therefore, the method, and class, is not thread-safe.
        */
        bool Start(ThreadEntryProc entryProc, void *entryData, bool block = true);

        /**
        *   \brief  Begins executing the thread using the job.
        */
        bool Start(Job &job, bool block = true);

        /// Marks the thread as wanting to stop executing.
        ///
        /// @remarks
        ///     This does not immediately cease execution of the thread, but rather sets a flag in the thread that can be
        ///     read by the executing function and used to determine whether or not that function should try terminating
        ///     early. This is useful for long-running background threads that run in loops such as video and audio streaming.
        ///     @par
        ///     This does not wait for execution to actually stop - it just sets a flag. To wait for execution to stop, immediately
        ///     follow this call with a call to Wait().
        void Stop();

        /// Determines whether or not the thread wants to stop executing.
        ///
        /// @remarks
        ///     This can be used by the thread's currently executing function to determine whether or not it should try terminating.
        bool WantsToStop() const;
            
        /**
        *   \brief  Waits for the thread to finish it's currently executing function.
        *
        *   \remarks
        *       Applications need to wait for the currently executing function to finish before they can start another one.
        */
        void Wait();
            
        /**
        *   \brief  Determines whether or not the thread is busy.
        *   \return True if the thread is currently executing a procedure; false otherwise.
        *
        *   \remarks
        *       A busy thread is simply a thread which is currently executing. If you need to wait for the current execution
        *       to finish, don't call this function in a loop. Instead use Wait().
        */
        bool Busy();
            
        /**
        *   \brief                Sets the priority of the thread.
        *   \param  priority [in] The new priority of the thread.
        */
        void SetPriority(ThreadPriority priority);
        
        
    private:
        
        /**
        *   \brief  Constructor implementation.
        */
        void ctor();
        
    private:

        /// The data associated with the thread. Only ever used internally.
        void *data;
            
            
    private:    // No copying.
        Thread(const Thread &);
        Thread & operator=(const Thread &);
    };
}

#endif
