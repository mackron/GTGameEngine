// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.
//
// Much of the code is based on the GLM project: http://glm.g-truc.net/0.9.6/index.html

#ifndef GT_Engine_Math
#define GT_Engine_Math

#include "Config.hpp"
#include <GTLib/Math.hpp>
#include <math.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#endif


// Technically we should be checking
#define GT_USE_SSE2

#if defined(GT_USE_SSE2)

#if defined(__MINGW32__)
#include <intrin.h>
#endif

#include <emmintrin.h>
#endif

namespace GT
{
    /// Converts the given value in degrees to radians.
    ///
    /// @param degrees [in] The degrees to convert to radians.
    inline float Radians(float degrees)
    {
        return degrees * static_cast<float>(0.01745329251994329576923690768489);
    }

    /// Converts the given value in radians to degrees.
    ///
    /// @param radians [in] The radians to convert to degrees.
    inline float Degrees(float radians)
    {
        return radians * static_cast<float>(57.295779513082320876798154814105);
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



    //////////////////////////////////
    // vec2

    struct vec2;

    inline vec2 operator+(const vec2 &v0, const vec2 &v1);
    inline vec2 operator+(const vec2 &v0, float v1);
    inline vec2 operator+(float v0, const vec2 &v1);

    inline vec2 operator-(const vec2 &v0, const vec2 &v1);
    inline vec2 operator-(const vec2 &v0, float v1);
    inline vec2 operator-(float v0, const vec2 &v1);

    inline vec2 operator*(const vec2 &v0, const vec2 &v1);
    inline vec2 operator*(const vec2 &v0, float v1);
    inline vec2 operator*(float v0, const vec2 &v1);

    inline vec2 operator/(const vec2 &v0, const vec2 &v1);
    inline vec2 operator/(const vec2 &v0, float v1);
    inline vec2 operator/(float v0, const vec2 &v1);
    
    /// Structure representing a vec2/float2.
    struct vec2
    {
        float x;
        float y;


        vec2()
        {
        }

        vec2(const vec2 &other)
            : x(other.x), y(other.y)
        {
        }

        vec2(float _x, float _y)
            : x(_x), y(_y)
        {
        }


        vec2 & operator=(const vec2 &other)
        {
            x = other.x;
            y = other.y;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        vec2 & operator+=(const vec2 &other)
        {
            return (*this) = (*this) + other;
        }
        vec2 & operator+=(float other)
        {
            return (*this) = (*this) + other;
        }

        vec2 & operator-=(const vec2 &other)
        {
            return (*this) = (*this) - other;
        }
        vec2 & operator-=(float other)
        {
            return (*this) = (*this) - other;
        }

        vec2 & operator*=(const vec2 &other)
        {
            return (*this) = (*this) * other;
        }
        vec2 & operator*=(float other)
        {
            return (*this) = (*this) * other;
        }

        vec2 & operator/=(const vec2 &other)
        {
            return (*this) = (*this) / other;
        }
        vec2 & operator/=(float other)
        {
            return (*this) = (*this) / other;
        }


        vec2 operator-()
        {
            return vec2(-x, -y);
        }


        ///////////////////////////////////
        // Static Members

        static const vec2 zero;
        static const vec2 one;
        static const vec2 posx;
        static const vec2 posy;
        static const vec2 negx;
        static const vec2 negy;
    };

    /// Performs an addition on the given vector.
    inline vec2 add(const vec2 &v0, const vec2 &v1)
    {
        return vec2(v0.x + v1.x, v0.y + v1.y);
    }
    inline vec2 add(const vec2 &v0, float v1)
    {
        return vec2(v0.x + v1, v0.y + v1);
    }


    /// Performs a subtraction on the given vector.
    inline vec2 sub(const vec2 &v0, const vec2 &v1)
    {
        return vec2(v0.x - v1.x, v0.y - v1.y);
    }
    inline vec2 sub(const vec2 &v0, float v1)
    {
        return vec2(v0.x - v1, v0.y - v1);
    }
    inline vec2 sub(float v0, const vec2 &v1)
    {
        return vec2(v0 - v1.x, v0 - v1.y);
    }


    /// Performs a multiplication on the given vector.
    inline vec2 mul(const vec2 &v0, const vec2 &v1)
    {
        return vec2(v0.x * v1.x, v0.y * v1.y);
    }
    inline vec2 mul(const vec2 &v0, float v1)
    {
        return vec2(v0.x * v1, v0.y * v1);
    }


    /// Performs a division on the given vector.
    inline vec2 div(const vec2 &v0, const vec2 &v1)
    {
        return vec2(v0.x / v1.x, v0.y / v1.y);
    }
    inline vec2 div(const vec2 &v0, float v1)
    {
        return vec2(v0.x / v1, v0.y / v1);
    }
    inline vec2 div(float v0, const vec2 &v1)
    {
        return vec2(v0 / v1.x, v0 / v1.y);
    }


    /// Calculates the squared length of a vector.
    inline float length2(const vec2 &v)
    {
        return v.x*v.x + v.y*v.y;
    }

    /// Calculates the length of a vector.
    inline float length(const vec2 &v)
    {
        return ::sqrtf(length2(v));
    }

    /// Returns the normalization of the given vector.
    inline vec2 normalize(const vec2 &v)
    {
        return div(v, length(v));
    }


    /// Performs a dot product between two vectors.
    inline float dot(const vec2 &v0, const vec2 &v1)
    {
        return v0.x*v1.x + v0.y*v1.y;
    }


    /// Performs a linear interpolation between two vectors.
    inline vec2 lerp(const vec2 &v0, const vec2 &v1, float t)
    {
        return v0*(1.0f - t) + v1*t;
    }



    //////////////////////////
    // Operator Overloads

    inline vec2 operator+(const vec2 &v0, const vec2 &v1)
    {
        return add(v0, v1);
    }
    inline vec2 operator+(const vec2 &v0, float v1)
    {
        return add(v0, v1);
    }
    inline vec2 operator+(float v0, const vec2 &v1)
    {
        return add(v1, v0);
    }

    inline vec2 operator-(const vec2 &v0, const vec2 &v1)
    {
        return sub(v0, v1);
    }
    inline vec2 operator-(const vec2 &v0, float v1)
    {
        return sub(v0, v1);
    }
    inline vec2 operator-(float v0, const vec2 &v1)
    {
        return sub(v0, v1);
    }

    inline vec2 operator*(const vec2 &v0, const vec2 &v1)
    {
        return mul(v0, v1);
    }
    inline vec2 operator*(const vec2 &v0, float v1)
    {
        return mul(v0, v1);
    }
    inline vec2 operator*(float v0, const vec2 &v1)
    {
        return mul(v1, v0);
    }

    inline vec2 operator/(const vec2 &v0, const vec2 &v1)
    {
        return div(v0, v1);
    }
    inline vec2 operator/(const vec2 &v0, float v1)
    {
        return div(v0, v1);
    }
    inline vec2 operator/(float v0, const vec2 &v1)
    {
        return div(v0, v1);
    }

    inline bool operator==(const vec2 &v0, const vec2 &v1)
    {
        return v0.x == v1.x && v0.y == v1.y;
    }
    inline bool operator!=(const vec2 &v0, const vec2 &v1)
    {
        return !(v0 == v1);
    }



    //////////////////////////////////
    // vec3

    struct vec3;

    inline vec3 operator+(const vec3 &v0, const vec3 &v1);
    inline vec3 operator+(const vec3 &v0, float v1);
    inline vec3 operator+(float v0, const vec3 &v1);

    inline vec3 operator-(const vec3 &v0, const vec3 &v1);
    inline vec3 operator-(const vec3 &v0, float v1);
    inline vec3 operator-(float v0, const vec3 &v1);

    inline vec3 operator*(const vec3 &v0, const vec3 &v1);
    inline vec3 operator*(const vec3 &v0, float v1);
    inline vec3 operator*(float v0, const vec3 &v1);

    inline vec3 operator/(const vec3 &v0, const vec3 &v1);
    inline vec3 operator/(const vec3 &v0, float v1);
    inline vec3 operator/(float v0, const vec3 &v1);

    /// Structure representing a vec3/float3.
    struct vec3
    {
        float x;
        float y;
        float z;


        vec3()
        {
        }

        vec3(const vec3 &other)
            : x(other.x), y(other.y), z(other.z)
        {
        }

        vec3(float _x, float _y, float _z)
            : x(_x), y(_y), z(_z)
        {
        }


        vec3 & operator=(const vec3 &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        vec3 & operator+=(const vec3 &other)
        {
            return (*this) = (*this) + other;
        }
        vec3 & operator+=(float other)
        {
            return (*this) = (*this) + other;
        }

        vec3 & operator-=(const vec3 &other)
        {
            return (*this) = (*this) - other;
        }
        vec3 & operator-=(float other)
        {
            return (*this) = (*this) - other;
        }

        vec3 & operator*=(const vec3 &other)
        {
            return (*this) = (*this) * other;
        }
        vec3 & operator*=(float other)
        {
            return (*this) = (*this) * other;
        }

        vec3 & operator/=(const vec3 &other)
        {
            return (*this) = (*this) / other;
        }
        vec3 & operator/=(float other)
        {
            return (*this) = (*this) / other;
        }


        vec3 operator-()
        {
            return vec3(-x, -y, -z);
        }


        ///////////////////////////////////
        // Static Members

        static const vec3 zero;
        static const vec3 one;
        static const vec3 posx;
        static const vec3 posy;
        static const vec3 posz;
        static const vec3 negx;
        static const vec3 negy;
        static const vec3 negz;
    };

    /// Performs an addition on the given vector.
    inline vec3 add(const vec3 &v0, const vec3 &v1)
    {
        return vec3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
    }
    inline vec3 add(const vec3 &v0, float v1)
    {
        return vec3(v0.x + v1, v0.y + v1, v0.z + v1);
    }


    /// Performs a subtraction on the given vector.
    inline vec3 sub(const vec3 &v0, const vec3 &v1)
    {
        return vec3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
    }
    inline vec3 sub(const vec3 &v0, float v1)
    {
        return vec3(v0.x - v1, v0.y - v1, v0.z - v1);
    }
    inline vec3 sub(float v0, const vec3 &v1)
    {
        return vec3(v0 - v1.x, v0 - v1.y, v0 - v1.z);
    }


    /// Performs a multiplication on the given vector.
    inline vec3 mul(const vec3 &v0, const vec3 &v1)
    {
        return vec3(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
    }
    inline vec3 mul(const vec3 &v0, float v1)
    {
        return vec3(v0.x * v1, v0.y * v1, v0.z * v1);
    }


    /// Performs a division on the given vector.
    inline vec3 div(const vec3 &v0, const vec3 &v1)
    {
        return vec3(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z);
    }
    inline vec3 div(const vec3 &v0, float v1)
    {
        return vec3(v0.x / v1, v0.y / v1, v0.z / v1);
    }
    inline vec3 div(float v0, const vec3 &v1)
    {
        return vec3(v0 / v1.x, v0 / v1.y, v0 / v1.z);
    }


    /// Calculates the squared length of a vector.
    inline float length2(const vec3 &v)
    {
        return v.x*v.x + v.y*v.y + v.z*v.z;
    }

    /// Calculates the length of a vector.
    inline float length(const vec3 &v)
    {
        return ::sqrtf(length2(v));
    }

    /// Returns the normalization of the given vector.
    inline vec3 normalize(const vec3 &v)
    {
        return div(v, length(v));
    }


    /// Performs a dot product between two vectors.
    inline float dot(const vec3 &v0, const vec3 &v1)
    {
        return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z;
    }


    /// Performs a linear interpolation between two vectors.
    inline vec3 lerp(const vec3 &v0, const vec3 &v1, float t)
    {
        return v0*(1.0f - t) + v1*t;
    }


    /// Performs a cross product between two vectors.
    ///
    /// The W component is ignored and set to 0 in the returned vector.
    inline vec3 cross(const vec3 &v0, const vec3 &v1)
    {
        return vec3(
            v0.y*v1.z - v0.z*v1.y,
            v0.z*v1.x - v0.x*v1.z,
            v0.x*v1.y - v0.y*v1.x
        );
    }


    //////////////////////////
    // Operator Overloads

    inline vec3 operator+(const vec3 &v0, const vec3 &v1)
    {
        return add(v0, v1);
    }
    inline vec3 operator+(const vec3 &v0, float v1)
    {
        return add(v0, v1);
    }
    inline vec3 operator+(float v0, const vec3 &v1)
    {
        return add(v1, v0);
    }

    inline vec3 operator-(const vec3 &v0, const vec3 &v1)
    {
        return sub(v0, v1);
    }
    inline vec3 operator-(const vec3 &v0, float v1)
    {
        return sub(v0, v1);
    }
    inline vec3 operator-(float v0, const vec3 &v1)
    {
        return sub(v0, v1);
    }

    inline vec3 operator*(const vec3 &v0, const vec3 &v1)
    {
        return mul(v0, v1);
    }
    inline vec3 operator*(const vec3 &v0, float v1)
    {
        return mul(v0, v1);
    }
    inline vec3 operator*(float v0, const vec3 &v1)
    {
        return mul(v1, v0);
    }

    inline vec3 operator/(const vec3 &v0, const vec3 &v1)
    {
        return div(v0, v1);
    }
    inline vec3 operator/(const vec3 &v0, float v1)
    {
        return div(v0, v1);
    }
    inline vec3 operator/(float v0, const vec3 &v1)
    {
        return div(v0, v1);
    }

    inline bool operator==(const vec3 &v0, const vec3 &v1)
    {
        return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
    }
    inline bool operator!=(const vec3 &v0, const vec3 &v1)
    {
        return !(v0 == v1);
    }



    //////////////////////////////////
    // vec4

    struct vec4;

    inline vec4 operator+(const vec4 &v0, const vec4 &v1);
    inline vec4 operator+(const vec4 &v0, float v1);
    inline vec4 operator+(float v0, const vec4 &v1);

    inline vec4 operator-(const vec4 &v0, const vec4 &v1);
    inline vec4 operator-(const vec4 &v0, float v1);
    inline vec4 operator-(float v0, const vec4 &v1);

    inline vec4 operator*(const vec4 &v0, const vec4 &v1);
    inline vec4 operator*(const vec4 &v0, float v1);
    inline vec4 operator*(float v0, const vec4 &v1);

    inline vec4 operator/(const vec4 &v0, const vec4 &v1);
    inline vec4 operator/(const vec4 &v0, float v1);
    inline vec4 operator/(float v0, const vec4 &v1);

    /// Structure representing a vec4/float4.
    struct alignas(16) vec4
    {
        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

#if defined(GT_USE_SSE2)
            __m128 xyzw;
#endif
        };


        vec4()
        {
        }

        vec4(const vec4 &other)
            : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        vec4(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }

        vec4(const vec3 &xyz, float _w)
            : x(xyz.x), y(xyz.y), z(xyz.z), w(_w)
        {
        }


        vec4 & operator=(const vec4 &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        vec4 & operator+=(const vec4 &other)
        {
            return (*this) = (*this) + other;
        }
        vec4 & operator+=(float other)
        {
            return (*this) = (*this) + other;
        }

        vec4 & operator-=(const vec4 &other)
        {
            return (*this) = (*this) - other;
        }
        vec4 & operator-=(float other)
        {
            return (*this) = (*this) - other;
        }

        vec4 & operator*=(const vec4 &other)
        {
            return (*this) = (*this) * other;
        }
        vec4 & operator*=(float other)
        {
            return (*this) = (*this) * other;
        }

        vec4 & operator/=(const vec4 &other)
        {
            return (*this) = (*this) / other;
        }
        vec4 & operator/=(float other)
        {
            return (*this) = (*this) / other;
        }


        vec4 operator-()
        {
            return vec4(-x, -y, -z, -w);
        }


        ///////////////////////////////////
        // Static Members

        static const vec4 zero;
        static const vec4 one;
        static const vec4 posx;
        static const vec4 posy;
        static const vec4 posz;
        static const vec4 posw;
        static const vec4 negx;
        static const vec4 negy;
        static const vec4 negz;
        static const vec4 negw;
    };

    /// Performs an addition on the given vector.
    inline vec4 add(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
    }
    inline vec4 add(const vec4 &v0, float v1)
    {
        return vec4(v0.x + v1, v0.y + v1, v0.z + v1, v0.w + v1);
    }


    /// Performs a subtraction on the given vector.
    inline vec4 sub(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
    }
    inline vec4 sub(const vec4 &v0, float v1)
    {
        return vec4(v0.x - v1, v0.y - v1, v0.z - v1, v0.w - v1);
    }
    inline vec4 sub(float v0, const vec4 &v1)
    {
        return vec4(v0 - v1.x, v0 - v1.y, v0 - v1.z, v0 - v1.w);
    }


    /// Performs a multiplication on the given vector.
    inline vec4 mul(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
    }
    inline vec4 mul(const vec4 &v0, float v1)
    {
        return vec4(v0.x * v1, v0.y * v1, v0.z * v1, v0.w * v1);
    }


    /// Performs a division on the given vector.
    inline vec4 div(const vec4 &v0, const vec4 &v1)
    {
        return vec4(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z, v0.w / v1.w);
    }
    inline vec4 div(const vec4 &v0, float v1)
    {
        return vec4(v0.x / v1, v0.y / v1, v0.z / v1, v0.w / v1);
    }
    inline vec4 div(float v0, const vec4 &v1)
    {
        return vec4(v0 / v1.x, v0 / v1.y, v0 / v1.z, v0 / v1.w);
    }


    /// Calculates the squared length of a vector.
    inline float length2(const vec4 &v)
    {
        return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
    }

    /// Calculates the length of a vector.
    inline float length(const vec4 &v)
    {
        return ::sqrtf(length2(v));
    }

    /// Returns the normalization of the given vector.
    inline vec4 normalize(const vec4 &v)
    {
        return div(v, length(v));
    }


    /// Performs a dot product between two vectors.
    inline float dot(const vec4 &v0, const vec4 &v1)
    {
        return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
    }


    /// Performs a linear interpolation between two vectors.
    inline vec4 lerp(const vec4 &v0, const vec4 &v1, float t)
    {
        return v0*(1.0f - t) + v1*t;
    }


    /// Performs a cross product between two vectors.
    ///
    /// The W component is ignored and set to 0 in the returned vector.
    inline vec4 cross(const vec4 &v0, const vec4 &v1)
    {
        return vec4(
            v0.y*v1.z - v0.z*v1.y,
            v0.z*v1.x - v0.x*v1.z,
            v0.x*v1.y - v0.y*v1.x,
            0.0f);
    }


    //////////////////////////
    // Operator Overloads

    inline vec4 operator+(const vec4 &v0, const vec4 &v1)
    {
        return add(v0, v1);
    }
    inline vec4 operator+(const vec4 &v0, float v1)
    {
        return add(v0, v1);
    }
    inline vec4 operator+(float v0, const vec4 &v1)
    {
        return add(v1, v0);
    }

    inline vec4 operator-(const vec4 &v0, const vec4 &v1)
    {
        return sub(v0, v1);
    }
    inline vec4 operator-(const vec4 &v0, float v1)
    {
        return sub(v0, v1);
    }
    inline vec4 operator-(float v0, const vec4 &v1)
    {
        return sub(v0, v1);
    }

    inline vec4 operator*(const vec4 &v0, const vec4 &v1)
    {
        return mul(v0, v1);
    }
    inline vec4 operator*(const vec4 &v0, float v1)
    {
        return mul(v0, v1);
    }
    inline vec4 operator*(float v0, const vec4 &v1)
    {
        return mul(v1, v0);
    }

    inline vec4 operator/(const vec4 &v0, const vec4 &v1)
    {
        return div(v0, v1);
    }
    inline vec4 operator/(const vec4 &v0, float v1)
    {
        return div(v0, v1);
    }
    inline vec4 operator/(float v0, const vec4 &v1)
    {
        return div(v0, v1);
    }

    inline bool operator==(const vec4 &v0, const vec4 &v1)
    {
        return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z && v0.w == v1.w;
    }
    inline bool operator!=(const vec4 &v0, const vec4 &v1)
    {
        return !(v0 == v1);
    }




    //////////////////////////////////
    // quat

    /// Structure representing a quaternion.
    struct alignas(16) quat
    {
        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

#if defined(GT_USE_SSE2)
            __m128 xyzw;
#endif
        };


        quat()
        {
        }

        quat(const quat &other)
            : x(other.x), y(other.y), z(other.z), w(other.w)
        {
        }

        quat(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }


        quat & operator=(const quat &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            return *this;
        }

        float & operator[](int index)
        {
            return *(&x + index);
        }

        float operator[](int index) const
        {
            return *(&x + index);
        }


        /// Creates a quaternion from an angle rotated around the given axis.
        static quat angle_axis(float angle, float axisX, float axisY, float axisZ)
        {
            const float a = angle;
		    const float s = sinf(a * 0.5f);

            return quat(
                axisX * s,
                axisY * s,
                axisZ * s,
                cosf(a * 0.5f)
            );
        }



        ///////////////////////////////////
        // Static Members

        static const quat identity;
    };


    //////////////////////////
    // Operator Overloads

    // quat * quat
    inline quat operator*(const quat &q0, const quat &q1)
    {
        return quat(
            q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y,
            q0.w * q1.y + q0.y * q1.w + q0.z * q1.x - q0.x * q1.z,
            q0.w * q1.z + q0.z * q1.w + q0.x * q1.y - q0.y * q1.x,
            q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z
        );
    }

    // quat * vec4
    inline vec4 operator*(const quat &q, const vec4 &v)
    {
        // TODO: SSE-ify this.

        vec4 qv(q.x, q.y, q.z, 1);
        vec4 uv(cross(qv, v));
        vec4 uuv(cross(qv, uv));

        vec4 result = v + ((uv * q.w) + uuv) * 2.0f;
        result.w = v.w;

        return result;
    }

    // quat * vec3
    inline vec3 operator*(const quat &q, const vec3 &v)
    {
        vec3 qv(q.x, q.y, q.z);
        vec3 uv(cross(qv, v));
        vec3 uuv(cross(qv, uv));

        vec3 result = v + ((uv * q.w) + uuv) * 2.0f;

        return result;
    }


    // quat / float
    inline quat operator/(const quat &q, float s)
    {
        return quat(q.x / s, q.y / s, q.z / s, q.w / s);
    }


    inline bool operator==(const quat &v0, const quat &v1)
    {
        return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z && v0.w == v1.w;
    }
    inline bool operator!=(const quat &v0, const quat &v1)
    {
        return !(v0 == v1);
    }


    inline quat conjugate(const quat &q)
    {
        return quat(-q.x, -q.y, -q.z, q.w);
    }

    inline float dot(const quat &q0, const quat &q1)
    {
        return q0.x*q1.x + q0.y*q1.y + q0.z*q1.z + q0.w*q1.w;
    }

    inline quat inverse(const quat &q)
    {
        return conjugate(q) / dot(q, q);
    }




    //////////////////////////////////
    // mat4

    /// Structure representing a column-major mat4
    struct alignas(16) mat4
    {
        vec4 col0;
        vec4 col1;
        vec4 col2;
        vec4 col3;

        mat4()
            : col0(), col1(), col2(), col3()
        {
        }

        mat4(const mat4 &other)
            : col0(other.col0), col1(other.col1), col2(other.col2), col3(other.col3)
        {
        }

        mat4(const vec4 &_col0, const vec4 &_col1, const vec4 &_col2, const vec4 &_col3)
            : col0(_col0), col1(_col1), col2(_col2), col3(_col3)
        {
        }


        mat4 & operator=(const mat4 &other)
        {
            col0 = other.col0;
            col1 = other.col1;
            col2 = other.col2;
            col3 = other.col3;

            return *this;
        }

        vec4 & operator[](int index)
        {
            return *(&col0 + index);
        }

        vec4 operator[](int index) const
        {
            return *(&col0 + index);
        }


        /// Creates a perspective matrix.
        ///
        /// @param fovy   [in] The field of view.
        /// @param aspect [in] The aspect ratio.
        /// @param znear  [in] The near plane.
        /// @param zfar   [in] The far plane.
        ///
        /// @return The perspective projection matrix.
        static mat4 perspective(float fovy, float aspect, float znear, float zfar)
        {
            const float tanHalfFovy = tanf(fovy * 0.5f);

		    mat4 result(mat4::zero);
            result[0][0] = 1.0f / (aspect * tanHalfFovy);
            result[1][1] = 1.0f / (tanHalfFovy);
            result[2][2] = -(zfar + znear) / (zfar - znear);
            result[2][3] = -1.0f;
            result[3][2] = -(2.0f * zfar * znear) / (zfar - znear);

            return result;
        }

        /// Creates an orthographic projection matrix.
        ///
        /// @param left   [in] The left vertical clipping plane.
        /// @param right  [in] The right vertical clipping plane.
        /// @param bottom [in] The bottom horizontal clipping plane.
        /// @param top    [in] The top horizontal clipping plane.
        /// @param znear  [in] The distance to the near depth clipping plane.
        /// @param zfar   [in] The distance to the far depth clipping plane.
        ///
        /// @return The orthographics projection matrix.
        static mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar)
        {
            mat4 result(mat4::identity);
            result[0][0] =  2.0f / (right - left);
            result[1][1] =  2.0f / (top   - bottom);
            result[2][2] = -2.0f / (zfar  - znear);
            result[3][0] = -(right + left)   / (right - left);
            result[3][1] = -(top   + bottom) / (top   - bottom);
            result[3][2] = -(zfar  + znear)  / (zfar  - znear);

            return result;
        }


        /// Creates a copy of the given matrix, translated by the given amount.
        ///
        /// @param m [in] The matrix to translate.
        /// @param v [in] The translation vector.
        ///
        /// @remarks
        ///     The w component of the translation vector is ignored.
        static mat4 translate(const mat4 &m, const vec4 &v)
        {
            mat4 result(m);
            result[3] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3];

            return result;
        }

        /// Creates a copy of the given matrix, rotated by the given angle and axis.
        ///
        /// @param m     [in] The matrix to rotate.
        /// @param angle [in] The angle to rotate, in radians.
        /// @param axis  [in] The axis to rotate around.
        ///
        /// @remarks
        ///     The w component of the rotation axis is ignored.
        ///     @par
        ///     \c axis should be normalized.
        static mat4 rotate(const mat4 &m, float angle, const vec4 &axis)
        {
            const float a = angle;
		    const float c = cosf(a);
		    const float s = sinf(a);

		    vec4 temp((1.0f - c) * axis);

		    mat4 rotation;
		    rotation[0][0] = c + temp[0] * axis[0];
		    rotation[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
		    rotation[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

		    rotation[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
		    rotation[1][1] = c + temp[1] * axis[1];
		    rotation[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

		    rotation[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
		    rotation[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
		    rotation[2][2] = c + temp[2] * axis[2];

		    mat4 result;
		    result[0] = m[0] * rotation[0][0] + m[1] * rotation[0][1] + m[2] * rotation[0][2];
		    result[1] = m[0] * rotation[1][0] + m[1] * rotation[1][1] + m[2] * rotation[1][2];
		    result[2] = m[0] * rotation[2][0] + m[1] * rotation[2][1] + m[2] * rotation[2][2];
		    result[3] = m[3];

            return result;
        }

        /// Creates a copy of the given matrix, scaled by the given vector.
        ///
        /// @param m [in] The matrix to scale.
        /// @param v [in] The scale vector.
        ///
        /// @remarks
        ///     The w component of the scaling vector is ignored.
        static mat4 scale(const mat4 &m, const vec4 &v)
        {
            mat4 result;
            result[0] = m[0] * v[0];
		    result[1] = m[1] * v[1];
		    result[2] = m[2] * v[2];
		    result[3] = m[3];

		    return result;
        }


        ///////////////////////////////////
        // Static Members

        static const mat4 zero;
        static const mat4 identity;
    };

    //////////////////////////
    // Operator Overloads

    // mat4 * mat4
    inline mat4 operator*(const mat4 &m1, const mat4 &m2)
    {
		return mat4(
            (m1[0] * m2[0][0]) + (m1[1] * m2[0][1]) + (m1[2] * m2[0][2]) + (m1[3] * m2[0][3]),
            (m1[0] * m2[1][0]) + (m1[1] * m2[1][1]) + (m1[2] * m2[1][2]) + (m1[3] * m2[1][3]),
            (m1[0] * m2[2][0]) + (m1[1] * m2[2][1]) + (m1[2] * m2[2][2]) + (m1[3] * m2[2][3]),
            (m1[0] * m2[3][0]) + (m1[1] * m2[3][1]) + (m1[2] * m2[3][2]) + (m1[3] * m2[3][3])
        );
    }

    // mat4 * vec4
    inline vec4 operator*(const mat4 &m, const vec4 &v)
    {
        return vec4(
            m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
			m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
			m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
			m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
        );
    }

    // vec4 * mat4
    inline vec4 operator*(const vec4 &v, const mat4 &m)
    {
        return vec4(
            (m[0][0] * v[0]) + (m[0][1] * v[1]) + (m[0][2] * v[2]) + (m[0][3] * v[3]),
			(m[1][0] * v[0]) + (m[1][1] * v[1]) + (m[1][2] * v[2]) + (m[1][3] * v[3]),
			(m[2][0] * v[0]) + (m[2][1] * v[1]) + (m[2][2] * v[2]) + (m[2][3] * v[3]),
			(m[3][0] * v[0]) + (m[3][1] * v[1]) + (m[3][2] * v[2]) + (m[3][3] * v[3])
        );
    }

    // mat4 * scalar
    inline mat4 operator*(const mat4 &m, float s)
    {
        return mat4(
			m[0] * s,
			m[1] * s,
			m[2] * s,
			m[3] * s
        );
    }


    /// Converts a quaternion to a matrix.
    inline mat4 quat_to_mat4(const quat &q)
    {
        mat4 result;

		float qxx(q.x * q.x);
		float qyy(q.y * q.y);
		float qzz(q.z * q.z);
		float qxz(q.x * q.z);
		float qxy(q.x * q.y);
		float qyz(q.y * q.z);
		float qwx(q.w * q.x);
		float qwy(q.w * q.y);
		float qwz(q.w * q.z);

		result[0][0] = 1.0f - 2.0f * (qyy +  qzz);
		result[0][1] = 2.0f * (qxy + qwz);
		result[0][2] = 2.0f * (qxz - qwy);
        result[0][3] = 0.0f;

		result[1][0] = 2.0f * (qxy - qwz);
		result[1][1] = 1.0f - 2.0f * (qxx +  qzz);
		result[1][2] = 2.0f * (qyz + qwx);
        result[1][3] = 0.0f;

		result[2][0] = 2.0f * (qxz + qwy);
		result[2][1] = 2.0f * (qyz - qwx);
		result[2][2] = 1.0f - 2.0f * (qxx +  qyy);
        result[2][3] = 0.0f;

        result[3][0] = 0.0f;
        result[3][1] = 0.0f;
        result[3][2] = 0.0f;
        result[3][3] = 1.0f;

		return result;
    }

    /// Converts a matrix to a quaternion
    inline quat mat4_to_quat(const mat4 &m)
    {
        float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		float biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		quat result;
		switch(biggestIndex)
		{
		case 0:
            {
                result.w = biggestVal;
			    result.x = (m[1][2] - m[2][1]) * mult;
			    result.y = (m[2][0] - m[0][2]) * mult;
			    result.z = (m[0][1] - m[1][0]) * mult;

			    break;
            }

		case 1:
            {
                result.w = (m[1][2] - m[2][1]) * mult;
			    result.x = biggestVal;
			    result.y = (m[0][1] + m[1][0]) * mult;
			    result.z = (m[2][0] + m[0][2]) * mult;

			    break;
            }

		case 2:
            {
                result.w = (m[2][0] - m[0][2]) * mult;
			    result.x = (m[0][1] + m[1][0]) * mult;
			    result.y = biggestVal;
			    result.z = (m[1][2] + m[2][1]) * mult;
			    break;
            }

		case 3:
            {
                result.w = (m[0][1] - m[1][0]) * mult;
			    result.x = (m[2][0] + m[0][2]) * mult;
			    result.y = (m[1][2] + m[2][1]) * mult;
			    result.z = biggestVal;

			    break;
            }


		default:					// Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
			assert(false);
			break;
		}

		return result;
    }


    /// Calculates the inverse of the given matrix.
    ///
    /// @remarks
    ///     This implementation is taken from GLM:
    inline mat4 inverse(const mat4 &m)
    {
        float Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		float Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
		float Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

		float Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		float Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
		float Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

		float Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		float Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
		float Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

		float Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		float Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
		float Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

		float Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		float Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
		float Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

		float Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		float Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
		float Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

		vec4 Fac0(Coef00, Coef00, Coef02, Coef03);
		vec4 Fac1(Coef04, Coef04, Coef06, Coef07);
		vec4 Fac2(Coef08, Coef08, Coef10, Coef11);
		vec4 Fac3(Coef12, Coef12, Coef14, Coef15);
		vec4 Fac4(Coef16, Coef16, Coef18, Coef19);
		vec4 Fac5(Coef20, Coef20, Coef22, Coef23);

		vec4 Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
		vec4 Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
		vec4 Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
		vec4 Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

		vec4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		vec4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		vec4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		vec4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		vec4 SignA(+1, -1, +1, -1);
		vec4 SignB(-1, +1, -1, +1);
		mat4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		vec4 Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

		vec4 Dot0(m[0] * Row0);
		float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

		float OneOverDeterminant = 1 / Dot1;

		return Inverse * OneOverDeterminant;
    }


    /// Projects a vertex.
    ///
    /// @remarks
    ///     This takes a position in the world and converts it to a point on the viewport.
    inline vec4 project(const vec4 &in, const mat4 &projection, const mat4 &model, const vec4 &viewport)
    {
        vec4 out(in);
		out = model * out;
		out = projection * out;

		out /= out.w;
		out = out * 0.5f + 0.5f;
		out[0] = out[0] * viewport[2] + viewport[0];
		out[1] = out[1] * viewport[3] + viewport[1];

		return out;
    }

    /// Unprojects a vertex.
    ///
    /// @remarks
    ///     This takes a position on the viewport and converts it to world space based on the given matrices.
    inline vec4 unproject(const vec4 &in, const mat4 &projection, const mat4 &model, const vec4 &viewport)
    {
		vec4 tmp(in);
		tmp.x = (tmp.x - viewport[0]) / viewport[2];
		tmp.y = (tmp.y - viewport[1]) / viewport[3];
		tmp = tmp * 2 - 1;

		vec4 out = inverse(projection * model) * tmp;
		out /= out.w;

		return out;
    }



    ///////////////////////////////////////////
    // Helper Utilities

    inline mat4 CalculateTransformMatrix(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        mat4 result;

        result = GT::quat_to_mat4(rotation);

        result[3][0] = position.x;
        result[3][1] = position.y;
        result[3][2] = position.z;

        result[0] *= scale.x;
        result[1] *= scale.y;
        result[2] *= scale.z;

        return result;
    }

    inline void CalculateViewMatrix(const vec3 &position, const quat &orientation, mat4 &result)
    {
        // rotation.
        result = quat_to_mat4(inverse(orientation));

        // position.
        result[3][0] = -position.x;
        result[3][1] = -position.y;
        result[3][2] = -position.z;
    }


    inline float Distance(float x1, float y1, float x2, float y2)
    {
        float diffX = x1 - x2;
        float diffY = y1 - y2;
        return fabsf(sqrtf(diffX*diffX + diffY*diffY));
    }


    /// Performs a linear interpolation between two vectors.
    inline float lerp(float v0, float v1, float t)
    {
        return v0*(1.0f - t) + v1*t;
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
