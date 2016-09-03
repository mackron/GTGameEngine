// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneNode.hpp>

#define GLM_FORCE_RADIANS
#include "glm/glm/gtx/transform.hpp"

namespace GT
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(CameraComponent, "Camera")

    CameraComponent::CameraComponent(SceneNode &node)
        : Component(node), projection(),
          ortho(),
          zNear(0.0f), zFar(0.0f)
    {
    }

    void CameraComponent::Set3DProjection(float fovIn, float aspectIn, float zNearIn, float zFarIn)
    {
        this->perspective.fov    = fovIn;
        this->perspective.aspect = aspectIn;
        this->zNear              = zNearIn;
        this->zFar               = zFarIn;
        
        // The aspect ratio must be larger than 0. If it's not, we'll default to 16/9.
        if (this->perspective.aspect <= 0)
        {
            this->perspective.aspect = 16.0f / 9.0f;
        }
        
        // We can't let the near and far planes equal each other. If this is the case, we're going to take the near plane and multiply it
        // by 100 to get the far plane.
        if (this->zNear == this->zFar)
        {
            if (this->zNear != 0)
            {
                this->zFar = this->zNear * 100;
            }
            else
            {
                this->zNear = 0.1f;
                this->zFar  = 1000.0f;
            }
        }
        

        this->projection = glm::perspective(glm::radians(this->perspective.fov * 0.5f), this->perspective.aspect, this->zNear, this->zFar);   // Although unintuitive, halving the FOV here is correct.


        this->OnChanged();
    }

    void CameraComponent::Set2DProjection(float leftIn, float rightIn, float bottomIn, float topIn, float zNearIn, float zFarIn)
    {
        this->ortho.left   = leftIn;
        this->ortho.right  = rightIn;
        this->ortho.bottom = bottomIn;
        this->ortho.top    = topIn;
        this->zNear        = zNearIn;
        this->zFar         = zFarIn;
        
        
        // We can't let the near and far planes equal each other. If this is the case, we're going to take the near plane and multiply it
        // by 100 to get the far plane.
        if (this->zNear == this->zFar)
        {
            if (this->zNear != 0)
            {
                this->zFar = this->zNear * 100;
            }
            else
            {
                this->zNear = 0.1f;
                this->zFar  = 1000.0f;
            }
        }
        

        this->projection = glm::ortho(this->ortho.left, this->ortho.right, this->ortho.bottom, this->ortho.top, this->zNear, this->zFar);


        this->OnChanged();
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




    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void CameraComponent::Serialize(Serializer &serializer) const
    {
        serializer.Write(this->projection);
        serializer.Write(this->ortho);           // <-- unioned with 'perspective'.
        serializer.Write(this->zNear);
        serializer.Write(this->zFar);
    }

    void CameraComponent::Deserialize(Deserializer &deserializer)
    {
        deserializer.Read(this->projection);
        deserializer.Read(this->ortho);
        deserializer.Read(this->zNear);
        deserializer.Read(this->zFar);

        this->OnChanged();
    }
}
