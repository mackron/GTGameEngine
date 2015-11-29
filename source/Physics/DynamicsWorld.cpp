// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Physics.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialiser list.
#endif

namespace GT
{
    DynamicsWorld::DynamicsWorld()
        : solver(),
          world(&dispatcher, &broadphase, &solver, &configuration),
          ghostPairCallback()
    {
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(&this->ghostPairCallback);
    }

    DynamicsWorld::~DynamicsWorld()
    {
        // Constraints need to be removed.
        for (int i = this->world.getNumConstraints() - 1; i >= 0; --i)
        {
            auto constraint = this->world.getConstraint(i);
            if (constraint != nullptr)
            {
                this->RemoveConstraint(*constraint);
            }
        }

        // Now we need to remove all of our rigid bodies. We go backwards here to avoid reshuffling of the internal buffer.
        for (int i = this->world.getNumCollisionObjects() - 1; i >= 0; --i)
        {
            auto obj  = this->world.getCollisionObjectArray()[i];
            auto body = btRigidBody::upcast(obj);

            if (body != nullptr)
            {
                this->RemoveRigidBody(*body);
            }
            else if (obj != nullptr)
            {
                this->RemoveCollisionObject(*obj);
            }
        }

        // We need to remove the ghost callback. Not doing this will result in a crash... Also, this must be done AFTER
        // removing all of the collision objects.
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(nullptr);
    }


    void DynamicsWorld::AddRigidBody(btRigidBody &bodyIn, short group, short mask)
    {
        auto body = dynamic_cast<RigidBody*>(&bodyIn);
        if (body != nullptr)
        {
            this->AddRigidBody(*body, group, mask);
        }
        else
        {
            this->world.addRigidBody(&bodyIn, group, mask);
        }
    }

    void DynamicsWorld::AddRigidBody(RigidBody &body, short group, short mask)
    {
        auto prevWorld = body.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveRigidBody(body);
        }

        body.SetWorld(this);
        this->world.addRigidBody(&body, group, mask);
    }

    void DynamicsWorld::RemoveRigidBody(btRigidBody &bodyIn)
    {
        auto body = dynamic_cast<RigidBody*>(&bodyIn);
        if (body != nullptr)
        {
            this->RemoveRigidBody(*body);
        }
        else
        {
            this->world.removeRigidBody(&bodyIn);
        }
    }

    void DynamicsWorld::RemoveRigidBody(RigidBody &body)
    {
        body.SetWorld(nullptr);
        this->world.removeRigidBody(&body);
    }


    void DynamicsWorld::AddGhostObject(btGhostObject &ghostIn, short group, short mask)
    {
        auto ghost = dynamic_cast<GhostObject*>(&ghostIn);
        if (ghost != nullptr)
        {
            this->AddGhostObject(*ghost, group, mask);
        }
        else
        {
            this->world.addCollisionObject(&ghostIn, group, mask);
        }
    }

    void DynamicsWorld::AddGhostObject(GhostObject &ghost, short group, short mask)
    {
        auto prevWorld = ghost.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveGhostObject(ghost);
        }

        ghost.SetWorld(this);
        this->world.addCollisionObject(&ghost, group, mask);
    }

    void DynamicsWorld::RemoveGhostObject(btGhostObject &ghostIn)
    {
        auto ghost = dynamic_cast<GhostObject*>(&ghostIn);
        if (ghost != nullptr)
        {
            this->RemoveGhostObject(*ghost);
        }
        else
        {
            this->world.removeCollisionObject(&ghostIn);
        }
    }

    void DynamicsWorld::RemoveGhostObject(GhostObject &ghost)
    {
        ghost.SetWorld(nullptr);
        this->world.removeCollisionObject(&ghost);
    }



    void DynamicsWorld::AddConstraint(btTypedConstraint &constraintIn, bool disableCollisionsBetweenLinkedBodies)
    {
        auto genericConstraint = dynamic_cast<GenericConstraint*>(&constraintIn);
        if (genericConstraint != nullptr)
        {
            this->AddConstraint(*genericConstraint, disableCollisionsBetweenLinkedBodies);
            return;
        }

        auto coneTwistConstraint = dynamic_cast<ConeTwistConstraint*>(&constraintIn);
        if (coneTwistConstraint != nullptr)
        {
            this->AddConstraint(*coneTwistConstraint, disableCollisionsBetweenLinkedBodies);
        }


        // We'll get here if the constraint is a standard Bullet constraint.
        this->world.addConstraint(&constraintIn, disableCollisionsBetweenLinkedBodies);
    }

    void DynamicsWorld::AddConstraint(GenericConstraint &constraint, bool disableCollisionsBetweenLinkedBodies)
    {
        auto prevWorld = constraint.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveConstraint(constraint);
        }

        constraint.SetWorld(this);
        constraint.IsCollisionBetweenLinkedBodiesDisabled(disableCollisionsBetweenLinkedBodies);

        this->world.addConstraint(&constraint, disableCollisionsBetweenLinkedBodies);
    }

    void DynamicsWorld::AddConstraint(ConeTwistConstraint &constraint, bool disableCollisionsBetweenLinkedBodies)
    {
        auto prevWorld = constraint.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveConstraint(constraint);
        }

        constraint.SetWorld(this);
        constraint.IsCollisionBetweenLinkedBodiesDisabled(disableCollisionsBetweenLinkedBodies);

        this->world.addConstraint(&constraint, disableCollisionsBetweenLinkedBodies);
    }

    void DynamicsWorld::AddConstraint(PointToPointConstraint &constraint, bool disableCollisionsBetweenLinkedBodies)
    {
        auto prevWorld = constraint.GetWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveConstraint(constraint);
        }

        constraint.SetWorld(this);
        constraint.IsCollisionBetweenLinkedBodiesDisabled(disableCollisionsBetweenLinkedBodies);

        this->world.addConstraint(&constraint, disableCollisionsBetweenLinkedBodies);
    }



    void DynamicsWorld::RemoveConstraint(btTypedConstraint &constraintIn)
    {
        auto genericConstraint = dynamic_cast<GenericConstraint*>(&constraintIn);
        if (genericConstraint != nullptr)
        {
            this->RemoveConstraint(*genericConstraint);
        }
        else
        {
            this->world.removeConstraint(&constraintIn);
        }
    }

    void DynamicsWorld::RemoveConstraint(GenericConstraint &constraint)
    {
        constraint.SetWorld(nullptr);
        this->world.removeConstraint(&constraint);
    }

    void DynamicsWorld::RemoveConstraint(ConeTwistConstraint &constraint)
    {
        constraint.SetWorld(nullptr);
        this->world.removeConstraint(&constraint);
    }

    void DynamicsWorld::RemoveConstraint(PointToPointConstraint &constraint)
    {
        constraint.SetWorld(nullptr);
        this->world.removeConstraint(&constraint);
    }




    void DynamicsWorld::Step(double timeStep, int maxSubSteps, double fixedTimeStep)
    {
        this->world.stepSimulation(static_cast<btScalar>(timeStep), maxSubSteps, static_cast<btScalar>(fixedTimeStep));
    }


    void DynamicsWorld::SetGravity(float x, float y, float z)
    {
        this->world.setGravity(btVector3(x, y, z));
    }

    void DynamicsWorld::GetGravity(float &x, float &y, float &z) const
    {
        btVector3 gravity = this->world.getGravity();
        x = gravity.x();
        y = gravity.y();
        z = gravity.z();
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
