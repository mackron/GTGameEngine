// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Physics/GenericConstraint.hpp>
#include <GTEngine/Physics/RigidBody.hpp>
#include <GTEngine/Physics/DynamicsWorld.hpp>
#include <GTEngine/Physics.hpp>

namespace GT
{
    GenericConstraint::GenericConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB)
        : btGeneric6DofConstraint(bodyA, bodyB, BulletUtils::CreateTransform(frameA), BulletUtils::CreateTransform(frameB), false),
          m_world(nullptr), isCollisionBetweenLinkedBodiesDisabled(false)
    {
    }

    GenericConstraint::GenericConstraint(RigidBody &bodyB, const glm::mat4 &frameB)
        : btGeneric6DofConstraint(bodyB, BulletUtils::CreateTransform(frameB), false),
          m_world(nullptr), isCollisionBetweenLinkedBodiesDisabled(false)
    {
    }

    GenericConstraint::~GenericConstraint()
    {
        if (m_world != nullptr)
        {
            m_world->RemoveConstraint(*this);
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

    void GenericConstraint::SetAttachments(RigidBody &bodyB, const glm::mat4 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~GenericConstraint();
        new (this) GenericConstraint(bodyB, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }
}
