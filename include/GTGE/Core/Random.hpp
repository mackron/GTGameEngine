// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Random
#define GT_Random

#include "Math.hpp"
#include "Limits.hpp"
#include "Timing.hpp"
#include <ctime>

#define GT_LCG_M    4294967296
#define GT_LCG_A    1103515245
#define GT_LCG_C    12345

// We get -Wtype-limit warnings when using Next() with an unsigned value. We will silence these.
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace GT
{
    /// Class representing a 32-bit Linear congruential generator (LCG).
    class RandomLCG
    {
    public:

        /// Default constructor. This will seed using the current time.
        RandomLCG()
            : x(static_cast<int32_t>(Timing::GetTimeInMilliseconds()))
        {
        }

        /// Copy constructor.
        RandomLCG(const RandomLCG &other)
            : x(other.x)
        {
        }

        /// Constructor.
        RandomLCG(int32_t seed)
            : x(seed == 0 ? GT_LCG_C : seed)
        {
        }


        /// Seeds the random number generator.
        void Seed(int32_t seed)
        {
            if (seed == 0)
            {
                this->x = GT_LCG_C;
            }
            else
            {
                this->x = seed;
            }
        }

        void Seed()
        {
            this->Seed(static_cast<int32_t>(Timing::GetTimeInMilliseconds()));
        }


    public:

        /// Generates a number between 0 and 1.
        ///
        /// @remarks
        ///     The <T> template parameter should be a float or double.
        template <typename T>
        T Next()
        {
            this->x = (GT_LCG_A * this->x + GT_LCG_C) % GT_LCG_M;

            uint32_t uX = static_cast<uint32_t>(this->x);
            return static_cast<T>(uX + Abs(MinValue<int32_t>())) / MaxValue<uint32_t>();
        }

        /// Generates a number in the bounds of <min> and <max>
        ///
        /// @param min [in] The minimum value.
        /// @param max [in] The maximum value.
        ///
        /// @return The next random value, bound in the range of <min> and <max>.
        template <typename T>
        T Next(T min, T max)
        {
            if (min < 0)
            {
                --min;
            }
            if (max > 0)
            {
                ++max;
            }

            return static_cast<T>(this->Next<float>(static_cast<float>(min), static_cast<float>(max)));
        }


        /// Generates a random number in the area of a circle of the given radius.
        template <typename T>
        void NextCircle(T radius, T &x, T &y)
        {
            do
            {
                x = this->Next<T>(-radius, radius);
                y = this->Next<T>(-radius, radius);

            }while (x*x + y*y > radius*radius);


#if 0
            radius = std::sqrt(radius);

            T a = this->Next<T>(T(0.0), T(3.14159265 * 2.0));
            x = std::cos(a) * radius;
            y = std::sin(a) * radius;
#endif
        }

        /// Generates a random number in the area of a sphere.
        template <typename T>
        void NextSphere(T radius, T &x, T &y, T &z)
        {
            do
            {
                x = this->Next<T>(-radius, radius);
                y = this->Next<T>(-radius, radius);
                z = this->Next<T>(-radius, radius);

            }while (x*x + y*y + z*z > radius*radius);
        }

        /// Generates a random number in the area of a box.
        template <typename T>
        void NextBox(T xLength, T yLength, T zLength, T &xOut, T &yOut, T &zOut)
        {
            T halfX = xLength * T(0.5);
            T halfY = yLength * T(0.5);
            T halfZ = zLength * T(0.5);

            xOut = this->Next<T>(-halfX, halfX);
            yOut = this->Next<T>(-halfY, halfY);
            zOut = this->Next<T>(-halfZ, halfZ);
        }


    private:

        /// The last number to be generated.
        int32_t x;
    };

    template <> inline float RandomLCG::Next(float min, float max)
    {
        return this->Next<float>() * (max - min) + min;
    }
    template <> inline double RandomLCG::Next(double min, double max)
    {
        return this->Next<double>() * (max - min) + min;
    }


    /// A typedef helper.
    typedef RandomLCG Random;
}

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#endif
