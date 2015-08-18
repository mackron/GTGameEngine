// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/FastParse.hpp>

namespace GT
{
    inline bool atof_IsWhitespace(char c)
    {
        return c == ' ' || c == '\t';
    }

    inline bool atof_IsValidDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    float atof(const char* str, const char* strEnd, const char** strEndOut)
    {
        // Skip leading whitespace.
        while (str < strEnd && atof_IsWhitespace(*str))
        {
            str += 1;
        }


        // Check that we have a string after moving the whitespace.
        if (str < strEnd)
        {
            float sign  = 1.0f;
            float value = 0.0f;

            // Sign.
            if (*str == '-')
            {
                sign = -1.0f;
                str += 1;
            }
            else if (*str == '+')
            {
                sign = 1.0f;
                str += 1;
            }


            // Digits before the decimal point.
            while (str < strEnd && atof_IsValidDigit(*str))
            {
                value = value * 10.0f + (*str - '0');

                str += 1;
            }

            // Digits after the decimal point.
            if (*str == '.')
            {
                float pow10 = 10.0f;

                str += 1;
                while (str < strEnd && atof_IsValidDigit(*str))
                {
                    value += (*str - '0') / pow10;
                    pow10 *= 10.0f;

                    str += 1;
                }
            }

            
            if (strEndOut != nullptr)
            {
                *strEndOut = str;
            }

            return sign * value;
        }
        else
        {
            // Empty string.
            return 0.0f;
        }
    }
}
