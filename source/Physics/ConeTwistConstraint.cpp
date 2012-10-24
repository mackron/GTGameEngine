
#include <GTEngine/Physics/ConeTwistConstraint.hpp>
#include <GTEngine/Physics/DynamicsWorld.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    ConeTwistConstraint::ConeTwistConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB)
        : btConeTwistConstraint(bodyA, bodyB, BulletUtils::CreateTransform(frameA), BulletUtils::CreateTransform(frameB)),
          world(nullptr), isCollisionBetweenLinkedBodiesDisabled(true)
    {
    }

    ConeTwistConstraint::ConeTwistConstraint(RigidBody &bodyB, const glm::mat4 &frameB)
        : btConeTwistConstraint(bodyB, BulletUtils::CreateTransform(frameB)),
          world(nullptr), isCollisionBetweenLinkedBodiesDisabled(true)
    {
    }

    ConeTwistConstraint::~ConeTwistConstraint()
    {
        if (this->world != nullptr)
        {
            this->world->RemoveConstraint(*this);
        }
    }

    void ConeTwistConstraint::SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~ConeTwistConstraint();
        new (this) ConeTwistConstraint(bodyA, bodyB, frameA, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }

    void ConeTwistConstraint::SetAttachments(RigidBody &bodyB, const glm::mat4 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~ConeTwistConstraint();
        new (this) ConeTwistConstraint(bodyB, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }


    void ConeTwistConstraint::SetLimits(float swingSpan1, float swingSpan2, float twistSpan)
    {
        this->setLimit(swingSpan1, swingSpan2, twistSpan);
    }
}
