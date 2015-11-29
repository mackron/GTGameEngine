// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Timing/Stopwatch.hpp>

namespace GT
{
    Stopwatch::Stopwatch()
        : startTime(0.0), elapsed(0.0)
    {
    }
    
    void Stopwatch::Start()
    {
        this->startTime = Timing::GetTimeInSeconds();
    }
    
    void Stopwatch::Stop()
    {
        this->elapsed = Timing::GetTimeInSeconds() - this->startTime;
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

