// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_Job
#define GTLib_Job

namespace GTLib
{
    namespace Threading
    {
        /**
        *   \brief  Base abstract class for a job.
        *
        *   The idea behind a job is that it will contain a relatively small section of code that needs to be executed. What the
        *   job itself performs is up to the inheritted classes.
        *
        *   The job is executed via the Run() method which takes no parameters and returns void. Any data will need to be managed
        *   by the inheritted class.
        *
        *   A job contains a pointer to the next job that should be executed after it. This allows jobs to be chained together, forming
        *   a list. The next job is only ever managed by the JobQueue class.
        */
        class Job
        {
        public:

            /// Constructor.
            Job();

            /// Destructor.
            virtual ~Job();

            /**
            *   \brief  Runs the job.
            */
            virtual void Run() = 0;


        private:

            // The JobQueue is a friend. It uses 'next' directly.
            friend class JobQueue;

            /// A pointer to the job that should be executed after this one.
            Job *next;


        private:    // No copying.
            Job(const Job &);
            Job & operator=(const Job &);
        };
    }
}


#endif
