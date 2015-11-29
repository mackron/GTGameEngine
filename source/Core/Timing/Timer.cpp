// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Timing/Timer.hpp>

namespace GTLib
{
    Timer::Timer()
         : lastTime(0.0), timeInterval(0.0)
    {
        this->lastTime = GTLib::Timing::GetTimeInSeconds();
        this->Update();
    }
    
    double Timer::Update()
    {
        double currentTime = GTLib::Timing::GetTimeInSeconds();

        this->timeInterval = currentTime - this->lastTime;
        this->lastTime     = currentTime;

        return this->GetTimeBetweenUpdates();
    }
    
    double Timer::LastTime() const
    {
        return this->lastTime;
    }
    
    double Timer::GetTimeSinceLastUpdate() const
    {
        return GTLib::Timing::GetTimeInSeconds() - this->lastTime;
    }
    
    double Timer::GetTimeBetweenUpdates() const
    {
        return this->timeInterval;
    }
    
    bool Timer::HasTimePassed(double timeSecs) const
    {
        if ((GTLib::Timing::GetTimeInSeconds() - this->lastTime) >= timeSecs)
        {
            return true;
        }

        return false;
    }
}
