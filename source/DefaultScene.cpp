
#include <GTEngine/DefaultScene.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/Components/SpriteComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/SceneNodeFactory.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/SceneCullingDbvtPolicy.hpp>
#include <GTEngine/CollisionGroups.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Strings/Equal.hpp>


namespace GTEngine
{
    /// Dbvt culling policy for the default scene.
    struct DefaultSceneCullingDbvtPolicy : SceneCullingDbvtPolicy
    {
        /// Constructor.
        DefaultSceneCullingDbvtPolicy(SceneViewport &viewport)
            : SceneCullingDbvtPolicy(viewport.GetMVPMatrix()), viewport(viewport)
        {
        }

        /// Destructor.
        ~DefaultSceneCullingDbvtPolicy()
        {
        }


        /// SceneCullingDbvtPolicy::ProcessModel(ModelComponent &)
        void ProcessModel(ModelComponent &modelComponent)
        {
            viewport.AddModelComponent(modelComponent);
        }

        /// SceneCullingDbvtPolicy::ProcessModel(PointLightComponent &)
        void ProcessPointLight(PointLightComponent &pointLightComponent)
        {
            viewport.AddPointLightComponent(pointLightComponent);
        }

        /// SceneCullingDbvtPolicy::ProcessModel(SpotLightComponent &)
        void ProcessSpotLight(SpotLightComponent &spotLightComponent)
        {
            viewport.AddSpotLightComponent(spotLightComponent);
        }



        //////////////////////////////////////////////////////////////////
        // Attributes

        /// The viewport this policy will be retrieving nodes for.
        SceneViewport &viewport;


    private:    // No copying.
        DefaultSceneCullingDbvtPolicy(const DefaultSceneCullingDbvtPolicy &);
        DefaultSceneCullingDbvtPolicy & operator=(const DefaultSceneCullingDbvtPolicy &);
    };
}


// Culling callbacks.
namespace GTEngine
{
    /// Callback structure for doing occlusion culling against a viewport.
    struct DefaultSceneViewportCullingCallback : SceneCullingManager::VisibleCallback
    {
        /// Constructor
        DefaultSceneViewportCullingCallback(SceneViewport &viewport)
            : viewport(viewport)
        {
        }

        /// Destructor.
        ~DefaultSceneViewportCullingCallback()
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
                    this->viewport.AddModelComponent(*component);
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
                    this->viewport.AddPointLightComponent(*component);
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
                    this->viewport.AddSpotLightComponent(*component);
                }
            }
        }


    private:

        /// The viewport.
        SceneViewport &viewport;


    private:    // No copying.
        DefaultSceneViewportCullingCallback(const DefaultSceneViewportCullingCallback &);
        DefaultSceneViewportCullingCallback & operator=(const DefaultSceneViewportCullingCallback &);
    };
}


// Bullet collision test callbacks.
namespace GTEngine
{
    struct DefaultSceneContactTestCallback : public btCollisionWorld::ContactResultCallback
    {
        ContactTestCallback &callback;


        DefaultSceneContactTestCallback(ContactTestCallback &callback)
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
        DefaultSceneContactTestCallback(const DefaultSceneContactTestCallback &);
        DefaultSceneContactTestCallback & operator=(const DefaultSceneContactTestCallback &);
    };



    struct DefaultSceneBulletRayResultCallback : public btCollisionWorld::RayResultCallback
    {
        /// A reference to the GTEngine ray test callback object that is used for input and output.
        RayTestCallback &callback;

        /// A pointer to the closest scene node.
        SceneNode* closestSceneNode;


        /// Constructor.
        DefaultSceneBulletRayResultCallback(RayTestCallback &callback)
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
        DefaultSceneBulletRayResultCallback(const DefaultSceneBulletRayResultCallback &);
        DefaultSceneBulletRayResultCallback & operator=(const DefaultSceneBulletRayResultCallback &);
    };
}


namespace GTEngine
{
    DefaultScene::DefaultScene()
        : viewports(), nodes(),
          /*updateManager(), physicsManager(),*/
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh()/*, navigationMeshNode(), navigationMeshModel()*/
    {
        //this->AddSceneNode(this->navigationMeshNode);
        //this->navigationMeshNode.Hide();
    }

    DefaultScene::DefaultScene(SceneUpdateManager &updateManager, ScenePhysicsManager &physicsManager, SceneCullingManager &cullingManager)
        : Scene(updateManager, physicsManager, cullingManager),
          viewports(), nodes(),
          ambientLightComponents(), directionalLightComponents(),
          navigationMesh()
    {
    }


    DefaultScene::~DefaultScene()
    {
        while (this->nodes.root != nullptr)
        {
            this->RemoveSceneNode(*this->nodes.root->value);
        }
    }


    void DefaultScene::AddSceneNode(SceneNode &node)
    {
        node.SetScene(this);
    }

    void DefaultScene::RemoveSceneNode(SceneNode &node)
    {
        // We just set the scene on the node to null. This will also apply the changes to children.
        node.SetScene(nullptr);
    }

    void DefaultScene::Update(double deltaTimeInSeconds)
    {
        // First we need to do our pre-update cleanup of caches, dead nodes, etc.
        this->DoPreUpdateClean();

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

        

        // Now we need to draw everything on every attached viewport.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            auto viewport = iViewport->value;
            assert(viewport != nullptr);

            viewport->Render();
        }
    }

    SceneNode * DefaultScene::FindFirstNode(const char* name)
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
                GTEngine::SceneNode *temp = i->value->FindFirstChild(name, true);
                if (temp != nullptr)
                {
                    return temp;
                }
            }
        }

        return nullptr;
    }

    SceneNode * DefaultScene::FindFirstNodeWithComponent(const char* componentName)
    {
        for (auto i = this->nodes.root; i != nullptr; i = i->next)
        {
            if (i->value->HasComponent(componentName))
            {
                return i->value;
            }
            else
            {
                GTEngine::SceneNode *temp = i->value->FindFirstChildWithComponent(componentName, true);
                if (temp != nullptr)
                {
                    return temp;
                }
            }
        }

        return nullptr;
    }


    void DefaultScene::AddViewport(SceneViewport &viewport)
    {
        // The viewport needs to be removed from the previous scene if it has one.
        if (viewport.GetScene() != nullptr)
        {
            viewport.GetScene()->RemoveViewport(viewport);
        }

        this->viewports.Append(&viewport);

        viewport.SetScene(this);
    }

    void DefaultScene::RemoveViewport(SceneViewport &viewport)
    {
        if (viewport.GetScene() == this)
        {
            this->viewports.Remove(this->viewports.Find(&viewport));

            viewport.SetScene(nullptr);
        }
    }


    void DefaultScene::GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        // We'll retrieve the global AABB from the culling manager. Might need to move this over to physics manager, perhaps.
        this->cullingManager.GetGlobalAABB(aabbMin, aabbMax);
    }


    SceneNode* DefaultScene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback)
    {
        // Before creating the Bullet ray test callback object, we first need to set the rayStart and rayEnd attributes of the input callback structure. We do this
        // because the Bullet callback will use them in calculating the world position of interestion points.
        callback.rayStart = rayStart;
        callback.rayEnd   = rayEnd;

        // We need to use our own ray test callback for this.
        DefaultSceneBulletRayResultCallback rayTestResult(callback);
        this->physicsManager.RayTest(rayStart, rayEnd, rayTestResult);

        return rayTestResult.closestSceneNode;
    }


    void DefaultScene::ContactTest(const SceneNode &node, ContactTestCallback &callback)
    {
        // We do the contact test against the nodes proximity component.
        auto proximity = node.GetComponent<GTEngine::ProximityComponent>();
        if (proximity != nullptr)
        {
            callback.collisionGroup = proximity->GetCollisionGroup();
            callback.collisionMask  = proximity->GetCollisionMask();

            DefaultSceneContactTestCallback bulletCallback(callback);
            this->physicsManager.ContactTest(proximity->GetGhostObject(), bulletCallback);
        }
    }



    void DefaultScene::AddVisibleComponents(SceneViewport &viewport)
    {
        // First we need to grab the veiwport's camera. If we don't have one, nothing will be visible...
        auto cameraNode = viewport.GetCameraNode();
        if (cameraNode != nullptr)
        {
            // Now we need to grab the camera component. If we don't have one, nothing will be visible...
            auto camera = cameraNode->GetComponent<CameraComponent>();
            if (camera != nullptr)
            {
                glm::mat4 projection = camera->GetProjectionMatrix();
                glm::mat4 view       = camera->GetViewMatrix();
                glm::mat4 mvp        = projection * view;

                DefaultSceneViewportCullingCallback callback(viewport);
                this->cullingManager.ProcessVisibleObjects(mvp, callback);

                
                // Here is where we manually add visible components. Ambient and Directional lights are always visible, so they are added.
                
                // Ambient.
                for (auto i = this->ambientLightComponents.root; i != nullptr; i = i->next)
                {
                    auto light = i->value;
                    if (light != nullptr && light->GetNode().IsVisible())
                    {
                        viewport.AddAmbientLightComponent(*light);
                    }
                }

                // Directional.
                for (auto i = this->directionalLightComponents.root; i != nullptr; i = i->next)
                {
                    auto light = i->value;
                    if (light != nullptr && light->GetNode().IsVisible())
                    {
                        viewport.AddDirectionalLightComponent(*light);
                    }
                }
            }
        }
    }

    void DefaultScene::SetGravity(float x, float y, float z)
    {
        this->physicsManager.SetGravity(x, y, z);
    }

    void DefaultScene::GetGravity(float &x, float &y, float &z) const
    {
        this->physicsManager.GetGravity(x, y, z);
    }


    void DefaultScene::SetWalkableHeight(float height)
    {
        this->navigationMesh.SetWalkableHeight(height);
    }
    void DefaultScene::SetWalkableRadius(float radius)
    {
        this->navigationMesh.SetWalkableRadius(radius);
    }
    void DefaultScene::SetWalkableSlopeAngle(float angle)
    {
        this->navigationMesh.SetWalkableSlope(angle);
    }
    void DefaultScene::SetWalkableClimbHeight(float height)
    {
        this->navigationMesh.SetWalkableClimb(height);
    }


    float DefaultScene::GetWalkableHeight() const
    {
        return this->navigationMesh.GetWalkableHeight();
    }
    float DefaultScene::GetWalkableRadius() const
    {
        return this->navigationMesh.GetWalkableRadius();
    }
    float DefaultScene::GetWalkableSlopeAngle() const
    {
        return this->navigationMesh.GetWalkableSlope();
    }
    float DefaultScene::GetWalkableClimbHeight() const
    {
        return this->navigationMesh.GetWalkableClimb();
    }

    void DefaultScene::BuildNavigationMesh()
    {
        this->navigationMesh.Build(*this);

        // TODO: Update the navigation meshe's bounding volume so that it's not erroneously culled.
    }

    void DefaultScene::FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output)
    {
        this->navigationMesh.FindPath(start, end, output);
    }


    void DefaultScene::ShowNavigationMesh()
    {
        /*
        if (!this->navigationMeshNode.IsVisible())
        {
            auto component = this->navigationMeshNode.GetComponent<GTEngine::ModelComponent>();
            if (component == nullptr)
            {
                this->navigationMeshModel.AttachMesh(&this->navigationMesh.visualVA, MaterialLibrary::Create("engine/materials/simple-emissive.material"));
                this->navigationMeshModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.5f, 0.75f, 0.75f);

                component = this->navigationMeshNode.AddModelComponent(&this->navigationMeshModel);
            }

            this->navigationMeshNode.Show();
        }
        */
    }

    void DefaultScene::HideNavigationMesh()
    {
        /*
        if (this->navigationMeshNode.IsVisible())
        {
            this->navigationMeshNode.Hide();
        }
        */
    }


    void DefaultScene::DoPreUpdateClean()
    {
    }

    
    void DefaultScene::OnSceneNodeAdded(SceneNode &node)
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

            if (dynamicsComponent->GetCollisionShape().getNumChildShapes() > 0)
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

            this->physicsManager.AddGhostObject(ghostObject, proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
        }


        this->cullingManager.AddObject(node);
    }

    void DefaultScene::OnSceneNodeRemoved(SceneNode& node)
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



    void DefaultScene::OnSceneNodeTransform(SceneNode &node)
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

    void DefaultScene::OnSceneNodeScale(SceneNode &node)
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


    void DefaultScene::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        if (/*GTCore::Strings::Equal(component.GetName(), ModelComponent::Name)      ||*/
            GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name) ||
            GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->cullingManager.UpdateObject(node);
        }

        // TODO: Proximity, occluders.
    }
}
