// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Strings_Find_hpp_
#define __GTLib_Strings_Find_hpp_

#include "NextChar.hpp"
#include "Equal.hpp"
#include "Size.hpp"
#include "Iterator.hpp"

namespace GTLib
{
    namespace Strings
    {
        /**
        *   \brief             Finds the first occurance of a string in another.
        *   \param  str   [in] The string to search.
        *   \param  query [in] The string that is being searched for.
        *   \return            A pointer to the start of the first occurance, or nullptr if it can not be found.
        *
        *   \remarks
        *       If \c query is an empty string, nullptr will be returned.
        */
        template <typename T>
        const T* FindFirst(const T* str, ptrdiff_t strSizeInTs, const T* query, ptrdiff_t querySizeInTs)
        {
            // We need the actual length of the query.
            if (querySizeInTs == -1)
            {
                querySizeInTs = SizeInTs(query);
            }

            // We'll use a basic character-by-character search here.
            do
            {
                if (Equal(str, querySizeInTs, query, querySizeInTs))
                {
                    return str;
                }

            } while((NextChar(str, strSizeInTs) != '\0') && (static_cast<size_t>(strSizeInTs) >= static_cast<size_t>(querySizeInTs)));

            // If we've made it here, it means we didn't find the query.
            return nullptr;
        }

        /**
        *   \brief                   Finds the first occurance of an individual character in a string.
        *   \param  str         [in] The string to search.
        *   \param  strSizeInTs [in] The size in Ts of \c str. Can be -1 if it's null terminated.
        *   \param  character   [in] The character to search for.
		*	\return                  nullptr if the character can not be found.
        */
        template <typename T>
        const T* FindFirst(const T* str, ptrdiff_t strSizeInTs, char32_t character)
        {
            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (i.character == character)
                {
                    return i.str;
                }

                ++i;
            }

            return nullptr;
        }

        /**
        *   \brief                   Finds the last occurance of an individual character in a string.
        *   \param  str         [in] The string to search.
        *   \param  strSizeInTs [in] The size in Ts of \c str. Can be -1 if it's null terminated.
        *   \param  character   [in] The character to search for.
        */
        template <typename T>
        const T* FindLast(const T* str, ptrdiff_t strSizeInTs, char32_t character)
        {
            const T* last = nullptr;

            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (i.character == character)
                {
                    last = i.str;
                }

                ++i;
            }

            return last;
        }

        /**
        *   \brief                  Finds the first occurance of an individual character in the given character string.
        *   \param  str        [in] The string to search.
        *   \param  characters [in] The characters to search for.
        */
        template <typename T>
        const T* FindFirstOf(const T* str, ptrdiff_t strSizeInTs, const T* characters, ptrdiff_t charactersSizeInTs = -1)
        {
            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (FindFirst(characters, charactersSizeInTs, i.character))
                {
                    return i.str;
                }

                ++i;
            }

            return nullptr;
        }

        /**
        *   \brief                  Finds the last occurance of an individual character in the given character string.
        *   \param  str        [in] The string to search.
        *   \param  characters [in] The characters to search for.
        */
        template <typename T>
        const T* FindLastOf(const T* str, ptrdiff_t strSizeInTs, const T* characters, ptrdiff_t charactersSizeInTs = -1)
        {
            const T* last = nullptr;

            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (FindFirst(characters, charactersSizeInTs, i.character))
                {
                    last = i.str;
                }

                ++i;
            }

            return last;
        }

        /**
        *   \brief                  Finds the first occurance of a character that is not part of the given character string.
        *   \param  str        [in] The string to search.
        *   \param  characters [in] The characters to search for.
        *   \return                 A pointer to the first character that is \b not in the \c characters string.
        */
        template <typename T>
        const T* FindFirstNotOf(const T* str, ptrdiff_t strSizeInTs, const T* characters, ptrdiff_t charactersSizeInTs = -1)
        {
            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (!FindFirst(characters, charactersSizeInTs, i.character))
                {
                    return i.str;
                }

                ++i;
            }

            return nullptr;
        }
            
        /**
        *   \brief                  Finds the last occurance of an individual character that is \b not in the given character string.
        *   \param  str        [in] The string to search.
        *   \param  characters [in] The characters to search for.
        */
        template <typename T>
        const T* FindLastNotOf(const T* str, ptrdiff_t strSizeInTs, const T* characters, ptrdiff_t charactersSizeInTs = -1)
        {
            const T* last = nullptr;

            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (!FindFirst(characters, charactersSizeInTs, i.character))
                {
                    last = i.str;
                }

                ++i;
            }

            return last;
        }


        /// Finds the first occurances of a whitespace character.
        template <typename T>
        const T* FindFirstWhitespace(const T* str, ptrdiff_t strSizeInTs = -1)
        {
            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (IsWhitespace(i.character))
                {
                    return i.str;
                }

                ++i;
            }

            return nullptr;
        }

        /// Finds the first occurances of a non-whitespace character.
        template <typename T>
        const T* FindFirstNotWhitespace(const T* str, ptrdiff_t strSizeInTs = -1)
        {
            Strings::Iterator<T> i(str, strSizeInTs);
            while (i)
            {
                if (!IsWhitespace(i.character))
                {
                    return i.str;
                }

                ++i;
            }

            return nullptr;
        }



        // *** Some inlined helpers for ease of use ***
        template <typename T>
        T* FindFirst(T* str, ptrdiff_t strSizeInBytes, const T* query, ptrdiff_t querySizeInBytes)
        {
            return (T*)FindFirst((const T*)str, strSizeInBytes, query, querySizeInBytes);
        }
        template <typename T>
        const T* FindFirst(const T* str, const T* query)
        {
            return FindFirst(str, -1, query, -1);
        }
        template <typename T>
        T* FindFirst(T* str, const T* query)
        {
            return FindFirst(str, -1, query, -1);
        }
        
        template <typename T>
        T* FindFirst(T* str, ptrdiff_t strSizeInBytes, char32_t character)
        {
            return (T*)FindFirst((const T*)str, strSizeInBytes, character);
        }
        template <typename T>
        const T* FindFirst(const T* str, char32_t character)
        {
            return FindFirst(str, -1, character);
        }
        template <typename T>
        T* FindFirst(T* str, char32_t character)
        {
            return FindFirst(str, -1, character);
        }
        inline const char* FindFirst(const char* str, char character)
        {
            return std::strchr(str, character);
        }
        inline char* FindFirst(char* str, char character)
        {
            return std::strchr(str, character);
        }
        
        template <typename T>
        T* FindLast(T* str, ptrdiff_t strSizeInBytes, char32_t character)
        {
            return (T*)FindLast((const T*)str, strSizeInBytes, character);
        }
        template <typename T>
        const T* FindLast(const T* str, char32_t character)
        {
            return FindLast(str, -1, character);
        }
        template <typename T>
        T* FindLast(T* str, char32_t character)
        {
            return FindLast(str, -1, character);
        }
        inline const char* FindLast(const char* str, char character)
        {
            return std::strrchr(str, character);
        }
        inline char* FindLast(char* str, char character)
        {
            return std::strrchr(str, character);
        }
        
        template <typename T>
        T* FindFirstOf(T* str, ptrdiff_t strSizeInBytes, const T* characters, ptrdiff_t charactersSizeInBytes = -1)
        {
            return (T*)FindFirstOf(str, strSizeInBytes, characters, charactersSizeInBytes);
        }
        template <typename T>
        const T* FindFirstOf(const T* str, const T* characters)
        {
            return FindFirstOf(str, -1, characters, -1);
        }
        template <typename T>
        T* FindFirstOf(T* str, const T* characters)
        {
            return FindFirstOf(str, -1, characters, -1);
        }
        
        template <typename T>
        T* FindLastOf(T* str, ptrdiff_t strSizeInBytes, const T* characters, ptrdiff_t charactersSizeInBytes = -1)
        {
            return (T*)FindLastOf((const T*)str, strSizeInBytes, characters, charactersSizeInBytes);
        }
        template <typename T>
        const T* FindLastOf(const T* str, const T* characters)
        {
            return FindLastOf(str, -1, characters, -1);
        }
        template <typename T>
        T* FindLastOf(T* str, const T* characters)
        {
            return FindLastOf(str, -1, characters, -1);
        }
        
        template <typename T>
        T* FindFirstNotOf(T* str, ptrdiff_t strSizeInBytes, const T* characters, ptrdiff_t charactersSizeInBytes = -1)
        {
            return (T*)FindFirstNotOf((const T*)str, strSizeInBytes, characters, charactersSizeInBytes);
        }
        template <typename T>
        const T* FindFirstNotOf(const T* str, const T* characters)
        {
            return FindFirstNotOf(str, -1, characters, -1);
        }
        template <typename T>
        T* FindFirstNotOf(T* str, const T* characters)
        {
            return FindFirstNotOf(str, -1, characters, -1);
        }
        
        template <typename T>
        T* FindLastNotOf(T* str, ptrdiff_t strSizeInBytes, const T* characters, ptrdiff_t charactersSizeInBytes = -1)
        {
            return (T*)FindLastNotOf((const T*)str, strSizeInBytes, characters, charactersSizeInBytes);
        }
        template <typename T>
        const T* FindLastNotOf(const T* str, const T* characters)
        {
            return FindLastNotOf(str, -1, characters, -1);
        }
        template <typename T>
        T* FindLastNotOf(T* str, const T* characters)
        {
            return FindLastNotOf(str, -1, characters, -1);
        }
    }
}


#endif
