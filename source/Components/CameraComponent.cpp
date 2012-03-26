
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <glm/gtx/transform.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(CameraComponent, "Camera");

    CameraComponent::CameraComponent(SceneNode &node)
        : Component(node), projection(),
          ortho(),
          zNear(0.0f), zFar(0.0f)
    {
    }

    void CameraComponent::Set3DProjection(float fov, float aspect, float zNear, float zFar)
    {
        this->perspective.fov    = fov;
        this->perspective.aspect = aspect;

        this->zNear  = zNear;
        this->zFar   = zFar;
        
        this->projection = glm::perspective(fov * 0.5f, aspect, zNear, zFar);   // Halving the FOV is correct.
    }

    void CameraComponent::Set2DProjection(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        this->ortho.left   = left;
        this->ortho.right  = right;
        this->ortho.bottom = bottom;
        this->ortho.top    = top;

        this->zNear  = zNear;
        this->zFar   = zFar;

        this->projection = glm::ortho(left, right, bottom, top, zNear, zFar);
    }

    glm::mat4 CameraComponent::GetViewMatrix() const
    {
        // We will need the world position and orientation for this camera.
        glm::vec3 position    = this->node.GetWorldPosition();
        glm::quat orientation = this->node.GetWorldOrientation();
        
        return glm::mat4_cast(glm::inverse(orientation)) * glm::translate(-position);
    }

    const glm::mat4 & CameraComponent::GetProjectionMatrix() const
    {
        return this->projection;
    }
}