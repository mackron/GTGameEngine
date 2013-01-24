
#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/GarbageCollector.hpp>
#include <GTEngine/Errors.hpp>
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
        SceneCullingCallback(GTCore::Vector<ModelComponent*> &modelComponentsOut, GTCore::Vector<PointLightComponent*> &pointLightComponentsOut, GTCore::Vector<SpotLightComponent*> &spotLightComponentsOut,
                             GTCore::Vector<AmbientLightComponent*> &ambientLightComponentsOut, GTCore::Vector<DirectionalLightComponent*> &directionalLightComponentsOut)
            : modelComponents(modelComponentsOut), pointLightComponents(pointLightComponentsOut), spotLightComponents(spotLightComponentsOut),
              ambientLightComponents(ambientLightComponentsOut), directionalLightComponents(directionalLightComponentsOut)
        {
        }

        /// Destructor.
        ~SceneCullingCallback()
        {
        }


        /// VisibleCallback::ProcessObjectModel().
        virtual void ProcessObjectModel(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<SceneNode &>(object).GetComponent<GTEngine::ModelComponent>();
                if (component != nullptr)
                {
                    this->modelComponents.PushBack(component);
                }
            }
        }

        /// VisibleCallback::ProcessObjectPointLight().
        virtual void ProcessObjectPointLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<SceneNode &>(object).GetComponent<GTEngine::PointLightComponent>();
                if (component != nullptr)
                {
                    this->pointLightComponents.PushBack(component);
                }
            }
        }

        /// VisibleCallback::ProcessObjectSpotLight().
        virtual void ProcessObjectSpotLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<SceneNode &>(object).GetComponent<GTEngine::SpotLightComponent>();
                if (component != nullptr)
                {
                    this->spotLightComponents.PushBack(component);
                }
            }
        }


        /// VisibleCallback::ProcessObjectAmbientLight().
        virtual void ProcessObjectAmbientLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<SceneNode &>(object).GetComponent<GTEngine::AmbientLightComponent>();
                if (component != nullptr)
                {
                    this->ambientLightComponents.PushBack(component);
                }
            }
        }

        /// VisibleCallback::ProcessObjectDirectionalLight().
        virtual void ProcessObjectDirectionalLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<SceneNode &>(object).GetComponent<GTEngine::DirectionalLightComponent>();
                if (component != nullptr)
                {
                    this->directionalLightComponents.PushBack(component);
                }
            }
        }


    private:
        GTCore::Vector<ModelComponent*>            &modelComponents;
        GTCore::Vector<PointLightComponent*>       &pointLightComponents;
        GTCore::Vector<SpotLightComponent*>        &spotLightComponents;
        GTCore::Vector<AmbientLightComponent*>     &ambientLightComponents;
        GTCore::Vector<DirectionalLightComponent*> &directionalLightComponents;


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

        virtual bool needsCollision(btBroadphaseProxy *proxy0) const
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


    Scene::Scene()
        : renderer(new DefaultSceneRenderer),
          updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          deleteRenderer(true), deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true),
          paused(false), isRefreshingObject(false),
          viewports(), sceneNodes(), nextSceneNodeID(0),
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true)
    {
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : renderer(new DefaultSceneRenderer),
          updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          deleteRenderer(true), deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false),
          paused(false), isRefreshingObject(false),
          viewports(), sceneNodes(), nextSceneNodeID(0),
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh(),
          eventHandlers(),
          stateStack(*this), isStateStackStagingEnabled(true)
    {
    }

    Scene::~Scene()
    {
        this->RemoveAllViewports();
        this->RemoveAllObjects();


        if (deleteRenderer)
        {
            GarbageCollector::MarkForCollection(this->renderer);
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

    void Scene::AddObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->AddSceneNode(static_cast<SceneNode &>(object));
        }
    }

    void Scene::RemoveObject(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            this->RemoveSceneNode(static_cast<SceneNode &>(object));
        }
    }

    
    void Scene::AddSceneNode(SceneNode &node)
    {
        node.SetScene(this);
    }

    void Scene::RemoveSceneNode(SceneNode &node)
    {
        node.SetScene(nullptr);
    }


    void Scene::RemoveAllObjects()
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


    void Scene::RefreshObject(SceneObject &object)
    {
        this->isRefreshingObject = true;
        {
            this->RemoveObject(object);
            this->AddObject(object);

            this->PostEvent_OnObjectRefreshed(object);
        }
        this->isRefreshingObject = false;
    }



    SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID)
    {
        return this->sceneNodes.FindByID(sceneNodeID);
    }

    const SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID) const
    {
        return this->sceneNodes.FindByID(sceneNodeID);
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


        // Now we need to render.
        this->renderer->Begin(*this);
        {
            for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
            {
                auto viewport = iViewport->value;
                assert(viewport != nullptr);

                this->renderer->RenderViewport(*this, *viewport);
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
        this->viewports.Append(&viewport);

        viewport.SetScene(this);
    }

    void Scene::RemoveViewport(SceneViewport &viewport)
    {
        assert(this->renderer != nullptr);

        if (viewport.GetScene() == this)
        {
            this->renderer->RemoveViewport(viewport);
            this->viewports.Remove(this->viewports.Find(&viewport));

            viewport.SetScene(nullptr);
        }
    }

    void Scene::RemoveAllViewports()
    {
        assert(this->renderer != nullptr);

        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            this->renderer->RemoveViewport(*iViewport->value);
            iViewport->value->SetScene(nullptr);
        }

        this->viewports.Clear();
    }


    void Scene::GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        // We'll retrieve the global AABB from the culling manager. Might need to move this over to physics manager, perhaps.
        this->cullingManager.GetGlobalAABB(aabbMin, aabbMax);
    }



    void Scene::SetRenderer(SceneRenderer &newRenderer)
    {
        assert(this->renderer != nullptr);

        // We need to remove all of the viewports from the renderer.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            assert(iViewport->value != nullptr);
            this->renderer->RemoveViewport(*iViewport->value);
        }

        if (this->deleteRenderer)
        {
            delete this->renderer;
        }


        this->renderer       = &newRenderer;
        this->deleteRenderer = false;


        // With the new renderer set, we now need to let it know about our existing viewports.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            assert(iViewport->value != nullptr);
            this->renderer->AddViewport(*iViewport->value);
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
                    if (node->IsSerializationEnabled())
                    {
                        this->stateStack.StageInsert(node->GetID());
                    }
                }
            }
        }


        this->stateStack.Commit();
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


    void Scene::QueryVisibleObjects(const glm::mat4 &mvp, SceneCullingManager::VisibilityCallback &callback)
    {
        this->cullingManager.ProcessVisibleObjects(mvp, callback);
    }



    void Scene::SetGravity(float x, float y, float z)
    {
        this->physicsManager.SetGravity(x, y, z);
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
            sizeof(uint64_t);           // <-- nextSceneNodeID

        serializer.Write(header);
        serializer.Write(this->nextSceneNodeID);



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


        if (readSceneNodes == false)
        {
            GTEngine::Log("Error deserializing scene. The scene node chunk (%d) was not found.");
            return false;
        }

        if (readSceneNodesHierarchy == false)
        {
            GTEngine::Log("Error deserializing scene. The scene node hierarchy chunk (%d) was not found.");

            // We may have nodes instantiated, so they'll need to be killed.
            for (size_t iNode = 0; iNode < deserializedNodes.count; ++iNode)
            {
                delete deserializedNodes[iNode];
            }

            return false;
        }


        // At this point everything will have loaded successfully. We will now want to clear the scene add add the new scene nodes.
        this->RemoveAllObjects();

        for (size_t iNode = 0; iNode < deserializedNodes.count; ++iNode)
        {
            auto node = deserializedNodes[iNode];
            assert(node != nullptr);
            {
                this->AddSceneNode(*node);
            }
        }

        return true;
    }




    void Scene::OnUpdate(double)
    {
    }


    void Scene::OnSceneNodeAdded(SceneNode &node)
    {
        uint64_t uniqueID = node.GetID();
        if (uniqueID == 0)
        {
            uniqueID = ++this->nextSceneNodeID;
            node.SetID(uniqueID);
        }
        else
        {
            // If a scene node of the same ID already exists, we have a bug somewhere.
            if (this->sceneNodes.FindByID(uniqueID) != nullptr)
            {
                GTEngine::PostError("Error adding scene node to scene. A scene node of the same ID (%s) already exists. The scene node was not added.", GTCore::ToString(uniqueID).c_str());
                return;
            }
        }


        this->sceneNodes.Insert(node);

        // We need to add the node to the update manager.
        if ((node.GetFlags() & SceneNode::NoUpdate) == 0)
        {
            if (this->updateManager.NeedsUpdate(node))
            {
                this->updateManager.AddObject(node);
            }
        }

        
        // Here we'll check the lighting components.
        auto ambientLightComponent = node.GetComponent<AmbientLightComponent>();
        if (ambientLightComponent != nullptr)
        {
            this->ambientLightComponents.Append(ambientLightComponent);
        }
        auto directionalLightComponent = node.GetComponent<DirectionalLightComponent>();
        if (directionalLightComponent != nullptr)
        {
            this->directionalLightComponents.Append(directionalLightComponent);
        }


        // If the scene node has a dynamics component, we need to add it's rigid body.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();

            // Now we need to ensure the rigid body is transformed correctly.
            auto &rigidBody = dynamicsComponent->GetRigidBody();


            if (node.IsVisible() && dynamicsComponent->GetCollisionShape().getNumChildShapes() > 0)
            {
                this->physicsManager.AddRigidBody(rigidBody, dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
            }
            else
            {
                Log("Warning: Attempting to add a dynamics component without collision shapes. The rigid body has not been added to the dynamics world.");
            }
        }

        // Just like DynamicsComponent, if we have proximity component, we need to add that also.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            auto &ghostObject = proximityComponent->GetGhostObject();
            if (ghostObject.getCollisionShape() != nullptr)
            {
                // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
                proximityComponent->ApplyScaling(node.GetWorldScale());

                btTransform transform;
                node.GetWorldTransform(transform);
                ghostObject.setWorldTransform(transform);

                if (node.IsVisible())
                {
                    this->physicsManager.AddGhostObject(ghostObject, proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
                }
            }
            else
            {
                Log("Warning: Attempting to add a proximity component without a collision shape. Ignoring.");
            }
        }


        // Again like the dynamics component, we need to add and constraint components.
        auto genericConstraintComponent = node.GetComponent<GenericConstraintComponent>();
        if (genericConstraintComponent != nullptr)
        {
            auto constraint = genericConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.AddConstraint(*constraint);
            }
            else
            {
                Log("Warning: Attempting to add a generic constraint component without attachments. Ignoring.");
            }
        }

        auto coneTwistConstraintComponent = node.GetComponent<ConeTwistConstraintComponent>();
        if (coneTwistConstraintComponent != nullptr)
        {
            auto constraint = coneTwistConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.AddConstraint(*constraint);
            }
            else
            {
                Log("Warning: Attempting to add a cone twist constraint component without attachments. Ignoring.");
            }
        }

        auto pointToPointConstraintComponent = node.GetComponent<PointToPointConstraintComponent>();
        if (pointToPointConstraintComponent != nullptr)
        {
            auto constraint = pointToPointConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.AddConstraint(*constraint);
            }
            else
            {
                Log("Warning: Attempting to add a point-to-point constraint component without attachments. Ignoring.");
            }
        }




        if (node.IsVisible())
        {
            this->cullingManager.AddObject(node);
        }



        // Event handlers need to know.
        if (!this->isRefreshingObject)
        {
            this->PostEvent_OnObjectAdded(node);
        }



        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageInsert(node.GetID());
        }
    }

    void Scene::OnSceneNodeRemoved(SceneNode &node)
    {
        // Important to stage this change before removing it.
        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageDelete(node.GetID());
        }


        // We just remove the scene node by it's ID.
        this->sceneNodes.Remove(node.GetID());



        // The node must be removed from the update manager.
        this->updateManager.RemoveObject(node);



        // The lighting components needs to be removed if applicable.
        auto ambientLightComponent = node.GetComponent<AmbientLightComponent>();
        if (ambientLightComponent != nullptr)
        {
            this->ambientLightComponents.Remove(this->ambientLightComponents.Find(ambientLightComponent));
        }
        auto directionalLightComponent = node.GetComponent<DirectionalLightComponent>();
        if (directionalLightComponent != nullptr)
        {
            this->directionalLightComponents.Remove(this->directionalLightComponents.Find(directionalLightComponent));
        }


        // Constraints must be removed before rigid bodies!
        auto genericConstraintComponent = node.GetComponent<GenericConstraintComponent>();
        if (genericConstraintComponent != nullptr)
        {
            auto constraint = genericConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.RemoveConstraint(*constraint);
            }
        }

        auto coneTwiseConstraintComponent = node.GetComponent<ConeTwistConstraintComponent>();
        if (coneTwiseConstraintComponent != nullptr)
        {
            auto constraint = coneTwiseConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.RemoveConstraint(*constraint);
            }
        }

        auto pointToPointConstraintComponent = node.GetComponent<PointToPointConstraintComponent>();
        if (pointToPointConstraintComponent != nullptr)
        {
            auto constraint = pointToPointConstraintComponent->GetConstraint();
            if (constraint != nullptr)
            {
                this->physicsManager.RemoveConstraint(*constraint);
            }
        }


        // TODO: Need to handle cases where we may be in the middle of a simulation...
        // If the node has a dynamics component, the rigid body needs to be removed.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            this->physicsManager.RemoveRigidBody(dynamicsComponent->GetRigidBody());
        }

        // Same for the proximity component as the dynamics component...
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            this->physicsManager.RemoveGhostObject(proximityComponent->GetGhostObject());
        }



        this->cullingManager.RemoveObject(node);


        // Event handlers need to know.
        if (!this->isRefreshingObject)
        {
            this->PostEvent_OnObjectRemoved(node);
        }
    }

    void Scene::OnSceneNodeTransform(SceneNode &node, bool updateDynamicsObject)
    {
        // We might need to update the rigid body, if we have one.
        if (updateDynamicsObject)
        {
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.UpdateTransform(dynamicsComponent->GetRigidBody(), node.GetWorldTransformWithoutScale());
            }
        }


        // We need to update the transformations of the ghost objects in the proximity component, if applicable.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            auto &ghostObject = proximityComponent->GetGhostObject();

            auto world = ghostObject.GetWorld();
            if (world != nullptr)
            {
                btTransform transform;
                node.GetWorldTransform(transform);

                ghostObject.setWorldTransform(transform);
                world->UpdateAABB(ghostObject);
            }
        }

        this->cullingManager.UpdateTransform(node);


        // Event handlers need to know about this.
        this->PostEvent_OnSceneNodeTransform(node);


        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }

    void Scene::OnSceneNodeScale(SceneNode &node)
    {
        // Culling information needs to be updated.
        this->cullingManager.UpdateScale(node);


        // The dynamics component needs to have scaling applied.
        auto dynamics = node.GetComponent<DynamicsComponent>();
        if (dynamics != nullptr)
        {
            dynamics->ApplyScaling(node.GetWorldScale());
        }

        // Like dynamics, scaling must be applied to the proximity component.
        auto proximity = node.GetComponent<DynamicsComponent>();
        if (proximity != nullptr)
        {
            proximity->ApplyScaling(node.GetWorldScale());
        }


        // Event handlers need to know about this.
        this->PostEvent_OnSceneNodeScale(node);


        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }

    void Scene::OnSceneNodeStaticChanged(SceneNode &node)
    {
        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }

    void Scene::OnSceneNodeVisibleChanged(SceneNode &node)
    {
        if (node.IsVisible())
        {
            // If the node has a dynamics component, the rigid body needs to be removed.
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.AddRigidBody(dynamicsComponent->GetRigidBody(), dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
            }

            // Same for the proximity component as the dynamics component.
            auto proximityComponent = node.GetComponent<ProximityComponent>();
            if (proximityComponent != nullptr)
            {
                this->physicsManager.AddGhostObject(proximityComponent->GetGhostObject(), proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
            }

            this->cullingManager.AddObject(node);


            // The event handlers need to know.
            this->PostEvent_OnSceneNodeShow(node);
        }
        else
        {
            // TODO: Need to handle cases where we may be in the middle of a simulation...
            // If the node has a dynamics component, the rigid body needs to be removed.
            auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                this->physicsManager.RemoveRigidBody(dynamicsComponent->GetRigidBody());
            }

            // Same for the proximity component as the dynamics component.
            auto proximityComponent = node.GetComponent<ProximityComponent>();
            if (proximityComponent != nullptr)
            {
                this->physicsManager.RemoveGhostObject(proximityComponent->GetGhostObject());
            }

            this->cullingManager.RemoveObject(node);


            // The event handlers need to know.
            this->PostEvent_OnSceneNodeHide(node);
        }


        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }

    void Scene::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name) ||
            GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.UpdateObject(node);
        }

        // TODO: Proximity, occluders.


        if (this->IsStateStackStagingEnabled() && node.IsSerializationEnabled())
        {
            this->stateStack.StageUpdate(node.GetID());
        }
    }




    ///////////////////////////////////////////////////////
    // Event Posting

    void Scene::PostEvent_OnObjectAdded(SceneObject &object)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnObjectAdded(object);
        }
    }

    void Scene::PostEvent_OnObjectRemoved(SceneObject &object)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnObjectRemoved(object);
        }
    }

    void Scene::PostEvent_OnObjectRefreshed(SceneObject &object)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnObjectRefreshed(object);
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

    void Scene::PostEvent_OnSceneNodeHide(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeHide(node);
        }
    }

    void Scene::PostEvent_OnSceneNodeShow(SceneNode &node)
    {
        for (size_t i = 0; i < this->eventHandlers.count; ++i)
        {
            this->eventHandlers[i]->OnSceneNodeShow(node);
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
