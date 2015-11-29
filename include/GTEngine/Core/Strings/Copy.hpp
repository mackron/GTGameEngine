// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Copy_hpp_
#define __GTLib_Strings_Copy_hpp_

#include "Iterator.hpp"
#include "Size.hpp"

namespace GT
{
    namespace Strings
    {
		/**
		*	\brief                      Makes a null terminated copy of the given string.
		*	\param	dest           [in] A pointer to the buffer that will receive the copy.
		*	\param	destSizeInTs   [in] The size of the buffer pointed to by \c dest. See remarks.
		*	\param	source         [in] A pointer to the buffer containing the string to copy.
		*	\param	sourceSizeInUs [in] The size in bytes of the source string, not including the null terminator.
		*
		*	\remarks
		*       The destination string will always be null terminated; the destination buffer must be large enough
		*       to store the null terminator.
        *       \par
        *       This does not check the validity of the string. It only does a simple copy.
		*/
        template <typename T, typename U>
        void Copy(T* dest, ptrdiff_t destSizeInTs, const U* source, ptrdiff_t sourceSizeInUs = -1)
        {
            char32_t c;
            while ((c = NextChar(source, sourceSizeInUs)) != '\0' && static_cast<size_t>(destSizeInTs) > 1)     // <-- size_t cast is important here - signed -> unsigned.
            {
                size_t charSize = WriteChar(dest, c);
                
                dest         += charSize;
                destSizeInTs -= static_cast<ptrdiff_t>(charSize);
            }

            WriteNullTerminator(dest);  // <-- null-terminate.
        }


        template <typename T, typename U>
        void Copy(T* dest, const U* source, ptrdiff_t sourceSizeInUs = -1)
        {
            return Copy(dest, -1, source, sourceSizeInUs);
        }


        // Optimized case.
        template <typename T>
        void Copy(T* dest, ptrdiff_t destSizeInTs, const T* source, ptrdiff_t sourceSizeInTs = -1)
        {
            while (sourceSizeInTs != 0 && (static_cast<size_t>(destSizeInTs) > 1) && *source != '\0')
			{
				*dest = *source;

				--sourceSizeInTs;
				--destSizeInTs;
				++source;
				++dest;
			}

			*dest = '\0';
        }

        template <typename T>
        void Copy(T* dest, const T* source, ptrdiff_t sourceSizeInTs = -1)
        {
            return Copy(dest, -1, source, sourceSizeInTs);
        }
    }
}


// *** CopySize ***
namespace GT
{
    namespace Strings
    {
        /// Retrieves the size of the buffer required to store a copy of the given string using the encoding defined by 'T'.
        template <typename T, typename U>
        size_t CopySize(const U* source, ptrdiff_t sourceSizeInUs)
        {
            size_t result = 1;  // Start at 1 for null-terminator.

            char32_t c;
            while ((c = NextChar(source, sourceSizeInUs)) != '\0')
            {
                result += GetCharSize<T>(c);
            }

            return result;
        }


        // Optimized case.
        template <typename T>
        size_t CopySize(const T* source, ptrdiff_t sourceSizeInTs)
        {
            // We +1 results here for the null-terminator.
            //
            // Also note how we branch here. We could generically use the bottom version, but my local knowledge of SizeInTs() tells
            // me that strlen() will be used for null-terminated UTF-8 strings, which is probably more efficient than GT's implementation.

            if (sourceSizeInTs == -1)
            {
                return Strings::SizeInTs(source) + 1;
            }
            else
            {
                return Strings::SizeInTs(source, sourceSizeInTs) + 1;
            }
        }
    }
}


#endif
