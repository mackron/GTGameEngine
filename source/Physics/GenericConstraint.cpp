
#include <GTEngine/Physics/GenericConstraint.hpp>
#include <GTEngine/Physics/RigidBody.hpp>
#include <GTEngine/Physics/DynamicsWorld.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GenericConstraint::GenericConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB)
        : btGeneric6DofConstraint(bodyA, bodyB, BulletUtils::CreateTransform(frameA), BulletUtils::CreateTransform(frameB), false),
          world(nullptr), isCollisionBetweenLinkedBodiesDisabled(false)
    {
    }

    GenericConstraint::~GenericConstraint()
    {
        if (this->world != nullptr)
        {
            this->world->RemoveConstraint(*this);
        }
    }


    void GenericConstraint::SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~GenericConstraint();
        new (this) GenericConstraint(bodyA, bodyB, frameA, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }


    void GenericConstraint::SetWorld(DynamicsWorld* newWorld)
    {
        this->world = newWorld;
    }

    DynamicsWorld* GenericConstraint::GetWorld()
    {
        return this->world;
    }
}
