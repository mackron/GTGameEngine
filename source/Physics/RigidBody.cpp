// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Physics.hpp>

namespace GT
{
    RigidBody::RigidBody(btScalar mass, const btCollisionShape &collisionShape, const btVector3 &localInertia, btMotionState* motionState)
        : btRigidBody(mass, motionState, (btCollisionShape *)&collisionShape, localInertia), world(nullptr)
    {
    }

    RigidBody::RigidBody(btScalar mass, const btCollisionShape &collisionShape, btMotionState* motionState)
        : btRigidBody(mass, motionState, (btCollisionShape*)&collisionShape, BulletUtils::CalculateLocalInertia(mass, collisionShape)), world(nullptr)
    {
    }

    RigidBody::RigidBody(const btRigidBodyConstructionInfo &constructionInfo)
        : btRigidBody(constructionInfo), world(nullptr)
    {
    }

    RigidBody::~RigidBody()
    {
        if (this->world != nullptr)
        {
            this->world->RemoveRigidBody(*this);
        }
    }

    void RigidBody::SetWorld(DynamicsWorld *worldIn)
    {
        this->world = worldIn;
    }

    DynamicsWorld * RigidBody::GetWorld()
    {
        return this->world;
    }
}
