// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Math_hpp_
#define __GTEngine_Math_hpp_

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    //#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Weffc++"
    
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wunused-parameter"
    #endif
#endif
#define GLM_FORCE_ONLY_XYZW
#define GLM_FORCE_SSE2
#define GLM_SIMD_ENABLE_XYZW_UNION
#define GLM_SIMD_ENABLE_DEFAULT_INIT
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/fast_exponential.hpp>
#include <glm/gtx/simd_mat4.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include "Math/Plane.hpp"
#include "AABB.hpp"

namespace GTEngine
{
    namespace Math
    {
        template <typename T> 
	    GLM_FUNC_QUALIFIER glm::detail::tquat<T> quatFromEulerFast
	    (
		    glm::detail::tvec3<T> const & eulerAngle
	    )
	    {
            glm::detail::tquat<T> result;

		    glm::detail::tvec3<T> c = glm::fastCos(eulerAngle * T(0.5));
		    glm::detail::tvec3<T> s = glm::fastSin(eulerAngle * T(0.5));
		
		    result.w = c.x * c.y * c.z + s.x * s.y * s.z;
		    result.x = s.x * c.y * c.z - c.x * s.y * s.z;
		    result.y = c.x * s.y * c.z + s.x * c.y * s.z;
		    result.z = c.x * c.y * s.z - s.x * s.y * c.z;

            return result;
	    }



        /// Calculates a view matrix from a position and orientation.
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result);

        /// Calculates a transform matrix from a position, orientation and scale.
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, glm::mat4 &result);

        /// Calculates a trainsform matrix from a position and orientation.
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result);
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, btTransform &result);
        


        /// Retrieves the frustum planes of the given matrix.
        ///
        /// @param mvp       [in ] The model-view-projection matrix whose frustum planes are being retrieved.
        /// @param planes    [out] The array that will receive the planes.
        /// @param normalize [in ] Whether or not the planes should be normalized.
        ///
        /// @remarks
        ///     The output planes are as follows:
        ///         0 - Right
        ///         1 - Left
        ///         2 - Top
        ///         3 - Bottom
        ///         4 - Far
        ///         5 - Near
        void CalculateFrustumPlanes(const glm::mat4 &mvp, Plane planes[6], bool normalize = true);


        /// Calculates a look-at matrix.
        glm::mat3 CalculateLookAtMatrix(const glm::vec3 &origin, const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));




        /// Math functions for trajectories.
        namespace Trajectory
        {
            /// Retrieves the angle required to hit a point when fired from (0,0), in radians.
            ///
            /// @param x [in] The range of the target point
            /// @param y [in] The altitude of the target point.
            /// @param g [in] The gravity. This should be 9.81 for Earth gravity.
            /// @param v [in] The initial velocity.
            ///
            /// @remarks
            ///     If the velocity is not large enough to reach the target, the maximum angle will be returned (45 degrees).
            float LaunchAngle(float x, float y, float g, float v);
            float LaunchAngle(const glm::vec3 &start, const glm::vec3 &target, float g, float v);


            /// Retrieves the maximum distance of an object based on the given gravity and velocity.
            ///
            /// @param g [in] The gravity.
            /// @param v [in] The velocity of the object.
            float MaxDistance(float g, float v);

            /// Retrieves the height of the object at range 'x'.
            ///
            /// @param x  [in] The range of the point whose height is being calculated.
            /// @param y0 [in] The starting height.
            /// @param a  [in] The launch angle.
            /// @param g  [in] The gravity. This should be 9.81 for Earth gravity.
            /// @param v  [in] The launch velocity.
            float HeightAtX(float x, float y0, float a, float g, float v);
        }


        /// Math functions for frustums.
        namespace Frustum
        {
            /// Determines whether or not a point is inside the given frustum planes.
            ///
            /// @param frustumPlanes [in] The 6 frustum planes.
            /// @param point         [in] The point to check.
            ///
            /// @return True if the point is inside the frustum planes.
            bool ContainsPoint(const Plane frustumPlanes[6], const glm::vec3 &point);

            /// Determines whether or not the given AABB is inside the given frustum planes.
            ///
            /// @param frustumPlanes [in] The 6 frustum planes.
            /// @param aabb          [in] The AABB to check.
            ///
            /// @return True if the AABB is inside the frustum planes.
            bool ContainsAABB(const Plane frustumPlanes[6], const AABB &aabb);
        }


        /// Math functions for lighting.
        namespace Lighting
        {
            /// Retrieves the approximate radius of a light based on the given attenuation factors.
            template <typename T>
            T ApproximateAttenuationRadius(T c, T l, T q)
            {
                // If 'q' is 0.0, we'll end up with a division by 0 bug. In this case, we'll replace it with a tiny value for approximation.
                if (q == T(0.0))
                {
                    q = T(0.000001);
                }

                return (-l + sqrt(l * l - T(4.0) * (c - T(1000.0)) * q)) / (T(2.0) * q);      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
            }
        }
    }
}


#endif
