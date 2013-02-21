// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/CollisionShapeComponent.hpp>

namespace GTEngine
{
    CollisionShapeComponent::CollisionShapeComponent(SceneNode &node)
        : Component(node),
          collisionShape(true)
    {
    }

    CollisionShapeComponent::~CollisionShapeComponent()
    {
        // Now we need to delete the collision shapes.
        while (this->collisionShape.getNumChildShapes() > 0)
        {
            auto child = this->collisionShape.getChildShape(0);
            this->collisionShape.removeChildShapeByIndex(0);

            delete child;
        }
    }


    size_t CollisionShapeComponent::GetCollisionShapeCount() const
    {
        return static_cast<size_t>(this->collisionShape.getNumChildShapes());
    }


    btCollisionShape & CollisionShapeComponent::GetCollisionShapeAtIndex(size_t index)
    {
        assert(index < this->GetCollisionShapeCount());
        {
            auto shape = this->collisionShape.getChildShape(index);
            assert(shape != nullptr);
            {
                return *shape;
            }
        }
    }

    const btCollisionShape & CollisionShapeComponent::GetCollisionShapeAtIndex(size_t index) const
    {
        assert(index < this->GetCollisionShapeCount());
        {
            auto shape = this->collisionShape.getChildShape(index);
            assert(shape != nullptr);
            {
                return *shape;
            }
        }
    }



}