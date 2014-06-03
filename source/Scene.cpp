// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTLib/ToString.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

// Culling Callbacks.
namespace GTEngine
{
    /// Callback structure for doing occlusion culling against a viewport.
    struct SceneCullingCallback : SceneCullingManager::VisibilityCallback
    {
        /// Constructor
        SceneCullingCallback(GTLib::Vector<const ModelComponent*> &modelComponentsOut, GTLib::Vector<const PointLightComponent*> &pointLightComponentsOut, GTLib::Vector<const SpotLightComponent*> &spotLightComponentsOut,
                             GTLib::Vector<const AmbientLightComponent*> &ambientLightComponentsOut, GTLib::Vector<const DirectionalLightComponent*> &directionalLightComponentsOut)
            : modelComponents(modelComponentsOut), pointLightComponents(pointLightComponentsOut), spotLightComponents(spotLightComponentsOut),
              ambientLightComponents(ambientLightComponentsOut), directionalLightComponents(directionalLightComponentsOut)
        {
        }

        /// Destructor.
        ~SceneCullingCallback()
        {
        }


        /// VisibleCallback::ProcessModel().
        virtual void ProcessModel(const SceneNode &sceneNode)
        {
            auto component = sceneNode.GetComponent<GTEngine::ModelComponent>();
            if (component != nullptr)
            {
                this->modelComponents.PushBack(component);
            }
        }

        /// VisibleCallback::ProcessPointLight().
        virtual void ProcessPointLight(const SceneNode &sceneNode)
        {
            auto component = sceneNode.GetComponent<GTEngine::PointLightComponent>();
            if (component != nullptr)
            {
                this->pointLightComponents.PushBack(component);
            }
        }

        /// VisibleCallback::ProcessSpotLight().
        virtual void ProcessSpotLight(const SceneNode &sceneNode)
        {
            auto component = sceneNode.GetComponent<GTEngine::SpotLightComponent>();
            if (component != nullptr)
            {
                this->spotLightComponents.PushBack(component);
            }
        }


        /// VisibleCallback::ProcessAmbientLight().
        virtual void ProcessAmbientLight(const SceneNode &sceneNode)
        {
            auto component = sceneNode.GetComponent<GTEngine::AmbientLightComponent>();
            if (component != nullptr)
            {
                this->ambientLightComponents.PushBack(component);
            }
        }

        /// VisibleCallback::ProcessDirectionalLight().
        virtual void ProcessDirectionalLight(const SceneNode &sceneNode)
        {
            auto component = sceneNode.GetComponent<GTEngine::DirectionalLightComponent>();
            if (component != nullptr)
            {
                this->directionalLightComponents.PushBack(component);
            }
        }


    private:
        GTLib::Vector<const ModelComponent*>            &modelComponents;
        GTLib::Vector<const PointLightComponent*>       &pointLightComponents;
        GTLib::Vector<const SpotLightComponent*>        &spotLightComponents;
        GTLib::Vector<const AmbientLightComponent*>     &ambientLightComponents;
        GTLib::Vector<const DirectionalLightComponent*> &directionalLightComponents;


    private:    // No copying.
        SceneCullingCallback(const SceneCullingCallback &);
        SceneCullingCallback & operator=(const SceneCullingCallback &);
    };
}

// Contact Callbacks
namespace GTEngine
{
    struct SceneContactTestCallback : public btCollisionWorld::ContactResultCallback
    {
        ContactTestCallback &callback;


        SceneContactTestCallback(ContactTestCallback &callback)
            : callback(callback)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy* proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper* colObj0, int, int, const btCollisionObjectWrapper* colObj1, int, int)
        {
            assert(colObj0 != nullptr);
            assert(colObj1 != nullptr);

            // We assume the user pointer is the scene node.
            auto sceneNodeA = static_cast<SceneNode*>(colObj0->getCollisionObject()->getUserPointer());
            auto sceneNodeB = static_cast<SceneNode*>(colObj1->getCollisionObject()->getUserPointer());

            if (sceneNodeA != nullptr && sceneNodeB != nullptr)
            {
                callback.ProcessCollision(*sceneNodeA, *sceneNodeB, cp);
            }

            return 0.0f;
        }


    private:
        SceneContactTestCallback(const SceneContactTestCallback &);
        SceneContactTestCallback & operator=(const SceneContactTestCallback &);
    };



    struct SceneBulletRayResultCallback : public btCollisionWorld::RayResultCallback
    {
        /// A reference to the GTEngine ray test callback object that is used for input and output.
        RayTestCallback &callback;

        /// A pointer to the closest scene node.
        SceneNode* closestSceneNode;


        /// Constructor.
        SceneBulletRayResultCallback(RayTestCallback &callback)
            : callback(callback), closestSceneNode(nullptr)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy* proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
		{
            // The user data of the collision object should be a scene node.
            auto sceneNode = static_cast<SceneNode*>(rayResult.m_collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                this->closestSceneNode = sceneNode;

                // Some stuff for Bullet.
                this->m_closestHitFraction = rayResult.m_hitFraction;
                this->m_collisionObject    = rayResult.m_collisionObject;

                // Now we need to find the world position and normal.
                glm::vec3 worldHitPosition = glm::mix(callback.rayStart, callback.rayEnd, rayResult.m_hitFraction);
                glm::vec3 worldHitNormal;

                if (normalInWorldSpace)
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_hitNormalLocal);
                }
                else
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal);
                }

                // Now we can call the corresponding handler on the GTEngine callback.
                this->callback.ProcessResult(*sceneNode, worldHitPosition, worldHitNormal);
            }

            // We have a collision, so we need to mark it as such.
            this->callback.hasCollision = true;

            return this->m_closestHitFraction;
		}


    private:    // No copying.
        SceneBulletRayResultCallback(const SceneBulletRayResultCallback &);
        SceneBulletRayResultCallback & operator=(const SceneBulletRayResultCallback &);
    };
}


namespace GTEngine
{
    static const bool DoNotPostEvents = false;              // <-- For Scene::OnSceneNodeComponentAdded() and Scene::OnSceneNodeComponentRemoved().


    Scene::Scene()
        : renderer(new DefaultSceneRenderer),
          updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          prefabLinker(new DefaultPrefabLinker(*this)),
          deleteRenderer(true), deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true), deletePrefabLinker(true),
          paused(false),
          viewports(), defaultViewport(), sceneNodes(), nextSceneNodeID(0), minAutoSceneNodeID(1), sceneNodesCreatedByScene(),
          sceneNodesWithProximityComponents(), sceneNodesWithParticleSystemComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true),
          registeredScript(nullptr), isScriptEventsBlocked(false),
          name(),
          isBackgroundClearEnabled(true), backgroundClearColour(0.5f),
          isHDREnabled(false), isBloomEnabled(false)
    {
        this->AddViewport(this->defaultViewport);
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : renderer(new DefaultSceneRenderer),
          updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          prefabLinker(new DefaultPrefabLinker(*this)),
          deleteRenderer(true), deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false), deletePrefabLinker(true),
          paused(false),
          viewports(), defaultViewport(), sceneNodes(), nextSceneNodeID(0), minAutoSceneNodeID(1), sceneNodesCreatedByScene(),
          sceneNodesWithProximityComponents(), sceneNodesWithParticleSystemComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true),
          registeredScript(nullptr), isScriptEventsBlocked(false),
          name(),
          isBackgroundClearEnabled(true), backgroundClearColour(0.5f),
          isHDREnabled(false), isBloomEnabled(false)
    {
        this->AddViewport(this->defaultViewport);
    }

    Scene::~Scene()
    {
        this->RemoveAllViewports();
        this->RemoveAllSceneNodes();


        // We need to unregister after removing every object.
        this->UnregisterFromScript();


        if (this->deleteRenderer)
        {
            delete this->renderer;
        }


        if (this->deleteUpdateManager)
        {
            delete &this->updateManager;
        }

        if (this->deletePhysicsManager)
        {
            delete &this->physicsManager;
        }

        if (this->deleteCullingManager)
        {
            delete &this->cullingManager;
        }


        if (this->deletePrefabLinker)
        {
            delete this->prefabLinker;
        }
    }


    bool Scene::LoadFromFile(const char* relativeFilePath)
    {
        auto file = GTLib::IO::Open(relativeFilePath, GTLib::IO::OpenMode::Read);
        if (file != nullptr)
        {
            bool result = false;

            GTLib::FileDeserializer deserializer(file);
            result = this->Deserialize(deserializer);

            GTLib::IO::Close(file);
            return result;
        }

        return false;
    }


    void Scene::AddSceneNode(SceneNode &node)
    {
        // We're not going to allow a node to be added if it's already part of another scene. It doesn't really make sense for scene nodes to
        // be moved around between scenes, and if it's really needed, they can just be removed from the old scene manually at a higher level.
        if (node.GetScene() == nullptr)
        {
            // We need to generate an ID, if it doesn't already have one.
            uint64_t uniqueID = node.GetID();
            if (uniqueID == 0)
            {
                uniqueID = this->GenerateSceneNodeID();
                node.SetID(uniqueID);
            }
            else
            {
                // If a scene node of the same ID already exists, we have a bug somewhere.
                if (this->sceneNodes.Exists(uniqueID))
                {
                    GTEngine::PostError("Error adding scene node to scene. A scene node of the same ID (%s) already exists. The scene node was not added.", GTLib::ToString(uniqueID).c_str());
                    return;
                }
            }


            // The scene node should not already exist!
            assert(!this->sceneNodes.Exists(node.GetID()));
            {
                // This adds the node to the main container.
                this->sceneNodes.Insert(node);

                // The node needs to know that we are now it's owner.
                node._SetScene(this);


                // With the scene node added, we'll just post the OnSceneNodeAdded() event. This isn't really needed, but it's sort of left over from the previous design.
                this->OnSceneNodeAdded(node);



                // With the node fully added, we will now add the children.
                for (auto iChild = node.GetFirstChild(); iChild != nullptr; iChild = iChild->GetNextSibling())
                {
                    this->AddSceneNode(*iChild);
                }
            }
        }
        else
        {
            GTEngine::PostError("Error: Attempting to add a scene node to a scene while it is already part of a different scene. Ignoring.");
        }
    }

    void Scene::RemoveSceneNode(SceneNode &node)
    {
        if (node.GetScene() == this)
        {
            assert(this->sceneNodes.Exists(node.GetID()));
            {
                // Children need to be removed first.
                GTLib::Vector<SceneNode*> children;

                for (auto iChild = node.GetFirstChild(); iChild != nullptr; iChild = iChild->GetNextSibling())
                {
                    children.PushBack(iChild);
                }

                for (size_t iChild = 0; iChild < children.count; ++iChild)
                {
                    this->RemoveSceneNode(*children[iChild]);
                }



                // We'll now post the OnSceneNodeRemoved() as the first thing for this node.
                this->OnSceneNodeRemoved(node);


                // The node needs to know that we are no longer it's owner.
                node._SetScene(nullptr);

                // This removes the node from the main container.
                this->sceneNodes.Remove(node.GetID());


                // If we were there creator of the scene node, it needs to be deleted.
                size_t index;
                if (this->sceneNodesCreatedByScene.Find(node.GetID(), index))
                {
                    this->sceneNodesCreatedByScene.RemoveAt(index);
                    delete &node;
                }
            }
        }
        else
        {
            GTEngine::Log("Warning: Attempting to remove a scene node from a scene it is not a part of. Ignoring.");
        }
    }

    void Scene::RemoveSceneNodeByID(uint64_t sceneNodeID)
    {
        auto sceneNode = this->GetSceneNodeByID(sceneNodeID);
        if (sceneNode != nullptr)
        {
            this->RemoveSceneNode(*sceneNode);
        }
    }

    void Scene::RemoveAllSceneNodes()
    {
        while (this->sceneNodes.GetCount() > 0)
        {
            auto node = this->sceneNodes.GetSceneNodeAtIndex(0);
            assert(node != nullptr);
            {
                this->RemoveSceneNode(*node);
            }
        }
    }


    SceneNode* Scene::CreateNewSceneNode()
    {
        return this->CreateNewSceneNode(0);     // <-- Specifying 0 as the ID will cause a unique ID to be generated.
    }

    SceneNode* Scene::CreateNewSceneNode(uint64_t id)
    {
        auto sceneNode = new SceneNode;
        sceneNode->SetID(id);

        this->AddSceneNode(*sceneNode);


        // The scene needs to know that it was the one who created the node.
        assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
        {
            this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
        }

        return sceneNode;
    }

    SceneNode* Scene::CreateNewSceneNode(GTLib::Deserializer &deserializer, bool createNewIDIfExists)
    {
        bool alreadyExists = false;

        // The first thing we need to do is peek at the ID and check that it's doesn't already exist.
        uint64_t newID;
        if (SceneNode::Deserialize_PeekID(deserializer, newID))
        {
            if (this->GetSceneNodeByID(newID) != nullptr)
            {
                if (!createNewIDIfExists)
                {
                    Log("Scene::CreateNewSceneNode(GTLib::Deserializer &) - A scene node of the same ID already exists. This is an erroneous condition. Instead, try 'Scene::CreateNewSceneNode()' followed by 'newSceneNode->Deserialize(deserializer)'");
                    return nullptr;
                }

                alreadyExists = true;
            }
        }


        // We need to create an empty node, add it to the scene, and then deserialize. Whether or not a scene node is deserialized with NoID is based on
        // whether or not a scene node of the same ID already exists.
        auto sceneNode = new SceneNode;
        this->AddSceneNode(*sceneNode);

        // The scene node is added, so now we need to deserialize.
        unsigned int deserializeFlags = 0;
        if (alreadyExists)
        {
            // A scene node with the ID specified in the deserializer already exists, so we don't want to set the ID to that. Instead, we just leave it as
            // the auto-generated one as generated by AddSceneNode().
            deserializeFlags |= SceneNode::NoID;
        }

        sceneNode->Deserialize(deserializer, deserializeFlags);


        // The scene needs to know that it was the one who created the node.
        assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
        {
            this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
        }


#if 0
        auto sceneNode = new SceneNode;
        sceneNode->Deserialize(deserializer);

        // We need to return null if a node of the same ID already exists.
        if (this->GetSceneNodeByID(sceneNode->GetID()) != nullptr)
        {
            if (createNewIDIfExists)
            {
                sceneNode->SetID(0);        // <-- Setting to 0 will cause a new ID to be generated.
            }
            else
            {
                Log("Scene::CreateNewSceneNode(GTLib::Deserializer &) - A scene node of the same ID already exists. This is an erroneous condition. Instead, try 'Scene::CreateNewSceneNode()' followed by 'newSceneNode->Deserialize(deserializer)'");

                delete sceneNode;
                return nullptr;
            }
        }


        this->AddSceneNode(*sceneNode);
#endif

        

        return sceneNode;
    }


    SceneNode* Scene::CreateNewSceneNode(const Prefab &sceneNodeClass)
    {
        auto &serializers = sceneNodeClass.GetSerializers();
        auto &hierarchy   = sceneNodeClass.GetHierarchy();

        if (serializers.count > 0)
        {
            struct SceneNodeSerializerPair
            {
                SceneNodeSerializerPair(SceneNode* sceneNode, GTLib::BasicSerializer* serializer)
                    : m_sceneNode(sceneNode), m_serializer(serializer)
                {
                }

                SceneNode* m_sceneNode;
                GTLib::BasicSerializer* m_serializer;
            };

            GTLib::Map<uint64_t, SceneNodeSerializerPair> newSceneNodes;
            SceneNode* rootSceneNode = nullptr;


            // We use the same general workflow as deserializing a whole scene:
            //   - Create empty nodes
            //   - Link new nodes to their parents
            //   - Add nodes to the scene
            //   - Deserialize (while the node is in the scene).

            // Create empty nodes.
            for (size_t iSerializer = 0; iSerializer < serializers.count; ++iSerializer)
            {
                auto id         = serializers.buffer[iSerializer]->key;
                auto serializer = serializers.buffer[iSerializer]->value;

                assert(id         != 0);
                assert(serializer != nullptr);
                {
                    auto newSceneNode = new SceneNode;
                    newSceneNodes.Add(id, SceneNodeSerializerPair(newSceneNode, serializer));

                    // We need the root node. It will always be the first one.
                    if (iSerializer == 0)
                    {
                        rootSceneNode = newSceneNode;
                    }
                }
            }


            // Link new nodes to their parents.
            for (size_t i = 0; i < hierarchy.count; ++i)
            {
                auto sceneNodeID       = hierarchy.buffer[i]->key;
                auto parentSceneNodeID = hierarchy.buffer[i]->value;

                auto iSceneNode       = newSceneNodes.Find(sceneNodeID);
                auto iParentSceneNode = newSceneNodes.Find(parentSceneNodeID);

                // There is a precondition that the root node is never actually referenced in the hierarchy.
                assert(iSceneNode       != nullptr);
                assert(iParentSceneNode != nullptr);
                {
                    auto sceneNode       = iSceneNode->value.m_sceneNode;
                    auto parentSceneNode = iParentSceneNode->value.m_sceneNode;

                    assert(sceneNode       != nullptr);
                    assert(parentSceneNode != nullptr);
                    {
                        parentSceneNode->AttachChild(*sceneNode);
                    }
                }
            }


            // Add nodes to the scene. Nodes are added to the scene recursively, so we only need to do this once with the root node.
            assert(rootSceneNode != nullptr);
            {
                this->AddSceneNode(*rootSceneNode);
            }


            // Deserialize.
            for (size_t iSceneNode = 0; iSceneNode < newSceneNodes.count; ++iSceneNode)
            {
                auto sceneNode           = newSceneNodes.buffer[iSceneNode]->value.m_sceneNode;
                auto sceneNodeSerializer = newSceneNodes.buffer[iSceneNode]->value.m_serializer;
                assert(sceneNode != nullptr);
                {
                    GTLib::BasicDeserializer deserializer(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
                    sceneNode->Deserialize(deserializer, SceneNode::NoID);          // <-- Passing NoID here means that the ID will not be changed during deserialization. This is super important.


                    // The scene needs to know that it was the one who create the node.
                    assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
                    {
                        this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
                    }
                }
            }



#if 0
            // We will iterate over each serializer and create nodes from them, ensuring we set the ID to 0 so that a new one is generated when we add them.
            for (size_t i = 0; i < serializers.count; ++i)
            {
                auto id         = serializers.buffer[i]->key;
                auto serializer = serializers.buffer[i]->value;

                assert(id         != 0);
                assert(serializer != nullptr);
                {
                    GTLib::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());

                    auto newSceneNode = new SceneNode;
                    newSceneNode->Deserialize(deserializer);

                    // We must ensure the ID is 0 so that a new one is generated when we add the node. This is super important!
                    newSceneNode->SetID(0);


                    newSceneNodes.Add(id, newSceneNode);


                    // We need the root node. It will always be the first one.
                    if (i == 0)
                    {
                        rootSceneNode = newSceneNode;
                    }
                }
            }


            // At this point the scene nodes will be created but will not yet be linked to their parent. We'll do that now.
            for (size_t i = 0; i < hierarchy.count; ++i)
            {
                auto sceneNodeID       = hierarchy.buffer[i]->key;
                auto parentSceneNodeID = hierarchy.buffer[i]->value;

                auto iSceneNode       = newSceneNodes.Find(sceneNodeID);
                auto iParentSceneNode = newSceneNodes.Find(parentSceneNodeID);

                // There is a precondition that the root node is never actually referenced in the hierarchy.
                assert(iSceneNode       != nullptr);
                assert(iParentSceneNode != nullptr);
                {
                    auto sceneNode       = iSceneNode->value;
                    auto parentSceneNode = iParentSceneNode->value;

                    assert(sceneNode       != nullptr);
                    assert(parentSceneNode != nullptr);
                    {
                        parentSceneNode->AttachChild(*sceneNode);
                    }
                }
            }


            // Everything is now parented and ready to be added. We just add the root node, which is the first one. This will add the others recursively.
            assert(rootSceneNode != nullptr);
            {
                this->AddSceneNode(*rootSceneNode);
            }


            // It's important that we let the scene know that it is the creator of this node.
            for (size_t i = 0; i < newSceneNodes.count; ++i)
            {
                auto newSceneNode = newSceneNodes.buffer[i]->value;
                assert(newSceneNode != nullptr);
                {
                    assert(!this->sceneNodesCreatedByScene.Exists(newSceneNode->GetID()));
                    {
                        this->sceneNodesCreatedByScene.Insert(newSceneNode->GetID());
                    }
                }
            }
#endif


            return rootSceneNode;
        }


        return nullptr;
    }



    SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID)
    {
        return this->sceneNodes.FindByID(sceneNodeID);
    }

    const SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID) const
    {
        return this->sceneNodes.FindByID(sceneNodeID);
    }


    SceneNode* Scene::GetSceneNodeByName(const char* sceneNodeName)
    {
        return this->sceneNodes.FindByName(sceneNodeName);
    }

    const SceneNode* Scene::GetSceneNodeByName(const char* sceneNodeName) const
    {
        return this->sceneNodes.FindByName(sceneNodeName);
    }


    uint64_t Scene::GetSceneNodeIDByName(const char* sceneNodeName) const
    {
        auto sceneNode = this->GetSceneNodeByName(sceneNodeName);
        if (sceneNode != nullptr)
        {
            return sceneNode->GetID();
        }

        return 0;
    }



    size_t Scene::GetSceneNodeCount() const
    {
        return this->sceneNodes.GetCount();
    }

    SceneNode* Scene::GetSceneNodeByIndex(size_t sceneNodeIndex)
    {
        return this->sceneNodes.GetSceneNodeAtIndex(sceneNodeIndex);
    }

    const SceneNode* Scene::GetSceneNodeByIndex(size_t sceneNodeIndex) const
    {
        return this->sceneNodes.GetSceneNodeAtIndex(sceneNodeIndex);
    }


    void Scene::SetMinAutoSceneNodeID(uint64_t newMinAutoSceneNodeID)
    {
        if (newMinAutoSceneNodeID > 0)
        {
            this->minAutoSceneNodeID = newMinAutoSceneNodeID;
        }
        else
        {
            this->minAutoSceneNodeID = 0;
        }
    }

    uint64_t Scene::GetMinAutoSceneNodeID() const
    {
        return this->minAutoSceneNodeID;
    }



    void Scene::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
        }
    }

    void Scene::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
        }
    }


    void Scene::Update(double deltaTimeInSeconds)
    {
        this->OnUpdate(deltaTimeInSeconds);


        // Now we need to update via the update manager.
        if (!this->IsPaused())
        {
            this->updateManager.Step(deltaTimeInSeconds, this->GetCullingManager());
        }

        // Physics. We do this after updating because the update might set velocity or whatnot.
        if (!this->IsPaused())
        {
            this->physicsManager.Step(deltaTimeInSeconds);
        }


        // We now want to check proximity components.
        for (size_t i = 0; i < this->sceneNodesWithProximityComponents.count; ++i)
        {
            auto proximityComponent = this->sceneNodesWithProximityComponents.buffer[i]->value;
            assert(proximityComponent != nullptr);
            {
                auto sceneNode = &proximityComponent->GetNode();

                GTLib::Vector<uint64_t> sceneNodesEntered;
                GTLib::Vector<uint64_t> sceneNodesLeft;
                proximityComponent->UpdateContainment(sceneNodesEntered, sceneNodesLeft);

                for (size_t iEntered = 0; iEntered < sceneNodesEntered.count; ++iEntered)
                {
                    auto enteredSceneNode = this->GetSceneNodeByID(sceneNodesEntered[iEntered]);
                    assert(enteredSceneNode != nullptr);
                    {
                        // TODO: Call the C++ events.

                        // If we're linked to a script we'll need to notify the script about this.
                        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
                        {
                            // OnObjectEnter().
                            auto scriptComponent = sceneNode->GetComponent<ScriptComponent>();
                            if (scriptComponent != nullptr)
                            {
                                if (scriptComponent->HasOnObjectEnter())
                                {
                                    Scripting::PostSceneNodeEvent_OnObjectEnter(*this->registeredScript, *sceneNode, *enteredSceneNode);
                                }
                            }

                            // OnEnterObject().
                            scriptComponent = enteredSceneNode->GetComponent<ScriptComponent>();
                            if (scriptComponent != nullptr)
                            {
                                if (scriptComponent->HasOnEnterObject())
                                {
                                    Scripting::PostSceneNodeEvent_OnEnterObject(*this->registeredScript, *enteredSceneNode, *sceneNode);
                                }
                            }
                        }
                    }
                }

                for (size_t iLeft = 0; iLeft < sceneNodesLeft.count; ++iLeft)
                {
                    auto leftSceneNode = this->GetSceneNodeByID(sceneNodesLeft[iLeft]);
                    if (leftSceneNode != nullptr)       // <-- Don't want an assert because there is a situation where the node might have been deleted, in which case this will be null. 
                    {
                        // TODO: Call the C++ events.

                        // If we're linked to a script we'll need to notify the script about this.
                        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
                        {
                            // OnObjectLeave().
                            auto scriptComponent = sceneNode->GetComponent<ScriptComponent>();
                            if (scriptComponent != nullptr)
                            {
                                if (scriptComponent->HasOnObjectLeave())
                                {
                                    Scripting::PostSceneNodeEvent_OnObjectLeave(*this->registeredScript, *sceneNode, *leftSceneNode);
                                }
                            }

                            // OnLeaveObject().
                            scriptComponent = leftSceneNode->GetComponent<ScriptComponent>();
                            if (scriptComponent != nullptr)
                            {
                                if (scriptComponent->HasOnLeaveObject())
                                {
                                    Scripting::PostSceneNodeEvent_OnLeaveObject(*this->registeredScript, *leftSceneNode, *sceneNode);
                                }
                            }
                        }
                    }
                }
            }
        }

        


        // Now we need to render.
        this->renderer->Begin(*this);
        {
            for (size_t i = 0; i < this->viewports.count; ++i)
            {
                auto viewport = this->viewports[i];
                assert(viewport != nullptr);
                {
                    this->renderer->RenderViewport(*this, *viewport);
                }
            }
        }
        this->renderer->End(*this);
    }


    SceneNode* Scene::FindFirstNode(const char* name)
    {
        // Here we check every node and their children.
        for (size_t i = 0; i < this->sceneNodes.GetCount(); ++i)
        {
            auto node = this->sceneNodes.GetSceneNodeAtIndex(i);
            assert(node != nullptr);
            {
                if (GTLib::Strings::Equal(node->GetName(), name))
                {
                    return node;
                }
            }
        }

        return nullptr;
    }

    SceneNode* Scene::FindFirstNodeWithComponent(const char* componentName)
    {
        for (size_t i = 0; i < this->sceneNodes.GetCount(); ++i)
        {
            auto node = this->sceneNodes.GetSceneNodeAtIndex(i);
            assert(node != nullptr);
            {
                if (node->HasComponent(componentName))
                {
                    return node;
                }
            }
        }

        return nullptr;
    }


    void Scene::AddViewport(SceneViewport &viewport)
    {
        assert(this->renderer != nullptr);

        // The viewport needs to be removed from the previous scene if it has one.
        if (viewport.GetScene() != nullptr)
        {
            viewport.GetScene()->RemoveViewport(viewport);
        }

        this->renderer->AddViewport(viewport);
        this->viewports.PushBack(&viewport);

        viewport.SetScene(this);
    }

    void Scene::RemoveViewport(SceneViewport &viewport)
    {
        assert(this->renderer != nullptr);

        if (viewport.GetScene() == this)
        {
            this->renderer->RemoveViewport(viewport);
            this->viewports.RemoveFirstOccuranceOf(&viewport);

            viewport.SetScene(nullptr);
        }
    }

    void Scene::RemoveAllViewports()
    {
        assert(this->renderer != nullptr);

        for (size_t i = 0; i < this->viewports.count; ++i)
        {
            auto viewport = this->viewports[i];
            assert(viewport != nullptr);
            {
                this->renderer->RemoveViewport(*viewport);
                viewport->SetScene(nullptr);
            }
        }
        this->viewports.Clear();
    }


    size_t Scene::GetViewportCount() const
    {
        return this->viewports.count;
    }

    SceneViewport & Scene::GetViewportByIndex(size_t index)
    {
        return *this->viewports[index];
    }

    const SceneViewport & Scene::GetViewportByIndex(size_t index) const
    {
        return *this->viewports[index];
    }

    void Scene::SetViewportCamera(size_t index, SceneNode &cameraNode)
    {
        this->viewports[index]->SetCameraNode(cameraNode);
    }


    void Scene::GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        // We'll retrieve the global AABB from the culling manager. Might need to move this over to physics manager, perhaps.
        this->cullingManager.GetGlobalAABB(aabbMin, aabbMax);
    }



    void Scene::SetRenderer(SceneRenderer &newRenderer)
    {
        assert(this->renderer != nullptr);

        // We need to remove all of the viewports from the old renderer.
        for (size_t i = 0; i < this->viewports.count; ++i)
        {
            auto viewport = this->viewports[i];
            assert(viewport != nullptr);
            {
                this->renderer->RemoveViewport(*viewport);
            }
        }

        if (this->deleteRenderer)
        {
            delete this->renderer;
        }


        this->renderer       = &newRenderer;
        this->deleteRenderer = false;


        // With the new renderer set, we now need to let it know about our existing viewports.
        //for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        for (size_t i = 0; i < this->viewports.count; ++i)
        {
            auto viewport = this->viewports[i];
            assert(viewport != nullptr);
            {
                this->renderer->AddViewport(*viewport);
            }
        }
    }


    void Scene::SetPrefabLinker(PrefabLinker &newPrefabLinker)
    {
        if (this->deletePrefabLinker)
        {
            delete this->prefabLinker;
        }

        this->prefabLinker       = &newPrefabLinker;
        this->deletePrefabLinker = false;
    }


    void Scene::AttachEventHandler(SceneEventHandler &eventHandler)
    {
        if (!this->eventHandlers.Exists(&eventHandler))
        {
            this->eventHandlers.PushBack(&eventHandler);
        }
    }

    void Scene::DetachEventHandler(SceneEventHandler &eventHandler)
    {
        this->eventHandlers.RemoveFirstOccuranceOf(&eventHandler);
    }



    void Scene::RegisterToScript(GTLib::Script &script)
    {
        if (this->registeredScript != &script)
        {
            // If we are already registered to a different script we'll need to unregister.
            if (this->registeredScript != nullptr)
            {
                this->UnregisterFromScript();
            }


            if (Scripting::RegisterScene(script, *this))
            {
                this->registeredScript = &script;
            }
        }
    }

    void Scene::UnregisterFromScript()
    {
        if (this->registeredScript != nullptr)
        {
            Scripting::UnregisterScene(*this->registeredScript, *this);
            this->registeredScript = nullptr;
        }
    }




    ////////////////////////////////////////////////////
    // State Stack

    uint32_t Scene::CreateStateStackBranch()
    {
        return this->stateStack.CreateBranch();
    }

    bool Scene::SwitchStateStackBranch(uint32_t branchID)
    {
        return this->stateStack.SwitchBranch(branchID);
    }

    bool Scene::IsStateStackStagingEnabled() const
    {
        return this->stateStack.HasInitialFrame() && this->isStateStackStagingEnabled;
    }

    void Scene::EnableStateStackStaging()
    {
        this->isStateStackStagingEnabled = true;
    }

    void Scene::DisableStateStackStaging()
    {
        this->isStateStackStagingEnabled = false;
    }

    void Scene::CommitStateStackFrame()
    {
        // If this is the initial commit then we'll need to stage insert commands for every existing scene node.
        if (!this->stateStack.HasInitialFrame())
        {
            for (size_t iNode = 0; iNode < this->sceneNodes.GetCount(); ++iNode)
            {
                auto node = this->sceneNodes.GetSceneNodeAtIndex(iNode);
                assert(node != nullptr);
                {
                    if (node->IsStateStackStagingEnabled())
                    {
                        this->stateStack.StageInsert(node->GetID());
                    }
                }
            }
        }

        this->stateStack.Commit();


        // We need to let the event handlers know about this.
        this->PostEvent_OnStateStackFrameCommitted();
    }

    void Scene::ClearStateStackStagingArea()
    {
        this->stateStack.ClearStagingArea();
    }

    size_t Scene::GetStateStackFrameCount() const
    {
        auto currentBranch = this->stateStack.GetCurrentBranch();
        assert(currentBranch != nullptr);
        {
            return currentBranch->GetTotalFrameCount();
        }
    }


    uint32_t Scene::GetStateStackCurrentFrameIndex() const
    {
        auto currentBranch = this->stateStack.GetCurrentBranch();
        assert(currentBranch != nullptr);
        {
            return currentBranch->GetCurrentFrameIndex();
        }
    }

    uint32_t Scene::GetStateStackMaxFrameIndex() const
    {
        auto currentBranch = this->stateStack.GetCurrentBranch();
        assert(currentBranch != nullptr);
        {
            return currentBranch->GetMaxFrameIndex();
        }
    }


    void Scene::SeekStateStack(int amount)
    {
        this->stateStack.Seek(amount);
    }

    void Scene::ApplyStateStack()
    {
        this->stateStack.ApplyToScene();
    }

    void Scene::RevertStateStackStagingArea()
    {
        this->stateStack.RevertStagingArea();
    }


    void Scene::StageInsertOnStateStack(uint64_t sceneNodeID)
    {
        this->stateStack.StageInsert(sceneNodeID);
    }

    void Scene::StageDeleteOnStateStack(uint64_t sceneNodeID)
    {
        this->stateStack.StageDelete(sceneNodeID);
    }

    void Scene::StageUpdateOnStateStack(uint64_t sceneNodeID)
    {
        this->stateStack.StageUpdate(sceneNodeID);
    }


    void Scene::BlockScriptEvents()
    {
        this->isScriptEventsBlocked = true;
        this->updateManager.BlockScriptEvents();
    }

    void Scene::UnblockScriptEvents()
    {
        this->isScriptEventsBlocked = false;
        this->updateManager.UnblockScriptEvents();
    }

    bool Scene::IsScriptEventsBlocked() const
    {
        return this->isScriptEventsBlocked;
    }



    ////////////////////////////////////////////
    // Properties

    void Scene::SetName(const char* nameIn)
    {
        this->name = nameIn;
    }

    const char* Scene::GetName() const
    {
        return this->name.c_str();
    }


    void Scene::EnableBackgroundClearing(float r, float g, float b)
    {
        this->isBackgroundClearEnabled = true;
        this->backgroundClearColour = glm::vec3(r, g, b);

        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsBackgroundClearEnabled", true);
            this->renderer->SetProperty("BackgroundClearColour", glm::vec3(r, g, b));
        }
    }

    void Scene::DisableBackgroundClearing()
    {
        this->isBackgroundClearEnabled = false;

        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsBackgroundClearEnabled", false);
        }
    }

    bool Scene::IsBackgroundClearingEnabled() const
    {
        return this->isBackgroundClearEnabled;
    }

    const glm::vec3 & Scene::GetBackgroundClearColour() const
    {
        return this->backgroundClearColour;
    }


    void Scene::EnableHDR()
    {
        this->isHDREnabled = true;
        
        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsHDREnabled", true);
        }
    }

    void Scene::DisableHDR()
    {
        this->isHDREnabled = false;

        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsHDREnabled", false);
        }
    }

    bool Scene::IsHDREnabled() const
    {
        return this->isHDREnabled;
    }


    void Scene::EnableBloom()
    {
        this->isBloomEnabled = true;

        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsBloomEnabled", true);
        }
    }

    void Scene::DisableBloom()
    {
        this->isBloomEnabled = false;

        if (this->renderer != nullptr)
        {
            this->renderer->SetProperty("IsBloomEnabled", false);
        }
    }

    bool Scene::IsBloomEnabled() const
    {
        return this->isBloomEnabled;
    }



    SceneNode* Scene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback)
    {
        // Before creating the Bullet ray test callback object, we first need to set the rayStart and rayEnd attributes of the input callback structure. We do this
        // because the Bullet callback will use them in calculating the world position of interestion points.
        callback.rayStart = rayStart;
        callback.rayEnd   = rayEnd;

        // We need to use our own ray test callback for this.
        SceneBulletRayResultCallback rayTestResult(callback);
        this->physicsManager.RayTest(rayStart, rayEnd, rayTestResult);

        return rayTestResult.closestSceneNode;
    }

    SceneNode* Scene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, short collisionGroup, short collisionMask)
    {
        RayTestCallback callback(collisionGroup, collisionMask);
        return this->RayTest(rayStart, rayEnd, callback);
    }


    void Scene::ContactTest(const SceneNode &node, ContactTestCallback &callback)
    {
        // At the moment we're going to do a collision test between both the proximity and dynamics component. Not sure what to do here, exactly...


        // We do the contact test against the nodes proximity component.
        auto proximity = node.GetComponent<GTEngine::ProximityComponent>();
        if (proximity != nullptr)
        {
            callback.collisionGroup = proximity->GetCollisionGroup();
            callback.collisionMask  = proximity->GetCollisionMask();

            SceneContactTestCallback bulletCallback(callback);
            this->physicsManager.ContactTest(proximity->GetGhostObject(), bulletCallback);
        }


        auto dynamics = node.GetComponent<GTEngine::DynamicsComponent>();
        if (dynamics != nullptr)
        {
            callback.collisionGroup = dynamics->GetCollisionGroup();
            callback.collisionMask  = dynamics->GetCollisionMask();

            SceneContactTestCallback bulletCallback(callback);
            this->physicsManager.ContactTest(dynamics->GetRigidBody(), bulletCallback);
        }
    }


    void Scene::QueryVisibleSceneNodes(const glm::mat4 &mvp, SceneCullingManager::VisibilityCallback &callback) const
    {
        this->cullingManager.ProcessVisibleSceneNodes(mvp, callback);
    }



    void Scene::SetGravity(float x, float y, float z)
    {
        this->physicsManager.SetGravity(x, y, z);

        // We need to let particle systems know about this. Should really optimize this, but it might not really be needed.
        for (size_t i = 0; i < this->sceneNodes.GetCount(); ++i)
        {
            auto sceneNode = this->sceneNodes.GetSceneNodeAtIndex(i);
            assert(sceneNode != nullptr);
            {
                auto particleSystemComponent = sceneNode->GetComponent<ParticleSystemComponent>();
                if (particleSystemComponent != nullptr)
                {
                    auto particleSystem = particleSystemComponent->GetParticleSystem();
                    if (particleSystem != nullptr)
                    {
                        particleSystem->SetGravity(this->GetGravity());
                    }
                }
            }
        }
    }

    void Scene::GetGravity(float &x, float &y, float &z) const
    {
        this->physicsManager.GetGravity(x, y, z);
    }


    void Scene::SetWalkableHeight(float height)
    {
        this->navigationMesh.SetWalkableHeight(height);
    }
    void Scene::SetWalkableRadius(float radius)
    {
        this->navigationMesh.SetWalkableRadius(radius);
    }
    void Scene::SetWalkableSlopeAngle(float angle)
    {
        this->navigationMesh.SetWalkableSlope(angle);
    }
    void Scene::SetWalkableClimbHeight(float height)
    {
        this->navigationMesh.SetWalkableClimb(height);
    }


    float Scene::GetWalkableHeight() const
    {
        return this->navigationMesh.GetWalkableHeight();
    }
    float Scene::GetWalkableRadius() const
    {
        return this->navigationMesh.GetWalkableRadius();
    }
    float Scene::GetWalkableSlopeAngle() const
    {
        return this->navigationMesh.GetWalkableSlope();
    }
    float Scene::GetWalkableClimbHeight() const
    {
        return this->navigationMesh.GetWalkableClimb();
    }


    void Scene::BuildNavigationMesh(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until we add support for multiple navigation meshes.
        (void)index;

        this->navigationMesh.Build(*this);
    }

    void Scene::FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTLib::Vector<glm::vec3> &output)
    {
        this->navigationMesh.FindPath(start, end, output);
    }


    NavigationMesh & Scene::GetNavigationMesh(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until we add support for multiple navigation meshes.
        (void)index;

        return this->navigationMesh;
    }

    const NavigationMesh & Scene::GetNavigationMesh(size_t index) const
    {
        assert(index == 0);     // <-- Temp assert until we add support for multiple navigation meshes.
        (void)index;

        return this->navigationMesh;
    }



    ////////////////////////////////////////////////////////////
    // Serialization/Deserialization.

    bool Scene::Serialize(GTLib::Serializer &serializer) const
    {
        // General structure:
        //
        // - Scene info
        // - Scene properties
        // - Scene navigation data (navigation meshes, etc)
        // - Scene node hierarchy
        // - Scene node data

        Serialization::ChunkHeader header;
        


        // Scene Info (nextSceneNodeID, etc).
        {
            header.id          = Serialization::ChunkID_Scene_Info;
            header.version     = 1;
            header.sizeInBytes =
                sizeof(uint64_t) +              // <-- nextSceneNodeID
                sizeof(uint64_t);               // <-- minAutoSceneNodeID

            serializer.Write(header);
            serializer.Write(this->nextSceneNodeID);
            serializer.Write(this->minAutoSceneNodeID);
        }

        // Properties.
        {
            GTLib::BasicSerializer secondarySerializer;
            secondarySerializer.WriteString(this->name);
        
            secondarySerializer.Write(this->isBackgroundClearEnabled);
            secondarySerializer.Write(this->backgroundClearColour);
            secondarySerializer.Write(this->isHDREnabled);
            secondarySerializer.Write(this->isBloomEnabled);


            header.id          = Serialization::ChunkID_Scene_Properties;
            header.version     = 1;
            header.sizeInBytes = secondarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(secondarySerializer.GetBuffer(), header.sizeInBytes);
        }

        // Navigation.
        {
            GTLib::BasicSerializer secondarySerializer;

            // The navigation mesh count. This is always 1 for now.
            uint32_t navigationMeshCount = 1;
            secondarySerializer.Write(navigationMeshCount);

            // Each navigation mesh. Just the one for now.
            this->navigationMesh.Serialize(secondarySerializer);

            header.id          = Serialization::ChunkID_Scene_Navigation;
            header.version     = 1;
            header.sizeInBytes = secondarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(secondarySerializer.GetBuffer(), header.sizeInBytes);
        }



        ////////////////////////////////////////////
        // Scene Nodes
        //
        // Not all scene nodes are serialized. We need to create a flat list containing pointers to the scene nodes that should be
        // serialized. A vector is used to enable us to efficiently use indices for the hierarchy.
        //
        // The hierarchy is made up of a series of local indices - that is, each index refers to the position of the scene node as
        // stored in the serialized data. This index is NOT the scene node's ID.
        //
        // The hierarchy will be written before the scene node data. This is because we want the deserialization process to have
        // the opportunity to construct empty nodes and build the hierarchy before deserializing the actual scene node data. We
        // want this because we want scene nodes to have access to the hierarchy, the owner scene and the scripting environment
        // during deserialization.
        //
        // When the actual data for scene nodes are serialized, we need to have the ID's of the scene nodes seperate. This is
        // because we'll need the IDs when creating the empty nodes during serialization.

        const size_t sceneNodeCount = this->sceneNodes.GetCount();

        // We need to build a flat list of scene nodes that should be serialized. We want to store the IDs in a seperate list so
        // we can serialize them in a single chunk.
        GTLib::Vector<const SceneNode*> serializedNodes(sceneNodeCount);
        GTLib::Vector<uint64_t> sceneNodeIDs(sceneNodeCount);

        for (size_t iSceneNode = 0; iSceneNode < sceneNodeCount; ++iSceneNode)
        {
            auto sceneNode = this->sceneNodes.GetSceneNodeAtIndex(iSceneNode);
            assert(sceneNode != nullptr);
            {
                if (sceneNode->IsSerializationEnabled())
                {
                    serializedNodes.PushBack(sceneNode);
                    sceneNodeIDs.PushBack(sceneNode->GetID());
                }
            }
        }



        // We now know which scene nodes are being serialized. Now we need to start the actual deserialization. We can do everything
        // in a single iteration by simply using discrete serializers for each section.

        GTLib::BasicSerializer hierarchySerializer;

        GTLib::BasicSerializer sceneNodeSerializer;
        sceneNodeSerializer.Write(static_cast<uint32_t>(sceneNodeIDs.GetCount()));                  // <-- Scene node count
        sceneNodeSerializer.Write(sceneNodeIDs.buffer, sceneNodeIDs.count * sizeof(uint64_t));      // <-- Flat list of scene node IDs, indexable by the local indices in the hierarchy list.
        

        for (size_t iSceneNode = 0; iSceneNode < serializedNodes.GetCount(); ++iSceneNode)
        {
            auto node = serializedNodes[iSceneNode];
            assert(node != nullptr);
            {
                // Serialize first...
                node->Serialize(sceneNodeSerializer);

                // And then find the index of the parent, if applicable.
                if (node->GetParent() != nullptr)
                {
                    Serialization::SceneNodeIndexPair indexPair;
                    indexPair.index0 = iSceneNode;

                    size_t parentIndex;
                    if (serializedNodes.FindFirstIndexOf(node->GetParent(), parentIndex))
                    {
                        indexPair.index1 = static_cast<uint32_t>(parentIndex);
                        hierarchySerializer.Write(indexPair);
                    }
                }
            }
        }


        // We have our hierarchy and scene node data; now we just need to write it. We write the hierarchy first.

        // Hierarchy
        header.id          = Serialization::ChunkID_Scene_NodesHierarchy;
        header.version     = 1;
        header.sizeInBytes = static_cast<uint32_t>(hierarchySerializer.GetBufferSizeInBytes());

        serializer.Write(header);
        serializer.Write(hierarchySerializer.GetBuffer(), header.sizeInBytes);


        // Scene Nodes
        header.id          = Serialization::ChunkID_Scene_Nodes;
        header.version     = 1;
        header.sizeInBytes = static_cast<uint32_t>(sceneNodeSerializer.GetBufferSizeInBytes());

        serializer.Write(header);
        serializer.Write(sceneNodeSerializer.GetBuffer(), header.sizeInBytes);



        return true;
    }

    bool Scene::Deserialize(GTLib::Deserializer &deserializer, SceneDeserializeCallback &callback)
    {
        // General flow:
        //
        // - Load general scene info
        // - Load scene properties
        // - Load scene node hierarchy
        // - Link scene nodes to parents
        // - Add empty scene nodes to the scene
        // - Load components
        // - Link to prefabs
        //
        // We are going to deserialize the main chunks in order. If any of these are out of order, we need to fail.


        // Everything needs to be cleared, including the state stack.
        this->RemoveAllSceneNodes();
        this->stateStack.Clear();


        Serialization::ChunkHeader header;

        // Info
        if (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            if (header.id == Serialization::ChunkID_Scene_Info)
            {
                if (header.version == 1)
                {
                    deserializer.Read(this->nextSceneNodeID);
                    deserializer.Read(this->minAutoSceneNodeID);
                }
                else
                {
                    GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                    deserializer.Seek(header.sizeInBytes);

                    return false;
                }
            }
            else
            {
                GTEngine::Log("Error deserializing scene. Expected chunk %d but got chunk %d instead.", Serialization::ChunkID_Scene_Info, header.id);
                deserializer.Seek(header.sizeInBytes);

                return false;
            }
        }

        // Properties
        if (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            if (header.id == Serialization::ChunkID_Scene_Properties)
            {
                if (header.version == 1)
                {
                    deserializer.ReadString(this->name);


                    // Read.
                    bool isBackgroundClearingEnabledIn;
                    deserializer.Read(isBackgroundClearingEnabledIn);

                    glm::vec3 backgroundClearColourIn;
                    deserializer.Read(backgroundClearColourIn);

                    bool isHDREnabledIn;
                    deserializer.Read(isHDREnabledIn);

                    bool isBloomEnabledIn;
                    deserializer.Read(isBloomEnabledIn);


                    // Set.
                    if (isBackgroundClearingEnabledIn)
                    {
                        this->EnableBackgroundClearing(backgroundClearColourIn);
                    }
                    else
                    {
                        this->DisableBackgroundClearing();
                        this->backgroundClearColour = backgroundClearColourIn;
                    }

                    if (isHDREnabledIn)
                    {
                        this->EnableHDR();
                    }
                    else
                    {
                        this->DisableHDR();
                    }

                    if (isBloomEnabledIn)
                    {
                        this->EnableBloom();
                    }
                    else
                    {
                        this->DisableBloom();
                    }
                }
                else
                {
                    GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                    deserializer.Seek(header.sizeInBytes);

                    return false;
                }
            }
            else
            {
                GTEngine::Log("Error deserializing scene. Expected chunk %d but got chunk %d instead.", Serialization::ChunkID_Scene_Properties, header.id);
                deserializer.Seek(header.sizeInBytes);

                return false;
            }
        }

        // Navigation
        if (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            if (header.id == Serialization::ChunkID_Scene_Navigation)
            {
                if (header.version == 1)
                {
                    // The navigation mesh count isn't actually used at the moment, but plans are in place for supporting multiple navigation meshes. This will be needed.
                    uint32_t navigationMeshCount;
                    deserializer.Read(navigationMeshCount);

                    this->navigationMesh.Deserialize(deserializer);
                }
                else
                {
                    GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                    deserializer.Seek(header.sizeInBytes);

                    return false;
                }
            }
            else
            {
                GTEngine::Log("Error deserializing scene. Expected chunk %d but got chunk %d instead.", Serialization::ChunkID_Scene_Navigation, header.id);
                deserializer.Seek(header.sizeInBytes);

                return false;
            }
        }

        // Hierarchy
        GTLib::Vector<Serialization::SceneNodeIndexPair> childParentPairs;
        if (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            if (header.id == Serialization::ChunkID_Scene_NodesHierarchy)
            {
                if (header.version == 1)
                {
                    size_t pairCount = header.sizeInBytes / sizeof(Serialization::SceneNodeIndexPair);

                    childParentPairs.Reserve(pairCount);
                    childParentPairs.count = pairCount;
                    deserializer.Read(childParentPairs.buffer, header.sizeInBytes);
                }
                else
                {
                    GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                    deserializer.Seek(header.sizeInBytes);

                    return false;
                }
            }
            else
            {
                GTEngine::Log("Error deserializing scene. Expected chunk %d but got chunk %d instead.", Serialization::ChunkID_Scene_NodesHierarchy, header.id);
                deserializer.Seek(header.sizeInBytes);

                return false;
            }
        }

        // Scene Nodes
        GTLib::Vector<SceneNode*> deserializedNodes;
        GTLib::Vector<SceneNode*> rootSceneNodesLinkedToPrefabs;
        if (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            if (header.id == Serialization::ChunkID_Scene_Nodes)
            {
                if (header.version == 1)
                {
                    // First thing to do here is read the count and the list of scene node IDs.
                    uint32_t sceneNodeCount;
                    deserializer.Read(sceneNodeCount);

                    // The scene node IDs
                    GTLib::Vector<uint64_t> sceneNodeIDs;
                    sceneNodeIDs.Reserve(sceneNodeCount);
                    sceneNodeIDs.count = sceneNodeCount;
                    deserializer.Read(sceneNodeIDs.buffer, sizeof(uint64_t) * sceneNodeCount);


                    // The scene nodes need to be instantiated here.
                    deserializedNodes.Reserve(sceneNodeCount);
                    for (uint32_t iSceneNode = 0; iSceneNode < sceneNodeCount; ++iSceneNode)
                    {
                        auto sceneNode = new SceneNode;
                        sceneNode->SetID(sceneNodeIDs[iSceneNode]);

                        deserializedNodes.PushBack(sceneNode);
                    }

                    // The new scene nodes now need to be linked to their parents.
                    for (size_t i = 0; i < childParentPairs.count; ++i)
                    {
                        auto &pair = childParentPairs[i];

                        auto childSceneNode  = deserializedNodes[pair.index0];
                        auto parentSceneNode = deserializedNodes[pair.index1];
            
                        assert(childSceneNode  != nullptr);
                        assert(parentSceneNode != nullptr);
                        {
                            parentSceneNode->AttachChild(*childSceneNode);
                        }
                    }

                    // The empty nodes now need to be added to the scene. We only need to add those without parents - children will be added recursively.
                    for (uint32_t iSceneNode = 0; iSceneNode < sceneNodeCount; ++iSceneNode)
                    {
                        auto sceneNode = deserializedNodes[iSceneNode];
                        if (sceneNode != nullptr)
                        {
                            if (sceneNode->GetParent() == nullptr)
                            {
                                this->AddSceneNode(*sceneNode);
                            }


                            // The scene node must be memory managed by the scene.
                            assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
                            {
                                this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
                            }
                        }
                    }


                    // At this point every scene node has been added to the scene, so now we need to deserialize them.
                    for (uint32_t iSceneNode = 0; iSceneNode < sceneNodeCount; ++iSceneNode)
                    {
                        auto sceneNode = deserializedNodes[iSceneNode];
                        if (sceneNode != nullptr)
                        {
                            sceneNode->Deserialize(deserializer, SceneNode::NoScriptOnDeserialize);

                            // If the new scene node is linked to a prefab and it's the root, we need to keep track of it. We will later be re-linking
                            // it to ensure it is up-to-date.
                            if (this->GetPrefabLinker().IsRootSceneNode(*sceneNode))
                            {
                                rootSceneNodesLinkedToPrefabs.PushBack(sceneNode);
                            }
                        }
                    }
                }
                else
                {
                    GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                    deserializer.Seek(header.sizeInBytes);

                    return false;
                }
            }
            else
            {
                GTEngine::Log("Error deserializing scene. Expected chunk %d but got chunk %d instead.", Serialization::ChunkID_Scene_Nodes, header.id);
                deserializer.Seek(header.sizeInBytes);

                return false;
            }
        }


        // Any other chunk we find needs to be handled seperately.
        //
        // TODO: Consider using a null chunk for terminating. Will need to have a callback system in the serialization routine so that custom chunks can be added.
        while (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            size_t targetReadPosition = deserializer.Tell() + header.sizeInBytes;


            if (callback.IsChunkHandled(header))
            {
                callback.HandleChunk(header, deserializer);
            }
            else
            {
                deserializer.Seek(header.sizeInBytes);
            }


            // We need to check the target read pointer and validate.
            size_t currentReadPosition = deserializer.Tell();
            if (currentReadPosition != targetReadPosition)
            {
                assert(false);      // <-- If you've hit this assert, you've got a serialization/deserialization error.

                // We're going to try to recover by seeking the difference.
                deserializer.Seek(targetReadPosition - currentReadPosition);

                assert(deserializer.Tell() == currentReadPosition);
            }
        }


        // Now we need to re-link all relevant scene nodes to their prefabs. If we don't do this, they may not be using the most up-to-date version of the prefab.
        for (size_t iSceneNode = 0; iSceneNode < rootSceneNodesLinkedToPrefabs.count; ++iSceneNode)
        {
            auto sceneNode = rootSceneNodesLinkedToPrefabs[iSceneNode];
            assert(sceneNode != nullptr);
            {
                auto prefabComponent = sceneNode->GetComponent<PrefabComponent>();
                assert(prefabComponent != nullptr);
                {
                    this->GetPrefabLinker().LinkSceneNodeToPrefab(*sceneNode, prefabComponent->GetPrefabRelativePath());
                }
            }
        }


#if 0
        GTLib::Vector<SceneNode*>                        deserializedNodes;
        GTLib::Vector<SceneNode*>                        rootSceneNodesLinkedToPrefabs;
        GTLib::Vector<Serialization::SceneNodeIndexPair> childParentPairs;

        bool readInfo                = false;
        bool readSceneNodes          = false;
        bool readSceneNodesHierarchy = false;
        bool readSceneProperties     = false;
        bool readNavigation          = false;


        Serialization::ChunkHeader header;
        while (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
            // We need to grab the read pointer we should be sitting on after reading this chunk. We use this for error checking and
            // basic recovery.
            size_t targetReadPosition = deserializer.Tell() + header.sizeInBytes;

            if (header.id == Serialization::ChunkID_Scene_Info)
            {
                readInfo = true;

                switch (header.version)
                {
                case 1:
                    {
                        deserializer.Read(this->nextSceneNodeID);
                        deserializer.Read(this->minAutoSceneNodeID);

                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing scene. The version of the info chunk (%d) is not supported.", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        return false;
                    }
                }
            }
            else if (header.id == Serialization::ChunkID_Scene_Nodes)
            {
                readSceneNodes = true;

                switch (header.version)
                {
                case 1:
                    {
                        // We will instantiate the nodes and place a pointer to them into 'deserializedNodes'. We will not add anything to the scene until the nodes have been
                        // linked to their parents.
                        uint32_t nodeCount;
                        deserializer.Read(nodeCount);

                        for (uint32_t iNode = 0; iNode < nodeCount; ++iNode)
                        {
                            auto node = new SceneNode;
                            node->Deserialize(deserializer, *this);

                            // If the new scene node is linked to a prefab and it's the root, we need to keep track of it. We will later be re-linking
                            // it to ensure it is up-to-date.
                            if (this->GetPrefabLinker().IsRootSceneNode(*node))
                            {
                                rootSceneNodesLinkedToPrefabs.PushBack(node);
                            }

                            deserializedNodes.PushBack(node);
                        }

                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing scene. The version of the scene node chunk (%d) is not supported.\n", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        return false;
                    }
                }
            }
            else if (header.id == Serialization::ChunkID_Scene_NodesHierarchy)
            {
                readSceneNodesHierarchy = true;

                switch (header.version)
                {
                case 1:
                    {
                        size_t pairCount = header.sizeInBytes / sizeof(Serialization::SceneNodeIndexPair);

                        childParentPairs.Reserve(pairCount);
                        childParentPairs.count = pairCount;
                        deserializer.Read(childParentPairs.buffer, header.sizeInBytes);

                        break;
                    }

                default:
                    {
                        GTEngine::Log("Error deserializing scene. The version of the scene node hierarchy chunk (%d) is not supported.", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        // We may have nodes instantiated, so they'll need to be killed.
                        for (size_t iNode = 0; iNode < deserializedNodes.count; ++iNode)
                        {
                            delete deserializedNodes[iNode];
                        }

                        return false;
                    }
                }
            }
            else if (header.id == Serialization::ChunkID_Scene_Properties)
            {
                readSceneProperties = true;

                if (header.version == 1)
                {
                    deserializer.ReadString(this->name);


                    // Read.
                    bool isBackgroundClearingEnabledIn;
                    deserializer.Read(isBackgroundClearingEnabledIn);

                    glm::vec3 backgroundClearColourIn;
                    deserializer.Read(backgroundClearColourIn);

                    bool isHDREnabledIn;
                    deserializer.Read(isHDREnabledIn);

                    bool isBloomEnabledIn;
                    deserializer.Read(isBloomEnabledIn);


                    // Set.
                    if (isBackgroundClearingEnabledIn)
                    {
                        this->EnableBackgroundClearing(backgroundClearColourIn);
                    }
                    else
                    {
                        this->DisableBackgroundClearing();
                        this->backgroundClearColour = backgroundClearColourIn;
                    }

                    if (isHDREnabledIn)
                    {
                        this->EnableHDR();
                    }
                    else
                    {
                        this->DisableHDR();
                    }

                    if (isBloomEnabledIn)
                    {
                        this->EnableBloom();
                    }
                    else
                    {
                        this->DisableBloom();
                    }
                }
                else
                {
                    GTEngine::Log("Error deserializing scene properties. Unsupported version (%d). Properties have been skipped.", header.version);
                    deserializer.Seek(header.sizeInBytes);
                }
            }
            else if (header.id == Serialization::ChunkID_Scene_Navigation)
            {
                readNavigation = true;

                if (header.version == 1)
                {
                    //deserializer.Seek(header.sizeInBytes);

                    // The navigation mesh count isn't actually used at the moment, but plans are in place for supporting multiple navigation meshes. This will be needed.
                    uint32_t navigationMeshCount;
                    deserializer.Read(navigationMeshCount);

                    this->navigationMesh.Deserialize(deserializer);
                }
                else
                {
                    GTEngine::Log("Error deserializing scene navigation. Unsupported version (%d). Navigation has been skipped.", header.version);
                    deserializer.Seek(header.sizeInBytes);
                }
            }
            else
            {
                // It's an unknown chunk. We need to push this over to the callback and let it handle the chunk.
                if (callback.IsChunkHandled(header))
                {
                    callback.HandleChunk(header, deserializer);
                }
                else
                {
                    deserializer.Seek(header.sizeInBytes);
                }
            }

            // We need to check the target read pointer and validate.
            size_t currentReadPosition = deserializer.Tell();
            if (currentReadPosition != targetReadPosition)
            {
                assert(false);      // <-- If you've hit this assert, you've got a serialization/deserialization error.

                // We're going to try to recover by seeking the difference.
                deserializer.Seek(targetReadPosition - currentReadPosition);

                assert(deserializer.Tell() == currentReadPosition);
            }
        }


        if (readInfo == false)
        {
            GTEngine::Log("Error deserializing scene. The info chunk (%d) was not found.", Serialization::ChunkID_Scene_Info);
            return false;
        }

        if (readSceneNodes == false)
        {
            GTEngine::Log("Error deserializing scene. The scene node chunk (%d) was not found.", Serialization::ChunkID_Scene_Nodes);
            return false;
        }

        if (readSceneNodesHierarchy == false)
        {
            GTEngine::Log("Error deserializing scene. The scene node hierarchy chunk (%d) was not found.", Serialization::ChunkID_Scene_NodesHierarchy);

            // We may have nodes instantiated, so they'll need to be killed.
            for (size_t iNode = 0; iNode < deserializedNodes.count; ++iNode)
            {
                delete deserializedNodes[iNode];
            }

            return false;
        }



        // At this point everything will have loaded successfully. We will now want to clear the scene add add the new scene nodes.
        this->RemoveAllSceneNodes();

        // The current state stack will be invalid, so that also needs to be cleared.
        this->stateStack.Clear();


        for (size_t iNode = 0; iNode < deserializedNodes.count; ++iNode)
        {
            auto node = deserializedNodes[iNode];
            assert(node != nullptr);
            {
                this->AddSceneNode(*node);

                // This node was created by this class, so we need to mark it as such.
                assert(!this->sceneNodesCreatedByScene.Exists(node->GetID()));
                {
                    this->sceneNodesCreatedByScene.Insert(node->GetID());
                }
            }
        }


        for (size_t i = 0; i < childParentPairs.count; ++i)
        {
            auto &pair = childParentPairs[i];

            auto childSceneNode  = deserializedNodes[pair.index0];
            auto parentSceneNode = deserializedNodes[pair.index1];
            
            assert(childSceneNode  != nullptr);
            assert(parentSceneNode != nullptr);
            {
                parentSceneNode->AttachChild(*childSceneNode);
            }
        }


        // Now we need to re-link all relevant scene nodes to their prefabs. If we don't do this, they will not be using the potentially new prefab definition.
        for (size_t iSceneNode = 0; iSceneNode < rootSceneNodesLinkedToPrefabs.count; ++iSceneNode)
        {
            auto node = rootSceneNodesLinkedToPrefabs[iSceneNode];
            assert(node != nullptr);
            {
                auto prefabComponent = node->GetComponent<PrefabComponent>();
                assert(prefabComponent != nullptr);
                {
                    this->GetPrefabLinker().LinkSceneNodeToPrefab(*node, prefabComponent->GetPrefabRelativePath());
                }
            }
        }
#endif


        return true;
    }


    bool Scene::SerializeStateStack(GTLib::Serializer &serializer) const
    {
        this->stateStack.Serialize(serializer);

        return true;
    }

    bool Scene::DeserializeStateStack(GTLib::Deserializer &deserializer)
    {
        this->stateStack.Deserialize(deserializer);

        return true;
    }




    void Scene::OnUpdate(double)
    {
    }


    void Scene::OnSceneNodeAdded(SceneNode &node)
    {
        // Can only stage this scene node after it's been given a valid ID.
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageInsert(node.GetID());
        }


        // We need to add the node to the update manager.
        if (this->updateManager.NeedsUpdate(node))
        {
            this->updateManager.AddSceneNode(node);
        }


        // What we're now going to do is call this->OnSceneNodeComponentAdded() for every component. This allows us to avoid a lot of annoying code duplication.
        GTLib::Vector<GTLib::String> componentNames;
        node.GetAttachedComponentNames(componentNames);

        for (size_t i = 0; i < componentNames.count; ++i)
        {
            auto component = node.GetComponentByName(componentNames[i].c_str());
            assert(component != nullptr);
            {
                this->OnSceneNodeComponentAdded(node, *component, DoNotPostEvents);
            }
        }


        // If the scene is registered to a script and the new scene node has a script component, we need to instantiate the scene node on the script side. If we
        // don't do this straight away, we'll run into problems when posting events to the scene node's script.
        if (this->registeredScript != nullptr && node.HasComponent<ScriptComponent>())
        {
            Scripting::InstantiateSceneNode(*this->registeredScript, node);

            // We now need to post an OnStartup event if applicable. All scene nodes should have 1 OnStartup and 1 OnShutdown called for each time they are part
            // of the scene.
            this->PostSceneNodeScriptEvent_OnStartup(node);
        }


        // Event handlers need to know.
        this->PostEvent_OnSceneNodeAdded(node);
    }

    void Scene::OnSceneNodeRemoved(SceneNode &node)
    {
        // Important to stage this change before removing it.
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageDelete(node.GetID());
        }


        // After the delete has been staged on the state stack, we need to completely disable state stack staging on the scene node. If we don't do this, removing
        // components will cause an Update to be placed in the staging area.
        node.DisableStateStackStaging();


        // We now need to post the OnShutdown event if applicable.
        this->PostSceneNodeScriptEvent_OnShutdown(node);


        // What we're now going to do is call this->OnSceneNodeComponentRemoved() for every component. This allows us to avoid a lot of annoying code duplication.
        GTLib::Vector<GTLib::String> componentNames;
        node.GetAttachedComponentNames(componentNames);

        for (size_t i = 0; i < componentNames.count; ++i)
        {
            auto component = node.GetComponentByName(componentNames[i].c_str());
            assert(component != nullptr);
            {
                this->OnSceneNodeComponentRemoved(node, *component, DoNotPostEvents);
            }
        }


        // The scene node will need to be uninstantiated if the scene is registered to a script.
        if (this->registeredScript != nullptr)
        {
            Scripting::UninstantiateSceneNode(*this->registeredScript, node);
        }


        // The node must be removed from the update manager.
        this->updateManager.RemoveSceneNode(node);

        // Event handlers need to know.
        this->PostEvent_OnSceneNodeRemoved(node);
    }

    void Scene::OnSceneNodeNameChanged(SceneNode &node)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        this->PostEvent_OnSceneNodeNameChanged(node);
    }

    void Scene::OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
            
            if (node.GetParent() != nullptr)
            {
                this->stateStack.StageUpdate(node.GetParent()->GetID());
            }

            if (previousParent != nullptr)
            {
                this->stateStack.StageUpdate(previousParent->GetID());
            }
        }


        // With a change in parent, we may also have a change in the transformation.
        this->OnSceneNodeTransform(node, true);
        this->OnSceneNodeScale(node);

        this->PostEvent_OnSceneNodeParentChanged(node, previousParent);
    }

    void Scene::OnSceneNodeTransform(SceneNode &node, bool updateDynamicsObject)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        // Culling.
        if (node.IsVisible())
        {
            if (node.HasComponent<ModelComponent>())
            {
                if (node.GetComponent<ModelComponent>()->GetModel() != nullptr)
                {
                    this->cullingManager.UpdateModelTransform(node);
                }
            }
            if (node.HasComponent<PointLightComponent>())
            {
                this->cullingManager.UpdatePointLightTransform(node);
            }
            if (node.HasComponent<SpotLightComponent>())
            {
                this->cullingManager.UpdateSpotLightTransform(node);
            }
            if (node.HasComponent<DirectionalLightComponent>())
            {
                this->cullingManager.UpdateDirectionalLightTransform(node);
            }
            if (node.HasComponent<AmbientLightComponent>())
            {
                this->cullingManager.UpdateAmbientLightTransform(node);
            }
            if (node.HasComponent<ParticleSystemComponent>())
            {
                this->cullingManager.UpdateParticleSystemTransform(node);
            }
            if (node.HasComponent<OccluderComponent>())
            {
                this->cullingManager.UpdateOccluderTransform(node);
            }
        }


        // Physics and Collision Detection.
        if (updateDynamicsObject)
        {
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.UpdateTransform(dynamicsComponent->GetRigidBody(), node.GetWorldTransformWithoutScale(), dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
            }
        }

        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            this->physicsManager.UpdateTransform(proximityComponent->GetGhostObject(), node.GetWorldTransformWithoutScale());
        }



        // Event handlers need to know about this.
        this->PostEvent_OnSceneNodeTransform(node);
    }

    void Scene::OnSceneNodeScale(SceneNode &node)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        // Culling.
        if (node.IsVisible())
        {
            if (node.HasComponent<ModelComponent>())
            {
                if (node.GetComponent<ModelComponent>()->GetModel() != nullptr)
                {
                    this->cullingManager.UpdateModelScale(node);
                }
            }
            if (node.HasComponent<OccluderComponent>())
            {
                this->cullingManager.UpdateOccluderScale(node);
            }
        }


        // Physics and Collision Detection.
        auto dynamics = node.GetComponent<DynamicsComponent>();
        if (dynamics != nullptr)
        {
            dynamics->ApplyScaling(node.GetWorldScale());
        }

        auto proximity = node.GetComponent<ProximityComponent>();
        if (proximity != nullptr)
        {
            proximity->ApplyScaling(node.GetWorldScale());
        }


        // Event handlers need to know about this.
        this->PostEvent_OnSceneNodeScale(node);
    }

    void Scene::OnSceneNodeStaticChanged(SceneNode &node)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }

    void Scene::OnSceneNodeVisibleChanged(SceneNode &node)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        if (node.IsVisible())
        {
            // Culling.
            if (node.HasComponent<ModelComponent>())
            {
                if (node.GetComponent<ModelComponent>()->GetModel() != nullptr)
                {
                    this->cullingManager.AddModel(node);
                }
            }
            if (node.HasComponent<PointLightComponent>())
            {
                this->cullingManager.AddPointLight(node);
            }
            if (node.HasComponent<SpotLightComponent>())
            {
                this->cullingManager.AddSpotLight(node);
            }
            if (node.HasComponent<DirectionalLightComponent>())
            {
                this->cullingManager.AddDirectionalLight(node);
            }
            if (node.HasComponent<AmbientLightComponent>())
            {
                this->cullingManager.AddAmbientLight(node);
            }
            if (node.HasComponent<ParticleSystemComponent>())
            {
                this->cullingManager.AddParticleSystem(node);
            }
            if (node.HasComponent<OccluderComponent>())
            {
                this->cullingManager.AddOccluder(node);
            }


            // Physics and Collision Detection.
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.AddRigidBody(dynamicsComponent->GetRigidBody(), dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
            }

            auto proximityComponent = node.GetComponent<ProximityComponent>();
            if (proximityComponent != nullptr)
            {
                this->physicsManager.AddGhostObject(proximityComponent->GetGhostObject(), proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
            }



            // The event handlers need to know.
            this->PostEvent_OnSceneNodeShow(node);
        }
        else
        {
            // Culling.
            if (node.HasComponent<ModelComponent>())
            {
                this->cullingManager.RemoveModel(node);
            }
            if (node.HasComponent<PointLightComponent>())
            {
                this->cullingManager.RemovePointLight(node);
            }
            if (node.HasComponent<SpotLightComponent>())
            {
                this->cullingManager.RemoveSpotLight(node);
            }
            if (node.HasComponent<DirectionalLightComponent>())
            {
                this->cullingManager.RemoveDirectionalLight(node);
            }
            if (node.HasComponent<AmbientLightComponent>())
            {
                this->cullingManager.RemoveAmbientLight(node);
            }
            if (node.HasComponent<ParticleSystemComponent>())
            {
                this->cullingManager.RemoveParticleSystem(node);
            }
            if (node.HasComponent<OccluderComponent>())
            {
                this->cullingManager.RemoveOccluder(node);
            }


            // Physics and Collision Detection.
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.RemoveRigidBody(dynamicsComponent->GetRigidBody());
            }

            auto proximityComponent = node.GetComponent<ProximityComponent>();
            if (proximityComponent != nullptr)
            {
                this->physicsManager.RemoveGhostObject(proximityComponent->GetGhostObject());
            }


            // The event handlers need to know.
            this->PostEvent_OnSceneNodeHide(node);
        }
    }

    void Scene::OnSceneNodeComponentAdded(SceneNode &node, Component &component, bool postEvents)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        if (GTLib::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            if (node.IsVisible())
            {
                if (static_cast<ModelComponent &>(component).GetModel() != nullptr)
                {
                    this->cullingManager.AddModel(node);
                }
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddPointLight(node);
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddSpotLight(node);
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddDirectionalLight(node);
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddAmbientLight(node);
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
        {
            auto &particleSystemComponent = static_cast<ParticleSystemComponent &>(component);

            if (node.IsVisible())
            {
                auto particleSystem = particleSystemComponent.GetParticleSystem();
                if (particleSystem != nullptr)
                {
                    this->cullingManager.AddParticleSystem(node);
                    
                    // Gravity needs to be set.
                    particleSystem->SetGravity(this->GetGravity());

                    // We will want to start playing immediately if applicable.
                    if (particleSystemComponent.IsPlayingOnStartup())
                    {
                        particleSystemComponent.Play();
                    }
                }
            }

            // We want the scene know about this scene node so we can easily update AABBs.
            this->sceneNodesWithParticleSystemComponents.Add(node.GetID(), &particleSystemComponent);
        }
        else if (GTLib::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddOccluder(node);
            }
        }
        else
        {
            if (GTLib::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                auto &dynamicsComponent = static_cast<DynamicsComponent &>(component);

                dynamicsComponent.ApplySceneNodeScaling();
                dynamicsComponent.ApplySceneNodeTransformation();

                if (node.IsVisible())
                {
                    if (dynamicsComponent.GetCollisionShape().getNumChildShapes() > 0)
                    {
                        this->physicsManager.AddRigidBody(dynamicsComponent.GetRigidBody(), dynamicsComponent.GetCollisionGroup(), dynamicsComponent.GetCollisionMask());
                    }
                    else
                    {
                        Log("Warning: Attempting to add a dynamics component without collision shapes. The rigid body has not been added to the dynamics world.");
                    }
                }
            }
            else if (GTLib::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                auto &proximityComponent = static_cast<ProximityComponent &>(component);

                proximityComponent.ApplySceneNodeScaling();
                proximityComponent.ApplySceneNodeTransformation();

                if (node.IsVisible())
                {
                    if (proximityComponent.GetGhostObject().getCollisionShape() != nullptr)
                    {
                        this->physicsManager.AddGhostObject(proximityComponent.GetGhostObject(), proximityComponent.GetCollisionGroup(), proximityComponent.GetCollisionMask());
                    }
                    else
                    {
                        Log("Warning: Attempting to add a proximity component without a collision shape. Ignoring.");
                    }
                }


                // We want to the scene to know about this scene node.
                this->sceneNodesWithProximityComponents.Add(node.GetID(), &proximityComponent);
            }
            else
            {
                // Constraints.
                if (GTLib::Strings::Equal(component.GetName(), GenericConstraintComponent::Name))
                {
                    auto constraint = static_cast<GenericConstraintComponent &>(component).GetConstraint();
                    if (constraint != nullptr)
                    {
                        this->physicsManager.AddConstraint(*constraint);
                    }
                    else
                    {
                        Log("Warning: Attempting to add a generic constraint component without attachments. Ignoring.");
                    }
                }
                else if (GTLib::Strings::Equal(component.GetName(), ConeTwistConstraintComponent::Name))
                {
                    auto constraint = static_cast<ConeTwistConstraintComponent &>(component).GetConstraint();
                    if (constraint != nullptr)
                    {
                        this->physicsManager.AddConstraint(*constraint);
                    }
                    else
                    {
                        Log("Warning: Attempting to add a cone twist constraint component without attachments. Ignoring.");
                    }
                }
                else if (GTLib::Strings::Equal(component.GetName(), PointToPointConstraintComponent::Name))
                {
                    auto constraint = static_cast<PointToPointConstraintComponent &>(component).GetConstraint();
                    if (constraint != nullptr)
                    {
                        this->physicsManager.AddConstraint(*constraint);
                    }
                    else
                    {
                        Log("Warning: Attempting to add a point-to-point constraint component without attachments. Ignoring.");
                    }
                }
                else
                {
                    if (GTLib::Strings::Equal(component.GetName(), ScriptComponent::Name))
                    {
                        this->updateManager.RemoveSceneNode(node);

                        if (this->updateManager.NeedsUpdate(node))
                        {
                            this->updateManager.AddSceneNode(node);
                        }


                        // The scene node needs to be instantiated in the scripting environment.
                        if (this->registeredScript != nullptr)
                        {
                            Scripting::InstantiateSceneNode(*this->registeredScript, node);
                        }
                    }
                }
            }
        }

        // The component may need to be registered.
        if (this->registeredScript != nullptr/* && node.HasComponent<ScriptComponent>()*/)
        {
            Scripting::RegisterComponent(*this->registeredScript, node, component.GetName());
        }


        if (postEvents)
        {
            this->PostEvent_OnSceneNodeComponentAdded(node, component);
        }
    }

    void Scene::OnSceneNodeComponentRemoved(SceneNode &node, Component &component, bool postEvents)
    {
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        if (GTLib::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            this->cullingManager.RemoveModel(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->cullingManager.RemovePointLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.RemoveSpotLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->cullingManager.RemoveDirectionalLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->cullingManager.RemoveAmbientLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
        {
            this->cullingManager.RemoveParticleSystem(node);

            // The scene needs to know about this.
            this->sceneNodesWithParticleSystemComponents.RemoveByKey(node.GetID());
        }
        else if (GTLib::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            this->cullingManager.RemoveOccluder(node);
        }
        else
        {
            if (GTLib::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                this->physicsManager.RemoveRigidBody(static_cast<DynamicsComponent &>(component).GetRigidBody());
            }
            else if (GTLib::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                this->physicsManager.RemoveGhostObject(static_cast<ProximityComponent &>(component).GetGhostObject());

                // We want to the scene to know about this change.
                this->sceneNodesWithProximityComponents.RemoveByKey(node.GetID());
            }
            else
            {
                if (GTLib::Strings::Equal(component.GetName(), GenericConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<GenericConstraintComponent &>(component).GetConstraint());
                }
                else if (GTLib::Strings::Equal(component.GetName(), ConeTwistConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<ConeTwistConstraintComponent &>(component).GetConstraint());
                }
                else if (GTLib::Strings::Equal(component.GetName(), PointToPointConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<PointToPointConstraintComponent &>(component).GetConstraint());
                }
                else
                {
                    if (GTLib::Strings::Equal(component.GetName(), ScriptComponent::Name))
                    {
                        // We do this exactly the same as when the script component is changed.
                        this->updateManager.RemoveSceneNode(node);

                        if (this->updateManager.NeedsUpdate(node))
                        {
                            this->updateManager.AddSceneNode(node);
                        }
                    }
                }
            }
        }

        // The component will need to be unregistered from the Lua representation, if applicable.
        if (this->registeredScript != nullptr/* && node.HasComponent<ScriptComponent>()*/)
        {
            Scripting::UnregisterComponent(*this->registeredScript, node, component.GetName());
        }


        if (postEvents)
        {
            this->PostEvent_OnSceneNodeComponentRemoved(node, component);
        }
    }

    void Scene::OnSceneNodeComponentChanged(SceneNode &node, Component &component, uint32_t whatChangedFlags)
    {
        // The node has been updated, so we need to stage it for the next commit.
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        if (GTLib::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            this->cullingManager.RemoveModel(node);

            if (static_cast<ModelComponent &>(component).GetModel() != nullptr)
            {
                this->cullingManager.AddModel(node);
            }
        }
        else if (GTLib::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->cullingManager.RemovePointLight(node);
            this->cullingManager.AddPointLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.RemoveSpotLight(node);
            this->cullingManager.AddSpotLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->cullingManager.RemoveDirectionalLight(node);
            this->cullingManager.AddDirectionalLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->cullingManager.RemoveAmbientLight(node);
            this->cullingManager.AddAmbientLight(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
        {
            this->cullingManager.RemoveParticleSystem(node);
            this->cullingManager.AddParticleSystem(node);
        }
        else if (GTLib::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            this->cullingManager.RemoveOccluder(node);
            this->cullingManager.AddOccluder(node);
        }
        else
        {
            if (GTLib::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                auto &dynamicsComponent = static_cast<DynamicsComponent &>(component);

                this->physicsManager.RemoveRigidBody(dynamicsComponent.GetRigidBody());

                dynamicsComponent.ApplySceneNodeScaling();
                dynamicsComponent.ApplySceneNodeTransformation();

                if (node.IsVisible())
                {
                    if (dynamicsComponent.GetCollisionShape().getNumChildShapes() > 0)
                    {
                        this->physicsManager.AddRigidBody(dynamicsComponent.GetRigidBody(), dynamicsComponent.GetCollisionGroup(), dynamicsComponent.GetCollisionMask());
                    }
                }
            }
            else if (GTLib::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                auto &proximityComponent = static_cast<ProximityComponent &>(component);

                // We just remove and re-add. OnTransform and OnScale will ensure everything is positioned and scaled properly.
                if (node.IsVisible())
                {
                    this->physicsManager.RemoveGhostObject(proximityComponent.GetGhostObject());
                    this->physicsManager.AddGhostObject(proximityComponent.GetGhostObject(), proximityComponent.GetCollisionGroup(), proximityComponent.GetCollisionMask());
                }
            }
            else
            {
                if (GTLib::Strings::Equal(component.GetName(), ScriptComponent::Name))
                {
                    // In this case of a script, there's a chance that it won't want to be updated anymore or vice versa. We can get around this easily
                    // enough by just removing and re-adding it.

                    this->updateManager.RemoveSceneNode(node);

                    if (this->updateManager.NeedsUpdate(node))
                    {
                        this->updateManager.AddSceneNode(node);
                    }
                }
            }
        }

        // We will re-register the component, if applicable. We do this to ensure the scripting representation of the scene node is up-to-date.
        if (this->registeredScript != nullptr/* && node.HasComponent<ScriptComponent>()*/)
        {
            Scripting::RegisterComponent(*this->registeredScript, node, component.GetName());
        }


        // The event handler needs to know about this.
        this->PostEvent_OnSceneNodeComponentChanged(node, component, whatChangedFlags);
    }


    void Scene::PostSceneNodeScriptEvent_OnStartup()
    {
        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            size_t sceneNodeCount = this->sceneNodes.GetCount();
            for (size_t i = 0; i < sceneNodeCount; ++i)
            {
                auto sceneNode = this->sceneNodes.GetSceneNodeAtIndex(i);
                assert(sceneNode != nullptr);
                {
                    this->PostSceneNodeScriptEvent_OnStartup(*sceneNode);
                }
            }
        }
    }

    void Scene::PostSceneNodeScriptEvent_OnStartup(SceneNode &sceneNode)
    {
        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            auto scriptComponent = sceneNode.GetComponent<ScriptComponent>();
            if (scriptComponent != nullptr && !scriptComponent->HasOnStartupBeenCalled())
            {
                if (scriptComponent->HasOnStartup())
                {
                    Scripting::PostSceneNodeEvent_OnStartup(*this->registeredScript, sceneNode);
                }

                scriptComponent->MarkOnStartupAsCalled();
            }
        }
    }


    void Scene::PostSceneNodeScriptEvent_OnShutdown()
    {
        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            size_t sceneNodeCount = this->sceneNodes.GetCount();
            for (size_t i = 0; i < sceneNodeCount; ++i)
            {
                auto sceneNode = this->sceneNodes.GetSceneNodeAtIndex(i);
                assert(sceneNode != nullptr);
                {
                    this->PostSceneNodeScriptEvent_OnShutdown(*sceneNode);
                }
            }
        }
    }

    void Scene::PostSceneNodeScriptEvent_OnShutdown(SceneNode &sceneNode)
    {
        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            auto scriptComponent = sceneNode.GetComponent<ScriptComponent>();
            if (scriptComponent != nullptr && scriptComponent->HasOnStartupBeenCalled())
            {
                if (scriptComponent->HasOnShutdown())
                {
                    Scripting::PostSceneNodeEvent_OnShutdown(*this->registeredScript, sceneNode);
                }

                scriptComponent->UnmarkOnStartupAsCalled();
            }
        }
    }



    ///////////////////////////////////////////////////////
    // Private

    uint64_t Scene::GenerateSceneNodeID()
    {
        assert(this->minAutoSceneNodeID > 0);
        {
            if (this->nextSceneNodeID + 1 < this->minAutoSceneNodeID)
            {
                this->nextSceneNodeID = this->minAutoSceneNodeID;
            }
            else
            {
                this->nextSceneNodeID += 1;
            }

            return this->nextSceneNodeID;
        }
    }



    // Event Posting

    void Scene::PostEvent_OnSceneNodeAdded(SceneNode &sceneNode)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeAdded(sceneNode);
        }
    }

    void Scene::PostEvent_OnSceneNodeRemoved(SceneNode &sceneNode)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeRemoved(sceneNode);
        }
    }


    void Scene::PostEvent_OnSceneNodeNameChanged(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeNameChanged(node);
        }
    }

    void Scene::PostEvent_OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeParentChanged(node, previousParent);
        }
    }


    void Scene::PostEvent_OnSceneNodeTransform(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeTransform(node);
        }
    }

    void Scene::PostEvent_OnSceneNodeScale(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeScale(node);
        }
    }


    void Scene::PostEvent_OnSceneNodeShow(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeShow(node);
        }

        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            auto scriptComponent = node.GetComponent<ScriptComponent>();
            if (scriptComponent != nullptr && scriptComponent->HasOnShow())
            {
                Scripting::PostSceneNodeEvent_OnShow(*this->registeredScript, node);
            }
        }
    }

    void Scene::PostEvent_OnSceneNodeHide(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeHide(node);
        }

        if (this->registeredScript != nullptr && !this->isScriptEventsBlocked)
        {
            auto scriptComponent = node.GetComponent<ScriptComponent>();
            if (scriptComponent != nullptr && scriptComponent->HasOnHide())
            {
                Scripting::PostSceneNodeEvent_OnHide(*this->registeredScript, node);
            }
        }
    }

    

    void Scene::PostEvent_OnSceneNodeComponentAdded(SceneNode &node, Component &component)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeComponentAdded(node, component);
        }
    }

    void Scene::PostEvent_OnSceneNodeComponentRemoved(SceneNode &node, Component &component)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeComponentRemoved(node, component);
        }
    }

    void Scene::PostEvent_OnSceneNodeComponentChanged(SceneNode &node, Component &component, uint32_t whatChangedFlags)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeComponentChanged(node, component, whatChangedFlags);
        }
    }

    void Scene::PostEvent_OnStateStackFrameCommitted()
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnStateStackFrameCommitted();
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
