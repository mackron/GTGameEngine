// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Math
#define GT_Math

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
        return DeBruijnBitPosition[static_cast<uint32_t>((value & -value) * 0x077CB531U) >> 27];
    }
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
#endif



    /// This 64-bit bit scan is taken from https://chessprogramming.wikispaces.com/BitScan.
    ///
    /// There is a CPU intrinsic for this called _BitScanForward64 which may be useful if this implementation ever comes
    /// up during profiling.
    static const int g_BitScanIndex64[64] =
    {
        0, 47,  1, 56, 48, 27,  2, 60,
       57, 49, 41, 37, 28, 16,  3, 61,
       54, 58, 35, 52, 50, 42, 21, 44,
       38, 32, 29, 23, 17, 11,  4, 62,
       46, 55, 26, 59, 40, 36, 15, 53,
       34, 51, 20, 43, 31, 22, 10, 45,
       25, 39, 14, 33, 19, 30,  9, 24,
       13, 18,  8, 12,  7,  6,  5, 63
    };

    /**
     * bitScanForward
     * @author Kim Walisch (2012)
     * @param bb bitboard to scan
     * @pre bb != 0
     * @return index (0..63) of least significant one bit
     */
    inline int BitScanForward(uint64_t bb)
    {
        assert (bb != 0);

        return g_BitScanIndex64[((bb ^ (bb-1)) * uint64_t(0x03f79d71b4cb0a89)) >> 58];
    }




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
