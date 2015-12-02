// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Utilities/DynamicCharacterController.hpp>
#include <GTGE/Scene.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    DynamicCharacterController::DynamicCharacterController(float heightIn, float radiusIn)
        : sceneNode(), sceneNodeEventHandler(*this),
          height(heightIn), radius(radiusIn),
          onGround(false), isHeadTouchingSomething(false),
          m_eventHandler(nullptr)
    {
        // TODO: Change to a capsule later.


        const float halfHeight = height * 0.5f;

        auto dynamics = this->sceneNode.AddComponent<DynamicsComponent>();
        //dynamics->AddCapsuleYCollisionShape(radius, height - (radius * 2.0f), 0.0f, halfHeight, 0.0f);
        dynamics->AddBoxCollisionShape(radius, halfHeight, radius, 0.0f, halfHeight, 0.0f);
        dynamics->SetMass(10.0f);
        dynamics->SetAngularFactor(0.0f, 0.0f, 0.0f);
        dynamics->SetFriction(0.0f);
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
        return this->IsOnGround();
    }

    bool DynamicCharacterController::IsOnGround() const
    {
        return this->onGround;
    }



    void DynamicCharacterController::SetEventHandler(CharacterControllerEventHandler &eventHandler)
    {
        m_eventHandler = &eventHandler;
    }

    void DynamicCharacterController::RemoveEventHandler()
    {
        m_eventHandler = nullptr;
    }




    //////////////////////////////////////////////////
    // Events

    void DynamicCharacterController::OnUpdate(double deltaTimeInSeconds)
    {
        auto scene = this->sceneNode.GetScene();
        if (scene != nullptr)
        {
            const bool wasOnGround              = this->onGround;
            const bool wasHeadTouchingSomething = this->isHeadTouchingSomething;

            // For ease of use.
            const float halfHeight = this->height * 0.5f;

            // We need to keep track of whether or not the player is on the ground. We're going to use a ray to do this. We will shoot the ray from
            // the center of the player towards the ground, while ignoring the controller body itself.
            ClosestRayExceptMeTestCallback callback;
            callback.excludedNode = &this->sceneNode;

            // The scene node will be at the player's feet.
            glm::vec3 rayStartCenter = this->sceneNode.GetPosition() + glm::vec3(0.0f, halfHeight, 0.0f);
            glm::vec3 rayEndCenter   = this->sceneNode.GetPosition() + glm::vec3(0.0f, -0.1f, 0.0f);
            glm::vec3 rayStartLeft   = rayStartCenter + glm::vec3(-this->radius, -halfHeight + 0.1f, 0.0f);
            glm::vec3 rayEndLeft     = rayEndCenter   + glm::vec3(-this->radius, 0.0f, 0.0f);
            glm::vec3 rayStartRight  = rayStartCenter + glm::vec3(+this->radius, -halfHeight + 0.1f, 0.0f);
            glm::vec3 rayEndRight    = rayEndCenter   + glm::vec3(+this->radius, 0.0f, 0.0f);

            if (scene->RayTest(rayStartCenter, rayEndCenter) != nullptr ||
                scene->RayTest(rayStartLeft,   rayEndLeft)   != nullptr ||
                scene->RayTest(rayStartRight,  rayEndRight)  != nullptr)
            {
                // TODO: Should check if the player is sitting on top of a dynamic body while it is falling or whatnot.
                this->onGround = true;

                if (!wasOnGround)
                {
                    this->OnLand();
                }
            }
            else
            {
                this->onGround = false;

                if (wasOnGround)
                {
                    this->OnRaise();
                }
            }


            // Now we'll check the head.
            rayStartCenter = this->sceneNode.GetPosition() + glm::vec3(0.0f, this->height,        0.0f);
            rayEndCenter   = this->sceneNode.GetPosition() + glm::vec3(0.0f, this->height + 0.1f, 0.0f);
            rayStartLeft   = rayStartCenter + glm::vec3(-this->radius, halfHeight - 0.1f, 0.0f);
            rayEndLeft     = rayEndCenter   + glm::vec3(-this->radius, 0.0f, 0.0f);
            rayStartRight  = rayStartCenter + glm::vec3(+this->radius, halfHeight - 0.1f, 0.0f);
            rayEndRight    = rayEndCenter   + glm::vec3(+this->radius, 0.0f, 0.0f);

            if (scene->RayTest(rayStartCenter, rayEndCenter) != nullptr ||
                scene->RayTest(rayStartLeft,   rayEndLeft)   != nullptr ||
                scene->RayTest(rayStartRight,  rayEndRight)  != nullptr)
            {
                this->isHeadTouchingSomething = true;

                if (!wasHeadTouchingSomething)
                {
                    this->OnHitHead();
                }
            }
            else
            {
                this->isHeadTouchingSomething = false;
            }




            if (m_eventHandler != nullptr)
            {
                m_eventHandler->OnUpdate(deltaTimeInSeconds);
            }
        }
    }

    void DynamicCharacterController::OnLand()
    {
        if (m_eventHandler != nullptr)
        {
            m_eventHandler->OnLand();
        }
    }

    void DynamicCharacterController::OnRaise()
    {
        if (m_eventHandler != nullptr)
        {
            m_eventHandler->OnRaise();
        }
    }

    void DynamicCharacterController::OnHitHead()
    {
        if (m_eventHandler != nullptr)
        {
            m_eventHandler->OnHitHead();
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
