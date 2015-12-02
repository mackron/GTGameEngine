// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Physics_CollisionWorld
#define GT_Physics_CollisionWorld

#include "BaseCollisionWorld.hpp"

namespace GT
{
    /// Class representing a world for doing collision detection.
    class CollisionWorld : public BaseCollisionWorld
    {
    public:

        struct ClosestRayTestCallback : public btCollisionWorld::ClosestRayResultCallback
        {
            ClosestRayTestCallback(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld)
                : btCollisionWorld::ClosestRayResultCallback(btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z), btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z))
            {
            }
        };



    public:

        /// Constructor.
        CollisionWorld();

        /// Destructor.
        virtual ~CollisionWorld();

        /// Retrieves a reference to the internal dynamics world.
              btCollisionWorld & GetInternalWorld()       { return this->world; }
        const btCollisionWorld & GetInternalWorld() const { return this->world; }


    private:

        /// The main dynamics world.
        btCollisionWorld world;
    };
}

#endif