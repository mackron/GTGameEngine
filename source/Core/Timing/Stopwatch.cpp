// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Timing/Stopwatch.hpp>

namespace GTLib
{
    Stopwatch::Stopwatch()
        : startTime(0.0), elapsed(0.0)
    {
    }
    
    void Stopwatch::Start()
    {
        this->startTime = GTLib::Timing::GetTimeInSeconds();
    }
    
    void Stopwatch::Stop()
    {
        this->elapsed = GTLib::Timing::GetTimeInSeconds() - this->startTime;
    }
    
    void Stopwatch::Reset()
    {
        this->startTime = 0.0;
        this->elapsed   = 0.0;
    }
    
    double Stopwatch::Elapsed() const
    {
        return this->elapsed;
    }
}

