// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    CollisionWorld::CollisionWorld()
        : BaseCollisionWorld(),
          world(&dispatcher, &broadphase, &configuration)
    {
    }

    CollisionWorld::~CollisionWorld()
    {
        // Now we need to remove all of our rigid bodies. We go backwards here to avoid reshuffling of the internal buffer.
        for (int i = this->world.getNumCollisionObjects() - 1; i >= 0; --i)
        {
            this->RemoveCollisionObject(*this->world.getCollisionObjectArray()[i]);
        }
    }
}
