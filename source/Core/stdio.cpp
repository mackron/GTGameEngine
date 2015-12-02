// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/stdio.hpp>

namespace GT
{
    namespace IO
    {
        int snprintf(char *buffer, size_t sizeOfBuffer, const char *format, ...)
        {
            va_list argList;
            va_start(argList, format);
                int result = IO::vsnprintf(buffer, sizeOfBuffer, format, argList);
            va_end(argList);

            return result;
        }
    }
}
