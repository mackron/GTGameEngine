
#ifndef __GTEngine_Math_hpp_
#define __GTEngine_Math_hpp_

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_FORCE_ONLY_XYZW
#define GLM_FORCE_SSE2
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/simd_mat4.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include "Math/Plane.hpp"

namespace GTEngine
{
    namespace Math
    {
        /// Calculates a view matrix from a position and orientation.
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result);

        /// Calculates a transform matrix from a position, orientation and scale.
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, glm::mat4 &result);



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


            /// Retrieves the maximum distance of an object.
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
    }


    // Workaround for glm::tquat<T>::mix().
    template <typename T>
    GLM_FUNC_QUALIFIER glm::detail::tquat<T> mix
	(
		glm::detail::tquat<T> const & x,
		glm::detail::tquat<T> const & y,
		T const & a
	)
    {
        if (a <= T(0))
        {
            return x;
        }
        if (a >= T(1))
        {
            return y;
        }
        

        glm::detail::tquat<T> z;

        float d = glm::dot(x, y);
        if (d < T(0))
        {
            d = -d;
            z = -y;
        }
        else
        {
            z = y;
        }

        float angle = glm::acos(d);
        if (angle > T(0))
        {
            return (glm::sin((T(1) - a) * angle) * x + glm::sin(a * angle) * z) / glm::sin(angle);
        }
        else
        {
            return x;
        }
	}
}


#endif