
#ifndef __GTEngine_Physics_DynamicsWorld_hpp_
#define __GTEngine_Physics_DynamicsWorld_hpp_

#include "BaseCollisionWorld.hpp"
#include "RigidBody.hpp"
#include "GhostObject.hpp"
#include "GenericConstraint.hpp"
#include "ConeTwistConstraint.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4324)
#endif

namespace GTEngine
{
    /// A dynamics world for doing physics.
    class DynamicsWorld : public BaseCollisionWorld
    {
    public:

        /// Constructor.
        DynamicsWorld();

        /// Destructor.
        virtual ~DynamicsWorld();


        /// Retrieves a reference to the internal dynamics world.
              btCollisionWorld & GetInternalWorld()       { return this->world; }
        const btCollisionWorld & GetInternalWorld() const { return this->world; }

        /// Retrieves a reference to the internal dynamics world.
              btDiscreteDynamicsWorld & GetInternalDynamicsWorld()       { return this->world; }
        const btDiscreteDynamicsWorld & GetInternalDynamicsWorld() const { return this->world; }

        /// Retrieves a reference to the internal constrain solver.
              btSequentialImpulseConstraintSolver & GetConstraintSolver()       { return this->solver; }
        const btSequentialImpulseConstraintSolver & GetConstraintSolver() const { return this->solver; }


        /// Adds a rigid body to the world.
        ///
        /// @param body [in] A reference to the body to add to the world.
        void AddRigidBody(btRigidBody &body, short group = 1, short mask = -1);
        void AddRigidBody(  RigidBody &body, short group = 1, short mask = -1);

        /// Removes a rigid body from the world.
        ///
        /// @param body [in] A reference to the body to remove from the world.
        void RemoveRigidBody(btRigidBody &body);
        void RemoveRigidBody(  RigidBody &body);


        /// Adds a ghost object to the world.
        ///
        /// @param ghost [in] A reference to the ghost object to add to the world.
        void AddGhostObject(btGhostObject &ghost, short group = 1, short mask = -1);
        void AddGhostObject(  GhostObject &ghost, short group = 1, short mask = -1);

        /// Removes a ghost object from the world.
        ///
        /// @param ghost [in] A reference to the body to remove from the world.
        void RemoveGhostObject(btGhostObject &ghost);
        void RemoveGhostObject(  GhostObject &ghost);



        /// Adds a constraint to the world.
        ///
        /// @param constraint                           [in] The constraint to add to the world.
        /// @param disableCollisionsBetweenLinkedBodies [in] Whether or not to disable collisions between the bodies linked to the constraint.
        void AddConstraint(btTypedConstraint &constraint, bool disableCollisionsBetweenLinkedBodies = false);
        void AddConstraint(GenericConstraint &constraint, bool disableCollisionsBetweenLinkedBodies = false);
        void AddConstraint(ConeTwistConstraint &constraint, bool disableCollisionsBetweenLinkedBodies = false);

        /// Removes a constraint from the world.
        ///
        /// @param constraint [in] The constraint to remove from the world.
        void RemoveConstraint(btTypedConstraint &constraint);
        void RemoveConstraint(GenericConstraint &constraint);
        void RemoveConstraint(ConeTwistConstraint &constraint);



        /// Steps the dynamics simulation.
        void Step(double timeStep, int maxSubSteps = 1, double fixedTimeStep = 1.0 / 60.0);


        /// Sets the gravity.
        void SetGravity(float x, float y, float z);
        void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the gravity.
        void GetGravity(float &x, float &y, float &z) const;
        void GetGravity(glm::vec3 &gravity) const { this->GetGravity(gravity.x, gravity.y, gravity.z); }

        glm::vec3 GetGravity() const
        {
            glm::vec3 gravity;
            this->GetGravity(gravity.x, gravity.y, gravity.z);

            return gravity;
        }



    private:

        /// The constraint solver.
        btSequentialImpulseConstraintSolver solver;

        /// The main dynamics world.
        btDiscreteDynamicsWorld world;

        /// The callback for ghost object collision detection.
        btGhostPairCallback ghostPairCallback;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif


#endif