
#ifndef __GTEngine_Physics_GenericConstraint_hpp_
#define __GTEngine_Physics_GenericConstraint_hpp_

#include "Bullet.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    class DynamicsWorld;
    class RigidBody;


    /// Custom class for a generic 6 DOF constraint. Mainly used to make it easier to gain access to the containing dynamics world.
    class GenericConstraint : public btGeneric6DofConstraint
    {
    public:

        /// Constructor.
        GenericConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB);

        /// Destructor.
        ///
        /// This will remove the constraint from the containing world.
        ~GenericConstraint();


        /// Sets the attachments.
        ///
        /// @param bodyA  [in] A reference to the first object to attach to the constraint.
        /// @param bodyB  [in] A reference to the second object to attach to the constraint.
        /// @param frameA [in] The transformation frame for objectA.
        /// @param frameB [in] The transformation frame for objectB.
        void SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB);


        /// Simple setter for setting the world that this constraint is contained in.
        ///
        /// @param newWorld [in] A pointer to the world that now owns this constraint.
        ///
        /// @remarks
        ///     This should only be used internally by DynamicsWorld.
        ///     @par
        ///     This will not remove the constraint from the previous world - it is just a simple setter.
        void SetWorld(DynamicsWorld* newWorld);

        /// Retrieves a pointer to the world that contains this constraint.
        DynamicsWorld* GetWorld();


        /// Sets whether or not collisions between linked bodies is enabled.
        ///
        /// @remarks
        ///     This is just a simple setter and should only be used internally by DynamicsWorld.
        void IsCollisionBetweenLinkedBodiesDisabled(bool set) { this->isCollisionBetweenLinkedBodiesDisabled = set; }

        /// Determines whether or not collisions between linked bodies is enabled.
        bool IsCollisionBetweenLinkedBodiesDisabled() const { return this->isCollisionBetweenLinkedBodiesDisabled; }



    private:

        /// The world that contains the constraint.
        DynamicsWorld* world;

        /// Keeps track of whether or not collisions between linked bodies are disabled.
        bool isCollisionBetweenLinkedBodiesDisabled;
    };
}

#endif