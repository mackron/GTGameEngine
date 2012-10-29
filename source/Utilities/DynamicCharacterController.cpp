
#include <GTEngine/Utilities/DynamicCharacterController.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    DynamicCharacterController::DynamicCharacterController(float heightIn, float radiusIn)
        : sceneNode(), sceneNodeEventHandler(*this),
          height(heightIn), radius(radiusIn),
          onGround(false)
    {
        const float halfHeight = height * 0.5f;

        auto dynamics = this->sceneNode.AddComponent<DynamicsComponent>();
        dynamics->AddCapsuleYCollisionShape(radius, height - (radius * 2.0f), 0.0f, halfHeight, 0.0f);
        dynamics->SetMass(10.0f);
        dynamics->SetAngularFactor(0.0f, 0.0f, 0.0f);
        dynamics->SetFriction(1.0f);
        dynamics->DisableDeactivation();


        this->sceneNode.AttachEventHandler(this->sceneNodeEventHandler);
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


    bool DynamicCharacterController::CanJump() const
    {
        return this->OnGround();
    }

    bool DynamicCharacterController::OnGround() const
    {
        return this->onGround;
    }




    //////////////////////////////////////////////////
    // Events

    void DynamicCharacterController::OnUpdate(double deltaTimeInSeconds)
    {
        auto scene = this->sceneNode.GetScene();
        if (scene != nullptr)
        {
            // We need to keep track of whether or not the player is on the ground. We're going to use a ray to do this. We will shoot the ray from
            // the center of the player towards the ground, while ignoring the controller body itself.
            GTEngine::ClosestRayExceptMeTestCallback callback;
            callback.excludedNode = &this->sceneNode;

            // The scene node will be at the player's feet.
            glm::vec3 rayStart = this->sceneNode.GetPosition() - glm::vec3(0.0f, -this->height * 0.5f, 0.0f);
            glm::vec3 rayEnd   = this->sceneNode.GetPosition() + glm::vec3(0.0f, -0.1f, 0.0f);

            auto rayTestResult = scene->RayTest(rayStart, rayEnd);
            if (rayTestResult != nullptr)
            {
                // TODO: Should check if the player is sitting on top of a dynamic body while it is falling or whatnot.
                this->onGround = true;
            }
            else
            {
                this->onGround = false;
            }
        }
    }
}
