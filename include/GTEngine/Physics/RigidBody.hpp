// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Physics_RigidBody_hpp_
#define __GTEngine_Physics_RigidBody_hpp_

#include "Bullet.hpp"

namespace GTEngine
{
    class DynamicsWorld;

    /**
    *   \brief  GTEngine rigid body class.
    *
    *   We make this class make it suit our own style more closely. The main thing we add here is a property containing a
    *   pointer to the scene the body is currently contained in. It is set by DynamicsScene when it is added. What this
    *   allows is for us to remove the body from the dynamics world when the body is destructed.
    */
    ATTRIBUTE_ALIGNED16(class) RigidBody : public btRigidBody
    {
    public:

        /**
        *   \brief  Constructor.
        */
        RigidBody(btScalar mass, const btCollisionShape &collisionShape, const btVector3 &localInertia, btMotionState *motionState);
        RigidBody(btScalar mass, const btCollisionShape &collisionShape, btMotionState* motionState);
        RigidBody(const btRigidBodyConstructionInfo &constructionInfo);

        /**
        *   \brief  Destructor.
        */
        virtual ~RigidBody();


        /**
        *   \brief  Simple world setter. Called by DynamicsWorld.
        *
        *   \remarks
        *       This should only ever be used internally by DynamicsWorld. Do NOT use this to add the rigid body to the world. Use DynamicsWorld::addRigidBody().
        */
        void SetWorld(DynamicsWorld* world);

        /**
        *   \brief  Retrieves a pointer to the world these body is part of.
        */
        DynamicsWorld* GetWorld();


    private:

        /// A pointer to the dynamics world.
        DynamicsWorld* world;


    private:    // No copying.
        RigidBody(const RigidBody &);
        RigidBody & operator=(const RigidBody &);
    };
}

#endif
