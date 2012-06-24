
#include <GTEngine/SceneObject.hpp>

namespace GTEngine
{
    SceneObject::SceneObject(SceneObjectType typeIn)
        : type(typeIn),
          position(), orientation(), scale(1.0f, 1.0f, 1.0f)
    {
    }

    SceneObject::~SceneObject()
    {
    }


    void SceneObject::Translate(const glm::vec3 &offset)
    {
        this->SetPosition(this->position + (this->orientation * offset));
    }

    void SceneObject::Rotate(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(this->orientation * glm::angleAxis(angle, axis));
    }

    void SceneObject::Scale(const glm::vec3 &scale)
    {
        this->SetScale(this->scale * scale);
    }


    void SceneObject::InterpolatePosition(const glm::vec3 &dest, float a)
    {
        this->SetPosition(glm::mix(this->position, dest, a));
    }

    void SceneObject::InterpolateOrientation(const glm::quat &dest, float a)
    {
        this->SetOrientation(GTEngine::mix(this->orientation, dest, a));
    }

    void SceneObject::InterpolateScale(const glm::vec3 &dest, float a)
    {
        this->SetScale(glm::mix(this->scale, dest, a));
    }


    glm::vec3 SceneObject::GetForwardVector() const
    {
        return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneObject::GetRightVector() const
    {
        return this->orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneObject::GetUpVector() const
    {
        return this->orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }


    void SceneObject::GetTransform(glm::mat4 &transform)
    {
        Math::CalculateTransformMatrix(this->position, this->orientation, this->scale, transform);
    }
}
