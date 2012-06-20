
#ifndef __GTEngine_Physics_DynamicsWorld_hpp_
#define __GTEngine_Physics_DynamicsWorld_hpp_

#include "Bullet.hpp"
#include "RigidBody.hpp"
#include "GhostObject.hpp"
#include "../Math.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4324)
#endif

namespace GTEngine
{
    /**
    *   \brief  An simplified dynamics world for physics.
    *
    *   \remarks
    *       In order to inherit directly from btDiscreteDynamicsWorld, we need to use multiple inheritance. It doesn't make logical sense to do this,
    *       but I think it's best that our own world uses an 'is-a' relationship instead of a 'has-a' relationship with btDiscreteDynamicsWorld.
    */
    class DynamicsWorld
    {
    public:

        /**
        *   \brief  Constructor.
        */
        DynamicsWorld();

        /**
        *   \brief  Destructor.
        */
        virtual ~DynamicsWorld();


        /// Retrieves the internal collision configuration.
        btDefaultCollisionConfiguration & GetCollisionConfiguration() { return this->configuration; }
        const btDefaultCollisionConfiguration & GetCollisionConfiguration() const { return this->configuration; } 

        /// Retrieves a reference to the internal collision dispatcher.
        btCollisionDispatcher & GetCollisionDispatcher() { return this->dispatcher; }
        const btCollisionDispatcher & GetCollisionDispatcher() const { return this->dispatcher; }

        /// Retrieves a reference to the internal broadphase.
        btDbvtBroadphase & GetBroadphase() { return this->broadphase; }
        const btDbvtBroadphase & GetBroadphase() const { return this->broadphase; }

        /// Retrieves a reference to the internal constrain solver.
        btSequentialImpulseConstraintSolver & GetConstraintSolver() { return this->solver; }
        const btSequentialImpulseConstraintSolver & GetConstraintSolver() const { return this->solver; }

        /// Retrieves a reference to the internal dynamics world.
        btDiscreteDynamicsWorld & GetInternalDynamicsWorld() { return this->world; }
        const btDiscreteDynamicsWorld & GetInternalDynamicsWorld() const { return this->world; }


        /// Steps the dynamics simulation.
        void Step(double timeStep, int maxSubSteps = 1, double fixedTimeStep = 1.0 / 60.0);


        /// Sets the gravity.
        void SetGravity(float x, float y, float z);
        void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the gravity.
        void GetGravity(float &x, float &y, float &z) const;
        glm::vec3 GetGravity() const
        {
            glm::vec3 gravity;
            this->GetGravity(gravity.x, gravity.y, gravity.z);

            return gravity;
        }




    // Custom functions for simplifying some stuff.
    public:

        /// Wrapper around rayTest() for use with glm::vec3.
        virtual void rayTest(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld, btDynamicsWorld::RayResultCallback &resultCallback) const;

        virtual void addRigidBody(RigidBody* body);
	    virtual void addRigidBody(RigidBody* body, short group, short mask);
        virtual void removeRigidBody(RigidBody* body);

        virtual void addRigidBody(btRigidBody* body);
        virtual void addRigidBody(btRigidBody* body, short group, short mask);
        virtual void removeRigidBody(btRigidBody* body);

        virtual void addGhostObject(GhostObject* ghost);
        virtual void addGhostObject(GhostObject* ghost, short group, short mask);
        virtual void removeGhostObject(GhostObject* ghost);

        virtual void removeCollisionObject(btCollisionObject* obj);

    private:

        /// Collision configuration.
        btDefaultCollisionConfiguration configuration;

        /// Collision dispatcher.
        btCollisionDispatcher dispatcher;

        /// The broadphase.
        btDbvtBroadphase broadphase;

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