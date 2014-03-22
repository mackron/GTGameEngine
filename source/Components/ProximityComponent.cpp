// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ProximityComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ProximityComponent, "Proximity")

    ProximityComponent::ProximityComponent(SceneNode &node)
        : CollisionShapeComponent(node), ghostObject(), world(nullptr), sceneNodesInsideVolume()
    {
        this->ghostObject.setCollisionShape(&this->collisionShape);
        this->ghostObject.setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        this->ghostObject.setUserPointer(&node);
    }

    ProximityComponent::~ProximityComponent()
    {
        auto world = this->ghostObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveGhostObject(this->ghostObject);
        }
    }


    void ProximityComponent::ApplySceneNodeTransformation()
    {
        btTransform transform;
        this->node.GetWorldTransform(transform);
        this->ghostObject.setWorldTransform(transform);
    }


    void ProximityComponent::UpdateContainment(GTLib::Vector<uint64_t> &sceneNodesEntered, GTLib::Vector<uint64_t> &sceneNodesLeft)
    {
        GTLib::SortedVector<uint64_t> sceneNodesLeaving(this->sceneNodesInsideVolume);

        ProximityComponent::Iterator i(*this);
        while (i)
        {
            if (i.otherNode != &this->GetNode())
            {
                assert(i.otherNode != nullptr);
                {
                    auto otherNodeID = i.otherNode->GetID();

                    if (!this->sceneNodesInsideVolume.Exists(otherNodeID))
                    {
                        this->sceneNodesInsideVolume.Insert(otherNodeID);
                        sceneNodesEntered.PushBack(otherNodeID);
                    }
                    else
                    {
                        sceneNodesLeaving.RemoveFirstOccuranceOf(otherNodeID);
                    }
                }
            }

            ++i;
        }


        // At this point we'll have a list of nodes that have left the volume. We need to copies these over to 'sceneNodesLeft'.
        for (size_t i = 0; i < sceneNodesLeaving.count; ++i)
        {
            sceneNodesLeft.PushBack(sceneNodesLeaving[i]);
            this->sceneNodesInsideVolume.RemoveFirstOccuranceOf(sceneNodesLeaving[i]);
        }
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ProximityComponent::Serialize(GTLib::Serializer &serializer) const
    {
        // All we actually want to write is the collision shapes.
        CollisionShapeComponent::Serialize(serializer);
    }

    void ProximityComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        auto world = this->ghostObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveGhostObject(this->ghostObject);
        }


        // First we deserialize the collision shapes.
        CollisionShapeComponent::Deserialize(deserializer);

        // Now we just need to update the scale and transformation based on the scene node.
        this->ApplySceneNodeScaling();
        this->ApplySceneNodeTransformation();


        if (world != nullptr)
        {
            world->AddGhostObject(this->ghostObject, this->collisionGroup, this->collisionMask);
        }


        this->OnChanged();
    }



    //////////////////////////////////////////
    // Private

    void ProximityComponent::OnPreCollisionShapeChanged()
    {
        this->world = this->ghostObject.GetWorld();
        if (this->world != nullptr)
        {
            this->world->RemoveGhostObject(this->ghostObject);
        }
    }

    void ProximityComponent::OnPostCollisionShapeChanged()
    {
        if (this->world != nullptr)
        {
            this->world->AddGhostObject(this->ghostObject, this->collisionGroup, this->collisionMask);
        }

        this->world = nullptr;
    }
}



// Iterator
namespace GTEngine
{
    ProximityComponent::Iterator::Iterator(ProximityComponent &component)
        : component(&component), otherNode(nullptr), manifoldArray(), i(0)
    {
        ++(*this);
    }

    ProximityComponent::Iterator::Iterator(SceneNode &sceneNode)
        : component(sceneNode.GetComponent<GTEngine::ProximityComponent>()), otherNode(nullptr), manifoldArray(), i(0)
    {
        ++(*this);
    }

    ProximityComponent::Iterator & ProximityComponent::Iterator::operator++()
    {
        if (this->component != nullptr)
        {
            this->otherNode = nullptr;

            auto world = this->component->ghostObject.GetWorld();
            if (world != nullptr)
            {
                auto overlappingPairCache = this->component->ghostObject.getOverlappingPairCache();

                // If it's the first iteration, we need to enable narrow-phase collision detection.
                if (this->i == 0)
                {
                    world->GetCollisionDispatcher().dispatchAllCollisionPairs(overlappingPairCache, world->GetInternalDynamicsWorld().getDispatchInfo(), &world->GetCollisionDispatcher());
                }


                auto &pairArray = overlappingPairCache->getOverlappingPairArray();

                GTEngine::SceneNode* nextNode = nullptr;
                while (nextNode == nullptr && this->i < pairArray.size())
                {
                    this->manifoldArray.clear();

                    auto &pair = pairArray[this->i];

                    auto collisionPair = world->GetInternalDynamicsWorld().getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
                    if (collisionPair != nullptr)
                    {
                        auto bodyA = static_cast<btCollisionObject*>(pair.m_pProxy0->m_clientObject);
                        auto bodyB = static_cast<btCollisionObject*>(pair.m_pProxy1->m_clientObject);

                        if (bodyA == &this->component->ghostObject)
                        {
                            nextNode = static_cast<GTEngine::SceneNode*>(bodyB->getUserPointer());
                        }
                        else if (bodyB == &this->component->ghostObject)
                        {
                            nextNode = static_cast<GTEngine::SceneNode*>(bodyA->getUserPointer());
                        }


                        if (nextNode != nullptr)
                        {
                            if (this->component->node.IsRelated(*nextNode))
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

                                if (manifoldArray.size() > 0)
                                {
                                    for (int j = 0; j < manifoldArray.size(); ++j)
                                    {
                                        auto manifold = manifoldArray[j];
                                        if (manifold->getNumContacts() == 0)
                                        {
                                            nextNode = nullptr;
                                        }
                                    }
                                }
                                else
                                {
                                    nextNode = nullptr;
                                }
                            }
                        }
                    }

                    ++this->i;
                }

                this->otherNode = nextNode;
            }
        }

        return *this;
    }


    ProximityComponent::Iterator::operator bool() const
    {
        return this->component != nullptr && this->otherNode != nullptr;
    }
}
