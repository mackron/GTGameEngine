// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/stdio.hpp>

namespace GTLib
{
    namespace IO
    {
        int snprintf(char *buffer, size_t sizeOfBuffer, const char *format, ...)
        {
            va_list argList;
            va_start(argList, format);
                int result = GTLib::IO::vsnprintf(buffer, sizeOfBuffer, format, argList);
            va_end(argList);

            return result;
        }
    }
}
