
#ifndef __GTEngine_Math_hpp_
#define __GTEngine_Math_hpp_

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #pragma GCC diagnostic ignored "-Winline"
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
        /// Calculates a view matrix from a position and orientation.
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result);

        /// Calculates a transform matrix from a position, orientation and scale.
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, glm::mat4 &result);

        /// Calculates a trainsform matrix from a position and orientation.
        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result);


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
    }
}


#endif
