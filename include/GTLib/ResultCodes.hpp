 // Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ResultCodes
#define GT_ResultCodes

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
        return int(1 << 31) | errorCode;
    }


    /////////////////////////////
    // Standard Result Codes

    static const ResultCode NoError      =  0;
    static const ResultCode UnknownError = -1;
}

#endif
