// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScenePhysicsManager_hpp_
#define __GTEngine_ScenePhysicsManager_hpp_

#include "Physics.hpp"

namespace GTEngine
{
    /// Base class for managing the physics in a scene.
    class ScenePhysicsManager
    {
    public:

        /// Destructor.
        virtual ~ScenePhysicsManager() {}


        /// Adds a collision object.
        virtual void AddCollisionObject(CollisionObject &object, short group, short mask) = 0;
        virtual void AddRigidBody(RigidBody &body, short group, short mask) = 0;
        virtual void AddGhostObject(GhostObject &object, short group, short mask) = 0;

        /// Removes a collision object.
        virtual void RemoveCollisionObject(CollisionObject &object) = 0;
        virtual void RemoveRigidBody(RigidBody &object) = 0;
        virtual void RemoveGhostObject(GhostObject &object) = 0;



        /// Updates the transformation of a rigid body.
        virtual void UpdateTransform(RigidBody &object,   const glm::mat4 &newTransform) = 0;
        virtual void UpdateTransform(GhostObject &object, const glm::mat4 &newTransform) = 0;



        /// Adds a constraint.
        virtual void AddConstraint(GenericConstraint &constraint) = 0;
        virtual void AddConstraint(ConeTwistConstraint &constraint) = 0;
        virtual void AddConstraint(PointToPointConstraint &constraint) = 0;
        
        /// Removes a constraint.
        virtual void RemoveConstraint(GenericConstraint &constraint) = 0;
        virtual void RemoveConstraint(ConeTwistConstraint &constraint) = 0;
        virtual void RemoveConstraint(PointToPointConstraint &constraint) = 0;


        /// Sets the gravity.
        virtual void SetGravity(float x, float y, float z) = 0;

        /// Retrieves the gravity.
        virtual void GetGravity(float &x, float &y, float &z) const = 0;


        /// Performs a ray test against the objects in the physics world.
        virtual void RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &callback) const = 0;

        /// Performs a contact test between the given object and every other object.
        virtual void ContactTest(const CollisionObject &object, btCollisionWorld::ContactResultCallback &callback) const = 0;
        virtual void ContactTest(const RigidBody       &object, btCollisionWorld::ContactResultCallback &callback) const = 0;
        virtual void ContactTest(const GhostObject     &object, btCollisionWorld::ContactResultCallback &callback) const = 0;
    

        /// Performs the update step.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds (time between updates).
        virtual void Step(double deltaTimeInSeconds) = 0;


        /// Sets the scale to apply to the entire physics simulation.
        ///
        /// @param scale [in] The speed scale to apply to the entire physics simulation.
        ///
        /// @remarks
        ///     Defaults to 1.0.
        virtual void SetSpeedScale(double scale) = 0;

    };
}

#endif
