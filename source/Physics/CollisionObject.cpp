
#include <GTEngine/Physics/CollisionObject.hpp>
#include <GTEngine/Physics/BaseCollisionWorld.hpp>

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
            this->world->RemoveCollisionObject(*this);
        }
    }

    void CollisionObject::setWorld(BaseCollisionWorld* world)
    {
        this->world = world;
    }

    BaseCollisionWorld* CollisionObject::getWorld()
    {
        return this->world;
    }
}
