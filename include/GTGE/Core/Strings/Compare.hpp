// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Strings_Compare
#define GT_Strings_Compare

namespace GT
{
    namespace Strings
    {
        /// Compares two strings in the same manner as strcmp().
        template <typename T, typename U>
        int Compare(const T* str1, ptrdiff_t str1SizeInTs, const U* str2, ptrdiff_t str2SizeInUs = -1)
        {
            int result = 0;

            char32_t c1;
            char32_t c2;

            do
            {
                c1 = NextChar(str1, str1SizeInTs);
                c2 = NextChar(str2, str2SizeInUs);

                result = static_cast<int>(c1) - static_cast<int>(c2);

            } while (result == 0 && c2 != '\0');


            if (result < 0)
            {
                return -1;
            }
            else if (result > 0)
            {
                return +1;
            }

            return 0;
        }

        template <typename T, typename U>
        int Compare(const T* str1, const U* str2)
        {
            return Compare(str1, -1, str2, -1);
        }


        /// Does a fast comparison of the two given strings. This is will not work properly for doing proper alphabetical sorting.
        template <typename T>
        int FastCompare(const T* str1, const T* str2)
        {
            while (*str1 != T(0) || *str2 != T(0))
            {
                if (*str1 != *str2)
                {
                    if (*str1 < *str2)
                    {
                        return -1;
                    }
                    else
                    {
                        return +1;
                    }
                }
                
                ++str1;
                ++str2;
            }

            return 0;
        }

        template <> inline int FastCompare(const char* str1, const char* str2)
        {
            return std::strcmp(str1, str2);
        }

        template <typename T>
        int FastCompare(const T* str1, ptrdiff_t str1SizeInTs, const T* str2, ptrdiff_t str2SizeInTs = -1)
        {
            if (str1SizeInTs == -1 && str2SizeInTs == -1)
            {
                return FastCompare(str1, str2);
            }
            else
            {
                if (str1SizeInTs >= 0 && str2SizeInTs >= 0)
                {
                    if (str1SizeInTs < str2SizeInTs)
                    {
                        return -1;
                    }
                    else if (str1SizeInTs > str2SizeInTs)
                    {
                        return +1;
                    }
                }
                

                int result = 0;

                T c1;
                T c2;

                do
                {
                    // str1
                    if (str1SizeInTs == -1)
                    {
                        c1 = *str1++;
                    }
                    else if (str1SizeInTs > 0)
                    {
                        c1 = *str1++;
                        str1SizeInTs -= 1;
                    }
                    else
                    {
                        c1 = T(0);
                    }


                    // str2
                    if (str2SizeInTs == -1)
                    {
                        c2 = *str2++;
                    }
                    else if (str2SizeInTs > 0)
                    {
                        c2 = *str2++;
                        str2SizeInTs -= 1;
                    }
                    else
                    {
                        c2 = T(0);
                    }


                    result = static_cast<int>(c1) - static_cast<int>(c2);

                } while (result == 0 && c2 != T(0));

            
                if (result < 0)
                {
                    return -1;
                }
                else if (result > 0)
                {
                    return +1;
                }

                return 0;
            }
        }
    }
}


#endif
