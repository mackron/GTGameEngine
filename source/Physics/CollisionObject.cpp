// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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

    void CollisionObject::SetWorld(BaseCollisionWorld* world)
    {
        this->world = world;
    }

    BaseCollisionWorld* CollisionObject::GetWorld()
    {
        return this->world;
    }
}
