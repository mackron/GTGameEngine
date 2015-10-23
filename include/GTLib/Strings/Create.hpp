// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Create_hpp_
#define __GTLib_Strings_Create_hpp_

#include "../BasicBuffer.hpp"
#include "../IO/cstdio.hpp"
#include "List.hpp"
#include "Copy.hpp"

#include <cstdarg>

namespace GTLib
{
    namespace Strings
    {
        /// Creates an empty string.
        template <typename T>
        T* CreateEmpty()
        {
            T* result = static_cast<T*>(malloc(sizeof(T)));

            *result = static_cast<T>(0);

            return result;
        }

        /// Creates a new string. If the input string is null, returns an empty string.
        ///
        /// Delete the string with GTLib::Strings::Delete().
        template <typename T>
        T* Create(const T* source, ptrdiff_t sourceSizeInTs = -1, size_t* lengthInTsOut = nullptr)
        {
            if (source != nullptr)
            {
                if (sourceSizeInTs == -1)
                {
                    sourceSizeInTs = SizeInTs(source);
                }

                size_t length = static_cast<size_t>(sourceSizeInTs);              // for ease of use.

                T* result = static_cast<T*>(malloc((length + 1) * sizeof(T)));   // +1 for null terminator.
                std::memcpy(result, source, length * sizeof(T));

                result[length] = static_cast<T>(0);


                if (lengthInTsOut != nullptr)
                {
                    *lengthInTsOut = length;
                }

                return result;
            }

            return CreateEmpty<T>();
        }

        template <typename T>
        T* Create(const T* source, GT::BasicBuffer &dest, ptrdiff_t sourceSizeInTs = -1, size_t* lengthInTsOut = nullptr)
        {
            if (source != nullptr)
            {
                if (sourceSizeInTs == -1)
                {
                    sourceSizeInTs = SizeInTs(source);
                }

                size_t length = static_cast<size_t>(sourceSizeInTs);    // for ease of use.

                dest.Allocate((length + 1) * sizeof(T));                // +1 for null terminator.
                std::memcpy(dest.m_ptr, source, length * sizeof(T));

                T* result = static_cast<T*>(dest.m_ptr);

                result[length] = static_cast<T>(0);


                if (lengthInTsOut != nullptr)
                {
                    *lengthInTsOut = length;
                }

                return result;
            }

            return CreateEmpty<T>();
        }

        template <typename T>
        T* Create(const GTLib::Strings::List<T> &source)
        {
            T* result = static_cast<T*>(malloc(source.BuildStringSize()));
            source.BuildString(result);

            return result;
        }


        template <typename T, typename U>
        T* Create(const U* source, ptrdiff_t sourceSizeInUs = -1, size_t* lengthInTsOut = nullptr)
        {
            // First we need to calculate the size of the buffer.
            size_t bufferSizeInTs = CopySize<T>(source, sourceSizeInUs);

            // Now we simply allocate the buffer and copy into it.
            T* newString = static_cast<T*>(malloc(bufferSizeInTs * sizeof(T))); // <-- Strings::CopySize() will include space for a null-terminator.
            Strings::Copy(newString, source, sourceSizeInUs);


            if (lengthInTsOut != nullptr)
            {
                *lengthInTsOut = bufferSizeInTs - 1;    // -1 because bufferSizeInTs includes the null-terminator.
            }

            // Strings::Copy() will null-terminate the destination, so it's now all good to return.
            return newString;
        }



        /// Creates a formatted string. Currently only UTF-8.
        inline char* CreateFormatted(const char* format, va_list argList1, va_list argList2, size_t* lengthInTsOut = nullptr)
        {
            size_t transformedValueLength = static_cast<size_t>(GTLib::IO::vsnprintf(nullptr, 0, format, argList1));
            auto   transformedValue       = static_cast<char*>(malloc(transformedValueLength + 1));

            GTLib::IO::vsnprintf(transformedValue, transformedValueLength + 1, format, argList2);


            if (lengthInTsOut != nullptr)
            {
                *lengthInTsOut = transformedValueLength;
            }

            return transformedValue;
        }




        /// Deletes a string that was created with GTLib::Strings::Create() or GTLib::Strings::CreateEmpty().
        template <typename T>
        void Delete(T* source)
        {
            free(source);
        }
    }
}

#endif
