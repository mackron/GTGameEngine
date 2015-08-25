// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Strings_Equal
#define GT_Strings_Equal

#include "NextChar.hpp"
#include "Compare.hpp"
#include <cctype>   // For toupper().

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127)   // conditional expression is constant.
#endif

namespace GTLib
{
    namespace Strings
    {
        template <bool caseSensitive, typename T, typename U>
        bool Equal(const T* str1, ptrdiff_t str1SizeInTs, const U* str2, ptrdiff_t str2SizeInUs = -1)
        {
            if (caseSensitive)
            {
                return Strings::FastCompare(str1, str1SizeInTs, str2, str2SizeInUs) == 0;
            }
            else
            {
                char32_t c1;
                char32_t c2;
                do
                {
                    c1 = NextChar(str1, str1SizeInTs);
                    c2 = NextChar(str2, str2SizeInUs);

                    if (std::toupper(int(c1)) != std::toupper(int(c2)))
                    {
                        return false;
                    }

                }while (c1 != '\0' && c2 != '\0');  // Do not check the sizes - NextChar() will return '\0' if a size of 0 is given.

                // If both strings are at the end, they're equal.
                return c1 == '\0' && c2 == '\0';
            }
        }

        template <bool caseSensitive, typename T, typename U>
        bool Equal(const T* str1, const U* str2, ptrdiff_t str2SizeInUs = -1)
        {
            return Equal<caseSensitive>(str1, -1, str2, str2SizeInUs);
        }


        /// Optimized case where the sizes of the strings are used to check for equality.
        template <bool caseSensitive, typename T>
        bool Equal(const T* str1, ptrdiff_t str1SizeInTs, const T* str2, ptrdiff_t str2SizeInTs = -1)
        {
            if (str1SizeInTs != -1 && str2SizeInTs != -1)
            {
                if (str1SizeInTs != str2SizeInTs)
                {
                    return false;
                }
            }

            return Equal<caseSensitive, T, T>(str1, str1SizeInTs, str2, str2SizeInTs);
        }

        template <bool caseSensitive, typename T>
        bool Equal(const T* str1, const T* str2, ptrdiff_t str2SizeInTs = -1)
        {
            return Equal<caseSensitive, T>(str1, -1, str2, str2SizeInTs);
        }



        /// *** The overloads below will default to caseSensitive. They exist only for ease-of-use. ***
        template <typename T, typename U>
        bool Equal(const T* str1, ptrdiff_t str1SizeInTs, const U* str2, ptrdiff_t str2SizeInUs = -1)
        {
            return Equal<true, T, U>(str1, str1SizeInTs, str2, str2SizeInUs);
        }

        template <typename T, typename U>
        bool Equal(const T* str1, const U* str2, ptrdiff_t str2SizeInUs = -1)
        {
            return Equal<true, T, U>(str1, str2, str2SizeInUs = -1);
        }


        /// Optimized case where the sizes of the strings are used to check for equality.
        template <typename T>
        bool Equal(const T* str1, ptrdiff_t str1SizeInTs, const T* str2, ptrdiff_t str2SizeInTs = -1)
        {
            return Equal<true, T>(str1, str1SizeInTs, str2, str2SizeInTs);
        }

        template <typename T>
        bool Equal(const T* str1, const T* str2, ptrdiff_t str2SizeInTs = -1)
        {
            return Equal<true, T>(str1, str2, str2SizeInTs);
        }



        template <typename T>
        bool IsNullOrEmpty(const T* str, ptrdiff_t strSizeInTs = -1)
        {
            if (str == nullptr || strSizeInTs == 0)
            {
                return true;
            }

            return str[0] == '\0';
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
