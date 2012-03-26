
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GhostObject::GhostObject()
        : world(nullptr)
    {
    }

    GhostObject::~GhostObject()
    {
        if (this->world != nullptr)
        {
            this->world->removeGhostObject(this);
        }
    }

    void GhostObject::setWorld(DynamicsWorld *world)
    {
        this->world = world;
    }

    DynamicsWorld * GhostObject::getWorld()
    {
        return this->world;
    }
}
