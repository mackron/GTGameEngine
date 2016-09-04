// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Benchmarker
#define GT_Benchmarker

namespace GT
{
    /// Class for performing simple benchmarking.
    ///
    /// This class is similar to Timer except it accumulates a total which can then be averaged.
    ///
    /// To use this, you call Start() at the beginning of the code segment. Then at the end of the code segment, you call End(),
    /// which will accumulate the time between Start() and End(). Use GetTotalTime() and GetAverageTime() to retrieve the timing
    /// information.
    ///
    /// The benchmarker can be reset with Reset().
    class Benchmarker
    {
    public:

        /// Constructor.
        Benchmarker()
            : totalTime(0.0), counter(0),
              lastStartTime(0.0)
        {
        }


    public:

        /// Marks the beginning of the section being benchmarked.
        void Start()
        {
            this->lastStartTime = Timing::GetTimeInSeconds();
        }

        /// Ends the current section, incrementing the total time and counter.
        void End()
        {
            this->totalTime += Timing::GetTimeInSeconds() - this->lastStartTime;
            ++this->counter;
        }

        /// Resets the benchmarker.
        void Reset()
        {
            this->totalTime = 0.0;
            this->counter   = 0;

            this->lastStartTime = 0.0;  // <-- don't really need to do this, but it's nice for consistency.
        }


        /// Retrieves the total time.
        double GetTotalTime() const
        {
            return totalTime;
        }

        /// Retrieves the average time.
        double GetAverageTime() const
        {
            if (counter > 0)
            {
                return this->totalTime / static_cast<double>(counter);
            }

            return 0.0;
        }


    public:

        /// The accumulated time.
        double totalTime;

        /// The number of segments contributing to <totalTime>. This is used in calculating the average time.
        unsigned int counter;

        /// The time Start() was last called.
        double lastStartTime;
    };
}

#endif
