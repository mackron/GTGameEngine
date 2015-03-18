// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/ToString.hpp>
#include <GTLib/Strings/Copy.hpp>
#include <GTLib/IO/cstdio.hpp>

namespace GTLib
{
    String ToString(int64_t value)
    {
        char dest[64];
        GTLib::IO::snprintf(dest, 64, "%lld", value);
        
        return dest;
    }
    
    String ToString(uint64_t value)
    {
        char dest[64];
        GTLib::IO::snprintf(dest, 64, "%llu", value);
        
        return dest;
    }


    String ToString(double value, const char *format)
    {
        char dest[64];

        if (format == nullptr)
        {
            GTLib::IO::snprintf(dest, 64, "%f", value);
        }
        else
        {
            ptrdiff_t formatLength = GTLib::Strings::SizeInTs(format);
            if (formatLength <= 16)
            {
                char printFormat[20];
                printFormat[0] = '%';
                GTLib::Strings::Copy(printFormat + 1, 20, format, formatLength);
                printFormat[formatLength + 1] = 'f';
                printFormat[formatLength + 2] = '\0';

                GTLib::IO::snprintf(dest, 64, printFormat, value);
            }
            else
            {
                GTLib::IO::snprintf(dest, 64, "%f", value);
            }
        }
        
        return dest;
    }

    String ToString(float value, const char *format)
    {
        return GTLib::ToString(static_cast<double>(value), format);
    }
}
