// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    DefaultScenePhysicsManager::DefaultScenePhysicsManager()
        : world(), speedScale(1.0)
    {
    }

    DefaultScenePhysicsManager::~DefaultScenePhysicsManager()
    {
    }


    void DefaultScenePhysicsManager::AddCollisionObject(CollisionObject &object, short group, short mask)
    {
        this->world.AddCollisionObject(object, group, mask);
    }
    void DefaultScenePhysicsManager::AddRigidBody(RigidBody &body, short group, short mask)
    {
        this->world.AddRigidBody(body, group, mask);
        body.activate(true);                            // <-- We want to make sure the body is initially activated.
    }
    void DefaultScenePhysicsManager::AddGhostObject(GhostObject &object, short group, short mask)
    {
        this->world.AddGhostObject(object, group, mask);
    }


    void DefaultScenePhysicsManager::RemoveCollisionObject(CollisionObject &object)
    {
        this->world.RemoveCollisionObject(object);
    }
    void DefaultScenePhysicsManager::RemoveRigidBody(RigidBody &object)
    {
        this->world.RemoveRigidBody(object);
    }
    void DefaultScenePhysicsManager::RemoveGhostObject(GhostObject &object)
    {
        this->world.RemoveGhostObject(object);
    }



    void DefaultScenePhysicsManager::UpdateTransform(RigidBody &object, const glm::mat4 &newTransform, short group, short mask)
    {
        auto linearVelocity  = object.getLinearVelocity();
        auto angularVelocity = object.getAngularVelocity();

        auto objectWorld = object.GetWorld();
        if (objectWorld == &this->world)
        {
            this->world.RemoveRigidBody(object);
        }



        btTransform bulletTransform = ToBulletTransform(newTransform);

        object.setWorldTransform(bulletTransform);
        object.setInterpolationWorldTransform(bulletTransform);



        if (objectWorld == &this->world)
        {
            this->world.AddRigidBody(object, group, mask);
        }


        object.setLinearVelocity(linearVelocity);
        object.setAngularVelocity(angularVelocity);
    }

    void DefaultScenePhysicsManager::UpdateTransform(GhostObject &object, const glm::mat4 &newTransform)
    {
        auto objectWorld = object.GetWorld();
        assert(objectWorld == &this->world);
        {
            object.setWorldTransform(ToBulletTransform(newTransform));
            objectWorld->UpdateAABB(object);
        }
    }



    void DefaultScenePhysicsManager::AddConstraint(GenericConstraint &constraint)
    {
        this->world.AddConstraint(constraint);
    }

    void DefaultScenePhysicsManager::AddConstraint(ConeTwistConstraint &constraint)
    {
        this->world.AddConstraint(constraint);
    }

    void DefaultScenePhysicsManager::AddConstraint(PointToPointConstraint &constraint)
    {
        this->world.AddConstraint(constraint);
    }


    void DefaultScenePhysicsManager::RemoveConstraint(GenericConstraint &constraint)
    {
        this->world.RemoveConstraint(constraint);
    }

    void DefaultScenePhysicsManager::RemoveConstraint(ConeTwistConstraint &constraint)
    {
        this->world.RemoveConstraint(constraint);
    }

    void DefaultScenePhysicsManager::RemoveConstraint(PointToPointConstraint &constraint)
    {
        this->world.RemoveConstraint(constraint);
    }



    void DefaultScenePhysicsManager::SetGravity(float x, float y, float z)
    {
        this->world.SetGravity(x, y, z);
    }
    void DefaultScenePhysicsManager::GetGravity(float &x, float &y, float &z) const
    {
        this->world.GetGravity(x, y, z);
    }


    void DefaultScenePhysicsManager::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &callback) const
    {
        this->world.RayTest(rayStart, rayEnd, callback);
    }

    void DefaultScenePhysicsManager::ContactTest(const CollisionObject &object, btCollisionWorld::ContactResultCallback &callback) const
    {
        this->world.ContactTest(object, callback);
    }
    void DefaultScenePhysicsManager::ContactTest(const RigidBody &object, btCollisionWorld::ContactResultCallback &callback) const
    {
        this->world.ContactTest(object, callback);
    }
    void DefaultScenePhysicsManager::ContactTest(const GhostObject &object, btCollisionWorld::ContactResultCallback &callback) const
    {
        this->world.ContactTest(object, callback);
    }


    void DefaultScenePhysicsManager::Step(double deltaTimeInSeconds)
    {
        this->world.Step(static_cast<btScalar>(deltaTimeInSeconds * this->speedScale), 4);
    }


    void DefaultScenePhysicsManager::SetSpeedScale(double scale)
    {
        this->speedScale = scale;
    }




    void DefaultScenePhysicsManager::ActivateAllRigidBodies()
    {
        auto &objects = this->world.GetInternalDynamicsWorld().getCollisionObjectArray();
        
        for (int i = 0; i < objects.size(); ++i)
        {
            auto object = objects[i];
            assert(object != nullptr);
            {
                object->activate(true);
            }
        }
    }
}