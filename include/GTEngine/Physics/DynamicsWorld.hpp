
#ifndef __GTEngine_Physics_DynamicsWorld_hpp_
#define __GTEngine_Physics_DynamicsWorld_hpp_

#include "Bullet.hpp"
#include "RigidBody.hpp"
#include "GhostObject.hpp"
#include "../Math.hpp"

// I don't like it, but there are some warnings about overloaded virtual functions that need to be hidden.
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Woverloaded-virtual"
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
    class DynamicsWorld : public btDefaultCollisionConfiguration, public btCollisionDispatcher, public btDbvtBroadphase, public btSequentialImpulseConstraintSolver, public btDiscreteDynamicsWorld
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


    // Custom functions for simplifying some stuff.
    public:

        /// Wrapper around rayTest() for use with glm::vec3.
        virtual void rayTest(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld, RayResultCallback &resultCallback) const;

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

        /// The callback for ghost object collision detection.
        btGhostPairCallback ghostPairCallback;
    };
}

#if defined(__GNUC__)
    #pragma GCC diagnostic push
#endif

#endif