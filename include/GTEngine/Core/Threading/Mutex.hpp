// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Threading_Mutex
#define GT_Threading_Mutex

namespace GT
{
    /// Class representing a mutex.
    ///
    /// These mutexes are not inter-process.
    class Mutex
    {
    public:
        
        /// Constructor.
        /// 
        /// @remarks
        ///     The mutex will be in a signaled/unlocked state by default.
        Mutex();
            
        /// Destructor.
        ///
        /// @remarks
        ///     Remember that once the mutex is destructed, Lock() and Unlock() will be undefined. Ensure any threads
        ///     using the mutex do not try to use it after it has been destructed.
        ~Mutex();
            
        /// Waits for the mutex to become signaled. This function will automatically put the mutex back into a non-signaled state.
        void Lock();
            
        /// Releases the mutex, putting it into a signaled state.
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
