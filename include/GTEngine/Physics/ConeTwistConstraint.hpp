
#ifndef __GTEngine_Physics_ConeTwistConstraint_hpp_
#define __GTEngine_Physics_ConeTwistConstraint_hpp_

#include "Bullet.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    class DynamicsWorld;
    class RigidBody;

    /// Class representing a cone twist constraint.
    class ConeTwistConstraint : public btConeTwistConstraint
    {
    public:

        /// Constructor.
        ConeTwistConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB);
        ConeTwistConstraint(RigidBody &bodyB, const glm::mat4 &frameB);

        /// Destructor.
        ~ConeTwistConstraint();


        /// Sets the attachments.
        ///
        /// @param bodyA  [in] A reference to the first object to attach to the constraint.
        /// @param bodyB  [in] A reference to the second object to attach to the constraint.
        /// @param frameA [in] The transformation frame for objectA.
        /// @param frameB [in] The transformation frame for objectB.
        void SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::mat4 &frameA, const glm::mat4 &frameB);

        /// Sets the attachments.
        ///
        /// @param bodyB  [in] A reference to the second object to attach to the constraint.
        /// @param frameB [in] The transformation frame for objectB.
        void SetAttachments(RigidBody &bodyB, const glm::mat4 &frameB);


        /// Sets the limits.
        void SetLimits(float swingSpan1, float swingSpan2, float twistSpan);



        /// Simple setter for setting the world that this constraint is contained in.
        ///
        /// @param newWorld [in] A pointer to the world that now owns this constraint.
        ///
        /// @remarks
        ///     This should only be used internally by DynamicsWorld.
        ///     @par
        ///     This will not remove the constraint from the previous world - it is just a simple setter.
        void SetWorld(DynamicsWorld* newWorld) { this->world = newWorld; }

        /// Retrieves a pointer to the world that contains this constraint.
        DynamicsWorld* GetWorld() { return this->world; }


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


    private:    // No copying
        ConeTwistConstraint(const ConeTwistConstraint &);
        ConeTwistConstraint & operator=(const ConeTwistConstraint &);
    };
}

#endif
