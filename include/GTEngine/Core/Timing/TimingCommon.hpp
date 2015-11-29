// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Timing_TimingCommon_hpp_
#define __GTLib_Timing_TimingCommon_hpp_

namespace GTLib
{
    namespace Timing
    {
        /**
        *   \brief  Retrieves a value representing the current time in seconds.
        *   \return A value in seconds that can be used to measure time.
        *
        *   \remarks
        *       The returned value is in seconds and should be usable with time based operations.
        *       As an example, if this function is called, and then it is called again 3 seconds
        *       later, the return value of the second call should be equal to the return value of the
        *       first call plus 3.
        *       \par
        *       The return value can be converted to milliseconds by multiplying by 1000.
        */
        double GetTimeInSeconds();
        
        /**
        *   \brief  The same as GetTimeInSeconds(), except the return value is in milliseconds.
        */
        inline double GetTimeInMilliseconds() { return GetTimeInSeconds() * 1000.0; }
    }
}

#endif
