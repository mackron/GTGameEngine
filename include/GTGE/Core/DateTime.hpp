// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_DateTime
#define GT_DateTime

#include <GTGE/Core/String.hpp>
#include <GTGE/Core/Parse.hpp>
#include <time.h>

namespace GT
{
    /**
    *   \brief  Class representing a date and time.
    */
    class DateTime
    {
    public:
    
        /**
        *   \brief  Default constructor.
        */
        DateTime();
        DateTime(time_t time);
        
        /**
        *   \brief              Converts the DateTime to a string.
        *   \param  format [in] The format of the date.
        *   \return             The formatted string.
        */
        String ToFormattedString(const char *format);
        
        /**
        *   \brief  Converts the DateTime to a string based on locale.
        *   \return The string representation of the DateTime.
        */
        String ToShortString();

        /**
        *   \brief  Converts the DateTime to a long time string which does not include the date component.
        *   \return The time of the day converted to a string.
        */
        String To24HourTimeString();
    
    
    private:
        
        /// The internal time object used by this class.
        time_t time;

        
    public:
    
        /**
        *   \brief  Static function for creating a new datetime object which is set to the time the object is created.
        */
        static DateTime Now()
        {
            return DateTime(::time(nullptr));
        }


        /// Creates a datetime object which is set to the compilation time.
        static DateTime BuildTime()
        {
            const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

            tm t;
            memset(&t, 0, sizeof(t));

            // __DATE__
            char datestr[] = __DATE__;
            datestr[3] = '\0';
            datestr[6] = '\0';

            t.tm_year = Parse<int>(datestr + 7) - 1900;
            t.tm_mday = Parse<int>(datestr + 4);

            for (int i = 0; i < 12; i++)
            {
                if (Strings::Equal(datestr, months[i]))
                {
                    t.tm_mon = i;
                    break;
                }
            }

            // __TIME__
            char timestr[] = __TIME__;
            timestr[2] = '\0';
            timestr[5] = '\0';

            t.tm_hour = Parse<int>(timestr + ((timestr[0] != '0') ? 0 : 1));
            t.tm_min  = Parse<int>(timestr + ((timestr[3] != '0') ? 3 : 4));
            t.tm_sec  = Parse<int>(timestr + ((timestr[6] != '0') ? 6 : 7));

            time_t time = ::mktime(&t);

            return DateTime(time);
        }
    };
}

#endif
