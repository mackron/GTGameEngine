
#ifndef __GTEngine_Physics_CollisionObject_hpp_
#define __GTEngine_Physics_CollisionObject_hpp_

#include "Bullet.hpp"

namespace GTEngine
{
    class BaseCollisionWorld;

    /**
    *   \brief  GTEngine rigid body class.
    *
    *   We make this class make it suit our own style more closely. The main thing we add here is a property containing a
    *   pointer to the scene the body is currently contained in. It is set by DynamicsScene when it is added. What this
    *   allows is for us to remove the body from the dynamics world when the body is deleted.
    */
    class CollisionObject : public btCollisionObject
    {
    public:

        /**
        *   \brief  Constructor.
        */
        CollisionObject();

        /**
        *   \brief  Destructor.
        */
        virtual ~CollisionObject();


        /**
        *   \brief  Simple scene setter. Called by DynamicsWorld.
        */
        void setWorld(BaseCollisionWorld* world);

        /**
        *   \brief  Retrieves a pointer to the world these body is part of.
        */
        BaseCollisionWorld* getWorld();


    private:

        /// A pointer to the world that owns this object.
        BaseCollisionWorld* world;
    };
}

#endif