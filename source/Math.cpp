
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