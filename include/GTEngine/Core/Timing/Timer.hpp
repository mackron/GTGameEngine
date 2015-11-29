// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Timing_Timer_hpp_
#define __GTLib_Timing_Timer_hpp_

#include "TimingCommon.hpp"

namespace GTLib
{
    /**
    *   \brief  Class used for timing.
    *
    *   Everything is expressed in seconds.
    */
    class Timer
    {
    public:
    
        /**
        *   \brief  Constructor.
        */
        Timer();
        
        /**
        *   \brief  Updates the timer.
        *
        *   \return
        *       The time in seconds since the previous update.
        */
        double Update();
        
        /**
        *   \brief  Retrieves the absolute time the last time Timer::Update() was called.
        *   \return	The absolute time the last time Timer::Update() was called, in seconds.
        */
        double LastTime() const;

        /**
        *   \brief  Retrieves the time in seconds since the last call to Timer::Update().
        *   \return The time in seconds since the last call to Timer::Update().
        */
        double GetTimeSinceLastUpdate() const;

        /**
        *   \brief  Retrieves the interval in seconds between the last two calls to Timer::Update().
        *   \return The time in seconds between the last two calls to Timer::Update().
        */
        double GetTimeBetweenUpdates() const;

        /**
        *   \brief                Determines if a given time in seconds has passed since the last call to Timer::update().
        *   \param	timeSecs [in] The time in seconds.
        *   \return               True if \c timeSecs has passed since the last call to Timer::update(); false otherwise.
        */
        bool HasTimePassed(double timeSecs) const;
    
    
    private:
    
        /// The last time Update() was called.
        double lastTime;

        /// The time between the last two calls to Update().
        double timeInterval;
        
    };
}


#endif
