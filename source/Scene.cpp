
#include <GTEngine/Scene.hpp>
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/DefaultScenePhysicsManager.hpp>
#include <GTEngine/DefaultSceneCullingManager.hpp>
#include <GTEngine/Logging.hpp>

// Culling Callbacks.
namespace GTEngine
{
    /// Callback structure for doing occlusion culling against a viewport.
    struct SceneCullingCallback : SceneCullingManager::VisibleCallback
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

            return this->m_closestHitFraction;
		}


    private:    // No copying.
        SceneBulletRayResultCallback(const SceneBulletRayResultCallback &);
        SceneBulletRayResultCallback & operator=(const SceneBulletRayResultCallback &);
    };
}


namespace GTEngine
{
    Scene::Scene()
        : renderer(new DefaultSceneRenderer), deleteRenderer(true),
          updateManager(*new DefaultSceneUpdateManager), physicsManager(*new DefaultScenePhysicsManager), cullingManager(*new DefaultSceneCullingManager),
          deleteUpdateManager(true), deletePhysicsManager(true), deleteCullingManager(true),
          paused(false),
          viewports(), nodes(),
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh()
    {
    }

    Scene::Scene(SceneUpdateManager &updateManagerIn, ScenePhysicsManager &physicsManagerIn, SceneCullingManager &cullingManagerIn)
        : renderer(new DefaultSceneRenderer), deleteRenderer(true),
          updateManager(updateManagerIn), physicsManager(physicsManagerIn), cullingManager(cullingManagerIn),
          deleteUpdateManager(false), deletePhysicsManager(false), deleteCullingManager(false),
          paused(false),
          viewports(), nodes(),
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh()
    {
    }

    Scene::~Scene()
    {
        while (this->nodes.root != nullptr)
        {
            this->RemoveSceneNode(*this->nodes.root->value);
        }


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


    void Scene::RefreshObject(SceneObject &object)
    {
        this->RemoveObject(object);
        this->AddObject(object);
    }

    void Scene::Pause()
    {
        if (!this->paused)
        {
            this->paused = true;
            this->OnPause();
        }
    }

    void Scene::Resume()
    {
        if (this->paused)
        {
            this->paused = false;
            this->OnResume();
        }
    }


    void Scene::Update(double deltaTimeInSeconds)
    {
        // First we need to do our pre-update cleanup of caches, dead nodes, etc.
        //this->DoPreUpdateClean();

        // Before doing anything we're going to step the dynamics.
        if (!this->IsPaused())
        {
            this->physicsManager.Step(deltaTimeInSeconds);

            /*
            this->dynamicsWorld.Step(static_cast<btScalar>(deltaTimeInSeconds), 4);

            // Here is where we're going to check for collisions with other rigid bodies.
            int numManifolds = this->dynamicsWorld.GetCollisionDispatcher().getNumManifolds();
	        for (int i = 0; i < numManifolds; i++)
	        {
		        auto contactManifold = this->dynamicsWorld.GetCollisionDispatcher().getManifoldByIndexInternal(i);
		        auto obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		        auto obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

                // We'll just use the first contact point for ours. Should probably experiment with looping over all points.
                for (int iContact = 0; iContact < contactManifold->getNumContacts(); ++iContact)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(iContact);

                    auto dataA = static_cast<SceneNode*>(obA->getUserPointer());
                    auto dataB = static_cast<SceneNode*>(obB->getUserPointer());

                    if (dataA != nullptr && dataB != nullptr)
                    {
                        dataA->OnContact(*dataB, pt);
                        dataB->OnContact(*dataA, pt);
                    }
                }
            }
            */
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

        
        /*
        // Now we need to draw everything on every attached viewport.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            auto viewport = iViewport->value;
            assert(viewport != nullptr);

            viewport->Render();
        }
        */

    }


    SceneNode* Scene::FindFirstNode(const char* name)
    {
        // Here we check every node and their children.
        for (auto i = this->nodes.root; i != nullptr; i = i->next)
        {
            if (GTCore::Strings::Equal(i->value->GetName(), name))
            {
                return i->value;
            }
            else
            {
                auto temp = i->value->FindFirstChild(name, true);
                if (temp != nullptr)
                {
                    return temp;
                }
            }
        }

        return nullptr;
    }

    SceneNode* Scene::FindFirstNodeWithComponent(const char* componentName)
    {
        for (auto i = this->nodes.root; i != nullptr; i = i->next)
        {
            if (i->value->HasComponent(componentName))
            {
                return i->value;
            }
            else
            {
                auto temp = i->value->FindFirstChildWithComponent(componentName, true);
                if (temp != nullptr)
                {
                    return temp;
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

        // We need to remove all of the viewports from teh renderer.
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


    void Scene::ContactTest(const SceneNode &node, ContactTestCallback &callback)
    {
        // We do the contact test against the nodes proximity component.
        auto proximity = node.GetComponent<GTEngine::ProximityComponent>();
        if (proximity != nullptr)
        {
            callback.collisionGroup = proximity->GetCollisionGroup();
            callback.collisionMask  = proximity->GetCollisionMask();

            SceneContactTestCallback bulletCallback(callback);
            this->physicsManager.ContactTest(proximity->GetGhostObject(), bulletCallback);
        }
    }


    void Scene::QueryVisibleObjects(const glm::mat4 &mvp, SceneCullingManager::VisibleCallback &callback)
    {
        this->cullingManager.ProcessVisibleObjects(mvp, callback);
    }


    void Scene::GetVisibleComponents(const glm::mat4 &mvp,
            GTCore::Vector<ModelComponent*>            &modelComponentsOut,
            GTCore::Vector<AmbientLightComponent*>     &ambientLightComponentsOut,
            GTCore::Vector<DirectionalLightComponent*> &directionalLightComponentsOut,
            GTCore::Vector<PointLightComponent*>       &pointLightComponentsOut,
            GTCore::Vector<SpotLightComponent*>        &spotLightComponentsOut)
    {
        SceneCullingCallback callback
        (
            modelComponentsOut,
            pointLightComponentsOut,
            spotLightComponentsOut,
            ambientLightComponentsOut,
            directionalLightComponentsOut
        );
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


    void Scene::OnSceneNodeAdded(SceneNode &node)
    {
        this->nodes.Append(&node);

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
            dynamicsComponent->ApplyScaling(node.GetWorldScale());

            // Now we need to ensure the rigid body is transformed correctly.
            auto &rigidBody = dynamicsComponent->GetRigidBody();

            btTransform transform;
            if (rigidBody.getMotionState() != nullptr)
            {
                rigidBody.getMotionState()->getWorldTransform(transform);
                rigidBody.setWorldTransform(transform);
            }

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
            // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
            proximityComponent->ApplyScaling(node.GetWorldScale());

            // Now we need to ensure the rigid body is transformed correctly.
            auto &ghostObject = proximityComponent->GetGhostObject();

            btTransform transform;
            node.GetWorldTransform(transform);
            ghostObject.setWorldTransform(transform);

            if (node.IsVisible())
            {
                this->physicsManager.AddGhostObject(ghostObject, proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
            }
        }


        if (node.IsVisible())
        {
            this->cullingManager.AddObject(node);
        }
    }

    void Scene::OnSceneNodeRemoved(SceneNode &node)
    {
        this->nodes.Remove(this->nodes.Find(&node));

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
    }

    void Scene::OnSceneNodeTransform(SceneNode &node)
    {
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
    }

    void Scene::OnSceneNodeStaticChanged(SceneNode &)
    {
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
        }
    }

    void Scene::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        // TODO: Perhaps consider removing this and requiring scene nodes to do a refresh?

        if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name) ||
            GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.UpdateObject(node);
        }

        // TODO: Proximity, occluders.
    }

    void Scene::OnPause()
    {
    }

    void Scene::OnResume()
    {
    }
}