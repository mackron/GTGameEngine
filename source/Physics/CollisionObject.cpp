
#include <GTEngine/Physics/CollisionObject.hpp>
#include <GTEngine/Physics/CollisionWorld.hpp>

namespace GTEngine
{
    CollisionObject::CollisionObject()
        : world(nullptr)
    {
    }

    CollisionObject::~CollisionObject()
    {
        if (this->world != nullptr)
        {
            this->world->removeCollisionObject(this);
        }
    }

    void CollisionObject::setWorld(CollisionWorld *world)
    {
        this->world = world;
    }

    CollisionWorld * CollisionObject::getWorld()
    {
        return this->world;
    }
}
