// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_NextChar_hpp_
#define __GTLib_Strings_NextChar_hpp_

#include "UnicodeBase.hpp"
#include <cassert>

namespace GTLib
{
    namespace Strings
    {
        /**
        *   \brief                            Retrieves the first character in a string.
        *   \param  str            [in, out]  The string whose first character is being retrieved. See remarks.
        *   \param  strSizeInBytes [in, out]  The size in Ts of the string.
        *
        *   \remarks
        *       For the sake of speed, we do not check that each character is valid - it assumes the string is
        *       constructed correctly.
        *       \par
        *       The input pointer will be moved to the start of the next character, unless it's a null terminator. Likewise,
        *       the 'strSizeInTs' input variable will be modified to account.
        */
        char32_t NextCharImplementation(const char    * &str, ptrdiff_t &strSizeInTs);
        char32_t NextCharImplementation(const char16_t* &str, ptrdiff_t &strSizeInTs);
        char32_t NextCharImplementation(const char32_t* &str, ptrdiff_t &strSizeInTs);
        char32_t NextCharImplementation(const wchar_t*  &str, ptrdiff_t &strSizeInTs);



        template <typename T>
        char32_t NextChar(const T* &str)
        {
            ptrdiff_t dummySize = -1;
            return NextCharImplementation(str, dummySize);
        }

        template <typename T>
        char32_t NextChar(T* &str, ptrdiff_t &strSizeInTs)
        {
            const T* &end = str;
            char32_t result = NextCharImplementation(end, strSizeInTs);

            str = static_cast<T*>(end);

            return result;
        }

        template <typename T>
        char32_t NextChar(T* &str)
        {
            ptrdiff_t dummySize = -1;
            return NextCharImplementation(str, dummySize);
        }
    }
}

#endif
