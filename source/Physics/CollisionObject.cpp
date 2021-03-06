// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Physics/CollisionObject.hpp>
#include <GTGE/Physics/BaseCollisionWorld.hpp>

namespace GT
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

    void CollisionObject::SetWorld(BaseCollisionWorld* worldIn)
    {
        this->world = worldIn;
    }

    BaseCollisionWorld* CollisionObject::GetWorld()
    {
        return this->world;
    }
}
