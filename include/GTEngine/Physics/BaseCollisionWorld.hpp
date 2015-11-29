// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Physics_BaseCollisionWorld_hpp_
#define __GTEngine_Physics_BaseCollisionWorld_hpp_

#include "Bullet.hpp"
#include "CollisionObject.hpp"
#include "../Math.hpp"

namespace GT
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
        void RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &resultCallback) const;

        /// Performs a contact test between an object and all other objects in the scene.
        ///
        /// @param object         [in     ] A reference to the object to test.
        /// @param resultCallback [in, out] A reference to the callback structure for handling the result.
        void ContactTest(const btCollisionObject &object, btCollisionWorld::ContactResultCallback &resultCallback) const;

        /// Performs a contact test between two objects.
        ///
        /// @param objectA        [in     ] The first object to test.
        /// @param objectB        [in     ] The second object to test.
        /// @param resultCallback [in, out] A reference to the callback structure for handling the result.
        void ContactTest(const btCollisionObject &objectA, const btCollisionObject &objectB, btCollisionWorld::ContactResultCallback &resultCallback) const;

        /// Performs a swept test againts the given convex collision shape and every object in the scene.
        ///
        /// @param shape                 [in     ] A reference to the convex shape to test against.
        /// @param from                  [in     ] The transformation of the start point.
        /// @param end                   [in     ] The transformation of the end point.
        /// @param resultCallback        [in, out] A reference to the callback structure for handling the result.
        /// @param allowedCCDPenetration [in     ] Defaults to 0.0f.
        void ConvexSweepTest(const btConvexShape &shape, const glm::mat4 &from,   const glm::mat4 &to,   btCollisionWorld::ConvexResultCallback &resultCallback, float allowedCCDPenetration = 0.0f);
        void ConvexSweepTest(const btConvexShape &shape, const btTransform &from, const btTransform &to, btCollisionWorld::ConvexResultCallback &resultCallback, float allowedCCDPenetration = 0.0f);


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