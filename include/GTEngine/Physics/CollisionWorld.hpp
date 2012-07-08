
#ifndef __GTEngine_Physics_CollisionWorld_hpp_
#define __GTEngine_Physics_CollisionWorld_hpp_

#include "BaseCollisionWorld.hpp"

namespace GTEngine
{
    /// Class representing a world for doing collision detection.
    class CollisionWorld : public BaseCollisionWorld
    {
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