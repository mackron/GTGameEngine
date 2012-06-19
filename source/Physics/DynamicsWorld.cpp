
#include <GTEngine/Physics.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialiser list.
#endif

namespace GTEngine
{
    DynamicsWorld::DynamicsWorld()
        : configuration(),
          dispatcher(&configuration),
          broadphase(),
          solver(),
          world(&dispatcher, &broadphase, &solver, &configuration),
          ghostPairCallback()
    {
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(&this->ghostPairCallback);
    }

    DynamicsWorld::~DynamicsWorld()
    {
        // Now we need to remove all of our rigid bodies. We go backwards here to avoid reshuffling of the internal buffer.
        for (int i = this->world.getNumCollisionObjects() - 1; i >= 0; --i)
        {
            auto obj  = this->world.getCollisionObjectArray()[i];
            auto body = btRigidBody::upcast(obj);

            if (body != nullptr)
            {
                this->removeRigidBody(body);
            }
            else
            {
                this->removeCollisionObject(obj);
            }
        }

        // We need to remove the ghost callback. Not doing this will result in a crash... Also, this must be done AFTER
        // removing all of the collision objects.
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(nullptr);
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



    void DynamicsWorld::rayTest(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld, btDynamicsWorld::RayResultCallback &resultCallback) const
    {
        this->world.rayTest(ToBulletVector3(rayFromWorld), ToBulletVector3(rayToWorld), resultCallback);
    }


    void DynamicsWorld::addRigidBody(RigidBody *body)
    {
        auto prevWorld = body->getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->removeRigidBody(body);
        }

        body->setWorld(this);
        this->world.addRigidBody(body);
    }

    void DynamicsWorld::addRigidBody(RigidBody *body, short group, short mask)
    {
        auto prevWorld = body->getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->removeRigidBody(body);
        }

        body->setWorld(this);
        this->world.addRigidBody(body, group, mask);
    }

    void DynamicsWorld::removeRigidBody(RigidBody* body)
    {
        body->setWorld(nullptr);
        this->world.removeRigidBody(body);
    }


    void DynamicsWorld::addRigidBody(btRigidBody *bodyIn)
    {
        auto body = static_cast<RigidBody*>(bodyIn);
        if (body != nullptr)
        {
            this->addRigidBody(body);
        }
        else
        {
            this->world.addRigidBody(bodyIn);
        }
    }

    void DynamicsWorld::addRigidBody(btRigidBody *bodyIn, short group, short mask)
    {
        auto body = static_cast<RigidBody*>(bodyIn);
        if (body != nullptr)
        {
            this->addRigidBody(body, group, mask);
        }
        else
        {
            this->world.addRigidBody(bodyIn, group, mask);
        }
    }

    void DynamicsWorld::removeRigidBody(btRigidBody* bodyIn)
    {
        auto body = static_cast<RigidBody*>(bodyIn);
        if (body != nullptr)
        {
            this->removeRigidBody(body);
        }
        else
        {
            this->world.removeRigidBody(bodyIn);
        }
    }


    void DynamicsWorld::addGhostObject(GhostObject *ghost)
    {
        auto prevWorld = ghost->getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->removeGhostObject(ghost);
        }

        ghost->setWorld(this);
        this->world.addCollisionObject(ghost);
    }

    void DynamicsWorld::addGhostObject(GhostObject *ghost, short group, short mask)
    {
        auto prevWorld = ghost->getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->removeGhostObject(ghost);
        }

        ghost->setWorld(this);
        this->world.addCollisionObject(ghost, group, mask);
    }

    void DynamicsWorld::removeGhostObject(GhostObject* ghost)
    {
        ghost->setWorld(nullptr);
        this->world.removeCollisionObject(ghost);
    }


    void DynamicsWorld::removeCollisionObject(btCollisionObject* objIn)
    {
        auto ghost = dynamic_cast<GhostObject *>(objIn);
        if (ghost != nullptr)
        {
            this->removeGhostObject(ghost);
        }
        else
        {
            this->world.removeCollisionObject(objIn);
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
