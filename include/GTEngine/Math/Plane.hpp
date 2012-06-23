
#ifndef __GTEngine_Plane_hpp_
#define __GTEngine_Plane_hpp_

#include <glm/glm.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#endif

namespace GTEngine
{
    namespace Math
    {
        /// Class representing a mathematical plane.
        class Plane
        {
        public:

            /// Constructor.
            Plane()
                : a(0.0f), b(0.0f), c(0.0f), d(0.0f)
            {
            }

            Plane(float aIn, float bIn, float cIn, float dIn)
                : a(aIn), b(bIn), c(cIn), d(dIn)
            {
            }

            Plane(const glm::vec3 &abc, float dIn)
                : a(abc.x), b(abc.y), c(abc.z), d(dIn)
            {
            }

            Plane(const glm::vec4 &abcd)
                : a(abcd.x), b(abcd.y), c(abcd.z), d(abcd.w)
            {
            }


            /// Normalizes the plane.
            void Normalize()
            {
//#if !defined(GT_NO_SIMD) && (GLM_ARCH & GLM_ARCH_SSE2)
//#else
                float length = glm::sqrt((a * a) + (b * b) + (c * c));
                a /= length;
                b /= length;
                c /= length;
                d /= length;
//#endif
            }




        private:

            union
            {
                struct
                {
                    float a;
                    float b;
                    float c;
                    float d;
                };

//#if !defined(GT_NO_SIMD)
//                __m128 abcd;
//#endif
            };
        };
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
