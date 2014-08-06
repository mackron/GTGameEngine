// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/OccluderComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(OccluderComponent, "Occluder")

    OccluderComponent::OccluderComponent(SceneNode &node)
        : Component(node), collisionObject(), collisionShape(nullptr)
    {
        this->collisionObject.setUserPointer(&node);
    }

    OccluderComponent::~OccluderComponent()
    {
        auto world = this->collisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->collisionObject);
        }

        delete this->collisionShape;
    }

    void OccluderComponent::SetBoxShape(float halfX, float halfY, float halfZ)
    {
        auto oldShape = this->collisionShape;
        this->collisionShape = new btBoxShape(btVector3(halfX, halfY, halfZ));

        auto world = this->collisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->collisionObject);
        }

        this->collisionObject.setCollisionShape(this->collisionShape);

        if (world != nullptr)
        {
            world->AddCollisionObject(this->collisionObject);
        }

        delete oldShape;


        this->OnChanged();
    }


    void OccluderComponent::ApplyScaling(float x, float y, float z)
    {
        if (this->collisionShape != nullptr)
        {
            // The geometry is changing, thus we need to remove the body from the world and re-add it after the changes.
            auto world = this->collisionObject.GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(this->collisionObject);
            }

            // Now we simply apply the scaling to the shape.
            this->collisionShape->setLocalScaling(btVector3(x, y, z));

            // Now we need to re-add the body.
            if (world != nullptr)
            {
                world->AddCollisionObject(this->collisionObject);
            }
        }
    }
}
