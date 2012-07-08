
#ifndef __GTEngine_DefaultScenePhysicsManager_hpp_
#define __GTEngine_DefaultScenePhysicsManager_hpp_

#include "ScenePhysicsManager.hpp"
#include "Physics.hpp"

namespace GTEngine
{
    /// The default physics manager for scenes.
    ///
    /// This can oftern act as the base class for custom managers instead of ScenePhysicsManager.
    class DefaultScenePhysicsManager : public ScenePhysicsManager
    {
    public:

        /// Constructor.
        DefaultScenePhysicsManager();

        /// Destructor.
        virtual ~DefaultScenePhysicsManager();


        /// ScenePhysicsManager::AddCollisionObject().
        virtual void AddCollisionObject(CollisionObject &object, short group, short mask);

        /// ScenePhysicsManager::AddRigidBody().
        virtual void AddRigidBody(RigidBody &body, short group, short mask);

        /// ScanePhysicsManager::AddGhostObject().
        virtual void AddGhostObject(GhostObject &object, short group, short mask);


        /// ScenePhysicsManager::RemoveCollisionObject().
        virtual void RemoveCollisionObject(CollisionObject &object);

        /// ScenePhysicsManager::RemoveRigidBody().
        virtual void RemoveRigidBody(RigidBody &object);

        /// ScenePhysicsManager::RemoveGhostObject().
        virtual void RemoveGhostObject(GhostObject &object);



        /// ScenePhysicsManager::SetGravity().
        virtual void SetGravity(float x, float y, float z);

        /// ScenePhysicsManager::GetGravity().
        virtual void GetGravity(float &x, float &y, float &z) const;


        /// ScenePhysicsManager::RayTest().
        virtual void RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &callback) const;

        /// ScenePhysicsManager::ContactTest().
        virtual void ContactTest(const CollisionObject &object, btCollisionWorld::ContactResultCallback &callback) const;
        virtual void ContactTest(const RigidBody       &object, btCollisionWorld::ContactResultCallback &callback) const;
        virtual void ContactTest(const GhostObject     &object, btCollisionWorld::ContactResultCallback &callback) const;


        /// ScenePhysicsManager::Step().
        virtual void Step(double deltaTimeInSeconds);



    protected:

        /// The dynamics world containing all of our physics objects.
        DynamicsWorld world;
    };
}

#endif