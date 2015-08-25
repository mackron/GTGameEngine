// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/DateTime.hpp>
#include <GTLib/Config.hpp>

namespace GTLib
{
    DateTime::DateTime()
        : m_time(0)
    {
    }

    DateTime::DateTime(time_t timeIn)
        : m_time(timeIn)
    {
    }

    GTLib::String DateTime::ToFormattedString(const char *format)
    {
        char stringBuffer[64];

#if defined(GT_COMPILER_VC)
		tm local;
		localtime_s(&local, &m_time);
        strftime(stringBuffer, 64, format, &local);
#else
		tm *local = localtime(&m_time);
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
