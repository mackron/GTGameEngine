// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/DateTime.hpp>

namespace GTLib
{
    DateTime::DateTime()
        : time(0)
    {
    }
    
    DateTime::DateTime(time_t time)
        : time(time)
    {
    }
    
    GTLib::String DateTime::ToFormattedString(const char *format)
    {
        char stringBuffer[64];

#if defined(_MSC_VER)
		tm local;
		localtime_s(&local, &this->time);
        strftime(stringBuffer, 64, format, &local);
#else
		tm *local = localtime(&this->time);
		strftime(stringBuffer, 64, format, local);
#endif
        
        return stringBuffer;
    }
    
    GTLib::String DateTime::ToShortString()
    {
        return this->ToFormattedString("%x %H:%M:%S");
    }

    GTLib::String DateTime::To24HourTimeString()
    {
        return this->ToFormattedString("%H:%M:%S");
    }



    ////////////////////////////////////////////////
    // Static Methods.
}
