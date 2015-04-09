// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "Config.hpp"

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

