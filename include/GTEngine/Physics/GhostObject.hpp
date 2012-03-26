
#ifndef __GTEngine_Physics_GhostObject_hpp_
#define __GTEngine_Physics_GhostObject_hpp_

#include "Bullet.hpp"

namespace GTEngine
{
    class DynamicsWorld;

    class GhostObject : public btPairCachingGhostObject
    {
    public:

        GhostObject();
        virtual ~GhostObject();

        /**
        *   \brief  Simple world setter. Called by DynamicsWorld.
        *
        *   \remarks
        *       This should only ever be used internally by DynamicsWorld. Do NOT use this to add the ghost object to the world. Use DynamicsWorld::addGhostObject().
        */
        void setWorld(DynamicsWorld *world);

        /**
        *   \brief  Retrieves a pointer to the world these body is part of.
        */
        DynamicsWorld * getWorld();


    private:

        /// A pointer to the dynamics world.
        DynamicsWorld *world;
    };
}

#endif
