
#ifndef __GTEngine_Physics_BaseCollisionWorld_hpp_
#define __GTEngine_Physics_BaseCollisionWorld_hpp_

#include "Bullet.hpp"
#include "CollisionObject.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    /// Class representing the base class for CollisionWorld and DynamicsWorld.
    ///
    /// Both CollisionWorld and DynamicsWorld is inheritted from this class.
    class BaseCollisionWorld
    {
    public:

        /// Constructor.
        BaseCollisionWorld();

        /// Destructor.
        virtual ~BaseCollisionWorld();


        /// Retrieves the internal collision configuration.
              btDefaultCollisionConfiguration & GetCollisionConfiguration()       { return this->configuration; }
        const btDefaultCollisionConfiguration & GetCollisionConfiguration() const { return this->configuration; } 

        /// Retrieves a reference to the internal collision dispatcher.
              btCollisionDispatcher & GetCollisionDispatcher()       { return this->dispatcher; }
        const btCollisionDispatcher & GetCollisionDispatcher() const { return this->dispatcher; }

        /// Retrieves a reference to the internal broadphase.
              btDbvtBroadphase & GetBroadphase()       { return this->broadphase; }
        const btDbvtBroadphase & GetBroadphase() const { return this->broadphase; }


        /// Adds a collision object to the world.
        ///
        /// @param object [in] A reference to the collision object to add.
        void AddCollisionObject(btCollisionObject &object, short int collisionFilterGroup = 1, short int collisionFilterMask = -1);
        void AddCollisionObject(  CollisionObject &object, short int collisionFilterGroup = 1, short int collisionFilterMask = -1);

        /// Removes a collision object from the world.
        ///
        /// @param object [in] A reference to the collision object to remove.
        void RemoveCollisionObject(btCollisionObject &object);
        void RemoveCollisionObject(  CollisionObject &object);


        /// Updates the AABB of the given object.
        ///
        /// @param object [in] A reference to the object whose AABB is being updated.
        void UpdateAABB(btCollisionObject &object);

        /// Updates the AABBs of every object.
        void UpdateAllAABBs();


        /// Performs a ray test.
        ///
        /// @param rayStart       [in     ] The start point of the ray.
        /// @param rayEnd         [in     ] The end point of the ray.
        /// @param resultCallback [in, out] A reference to the callback structure for handling the result.
        ///
        void RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &resultCallback) const;



        //////////////////////////////////////////////////////////////
        // Virtual Methods.

        /// Retrieves a reference to the internal dynamics world.
        virtual       btCollisionWorld & GetInternalWorld()       = 0;
        virtual const btCollisionWorld & GetInternalWorld() const = 0;


    protected:

        /// Collision configuration.
        btDefaultCollisionConfiguration configuration;

        /// Collision dispatcher.
        btCollisionDispatcher dispatcher;

        /// The broadphase.
        btDbvtBroadphase broadphase;
    };
}

#endif