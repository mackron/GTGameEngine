// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Math_hpp_
#define __GTLib_Math_hpp_

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cassert>

#define GT_EPSILON_F          0.00001f
#define GT_EPSILON_D          0.00000000001

// Some pi's.
#define GT_PI                 3.141592653589793238462643383279f
#define GT_2PI                6.283185307179586476925286766559f
#define GT_PI_D               3.141592653589793238462643383279
#define GT_2PI_D              6.283185307179586476925286766559

namespace GTLib
{
    template <typename T>
    T Min(T x, T y)
    {
        return (x < y) ? x : y;
    }

    template <typename T>
    T Max(T x, T y)
    {
        return (x > y) ? x : y;
    }

    template <typename T>
    T Clamp(T x, T low, T high)
    {
        if (x > high)
        {
            return high;
        }

        if (x < low)
        {
            return low;
        }

        return x;
    }

    template <typename T>
    T Abs(T value)
    {
        if (value < static_cast<T>(0))
        {
            return -value;
        }

        return value;
    }


    inline unsigned int NextPowerOf2(unsigned int value)
    {
        --value;

        value = (value >> 1)  | value;
        value = (value >> 2)  | value;
        value = (value >> 4)  | value;
        value = (value >> 8)  | value;
        value = (value >> 16) | value;
        
        return value + 1;
    }
    inline int NextPowerOf2(int value)
    {
        return static_cast<int>(NextPowerOf2(static_cast<unsigned int>(value)));
    }


    static const uint32_t DeBruijnBitPosition[32] = 
    {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4146)   // Unsigned is still unsigned after negation.
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
    inline uint32_t NextBitIndex(uint32_t value)
    {
        return DeBruijnBitPosition[((uint32_t)((value & -value) * 0x077CB531U)) >> 27];
    }
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
#endif


    /// Retrieves the number of set bits in the given bit field.
    ///
    /// @remarks
    ///     This is ripped straight from Wikipedia: http://en.wikipedia.org/wiki/Hamming_weight
    inline int CountSetBits(uint64_t x)
    {
        const uint64_t m1  = 0x5555555555555555;
        const uint64_t m2  = 0x3333333333333333;
        const uint64_t m4  = 0x0f0f0f0f0f0f0f0f;
        const uint64_t h01 = 0x0101010101010101;
        
        x -= (x >> 1) & m1;
        x  = (x & m2) + ((x >> 2) & m2);
        x  = (x + (x >> 4)) & m4;
        return (x * h01) >> 56;
    }



    /// Rounds a float to the nearest integer.
    inline float Round(float value)
    {
        return (value > 0.0f) ? std::floor(value + 0.5f) : std::ceil(value - 0.5f);
    }

    /// Rounds a double to the nearest integer.
    inline double Round(double value)
    {
        return (value > 0.0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
    }
    
    /// Rounds up to the nearest multiple of a number that is a power of 2.
    inline unsigned int RountUpToMultipleOfPowerOf2(unsigned int value, unsigned int multiple)
    {
        // assert multiple is a power of 2.
        return (value + multiple - 1) & ~(multiple - 1);
    }

    /// Rounds up to the nearest multiple of 8.
    inline unsigned int RoundUpToMultipleOf8(unsigned int value)
    {
        return RountUpToMultipleOfPowerOf2(value, 8U);
    }

    /// Rounds up to the nearest multiple of 2.
    inline unsigned int RoundUpToMultipleOf2(unsigned int value)
    {
        return RountUpToMultipleOfPowerOf2(value, 2U);
    }

    
    /// Builds a 32-bit hash representation of the given string using FNV-1a.
    inline uint32_t Hash(const char* str, ptrdiff_t strLength = -1)
    {
        uint32_t hash = 2166136261UL;
        
        if (strLength == -1)
        {
            while (*str)
            {
                hash = (hash ^ *str++) * 0x01000193;
            }
        }
        else
        {
            const ptrdiff_t chunkCount = strLength / 4;
            
            for (ptrdiff_t i = 0; i < chunkCount; ++i)
            {
                hash = (hash ^ str[0]) * 0x01000193;
                hash = (hash ^ str[1]) * 0x01000193;
                hash = (hash ^ str[2]) * 0x01000193;
                hash = (hash ^ str[3]) * 0x01000193;
                str += 4;
            }
            
            switch (strLength % 4)
            {
            case 3:
                {
                    hash = (hash ^ str[0]) * 0x01000193;
                    hash = (hash ^ str[1]) * 0x01000193;
                    hash = (hash ^ str[2]) * 0x01000193;
                    break;
                }
            
            case 2:
                {
                    hash = (hash ^ str[0]) * 0x01000193;
                    hash = (hash ^ str[1]) * 0x01000193;
                    break;
                }
                
            case 1:
                {
                    hash = (hash ^ str[0]) * 0x01000193;
                    break;
                }
                
            default: break;
            }
        }
        
        return hash;

        
        /*
        uint32_t hash = 5381;
        
        if (strLength == -1)
        {
            while (*str)
            {
                hash = ((hash << 5) + hash) ^ *str++;
            }
        }
        else
        {
            while (strLength > 3)
            {
                hash = ((hash << 5) + hash) ^ str[0];
                hash = ((hash << 5) + hash) ^ str[1];
                hash = ((hash << 5) + hash) ^ str[2];
                hash = ((hash << 5) + hash) ^ str[3];
                
                str       += 4;
                strLength -= 4;
            }
            
            switch (strLength)
            {
            case 3:
                {
                    hash = ((hash << 5) + hash) ^ str[0];
                    hash = ((hash << 5) + hash) ^ str[1];
                    hash = ((hash << 5) + hash) ^ str[2];
                    break;
                }
                
            case 2:
                {
                    hash = ((hash << 5) + hash) ^ str[0];
                    hash = ((hash << 5) + hash) ^ str[1];
                    break;
                }
                
            case 1:
                {
                    hash = ((hash << 5) + hash) ^ str[0];
                    break;
                }
                
            default: break;
            }
        }
        

        return hash;
        */
    }
}

#endif
