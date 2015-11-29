// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Threading_Mutex_hpp_
#define __GTLib_Threading_Mutex_hpp_

namespace GT
{
    /**
    *   \brief  Class representing a mutex.
    *
    *   A mutex is a simple sycnronization object prevents more than one thread from executing the same code at the same time.
    *
    *   These mutexes do not work across processes.
    */
    class Mutex
    {
    public:
        
        /**
        *   \brief  Constructor.
        *
        *   \remarks
        *       The mutex will be in a signaled/unlocked state by default.
        */
        Mutex();
            
        /**
        *   \brief  Destructor.
        *
        *   Remember that once the mutex is destructed, wait() and release() will be undefined. Ensure any threads
        *   using the mutex do not try to use it after it has been destructed.
        */
        ~Mutex();
            
        /**
        *   \brief  Waits for the mutex to become signaled. This function will automatically put the mutex back into a non-signaled state.
        */
        void Lock();
            
        /**
        *   \brief  Releases the mutex, putting it into a signaled state.
        */
        void Unlock();
        
        
    private:
        
        /// The data associated with the mutex. Only used internally.
        void *data;
            
            
    private:    // No copying.
        Mutex(const Mutex &);
        Mutex & operator=(const Mutex &);
    };
}


#endif
