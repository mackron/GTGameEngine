// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Timing_Stopwatch
#define GT_Timing_Stopwatch

#include "TimingCommon.hpp"

namespace GT
{
    /**
    *   \brief  High-resolution stopwatch class.
    *
    *   Everything in this class is expressed in seconds.
    *
    *   This class is good for timing. The class works just like a real stopwatch. To
    *   begin timing use Start(). To stop the stopwatch use Stop(). Reset the stopwatch
    *   with Reset(). The total elapsed time seconds can be retrieved with Elapsed().
    */
    class Stopwatch
    {
    public:
    
        Stopwatch();
        
        /**
        *   \brief  Starts or resumes the stopwatch.
        */
        void Start();

        /**
        *   \brief  Stops or pauses the stopwatch.
        */
        void Stop();

        /**
        *   \brief  Resets the stopwatch.
        */
        void Reset();

        /**
        *   \brief  Retrieves the total elapsed time in seconds.
        *   \return The total elapsed time in seconds.
        */
        double Elapsed() const;
    
    private:
    
        /// The start time.
        double startTime;

        /// The total elapsed time.
        double elapsed;
    };
}

#endif
