
#ifndef __GTEngine_Physics_PointToPointConstraint_hpp_
#define __GTEngine_Physics_PointToPointConstraint_hpp_

#include "Bullet.hpp"
#include "../Math.hpp"

namespace GTEngine
{
    class DynamicsWorld;
    class RigidBody;

    /// Class representing a point-to-point constraint.
    class PointToPointConstraint : public btPoint2PointConstraint
    {
    public:

        /// Constructor.
        PointToPointConstraint(RigidBody &bodyA, RigidBody &bodyB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB);
        PointToPointConstraint(RigidBody &bodyB, const glm::vec3 &pivotInB);

        /// Destructor.
        ~PointToPointConstraint();


        /// Sets the attachments.
        ///
        /// @param bodyA    [in] A reference to the first object to attach to the constraint.
        /// @param bodyB    [in] A reference to the second object to attach to the constraint.
        /// @param pivotInA [in] The pivot location for objectA.
        /// @param pivotInB [in] The pivot location for objectB.
        void SetAttachments(RigidBody &bodyA, RigidBody &bodyB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB);

        /// Sets the attachments.
        ///
        /// @param bodyB    [in] A reference to the second object to attach to the constraint.
        /// @param pivotInB [in] The pivot location for objectB.
        void SetAttachments(RigidBody &bodyB, const glm::vec3 &pivotInB);



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


    private:    // No copying.
        PointToPointConstraint(const PointToPointConstraint &);
        PointToPointConstraint & operator=(const PointToPointConstraint &);
    };
}

#endif
