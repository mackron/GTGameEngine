
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    namespace Math
    {
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result)
        {
            // orientation.
            result = glm::mat4_cast(glm::inverse(orientation));

            // position.
            result[3][0] = -position.x;
            result[3][1] = -position.y;
            result[3][2] = -position.z;
        }

        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, glm::mat4 &result)
        {
            // orientation.
            result = glm::mat4_cast(orientation);

            // position.
            result[3][0] = position.x;
            result[3][1] = position.y;
            result[3][2] = position.z;

            // scale.
            result[0] *= scale.x;
            result[1] *= scale.y;
            result[2] *= scale.z;
        }
    }
}


// Trajectory.
namespace GTEngine
{
    namespace Math
    {
        namespace Trajectory
        {
            float LaunchAngle(float x, float y, float g, float v)
            {
                float gx =  g * x;
                float x2 =  x * x;
                float v2 =  v * v;
                float v4 = v2 * v2;

                float root = v4 - (g * ((g * x2) + (2.0f * y * v2)));
                if (root < 0.0f)
                {
                    root = 0.0f;
                }

                return glm::atan((v2 - glm::sqrt(root)) / gx);
            }

            float LaunchAngle(const glm::vec3 &start, const glm::vec3 &target, float g, float v)
            {
                float x = glm::min(glm::distance(start, target), MaxDistance(g, v));     // Range
                float y = target.y - start.y;

                return LaunchAngle(x, y, g, v);
            }


            float MaxDistance(float g, float v)
            {
                return (v * v) / g;
            }

            float HeightAtX(float x, float y0, float a, float g, float v)
            {
                float x2     = x * x;
                float vcosa  = v * glm::cos(a);
                float vcosa2 = vcosa * vcosa;

                return y0 + (x * glm::tan(a)) - ((g * x2) / (2.0f * vcosa2));
            }
        }
    }
}