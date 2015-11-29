/**
*   Some of the code here is taken from ftp://www.unicode.org/Public/PROGRAMS/CVTUTF/.
*   The full copyright notice can be found at the bottom of this document.
*/
#ifndef __GTLib_Strings_UnicodeBase_hpp_
#define __GTLib_Strings_UnicodeBase_hpp_

#include <cstddef>
#include <cassert>

namespace GT
{
    namespace Strings
    {
        /**
        *   \brief  Retrieves the number of bytes making up a UTF-8 character based on it's first byte.
        *   \return The size of the UTF-8 character, or -1 if the first byte is not valid (>= 248)
        *
        *   The size of a UTF-8 character can be determined by looking at it's first byte.
        */
        /*
        int UTF8CharSize(unsigned char8 firstByte)
        {
            if (firstByte < 192)
            {
                return 0;
            }
            
            if (firstByte < 224)
            {
                return 1;
            }
            
            if (firstByte < 240)
            {
                return 2;
            }
            
            if (firstByte < 248)
            {
                return 3;
            }
            
            // 5 and 6 byte UTF-8 strings are no longer legal.
            return -1;
        }
        */
        
        /**
        *   \brief  Retrieves an offset value that is subtracted from the UTF-8 character after the bytes have been combined. This is based on the size.
        */
        /*
        charUTF3 UTF8Offset(int size)
        {
            if (size == 1)
            {
                return 0x00000000UL;
            }
            
            if (size == 2)
            {
                return 0x00003080UL;
            }
            
            if (size == 3)
            {
                return 0x000E2080UL;
            }
            
            if (size == 4)
            {
                return 0x03C82080UL;
            }
        }
        */
        
        #define UNICODE_BOM             0xFEFF

        #define UNI_REPLACEMENT_CHAR    (char32_t)0x0000FFFD
        #define UNI_MAX_BMP             (char32_t)0x0000FFFF
        #define UNI_MAX_UTF16           (char32_t)0x0010FFFF
        #define UNI_MAX_UTF32           (char32_t)0x7FFFFFFF
        #define UNI_MAX_LEGAL_UTF32     (char32_t)0x0010FFFF

        #define UNI_SUR_HIGH_START      (char32_t)0xD800
        #define UNI_SUR_HIGH_END        (char32_t)0xDBFF
        #define UNI_SUR_LOW_START       (char32_t)0xDC00
        #define UNI_SUR_LOW_END         (char32_t)0xDFFF

        #define UNI_HALF_SHIFT          (char32_t)10
        #define UNI_HALF_BASE           (char32_t)0x0010000UL
        #define UNI_HALF_MASK           (char32_t)0x3FFUL


        /*
         * Index into the table below with the first byte of a UTF-8 sequence to
         * get the number of trailing bytes that are supposed to follow it.
         * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
         * left as-is for anyone who may want to do such conversion, which was
         * allowed in earlier algorithms.
         */
         /*
        static const char g_trailingBytesForUTF8[256] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
        };
        */

        /*
         * Magic values subtracted from a buffer value during UTF8 conversion.
         * This table contains as many values as there might be trailing bytes
         * in a UTF-8 sequence.
         */
         /*
        static const charUTF32 g_offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                       0x03C82080UL, 0xFA082080UL, 0x82082080UL};
                                                       */

        /*
         * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
         * into the first byte, depending on how many bytes follow.  There are
         * as many entries in this table as there are UTF-8 sequence types.
         * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
         * for *legal* UTF-8 will be 4 or fewer bytes total.
         */
        static const unsigned char g_firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

        /**
        *   \brief              Determines if the next UTF-8 character in the specified string is legal.
        *   \param  str    [in] The string whose first character needs to be checked.
        *   \param  length [in] The length of the character in the string.
        *   \return             True if the character is legal; false otherwise.
        */
        inline bool IsLegalUTF8(const char *str, unsigned short sizeInBytes)
        {
            unsigned char a;
            const char *srcptr = str + sizeInBytes;

            switch (sizeInBytes)
            {
            default:
                {
                    return false;
                }

            case 4:
                {
                    a = (unsigned char)*--srcptr;
                    if (a < 0x80 || a > 0xBF)
                    {
                        return false;
                    }
                }

            case 3:
                {
                    a = (unsigned char)*--srcptr;
                    if (a < 0x80 || a > 0xBF)
                    {
                        return false;
                    }
                }

            case 2:
                {
                    a = (unsigned char)*--srcptr;
                    if (a > 0xBF)
                    {
                        return false;
                    }

                    // Note that there is no fall-throughs with this switch.
                    switch ((unsigned char)*str)
                    {
                    case 0xE0:
                        {
                            if (a < 0xA0)
                            {
                                return false;
                            }

                            break;
                        }

                    case 0xED:
                        {
                            if (a < 0x9F)
                            {
                                return false;
                            }

                            break;
                        }

                    case 0xF0:
                        {
                            if (a < 0x90)
                            {
                                return false;
                            }

                            break;
                        }

                    case 0xF4:
                        {
                            if (a < 0x8F)
                            {
                                return false;
                            }

                            break;
                        }

                    default:
                        {
                            if (a < 0x80)
                            {
                                return false;
                            }
                        }
                    }
                }

            case 1:
                {
                    if ((unsigned char)*str >= 0x80 && (unsigned char)*str < 0xC2)
                    {
                        return false;
                    }
                }
            }

            if ((unsigned char)*str > 0xF4)
            {
                return false;
            }

            return true;
        }

        /**
        *   \brief                  Validates a UTF-32 character.
        *   \param  character [in]  The character to validate.
        *   \return                 The validated character.
        *
        *   \remarks
        *       If the input character is already valid, the return value will be the same
        *       as the input character. If it is invalid, the replacement character will
        *       be returned. If the replacement character can not be used, null will be
        *       returned.
        */
        inline char32_t ValidateUTF32Char(char32_t character)
        {
            // We first need to make sure that it is not higher than the 17'th plane.
            if (character <= UNI_MAX_LEGAL_UTF32)
            {
                // We need to make sure that the final character is still valid and not equal
                // to an illegal character code.
                if (character >= UNI_SUR_HIGH_START && character <= UNI_SUR_LOW_END)
                {
                    // The character is illegal, so replace it with our replacement character.
                    return UNI_REPLACEMENT_CHAR;
                }
                else
                {
                    return character;
                }
            }

            // If we've made it here, the final character is larger than the maximum value
            // defined by Unicode (0x10FFFF).
            return UNI_REPLACEMENT_CHAR;
        }

        /*
        template <typename T>
        void _movestr(T *dest, T *source, size_t count)
        {
            if (source > dest)
            {
                while (count > 0 && *source != 0)
                {
                    *dest = *source;

                    ++dest;
                    ++source;
                    --count;
                }
            }
            else
            {
                // In this case, we need to move from the end.
                while (count > 0)
                {
                    dest[count - 1] = source[count - 1];
                    --count;
                }
            }
        }
        */

        /**
        *   \brief                  Moves a string down in memory location.
        *   \param  source     [in] Pointer to the string that should be moved down.
        *   \param  count      [in] The number of T's to move the string down.
        *   \param  sourceSize [in] The number of T's to move down.
        *
        *   \remarks
        *       This function will not read any data past a null terminator.
        */
        /*
        template <typename T>
        void _movestrdown(T *source, size_t count, size_t sourceSize)
        {
            T *dest = source - count;

            while (sourceSize > 0 && *source != 0)
            {
                *dest = *source;

                ++dest;
                ++source;
                --sourceSize;
            }

            // If the loop was terminated because of a null terminator, we need to
            // make sure that the destination is also null terminated.
            if (sourceSize > 0 && *source == 0)
            {
                *dest = '\0';
            }
        }
        */
    }
}

namespace GT
{
    namespace Strings
    {
        /// Checks if the given unicode character is whitespace.
        inline bool IsWhitespace(char32_t c)
        {
            return (c >= 0x0009 && c <= 0x000D) ||
                   (c == 0x0020)                ||
                   (c == 0x0085)                ||
                   (c == 0x00A0)                ||
                   (c == 0x1680)                ||
                   (c == 0x180E)                ||
                   (c >= 0x2000 && c <= 0x200A) ||
                   (c == 0x2028)                ||
                   (c == 0x2029)                ||
                   (c == 0x202F)                ||
                   (c == 0x205F)                ||
                   (c == 0x3000);
        }
        
        
        /// Checks if the character at the start of the given UTF-8string is a whitespace character, and if so, returns it's size in bytes.
        ///
        /// @return the number of UTF-8 encoded bytes making up the character, or 0 if the character is not whitespace.
        ///
        /// @remarks
        ///     Use this in determining whether or not the next character in a string is a whitespace character. It can replace IsWhitespace()
        ///     if the higher level algorithm is appropriate.
        ///
        ///     The advantage to this function over IsWhitespace() is that the character does not need to be converted to UTF-32 beforehand.
        inline size_t GetWhitespaceSize(const char* str)
        {
            // TODO: This needs to be tested properly.
            
            const unsigned char byte1 = static_cast<unsigned char>(str[0]);
            if (byte1 >= 0x0009)
            {
                if (byte1 <= 0x000D)
                {
                    return 1;
                }
                if (byte1 == 0x20)
                {
                    return 1;
                }
                
                if (byte1 >= 0xC2)
                {
                    const unsigned char byte2 = static_cast<unsigned char>(str[1]);
                    
                    if (byte1 == 0xC2)
                    {
                        if (byte2 == 0x85 || byte2 == 0xA0)
                        {
                            return 2;
                        }
                    }
                    else if (byte1 >= 0xE1)
                    {
                        const unsigned char byte3 = static_cast<unsigned char>(str[2]);
                        
                        if (byte1 == 0xE1)
                        {
                            if (byte2 == 0x9A && byte3 == 0x80)
                            {
                                return 3;
                            }
                            if (byte2 == 0xA0 && byte3 == 0x8E)
                            {
                                return 3;
                            }
                        }
                        else if (byte1 == 0xE2)
                        {
                            if (byte2 == 0x80)
                            {
                                if (byte3 >= 0x80)
                                {
                                    if (byte3 <= 0x8A)
                                    {
                                        return 3;
                                    }
                                    
                                    if (byte3 == 0xA8 || byte3 == 0x89 || byte3 == 0xAF)
                                    {
                                        return 3;
                                    }
                                }
                            }
                            else if (byte2 == 0x81 && byte3 == 0x9F)
                            {
                                return 3;
                            }
                        }
                        else if (byte1 == 0xE3 && byte2 == 0x80 && byte3 == 0x80)
                        {
                            return 3;
                        }
                    }
                }
            }
            
            return 0;
        }
        


        /// Retrieves the number of Ts making up a single character.
        template <typename T>
        size_t GetCharSize(char32_t c)
        {
            // The character is not allowed to be equal to a UTF-16 surrogate. If it is,
            // we return 0.
            if (c >= UNI_SUR_HIGH_START && c <= UNI_SUR_LOW_END)
            {
                return 0;
            }

            // The character is not allowed to be larger than the largest legal character.
            if (c >= UNI_MAX_LEGAL_UTF32)
            {
                return 0;
            }

            return 1;
        }

        template <> inline size_t GetCharSize<char>(char32_t c)     // UTF-8
        {
            if (c < 0x80)
            {
                return 1;
            }
            else if (c < 0x800)
            {
                return 2;
            }
            else if (c < 0x10000)
            {
                return 3;
            }
            else if (c < 0x110000)
            {
                return 4;
            }

            return 0;
        }

        template <> inline size_t GetCharSize<char16_t>(char32_t c) // UTF-16
        {
            if (c <= UNI_MAX_BMP)
            {
                // The character is not allowed to be equal to a UTF-16 surrogate. If it is, we return 0.
                if (c >= UNI_SUR_HIGH_START && c <= UNI_SUR_LOW_END)
                {
                    return 0;
                }

                return 1;
            }

            // The character is not allowed to be larger than the largest legal character.
            if (c >= UNI_MAX_LEGAL_UTF32)
            {
                return 0;
            }
    
            return 2;
        }


        /// Writes a character to the given buffer.
        /// @param  dest [in] A pointer to the buffer that will receive the character. Can not be null.
        /// @param  c    [in] The character to write tho the buffer.
        /// @return The number of Ts (char, char16_t, char32_t) written to 'dest'.
        ///
        /// Do not use this function for determining the size of a character. Use GetCharSize() for that.
        inline size_t WriteChar(char* dest, char32_t c)
        {
            if (dest != nullptr)
            {
                // Grab the width of the character.
                size_t charSize = GetCharSize<char>(c);
                if (charSize == 0)
                {
                    charSize = 3;
                    c = UNI_REPLACEMENT_CHAR;
                }


                dest += charSize;

                // TODO: We might want to do some sort of overflow check here. The only real
                // way to do this is to have another parameter detailing the size of the
                // destination buffer.

                // Now we need to copy all of our bytes over. We use the fall-through switch
                // like the one in the code by Unicode, Inc.
                switch (charSize)
                {
                case 4: *--dest = static_cast<char>(((c | 0x80) & 0xBF)); c >>= 6;
                case 3: *--dest = static_cast<char>(((c | 0x80) & 0xBF)); c >>= 6;
                case 2: *--dest = static_cast<char>(((c | 0x80) & 0xBF)); c >>= 6;
                case 1: *--dest = static_cast<char>((c | g_firstByteMark[charSize]));
                default: break;
                }

                return charSize;
            }

            return 0;
        }

        inline size_t WriteChar(char16_t* dest, char32_t c)
        {
            assert(dest != nullptr);

            // Our character width will always be equal to at least 1.
            size_t charSize = 1;

            // If the character is lower or equal to our maximum BMP we can store the character
            // as a single char16_t.
            if (c <= UNI_MAX_BMP)
            {
                // The character is not allowed to equal a UTF-16 surrogate. If it does, we will
                // replace the character.
                if (c >= UNI_SUR_HIGH_START && c <= UNI_SUR_LOW_END)
                {
                    *dest++ = (char16_t)UNI_REPLACEMENT_CHAR;
                }
                else
                {
                    *dest++ = static_cast<char16_t>(c);
                }
            }
            else if (c > UNI_MAX_UTF16)
            {
                // If we've made it here the character is illegal, so we will replace it.
                *dest++ = (char16_t)UNI_REPLACEMENT_CHAR;
            }
            else
            {
                // If we've made it here, the character needs to be divided into two parts.

                // TODO: We should probably do some overflow check here. Again, we will probably
                // need a third parameter detailing the size of the destination buffer.

                // We now need split the character into two char16_t's.
                c -= UNI_HALF_BASE;

                *dest++ = static_cast<char16_t>((c >> UNI_HALF_SHIFT) + UNI_SUR_HIGH_START);
                *dest++ = static_cast<char16_t>((c &  UNI_HALF_MASK)  + UNI_SUR_LOW_START);

                // We need to increment our counter by one more.
                ++charSize;
            }

            return charSize;
        }

        inline size_t WriteChar(char32_t* dest, char32_t c)
        {
            assert(dest != nullptr);

            *dest = ValidateUTF32Char(c);
            return 1;
        }


        /// A nice optimized case for writing a null-terminator.
        template <typename T>
        size_t WriteNullTerminator(T* dest)
        {
            assert(dest != nullptr);

            *dest = '\0';
            return 1;
        }
    }
}

#endif



/*
 * Copyright 2001-2004 Unicode, Inc.
 *
 * Disclaimer
 *
 * This source code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 *
 * Limitations on Rights to Redistribute This Code
 *
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.
 */
