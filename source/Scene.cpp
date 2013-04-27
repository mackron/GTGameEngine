// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTCore/ToString.hpp>

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
        SceneCullingCallback(GTCore::Vector<const ModelComponent*> &modelComponentsOut, GTCore::Vector<const PointLightComponent*> &pointLightComponentsOut, GTCore::Vector<const SpotLightComponent*> &spotLightComponentsOut,
                             GTCore::Vector<const AmbientLightComponent*> &ambientLightComponentsOut, GTCore::Vector<const DirectionalLightComponent*> &directionalLightComponentsOut)
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
        GTCore::Vector<const ModelComponent*>            &modelComponents;
        GTCore::Vector<const PointLightComponent*>       &pointLightComponents;
        GTCore::Vector<const SpotLightComponent*>        &spotLightComponents;
        GTCore::Vector<const AmbientLightComponent*>     &ambientLightComponents;
        GTCore::Vector<const DirectionalLightComponent*> &directionalLightComponents;


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
    static const uint32_t SceneMagicNumber = 0x4f25a8b0;

    static const bool     DoNotPostEvents  = false;              // <-- For Scene::OnSceneNodeComponentAdded() and Scene::OnSceneNodeComponentRemoved().


    Scene::Scene()
        : renderer(new DefaultSceneRenderer),
          updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          deleteRenderer(true), deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true),
          paused(false),
          viewports(), sceneNodes(), nextSceneNodeID(0), minAutoSceneNodeID(1), sceneNodesCreatedByScene(),
          sceneNodesWithProximityComponents(), sceneNodesWithParticleSystemComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true),
          registeredScript(nullptr)
    {
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : renderer(new DefaultSceneRenderer),
          updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          deleteRenderer(true), deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false),
          paused(false),
          viewports(), sceneNodes(), nextSceneNodeID(0), minAutoSceneNodeID(1), sceneNodesCreatedByScene(),
          sceneNodesWithProximityComponents(), sceneNodesWithParticleSystemComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true),
          registeredScript(nullptr)
    {
    }

    Scene::~Scene()
    {
        this->RemoveAllViewports();
        this->RemoveAllSceneNodes();


        // We need to unregister after removing every object.
        this->UnregisterFromScript();


        if (deleteRenderer)
        {
            delete this->renderer;
        }


        if (deleteUpdateManager)
        {
            delete &this->updateManager;
        }

        if (deletePhysicsManager)
        {
            delete &this->physicsManager;
        }

        if (deleteCullingManager)
        {
            delete &this->cullingManager;
        }
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
                    GTEngine::PostError("Error adding scene node to scene. A scene node of the same ID (%s) already exists. The scene node was not added.", GTCore::ToString(uniqueID).c_str());
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
                GTCore::Vector<SceneNode*> children;

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
        auto sceneNode = new SceneNode;
        this->AddSceneNode(*sceneNode);     // <-- This will generate the unique ID.

        assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
        {
            this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
        }

        return sceneNode;
    }

    SceneNode* Scene::CreateNewSceneNode(GTCore::Deserializer &deserializer, bool createNewIDIfExists)
    {
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
                Log("Scene::CreateNewSceneNode(GTCore::Deserializer &) - A scene node of the same ID already exists. This is an erroneous condition. Instead, try 'Scene::CreateNewSceneNode()' followed by 'newSceneNode->Deserialize(deserializer)'");

                delete sceneNode;
                return nullptr;
            }
        }


        this->AddSceneNode(*sceneNode);

        assert(!this->sceneNodesCreatedByScene.Exists(sceneNode->GetID()));
        {
            this->sceneNodesCreatedByScene.Insert(sceneNode->GetID());
        }

        return sceneNode;
    }


    SceneNode* Scene::CreateNewSceneNode(const SceneNodeClass &sceneNodeClass)
    {
        auto &serializers = sceneNodeClass.GetSerializers();
        auto &hierarchy   = sceneNodeClass.GetHierarchy();

        if (serializers.count > 0)
        {
            GTCore::Map<uint64_t, SceneNode*> newSceneNodes;
            SceneNode*                        rootSceneNode = nullptr;

            // We will iterate over each serializer and create nodes from them, ensuring we set the ID to 0 so that a new one is generated when we add them.
            for (size_t i = 0; i < serializers.count; ++i)
            {
                auto id         = serializers.buffer[i]->key;
                auto serializer = serializers.buffer[i]->value;

                assert(id         != 0);
                assert(serializer != nullptr);
                {
                    GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());

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


        // Before doing anything we're going to step the dynamics.
        if (!this->IsPaused())
        {
            this->physicsManager.Step(deltaTimeInSeconds);
        }


        // Now we need to update via the update manager.
        if (!this->IsPaused())
        {
            this->updateManager.Step(deltaTimeInSeconds);
        }


        // We want to update the AABBs of each particle system.
        for (size_t iParticleSystem = 0; iParticleSystem < this->sceneNodesWithParticleSystemComponents.count; ++iParticleSystem)
        {
            auto particleSystemComponent = this->sceneNodesWithParticleSystemComponents.buffer[iParticleSystem]->value;
            assert(particleSystemComponent != nullptr);
            {
                if (particleSystemComponent->GetParticleSystem() != nullptr)
                {
                    this->cullingManager.UpdateParticleSystemAABB(particleSystemComponent->GetNode());
                }
            }
        }


        // We now want to check proximity components.
        for (size_t i = 0; i < this->sceneNodesWithProximityComponents.count; ++i)
        {
            auto proximityComponent = this->sceneNodesWithProximityComponents.buffer[i]->value;
            assert(proximityComponent != nullptr);
            {
                auto sceneNode = &proximityComponent->GetNode();

                GTCore::Vector<uint64_t> sceneNodesEntered;
                GTCore::Vector<uint64_t> sceneNodesLeft;
                proximityComponent->UpdateContainment(sceneNodesEntered, sceneNodesLeft);

                for (size_t iEntered = 0; iEntered < sceneNodesEntered.count; ++iEntered)
                {
                    auto enteredSceneNode = this->GetSceneNodeByID(sceneNodesEntered[iEntered]);
                    assert(enteredSceneNode != nullptr);
                    {
                        // TODO: Call the C++ events.

                        // If we're linked to a script we'll need to notify the script about this.
                        if (this->registeredScript != nullptr)
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
                        if (this->registeredScript != nullptr)
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
                if (GTCore::Strings::Equal(node->GetName(), name))
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



    void Scene::RegisterToScript(GTCore::Script &script)
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


    void Scene::BuildNavigationMesh()
    {
        this->navigationMesh.Build(*this);
    }

    void Scene::FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output)
    {
        this->navigationMesh.FindPath(start, end, output);
    }



    ////////////////////////////////////////////////////////////
    // Serialization/Deserialization.

    bool Scene::Serialize(GTCore::Serializer &serializer) const
    {
        Serialization::ChunkHeader header;


        // Scene Info (nextSceneNodeID, etc).
        header.id          = Serialization::ChunkID_Scene_Info;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(uint64_t) +              // <-- nextSceneNodeID
            sizeof(uint64_t);               // <-- minAutoSceneNodeID

        serializer.Write(header);
        serializer.Write(this->nextSceneNodeID);
        serializer.Write(this->minAutoSceneNodeID);



        // This chunk is the flat list of scene nodes. Not every scene node should be serialized here, so what we'll do is create
        // a flat vector containing pointers to the scene nodes that should be written. We use a vector here to make it easier to
        // grab the indices for the hierarchy.
        GTCore::Vector<const SceneNode*> serializedNodes;
        for (size_t i = 0; i < this->sceneNodes.GetCount(); ++i)
        {
            auto node = this->sceneNodes.GetSceneNodeAtIndex(i);
            assert(node != nullptr);
            {
                if (node->IsSerializationEnabled())
                {
                    serializedNodes.PushBack(node);
                }
            }
        }

        // At this point we have a list of all the scene nodes being serialized. We can now start on serializing. The chunk ID requires
        // a body size in bytes. What we will do is use a second basic serializer, and then copy that data into the main serializer. While
        // we're in this loop, we're actually going to construct data for the next chunk, which is the hierarchy chunk. We do this here
        // to save us from another loop later on.
        GTCore::BasicSerializer secondarySerializer;
        GTCore::Vector<Serialization::SceneNodeIndexPair> childParentPairs(serializedNodes.count);

        // We need a count here so we can deserialize effectively.
        secondarySerializer.Write(static_cast<uint32_t>(serializedNodes.count));

        for (size_t iNode = 0; iNode < serializedNodes.count; ++iNode)
        {
            auto node = serializedNodes[iNode];
            assert(node != nullptr);
            {
                // Serialize first...
                node->Serialize(secondarySerializer);

                // And then find the index of the parent, if applicable.
                if (node->GetParent() != nullptr)
                {
                    Serialization::SceneNodeIndexPair indexPair;
                    indexPair.index0 = iNode;

                    size_t parentIndex;
                    if (serializedNodes.FindFirstIndexOf(node->GetParent(), parentIndex))
                    {
                        indexPair.index1 = static_cast<uint32_t>(parentIndex);
                        childParentPairs.PushBack(indexPair);
                    }
                }
            }
        }


        // We have the data, now we just write it.
        header.id          = Serialization::ChunkID_Scene_Nodes;
        header.version     = 1;
        header.sizeInBytes = static_cast<uint32_t>(secondarySerializer.GetBufferSizeInBytes());

        serializer.Write(header);
        serializer.Write(secondarySerializer.GetBuffer(), header.sizeInBytes);



        // At this point the first chunck has been done. We can now save the second chunk, which is the hierarchy information. This is
        // made up of a series of index pairs, 32-bits each (64-bits in total). The first of each pair is the scene node in question. The
        // second index is that of it's parent, as defined in the serialized data (not the data defined in the C++ structure).
        //
        // We have already retrieved these pairs from the previous pass. We can now just copy them straight in.
        header.id          = Serialization::ChunkID_Scene_NodesHierarchy;
        header.version     = 1;
        header.sizeInBytes = sizeof(Serialization::SceneNodeIndexPair) * childParentPairs.count;

        serializer.Write(header);
        serializer.Write(childParentPairs.buffer, header.sizeInBytes);

        return true;
    }

    bool Scene::Deserialize(GTCore::Deserializer &deserializer)
    {
        GTCore::Vector<SceneNode*>                        deserializedNodes;
        GTCore::Vector<Serialization::SceneNodeIndexPair> childParentPairs;

        bool readInfo                = false;
        bool readSceneNodes          = false;
        bool readSceneNodesHierarchy = false;


        Serialization::ChunkHeader header;
        while (deserializer.Read(header) == sizeof(Serialization::ChunkHeader))
        {
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
                            node->Deserialize(deserializer);

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
            else
            {
                deserializer.Seek(header.sizeInBytes);
            }


            // We can break if all the chunks we need have been read.
            if (readSceneNodes && readSceneNodesHierarchy)
            {
                break;
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


        return true;
    }


    bool Scene::SerializeStateStack(GTCore::Serializer &serializer) const
    {
        this->stateStack.Serialize(serializer);

        return true;
    }

    bool Scene::DeserializeStateStack(GTCore::Deserializer &deserializer)
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
        GTCore::Vector<GTCore::String> componentNames;
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


        // We now need to post the OnShutdown event if applicable.
        this->PostSceneNodeScriptEvent_OnShutdown(node);


        // What we're now going to do is call this->OnSceneNodeComponentRemoved() for every component. This allows us to avoid a lot of annoying code duplication.
        GTCore::Vector<GTCore::String> componentNames;
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
                this->physicsManager.UpdateTransform(dynamicsComponent->GetRigidBody(), node.GetWorldTransformWithoutScale());
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

        auto proximity = node.GetComponent<DynamicsComponent>();
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


        if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            if (node.IsVisible())
            {
                if (static_cast<ModelComponent &>(component).GetModel() != nullptr)
                {
                    this->cullingManager.AddModel(node);
                }
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddPointLight(node);
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddSpotLight(node);
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddDirectionalLight(node);
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddAmbientLight(node);
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
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
                }
            }

            // We want the scene know about this scene node so we can easily update AABBs.
            this->sceneNodesWithParticleSystemComponents.Add(node.GetID(), &particleSystemComponent);
        }
        else if (GTCore::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            if (node.IsVisible())
            {
                this->cullingManager.AddOccluder(node);
            }
        }
        else
        {
            if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
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
            else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
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
                if (GTCore::Strings::Equal(component.GetName(), GenericConstraintComponent::Name))
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
                else if (GTCore::Strings::Equal(component.GetName(), ConeTwistConstraintComponent::Name))
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
                else if (GTCore::Strings::Equal(component.GetName(), PointToPointConstraintComponent::Name))
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
                    if (GTCore::Strings::Equal(component.GetName(), ScriptComponent::Name))
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


        if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            this->cullingManager.RemoveModel(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->cullingManager.RemovePointLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.RemoveSpotLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->cullingManager.RemoveDirectionalLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->cullingManager.RemoveAmbientLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
        {
            this->cullingManager.RemoveParticleSystem(node);

            // The scene needs to know about this.
            this->sceneNodesWithParticleSystemComponents.RemoveByKey(node.GetID());
        }
        else if (GTCore::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            this->cullingManager.RemoveOccluder(node);
        }
        else
        {
            if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
            {
                this->physicsManager.RemoveRigidBody(static_cast<DynamicsComponent &>(component).GetRigidBody());
            }
            else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
            {
                this->physicsManager.RemoveGhostObject(static_cast<ProximityComponent &>(component).GetGhostObject());

                // We want to the scene to know about this change.
                this->sceneNodesWithProximityComponents.RemoveByKey(node.GetID());
            }
            else
            {
                if (GTCore::Strings::Equal(component.GetName(), GenericConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<GenericConstraintComponent &>(component).GetConstraint());
                }
                else if (GTCore::Strings::Equal(component.GetName(), ConeTwistConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<ConeTwistConstraintComponent &>(component).GetConstraint());
                }
                else if (GTCore::Strings::Equal(component.GetName(), PointToPointConstraintComponent::Name))
                {
                    this->physicsManager.RemoveConstraint(*static_cast<PointToPointConstraintComponent &>(component).GetConstraint());
                }
                else
                {
                    if (GTCore::Strings::Equal(component.GetName(), ScriptComponent::Name))
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


        if (postEvents)
        {
            this->PostEvent_OnSceneNodeComponentRemoved(node, component);
        }
    }

    void Scene::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        // The node has been updated, so we need to stage it for the next commit.
        if (this->IsStateStackStagingEnabled() && node.IsStateStackStagingEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }


        if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            this->cullingManager.RemoveModel(node);

            if (static_cast<ModelComponent &>(component).GetModel() != nullptr)
            {
                this->cullingManager.AddModel(node);
            }
        }
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->cullingManager.RemovePointLight(node);
            this->cullingManager.AddPointLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.RemoveSpotLight(node);
            this->cullingManager.AddSpotLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->cullingManager.RemoveDirectionalLight(node);
            this->cullingManager.AddDirectionalLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->cullingManager.RemoveAmbientLight(node);
            this->cullingManager.AddAmbientLight(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), ParticleSystemComponent::Name))
        {
            this->cullingManager.RemoveParticleSystem(node);
            this->cullingManager.AddParticleSystem(node);
        }
        else if (GTCore::Strings::Equal(component.GetName(), OccluderComponent::Name))
        {
            this->cullingManager.RemoveOccluder(node);
            this->cullingManager.AddOccluder(node);
        }
        else
        {
            if (GTCore::Strings::Equal(component.GetName(), DynamicsComponent::Name))
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
            else if (GTCore::Strings::Equal(component.GetName(), ProximityComponent::Name))
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
                if (GTCore::Strings::Equal(component.GetName(), ScriptComponent::Name))
                {
                    // In this case of a script, there's a chance that it won't want to be updated anymore or vice versa. We can get around this easily
                    // enough by just removing and re-adding it.

                    this->updateManager.RemoveSceneNode(node);

                    if (this->updateManager.NeedsUpdate(node))
                    {
                        this->updateManager.AddSceneNode(node);
                    }


                    // If we're registered to a script we will need to update the public variables to those defined in the component.
                    if (this->registeredScript != nullptr)
                    {
                        Scripting::UpdatePublicVariables(*this->registeredScript, node);
                    }
                }
            }
        }



        // The event handler needs to know about this.
        this->PostEvent_OnSceneNodeComponentChanged(node, component);
    }


    void Scene::PostSceneNodeScriptEvent_OnStartup()
    {
        if (this->registeredScript != nullptr)
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
        if (this->registeredScript != nullptr)
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
        if (this->registeredScript != nullptr)
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
        if (this->registeredScript != nullptr)
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

        if (this->registeredScript != nullptr)
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

        if (this->registeredScript != nullptr)
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

    void Scene::PostEvent_OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeComponentChanged(node, component);
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
