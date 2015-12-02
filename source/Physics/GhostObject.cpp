// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Physics.hpp>

namespace GT
{
    GhostObject::GhostObject()
        : world(nullptr)
    {
        this->setCollisionFlags(this->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    GhostObject::~GhostObject()
    {
        if (this->world != nullptr)
        {
            this->world->RemoveGhostObject(*this);
        }
    }

    void GhostObject::SetWorld(DynamicsWorld *worldIn)
    {
        this->world = worldIn;
    }

    DynamicsWorld * GhostObject::GetWorld()
    {
        return this->world;
    }
}
