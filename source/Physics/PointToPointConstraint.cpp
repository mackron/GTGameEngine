// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Physics/PointToPointConstraint.hpp>
#include <GTEngine/Physics.hpp>


namespace GTEngine
{
    PointToPointConstraint::PointToPointConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB)
        : btPoint2PointConstraint(bodyA, bodyB, ToBulletVector3(pivotInA), ToBulletVector3(pivotInB)),
          m_world(nullptr), isCollisionBetweenLinkedBodiesDisabled(false)
    {
    }

    PointToPointConstraint::PointToPointConstraint(RigidBody &bodyB, const glm::vec3 &pivotInB)
        : btPoint2PointConstraint(bodyB, ToBulletVector3(pivotInB)),
          m_world(nullptr), isCollisionBetweenLinkedBodiesDisabled(false)
    {
    }

    PointToPointConstraint::~PointToPointConstraint()
    {
        if (m_world != nullptr)
        {
            m_world->RemoveConstraint(*this);
        }
    }


    void PointToPointConstraint::SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::vec3 &frameA, const glm::vec3 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~PointToPointConstraint();
        new (this) PointToPointConstraint(bodyA, bodyB, frameA, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }

    void PointToPointConstraint::SetAttachments(RigidBody &bodyB, const glm::vec3 &frameB)
    {
        auto world                               = this->GetWorld();
        auto disableCollisionBetweenLinkedBodies = this->IsCollisionBetweenLinkedBodiesDisabled();

        // The attachments are actually set via the constructor. We're going to do something really bad here. We're going to call the destructor
        // and then re-construct.
        this->~PointToPointConstraint();
        new (this) PointToPointConstraint(bodyB, frameB);

        if (world != nullptr)
        {
            world->AddConstraint(*this, disableCollisionBetweenLinkedBodies);
        }
    }
}
