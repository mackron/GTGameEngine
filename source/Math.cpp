
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    namespace Math
    {
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result)
        {
            // We will need the world position and orientation for this camera.
            result = glm::mat4_cast(glm::inverse(orientation)) * glm::translate(-position);
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
                float gx = g * x;
                float x2 = x * x;
                float v2 = v * v;
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