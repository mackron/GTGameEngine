
#include <GTEngine/Components/ProximityComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ProximityComponent, "Proximity");

    ProximityComponent::ProximityComponent(SceneNode &node)
        : Component(node), ghostObject(), collisionShape(nullptr), collisionGroup(1), collisionMask(-1)
    {
        this->ghostObject.setUserPointer(&node);
    }

    ProximityComponent::~ProximityComponent()
    {
        auto world = this->ghostObject.getWorld();
        if (world != nullptr)
        {
            world->removeGhostObject(&this->ghostObject);
        }

        delete this->collisionShape;
    }

    void ProximityComponent::SetBoxShape(float halfX, float halfY, float halfZ)
    {
        this->SetShape(new btBoxShape(btVector3(halfX, halfY, halfZ)));
    }

    void ProximityComponent::SetSphereShape(float radius)
    {
        this->SetShape(new btSphereShape(radius));
    }

    void ProximityComponent::SetEllipsoidShape(float radiusX, float radiusY, float radiusZ)
    {
        this->SetShape(new btEllipsoidShape(btVector3(radiusX, radiusY, radiusZ)));
    }

    
    void ProximityComponent::SetCollisionFilter(short group, short mask)
    {
        if (this->collisionGroup != group || this->collisionMask != mask)
        {
            this->collisionGroup = group;
            this->collisionMask  = mask;

            // The body needs to be removed and re-added to it's world for changes to take effect.
            auto world = this->ghostObject.getWorld();
            if (world != nullptr)
            {
                world->removeGhostObject(&this->ghostObject);
                world->addGhostObject(&this->ghostObject, this->collisionGroup, this->collisionMask);
            }
        }
    }


    void ProximityComponent::ApplyScaling(float x, float y, float z)
    {
        if (this->collisionShape != nullptr)
        {
            // The geometry is changing, thus we need to remove the body from the world and re-add it after the changes.
            auto world = this->ghostObject.getWorld();
            if (world != nullptr)
            {
                world->removeGhostObject(&this->ghostObject);
            }

            // Now we simply apply the scaling to the shape.
            this->collisionShape->setLocalScaling(btVector3(x, y, z));

            // Now we need to re-add the body.
            if (world != nullptr)
            {
                world->addGhostObject(&this->ghostObject, this->collisionGroup, this->collisionMask);
            }
        }
    }


    void ProximityComponent::SetShape(btCollisionShape* newShape)
    {
        auto oldShape = this->collisionShape;

        auto world = this->ghostObject.getWorld();
        if (world != nullptr)
        {
            world->removeGhostObject(&this->ghostObject);
        }

        this->ghostObject.setCollisionShape(newShape);

        if (world != nullptr)
        {
            world->addGhostObject(&this->ghostObject, this->collisionGroup, this->collisionMask);
        }

        delete oldShape;
    }
}



// Iterator
namespace GTEngine
{
    ProximityComponent::Iterator & ProximityComponent::Iterator::operator++()
    {
        this->otherNode = nullptr;

        auto world = this->component.ghostObject.getWorld();
        if (world != nullptr)
        {
            auto &pairArray = this->component.ghostObject.getOverlappingPairCache()->getOverlappingPairArray();

            GTEngine::SceneNode* nextNode = nullptr;

            while (nextNode == nullptr && this->i < pairArray.size())
            {
                this->manifoldArray.clear();

                auto &pair = pairArray[this->i];

                auto collisionPair = world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
                if (collisionPair != nullptr)
                {
                    auto bodyA = static_cast<btCollisionObject*>(pair.m_pProxy0->m_clientObject);
                    auto bodyB = static_cast<btCollisionObject*>(pair.m_pProxy1->m_clientObject);

                    if (bodyA == &this->component.ghostObject)
                    {
                        nextNode = static_cast<GTEngine::SceneNode*>(bodyB->getUserPointer());
                    }
                    else if (bodyB == &this->component.ghostObject)
                    {
                        nextNode = static_cast<GTEngine::SceneNode*>(bodyA->getUserPointer());
                    }

                        
                    if (nextNode != nullptr)
                    {
                        if (this->component.node.IsRelated(*nextNode))
                        {
                            nextNode = nullptr;
                        }
                        else
                        {
                            // Here is where we check the contact manifolds. If we don't find anything here, the nodes aren't touching.
                            if (collisionPair->m_algorithm)
                            {
                                collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
                            }

                            for (int j = 0; j < manifoldArray.size(); ++j)
                            {
                                auto manifold = manifoldArray[j];
                                if (manifold->getNumContacts() == 0)
                                {
                                    nextNode = nullptr;
                                }
                            }
                        }
                    }
                }

                ++this->i;
            }

            this->otherNode = nextNode;
        }

        return *this;
    }


    ProximityComponent::Iterator::operator bool() const
    {
        return this->otherNode != nullptr;
    }
}
