// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Trim_hpp_
#define __GTLib_Strings_Trim_hpp_

#include "NextChar.hpp"
#include "Copy.hpp"

namespace GTLib
{
    namespace Strings
    {
        /// Retrieves a pointer to the first non-whitespace character in the given string.
        ///
        /// \param  str         [in] The string to search.
        /// \param  strSizeInTs [in] The size in Ts of the input string. Can be -1 if the string is null terminated.
        ///
        /// \remarks
        ///     The input string is not modified.
        template <typename T>
        const T* TrimStart(const T* str, ptrdiff_t strSizeInTs = -1)
        {
            auto prev = str;

            char32_t c;
            while ((c = NextChar(str, strSizeInTs)) != '\0')
            {
                if (!IsWhitespace(c))
                {
                    return prev;
                }

                prev = str;
            }

            // If we make it here, the entire string was whitespace.
            return str;
        }

        /// Retrieves a pointer to 1 character past the last non-whitespace character in a string.
        ///
        /// \param  str         [in] The string to search.
        /// \param  strSizeInTs [in] The size in Ts of the input string. Can be -1 if the string is null-terminated.
        ///
        /// \remarks
        ///     Remember to note that the returned calue is 1 past the last non-whitespace character. If there is nothing to trim,
        ///     it will be a pointer to the null terminator.
        ///     \par
        ///     Unfortunately UTF encoding does not allow backwards iteration, thus the entire string will need to be iterated.
        template <typename T>
        const T* TrimEnd(const T* str, ptrdiff_t strSizeInTs = -1)
        {
            const T* end = str;

            char32_t c;
            while ((c = NextChar(str, strSizeInTs)) != '\0')
            {
                if (!IsWhitespace(c))
                {
                    end = str;
                }
            }

            return end;
        }


        /// Trims the start of a non-const string.
        template <typename T>
        T* TrimStart(T* str, ptrdiff_t &strSizeInTs)
        {
            const T*  newStr         = TrimStart(const_cast<const T*>(str), strSizeInTs);
            ptrdiff_t newStrSizeInTs = strSizeInTs - (newStr - str);

            Copy(str, newStr, newStrSizeInTs);
            strSizeInTs = newStrSizeInTs;

            return str;
        }

        template <typename T>
        T* TrimStart(T* str)
        {
            ptrdiff_t dummySize = -1;
            return TrimStart(str, dummySize);
        }


        /// Trims the end of a non-const string.
        template <typename T>
        T* TrimEnd(T* str, ptrdiff_t &strSizeInTs)
        {
            const T* end = TrimEnd(const_cast<const T*>(str), strSizeInTs);
            if ((end - str) > strSizeInTs)
            {
                *const_cast<T*>(end) = '\0';
            }

            strSizeInTs = (end - str);

            return const_cast<T*>(end);
        }

        template <typename T>
        T* TrimEnd(T* str)
        {
            ptrdiff_t dummySize = -1;
            return TrimEnd(str, dummySize);
        }


        /// Trims the start and end of a non-const string.
        template <typename T>
        T* Trim(T* str, ptrdiff_t &strSizeInTs)
        {
            TrimEnd(str, strSizeInTs);
            return TrimStart(str, strSizeInTs);
        }

        template <typename T>
        T* Trim(T* str)
        {
            ptrdiff_t dummySize = -1;
            return Trim(str, dummySize);
        }
    }
}

#endif
