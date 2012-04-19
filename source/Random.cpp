
#include <GTEngine/Random.hpp>
#include <GTCore/Math.hpp>

namespace GTEngine
{
    bool g_haveSeeded = false;

    template <> float GenerateRandomNumber(float min, float max)
    {
        return static_cast<float>(GenerateRandomNumber(
            static_cast<int>(static_cast<double>(min) * 10000),
            static_cast<int>(static_cast<double>(max) * 10000))) / 10000.0f;
    }

    template <> double GenerateRandomNumber(double min, double max)
    {
        return static_cast<double>(GenerateRandomNumber(
            static_cast<int>(min * 10000),
            static_cast<int>(max * 10000))) / 10000.0;
    }

    glm::vec2 GenerateRandomNumberCircle(float radius)
    {
        float a = GenerateRandomNumber(0.0f, GTCORE_2PI);
        float len = GenerateRandomNumber(-radius, radius);

        return glm::vec2(cos(a), sin(a)) * len;
    }

    glm::vec3 GenerateRandomNumberSphere(float radius)
    {
        float a = GenerateRandomNumber(0.0f, GTCORE_2PI);
        float len = GenerateRandomNumber(-radius, radius);

        float z = GenerateRandomNumber(-1.0f, 1.0f);
        float r = sqrt(1.0f - z * z);

        return glm::vec3(r * cos(a), r * cos(a), z) * len;
    }


    glm::vec2 GenerateRandomNumberEllipse(float width, float height)
    {
        glm::vec2 v(GenerateRandomNumberCircle(1.0f));

        return glm::vec2(v.x * width, v.y * height);
    }

    glm::vec3 GenerateRandomNumberEllipse(float width, float height, float depth)
    {
        glm::vec3 v(GenerateRandomNumberSphere(1.0f));

        return glm::vec3(v.x * width, v.y * height, v.z * depth);
    }

}
