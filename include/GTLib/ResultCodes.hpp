 // Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ResultCodes_hpp_
#define __GT_ResultCodes_hpp_

namespace GT
{
    typedef int ResultCode;


    static inline bool Failed(ResultCode code)
    {
        return code < 0;
    }
    static inline bool Succeeded(ResultCode code)
    {
        return code >= 0;
    }

    static inline ResultCode GenError(int errorCode)
    {
        return (1 << 31) | errorCode;
    }
    

    /////////////////////////////
    // Standard Result Codes

    static const ResultCode NoError      =  0;
    static const ResultCode UnknownError = -1;
}

#endif
