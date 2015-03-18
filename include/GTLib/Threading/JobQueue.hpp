// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_JobQueue_hpp_
#define __GTLib_JobQueue_hpp_

#include "Job.hpp"

namespace GTLib
{
    namespace Threading
    {
        /**
        *   \brief  Class representing a job queue.
        *
        *   A job queue is how an operation can be performed across threads. A thread can populate a queue which
        *   in turn can be processed by another thread.
        *
        *   Job queues are completely non thread safe. This is for performance. Instead, safety can be achived
        *   by following a few rules.
        *
        *   Job queues can only be filled by a single therad, and can only be emptied by a single thread. The
        *   thread that fills the job queue can, and usually is, different to the thread that empties/processes
        *   the queue. There is a front job list and a back job list. The front command list is the list of jobs
        *   that is currently being processed. The back command list is the list that is currently being filled.
        *   You can think of the front and back lists as something similar to the front and back buffers of a
        *   renderer.
        *
        *   When a job is pushed onto the job queue, it's ownership is given to the job queue. This means the queue
        *   will manage the deletion of a job when it is executed/popped. When a job is popped, the queue will delete
        *   it with 'delete'. Therefore, do not attempt to delete the job object after it has been pushed onto the
        *   queue.
        *
        *   Any pending jobs are NOT executed upon destruction. Ensure all jobs have been executed before
        *   destructing. This is because it can not be guaranteed that all threads are still alive when the
        *   destructor is called.
        *
        *   Before jobs on the back list can be executed with Pop(), they must first be moved to the front list by
        *   calling PumpJobs(). The idea is to move, or pump, the jobs on the back list onto the front list. PumpJobs()
        *   is not thread-safe. It is up to the application to ensure that PumpJobs() is called synchronously.
        */
        class JobQueue
        {
        public:

            /// Constructor.
            JobQueue();
            
            /// Destructor.
            ~JobQueue();
            
            /**
            *   \brief  Pushes a pointer to a job to the back of the back job list.
            *
            *   \remarks
            *       This is called from the thread that does not execute the jobs.
            */
            void Push(GTLib::Threading::Job *job);
            
            /**
            *   \brief  Pops the next job off the front list and executes that job.
            *   \return A pointer to the new root job, or nullptr if there are no more jobs.
            *
            *   \remarks
            *       This is called from the owner thread - the thread that executes the jobs.
            */
            Job * Pop();
            
            /**
            *   \brief  Clears the back list and appends it to the front list.
            *
            *   \remarks
            *       Because this method crosses between the back and front lists, it is important that the application
            *       synchronizes calls to Push(), Pop() and PumpJobs().
            */
            void PumpJobs();


        private:
        
            /// The root object of the front list.
            Job *frontRoot;
            
            /// The last job in the front list.
            Job *frontEnd;
            
            /// The root object of the back list.
            Job *backRoot;
            
            /// The last job in the back list.
            Job *backEnd;
            
            
        private:    // No copying.
            JobQueue(const JobQueue &);
            JobQueue & operator=(const JobQueue &);
        };
    }
}


#endif
