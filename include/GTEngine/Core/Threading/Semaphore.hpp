// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Threading_Semaphore
#define GT_Threading_Semaphore

#include <cstdint>

namespace GT
{
    /// Class representing a semaphore.
    ///
    /// When a semaphores count is larger than 0, it will allow a thread to pass it's gate. Otherwise, that thread
    /// will need to wait until a slot becomes available.
    class Semaphore
    {
    public:
        
        /// Constructor.
        Semaphore(int value);
            
        /// Destructor.
        ///
        /// @remarks
        ///     Remember that once the semaphore is destructed, Wait() and Release() will be undefined. Ensure any threads
        ///     using the semaphore do not try to use it after it has been destructed.
        ~Semaphore();
            
        /// Waits for the semaphore to become signaled and decrements the counter of the semaphore by one.
        void Wait();
            
        /// Increments the counter by one, allowing another thread to pass the semaphore gate.
        void Release();

            
            
    private:
        
        /// The data associated with the semaphore. Only used internally.
        void *data;
            
            
    private:    // No copying.
        Semaphore(const Semaphore &);
        Semaphore & operator=(const Semaphore &);
    };
}


#endif
