// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Timing/TimingCommon.hpp>

#if defined(_WIN32)
#include <GTLib/windows.hpp>
#include <cstdint>

namespace GTLib
{
    namespace Timing
    {
        static bool    _TimingInitialised = false;
        static int64_t _HighResFrequency  = 1;

        inline void _InitialiseTiming()
        {
            LARGE_INTEGER li;
            QueryPerformanceFrequency(&li);

            _HighResFrequency = li.QuadPart;
            _TimingInitialised = true;
        }

        double GetTimeInSeconds()
        {
            if (!_TimingInitialised)
            {
                _InitialiseTiming();
            }

            LARGE_INTEGER li;
            QueryPerformanceCounter(&li);

            return (double)li.QuadPart / (double)_HighResFrequency;
        }
    }
}


#else
#include <sys/time.h>

namespace GTLib
{
    namespace Timing
    {
        double GetTimeInSeconds()
        {
            timeval t;
            gettimeofday(&t, nullptr);

            return t.tv_sec + (t.tv_usec * 0.000001);
        }
    }
}
#endif
