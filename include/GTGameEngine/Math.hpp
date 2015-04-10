// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "Config.hpp"
#include <GTLib/Math.hpp>

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
    /// Structure representing a vec4/float4.
    ALIGNED_TYPE(struct, 16) vec4
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

        vec4(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }
    };

    /// Performs an addition on the given vector.
    inline vec4 add(vec4 v0, vec4 v1)
    {
        return vec4(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
    }

    inline vec4 add(vec4 v0, float v1)
    {
        return vec4(v0.x + v1, v0.y + v1, v0.z + v1, v0.w + v1);
    }


    /// Performs a subtraction on the given vector.
    inline vec4 sub(vec4 v0, vec4 v1)
    {
        return vec4(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
    }

    inline vec4 sub(vec4 v0, float v1)
    {
        return vec4(v0.x - v1, v0.y - v1, v0.z - v1, v0.w - v1);
    }


    /// Performs a multiplication on the given vector.
    inline vec4 mul(vec4 v0, vec4 v1)
    {
        return vec4(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
    }

    inline vec4 mul(vec4 v0, float v1)
    {
        return vec4(v0.x * v1, v0.y * v1, v0.z * v1, v0.w * v1);
    }


    /// Performs a division on the given vector.
    inline vec4 div(vec4 v0, vec4 v1)
    {
        return vec4(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z, v0.w / v1.w);
    }

    inline vec4 div(vec4 v0, float v1)
    {
        return vec4(v0.x / v1, v0.y / v1, v0.z / v1, v0.w / v1);
    }


    /// Calculates the squared length of a vector.
    inline float length2(vec4 v)
    {
        return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
    }

    /// Calculates the length of a vector.
    inline float length(vec4 v)
    {
        return ::sqrtf(length2(v));
    }

    /// Returns the normalization of the given vector.
    inline vec4 normalize(vec4 v)
    {
        return div(v, length(v));
    }


    /// Performs a dot product between two vectors.
    inline float dot(vec4 v0, vec4 v1)
    {
        return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z + v0.w*v1.w;
    }

    /// Performs a cross product between two vectors.
    ///
    /// The W component is ignored and set to 0 in the returned vector.
    inline vec4 cross(vec4 v0, vec4 v1)
    {
        return vec4(
            v0.y*v1.z - v0.z*v1.y,
            v0.z*v1.x - v0.x*v1.z,
            v0.x*v1.y - v0.y*v1.x,
            0.0f);
    }



    /// Structure representing a quaternion.
    ALIGNED_TYPE(struct, 16) quat
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

        quat(float _x, float _y, float _z, float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

