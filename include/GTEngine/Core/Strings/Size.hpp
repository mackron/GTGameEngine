// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Size_hpp_
#define __GTLib_Strings_Size_hpp_

#include "NextChar.hpp"
#include <cstddef>
#include <cstring>

namespace GTLib
{
    namespace Strings
    {
		/**
		*	\brief	Retrieves the number of basic character units (char, char16_t, char32_t's) in the given string up to it's null-terminator, but does not include the null terminator in the result.
		*
		*	\remarks
		*		We return a ptrdiff_t because:
		*		    - Allows us to return -1 if the string is invalid.
		*			- It plays nicely with string-function arguments allowing the size of the string, or -1 if it's null terminated.
		*/
        template <typename T>
        ptrdiff_t SizeInTs(const T *str)
        {
            if (str != nullptr)
            {
                ptrdiff_t result = 0;

                while (*str++ != 0)
                {
                    ++result;
                }

                return result;
            }

            return 0;
        }

        // strlen() is probably faster than our generic version. We'll use that here.
        template <> inline ptrdiff_t SizeInTs(const char *str)
        {
            if (str != nullptr)
            {
                return static_cast<ptrdiff_t>(std::strlen(str));
            }

            return 0;
        }


        // Same as the others, except it will use the input size as a boundry.
        template <typename T>
        ptrdiff_t SizeInTs(const T* str, ptrdiff_t strSizeInTs)
        {
            if (str != nullptr)
            {
                ptrdiff_t result = 0;

                while ((*str++ != 0) && strSizeInTs != 0)
                {
                    ++result;
                }

                return result;
            }

            return 0;
        }

        


		/**
		*	\brief			   Retrieves the size of a string based on it's start and end position.
		*	\param	start [in] The start of the string.
		*	\param	end   [in] The end of the string.
		*
		*	\remarks
		*		It's asserted that end >= start.
		*/
        template <typename T>
        ptrdiff_t SizeInTs(const T *start, const T *end)
        {
            return end - start;
        }


        /// The same as SizeInTs(), but returns the size in bytes instead.
        template <typename T>
        ptrdiff_t SizeInBytes(const T* str)
        {
            return SizeInTs(str) * static_cast<ptrdiff_t>(sizeof(T));
        }



        /// Retrieves the number of characters in the string. Note this is NOT the number of Ts in the string.
        template <typename T>
        size_t GetCharacterCount(const T* str, ptrdiff_t sizeInTs = -1)
        {
            size_t count = 0;

            while (NextChar(str, sizeInTs) != '\0')
            {
                ++count;
            }

            return count;
        }
    }
}


#endif
