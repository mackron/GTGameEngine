// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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

            Plane(const Plane &other)
                : a(other.a), b(other.b), c(other.c), d(other.d)
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
            Plane & Normalize()
            {
                float invLength = glm::inversesqrt((a * a) + (b * b) + (c * c));
                a *= invLength;
                b *= invLength;
                c *= invLength;
                d *= invLength;

                return *this;
            }

            /// Retrieves a normalized copy of the plane.
            Plane Normalized() const
            {
                Plane result(*this);
                result.Normalize();

                return result;
            }


            /// Retrieves the distance between the given point and the plane. Assumes the plane is normalized.
            ///
            /// @param point [in] The point to check.
            ///
            /// @remarks
            ///     If this returns < 0, it means the point is behind the plane.
            ///     @par
            ///     This method assumes the plane is normalized first.
            float DistanceTo(const glm::vec3 &point) const
            {
                return glm::dot(glm::vec3(a, b, c), point) + d;
            }


            //////////////////////////////////////////////////////
            // Operators.

            Plane & operator=(const Plane &other)
            {
                this->a = other.a;
                this->b = other.b;
                this->c = other.c;
                this->d = other.d;

                return *this;
            }


        public:

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
