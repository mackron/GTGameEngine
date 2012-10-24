
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    DefaultScenePhysicsManager::DefaultScenePhysicsManager()
        : world()
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



    void DefaultScenePhysicsManager::AddConstraint(GenericConstraint &constraint)
    {
        this->world.AddConstraint(constraint);
    }

    void DefaultScenePhysicsManager::RemoveConstraint(GenericConstraint &constraint)
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
        this->world.Step(static_cast<btScalar>(deltaTimeInSeconds), 4);
    }
}