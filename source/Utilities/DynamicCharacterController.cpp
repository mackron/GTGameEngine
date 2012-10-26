
#include <GTEngine/Utilities/DynamicCharacterController.hpp>

namespace GTEngine
{
    DynamicCharacterController::DynamicCharacterController(float height, float radius)
        : sceneNode()
    {
        const float halfHeight = height * 0.5f;

        auto dynamics = this->sceneNode.AddComponent<DynamicsComponent>();
        dynamics->AddCapsuleYCollisionShape(radius, halfHeight, 0.0f, halfHeight, 0.0f);
        dynamics->SetMass(10.0f);
        dynamics->SetAngularFactor(0.0f, 0.0f, 0.0f);
        dynamics->SetFriction(1.0f);
        dynamics->DisableDeactivation();
    }

    DynamicCharacterController::~DynamicCharacterController()
    {
    }


    void DynamicCharacterController::SetMass(float mass)
    {
        if (mass <= 0.0f)
        {
            mass = 10.0f;
        }

        this->sceneNode.GetComponent<DynamicsComponent>()->SetMass(mass);
    }

    void DynamicCharacterController::SetLinearVelocity(const glm::vec3 &velocity)
    {
        this->sceneNode.GetComponent<DynamicsComponent>()->SetLinearVelocity(velocity);
    }

    glm::vec3 DynamicCharacterController::GetLinearVelocity() const
    {
        return this->sceneNode.GetComponent<DynamicsComponent>()->GetLinearVelocity();
    }
}
