// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/Strings/NextChar.hpp>

namespace GT
{
    namespace Strings
    {
        char32_t NextCharImplementation(const char* &str, ptrdiff_t &strSizeInTs)
        {
            // We can't dereference if there are no bytes.
            if (str && strSizeInTs != 0)
            {
                // We'll do an optimized case when the character is only a single byte in length.
                const unsigned char firstByte = (unsigned char)*str;
                if (firstByte != '\0')
                {
                    // We're not null terminated, so we can move by at least 1 byte.
                    ++str;

                    if (firstByte < 0xC0)   // 0xC0 = 192
                    {
                        // It's only a single byte in length.
                        --strSizeInTs;
                        return firstByte;
                    }
                    else
                    {
                        // More than 1 byte - slightly slower here. Remember that 'str' is sitting on it's second byte right now...
                        char32_t ch = firstByte;

                        if (firstByte < 0xE0 && strSizeInTs >= 2)
                        {
                            ch <<= 6;
                            ch += (unsigned char)*str++;
                            ch -= 0x00003080UL;

                            strSizeInTs -= 2;
                        }
                        else if (firstByte < 0xF0 && strSizeInTs >= 3)
                        {
                            ch <<= 6;
                            ch += (unsigned char)*str++; ch <<= 6;
                            ch += (unsigned char)*str++;
                            ch -= 0x000E2080UL;

                            strSizeInTs -= 3;
                        }
                        else if (firstByte < 0xF8 && strSizeInTs >= 4)
                        {
                            ch <<= 6;
                            ch += (unsigned char)*str++; ch <<= 6;
                            ch += (unsigned char)*str++; ch <<= 6;
                            ch += (unsigned char)*str++;
                            ch -= 0x03C82080UL;

                            strSizeInTs -= 4;
                        }
                        else
                        {
                            // Anything else is invalid. Return null.
                            return '\0';
                        }

                        return ch;
                    }
                }
                else
                {
                    // We've hit a null terminator. We just return without modifying the string.
                    return firstByte;
                }
            }

            return '\0';
        }


        char32_t NextCharImplementation(const char16_t *&str, ptrdiff_t &strSizeInTs)
        {
            if (str != nullptr && strSizeInTs != 0)
            {
                // The character that we'll eventually be returning.
                char32_t ch = *str;

                // Don't do anything if we're at a null-terminator...
                if (ch != '\0')
                {
                    ++str;
                    --strSizeInTs;

                    // We need to check if we have a surrogate pair. If we do, we need to do a conversion.
                    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_START)
                    {
                        // Grab our next 16-bits. This should be the low surrogate. We are not performing an explicit check here.
                        char32_t ch2 = *str;
                        ch = ((ch - UNI_SUR_HIGH_START) << UNI_HALF_SHIFT) + (ch2 - UNI_SUR_LOW_START) + UNI_HALF_BASE;

                        ++str;
                        --strSizeInTs;
                    }
                }

                return ch;
            }

            return '\0';
        }

        char32_t NextCharImplementation(const char32_t* &str, ptrdiff_t &strSizeInTs)
        {
            if (str != nullptr && strSizeInTs != 0)
            {
                char32_t c = *str;

                if (c != '\0')
                {
                    // We're not null terminated, so we move the string forward.
                    ++str;
                    --strSizeInTs;
                }

                return c;
            }

            return '\0';
        }


        char32_t NextCharImplementation(const wchar_t* &str, ptrdiff_t &strSizeInTs)
        {
#if defined(_WIN32)
            return NextCharImplementation(reinterpret_cast<const char16_t* &>(str), strSizeInTs);
#else
            return NextCharImplementation(reinterpret_cast<const char32_t* &>(str), strSizeInTs);
#endif
        }
    }
}
