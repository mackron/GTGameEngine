// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_FileDataTypes_hpp_
#define __GT_FileDataTypes_hpp_

#include <GTLib/ResultCodes.hpp>

namespace GT
{
    enum class FileAccessMode
    {
        None = 0,
        Read,
        Write,
    };

    enum class FileSeekOrigin
    {
        Current = 0,
        Start,
        End
    };


    //////////////////////////////////////////
    // Error Codes

    static const ResultCode FileNotFound     = 0x00000010;
    static const ResultCode AccessDenied     = 0x00000011;
    static const ResultCode TooManyOpenFiles = 0x00000012;
}

#endif
