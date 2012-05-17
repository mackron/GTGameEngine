
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
#include <GTCore/Strings/Equal.hpp>


namespace GTEngine
{
    /// Dbvt culling policy for the default scene.
    struct DefaultSceneCullingDbvtPolicy : SceneCullingDbvtPolicy
    {
        /// Constructor.
        DefaultSceneCullingDbvtPolicy(SceneViewport &viewport)
            : viewport(viewport)
        {
        }

        /// Destructor.
        ~DefaultSceneCullingDbvtPolicy()
        {
        }


        /// SceneCullingDbvtPolicy::Process(SceneNode &)
        void ProcessSceneNode(SceneNode &node)
        {
            auto modelComponent = node.GetComponent<ModelComponent>();
            if (modelComponent != nullptr)
            {
                viewport.AddModelComponent(*modelComponent);
                //viewport.AddVisibleNode(node);
            }
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


namespace GTEngine
{
    namespace CollisionGroup
    {
        static const short All  = (short)-1;
        static const short None = 0;

        static const short Picking   = (1 << 0);
        static const short Occlusion = (1 << 1);
    }



    DefaultScene::DefaultScene()
        : viewports(), nodes(), dynamicsWorld(), occlusionCollisionWorld(),
          cameraNodes(), staticNodes(), dynamicNodes()
    {
    }

    DefaultScene::~DefaultScene()
    {
        //this->RemoveSceneNode(this->rootNode);

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
            this->dynamicsWorld.stepSimulation(static_cast<btScalar>(deltaTimeInSeconds), 4);

            // Here is where we're going to check for collisions with other rigid bodies.
            int numManifolds = this->dynamicsWorld.getDispatcher()->getNumManifolds();
	        for (int i = 0; i < numManifolds; i++)
	        {
		        btPersistentManifold* contactManifold = this->dynamicsWorld.getDispatcher()->getManifoldByIndexInternal(i);
		        btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		        btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

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
        }


        // We need to update before rendering.
        for (auto iNode = this->nodes.root; iNode != nullptr; iNode = iNode->next)
        {
            if (iNode->value != nullptr)
            {
                auto &node = *iNode->value;

                if (!this->IsPaused())
                {
                    this->UpdateNode(node, deltaTimeInSeconds);
                }
            }
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


    SceneNode* DefaultScene::PickSceneNode(const glm::vec3 &rayStart, const glm::vec3 &rayEnd)
    {
        // This will store the result of our ray-test query.
        btCollisionWorld::ClosestRayResultCallback rayTestResult(GTEngine::ToBulletVector3(rayStart), GTEngine::ToBulletVector3(rayStart));
        rayTestResult.m_collisionFilterGroup = CollisionGroup::Picking;
        rayTestResult.m_collisionFilterMask  = CollisionGroup::Picking;      // We only want collisions with picking objects.

        // We use the occlusion world for picking. This will cause objects to be picked based on their mesh volumes. The scene node pointer
        // is stored as the user pointer on the collision object.
        this->occlusionCollisionWorld.rayTest(rayStart, rayEnd, rayTestResult);
        if (rayTestResult.hasHit())
        {
            return static_cast<SceneNode*>(rayTestResult.m_collisionObject->getUserPointer());
        }

        return nullptr;
    }

    SceneNode* DefaultScene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd)
    {
        // This will store the result of our ray-test query.
        btCollisionWorld::ClosestRayResultCallback rayTestResult(GTEngine::ToBulletVector3(rayStart), GTEngine::ToBulletVector3(rayStart));
        //rayTestResult.m_collisionFilterGroup = CollisionGroup::Picking;
        //rayTestResult.m_collisionFilterMask  = CollisionGroup::Picking;      // We only want collisions with picking objects.

        // We use the occlusion world for picking. This will cause objects to be picked based on their mesh volumes. The scene node pointer
        // is stored as the user pointer on the collision object.
        this->dynamicsWorld.rayTest(rayStart, rayEnd, rayTestResult);
        if (rayTestResult.hasHit())
        {
            return static_cast<SceneNode*>(rayTestResult.m_collisionObject->getUserPointer());
        }

        return nullptr;
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
                // We're using Bullet for this. Specifically, we're using the Dbvt broadphase of the collision world.
                btDbvtBroadphase &broadphase = this->occlusionCollisionWorld;

                glm::mat4 mvp = camera->GetProjectionMatrix() * camera->GetViewMatrix();

                btVector3 planes_n[6];
		        btScalar  planes_o[6];

			    planes_n[0]	= btVector3(mvp[0][3] - mvp[0][0], mvp[1][3] - mvp[1][0], mvp[2][3] - mvp[2][0]);     // Right
			    planes_n[1]	= btVector3(mvp[0][3] + mvp[0][0], mvp[1][3] + mvp[1][0], mvp[2][3] + mvp[2][0]);     // Left
			    planes_n[2]	= btVector3(mvp[0][3] - mvp[0][1], mvp[1][3] - mvp[1][1], mvp[2][3] - mvp[2][1]);     // Top
			    planes_n[3]	= btVector3(mvp[0][3] + mvp[0][1], mvp[1][3] + mvp[1][1], mvp[2][3] + mvp[2][1]);     // Bottom
			    planes_n[4]	= btVector3(mvp[0][3] - mvp[0][2], mvp[1][3] - mvp[1][2], mvp[2][3] - mvp[2][2]);     // Far
                planes_n[5]	= btVector3(mvp[0][3] + mvp[0][2], mvp[1][3] + mvp[1][2], mvp[2][3] + mvp[2][2]);     // Near
	
			    planes_o[0] = mvp[3][3] - mvp[3][0];    // Right
			    planes_o[1] = mvp[3][3] + mvp[3][0];    // Left
			    planes_o[2] = mvp[3][3] - mvp[3][1];    // Top
			    planes_o[3] = mvp[3][3] + mvp[3][1];    // Bottom
			    planes_o[4] = mvp[3][3] - mvp[3][2];    // Far
                planes_o[5] = mvp[3][3] + mvp[3][2];    // Near


                DefaultSceneCullingDbvtPolicy dbvtPolicy(viewport);

                btDbvt::collideKDOP(broadphase.m_sets[1].m_root, planes_n, planes_o, 6, dbvtPolicy);
			    btDbvt::collideKDOP(broadphase.m_sets[0].m_root, planes_n, planes_o, 6, dbvtPolicy);



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

                // TEMP: Point.
                for (auto i = this->pointLightComponents.root; i != nullptr; i = i->next)
                {
                    auto light = i->value;
                    if (light != nullptr && light->GetNode().IsVisible())
                    {
                        viewport.AddPointLightComponent(*light);
                    }
                }
            }
        }
    }


    void DefaultScene::UpdateNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // If the node has a model, and that model is animating, we should step the animation.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr && model->IsAnimating())
            {
                model->StepAnimation(deltaTimeInSeconds);
            }
        }

        // The node needs to know that it's being updated.
        node.OnUpdate(deltaTimeInSeconds);
    }

    void DefaultScene::DoPreUpdateClean()
    {
        /*
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            assert(iViewport->value != nullptr);

            iViewport->value->ClearVisibleNodes();
        }
        */
    }




    void DefaultScene::OnSceneNodeAdded(SceneNode &node)
    {
        this->nodes.Append(&node);

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

        // TEMP: Point.
        auto pointLightComponent = node.GetComponent<PointLightComponent>();
        if (pointLightComponent != nullptr)
        {
            this->pointLightComponents.Append(pointLightComponent);
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
            
            this->dynamicsWorld.addRigidBody(&rigidBody, dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
        }

        // Just like DynamicsComponent, if we have proximity component, we need to add that also.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            proximityComponent->ApplyScaling(node.GetWorldScale());
            
            // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
            proximityComponent->ApplyScaling(node.GetWorldScale());

            // Now we need to ensure the rigid body is transformed correctly.
            auto &ghostObject = proximityComponent->GetGhostObject();

            btTransform transform;
            node.GetWorldTransform(transform);
            ghostObject.setWorldTransform(transform);

            this->dynamicsWorld.addGhostObject(&ghostObject, proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
        }


        // The node needs to have some metadata associated with it.
        auto metadata = new SceneNodeMetadata;
        metadata->pickingObject.setCollisionShape(&metadata->pickingShape);
        metadata->pickingObject.setUserPointer(&node);

        // This variable keeps track of whether or not the picking object should be added to the collision world.
        bool addToWorld = false;

        // If the scene node has a model component, we need to create a picking and occlusion collision object.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            // The collision shapes of a model are based on the meshes of the model.
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    metadata->pickingShape.addChildShape(btTransform::getIdentity(), model->meshes[iMesh]->BuildCollisionShape(node.GetWorldScale()));
                }

                addToWorld = true;
            }
        }

        if (addToWorld)
        {
            btTransform transform;
            node.GetWorldTransform(transform);
            metadata->pickingObject.setWorldTransform(transform);

            this->occlusionCollisionWorld.addCollisionObject(&metadata->pickingObject, CollisionGroup::Picking, CollisionGroup::Picking);
        }

        // Now we need to attach the metadata to the node.
        node.SetDataPointer(reinterpret_cast<size_t>(this), metadata);

#if 0
        // This will be called for each individual node. We do not need to traverse children here. We want to categorise
        // scene nodes based on their components. If a node is a camera component, we need to put it in the cameraNodes
        // list. This will let us do a pass for each camera where appropriate.
        if (node.HasComponent<CameraComponent>() && this->cameraNodes.Find(&node) == nullptr)
        {
            this->cameraNodes.Append(&node);
        }

        if (node.IsStatic())
        {
            this->staticNodes.Append(&node);
        }
        else
        {
            this->dynamicNodes.Append(&node);
        }
#endif
    }

    void DefaultScene::OnSceneNodeRemoved(SceneNode& node)
    {
        this->nodes.Remove(this->nodes.Find(&node));

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

        // TEMP: Point light.
        auto pointLightComponent = node.GetComponent<PointLightComponent>();
        if (pointLightComponent != nullptr)
        {
            this->pointLightComponents.Remove(this->pointLightComponents.Find(pointLightComponent));
        }




        // TODO: Need to handle cases where we may be in the middle of a simulation...
        // If the node has a dynamics component, the rigid body needs to be removed.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            this->dynamicsWorld.removeRigidBody(&dynamicsComponent->GetRigidBody());
        }

        // Same for the proximity component as the dynamics component.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            this->dynamicsWorld.removeGhostObject(&proximityComponent->GetGhostObject());
        }

        // If we have metadata, it needs to be removed. 
        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        delete metadata;

        node.RemoveDataPointer(reinterpret_cast<size_t>(this));

#if 0
        // Like OnSceneNodeAdded(), this function will be called on each individual node, meaning we don't need to traverse
        // children. We need to remove the node from cameraNodes if appropriate.
        this->cameraNodes.Remove(this->cameraNodes.Find(&node));

        if (node.IsStatic())
        {
            this->staticNodes.Remove(this->staticNodes.Find(&node));
        }
        else
        {
            this->dynamicNodes.Remove(this->dynamicNodes.Find(&node));
        }
#endif
    }



    void DefaultScene::OnSceneNodeTransform(SceneNode &node)
    {
        // We need to update the transformations of the ghost objects in the proximity component, if applicable.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            auto &ghostObject = proximityComponent->GetGhostObject();

            auto world = ghostObject.getWorld();
            if (world != nullptr)
            {
                btTransform transform;
                node.GetWorldTransform(transform);

                ghostObject.setWorldTransform(transform);
                world->updateSingleAabb(&ghostObject);
            }
        }

        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)    // Use an assert?
        {
            btTransform transform;
            node.GetWorldTransform(transform);

            auto world = metadata->pickingObject.getWorld();
            if (world != nullptr)
            {
                metadata->pickingObject.setWorldTransform(transform);
                this->occlusionCollisionWorld.updateSingleAabb(&metadata->pickingObject);
            }
        }
    }

    void DefaultScene::OnSceneNodeScale(SceneNode &node)
    {
        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)    // Use an assert?
        {
            glm::vec3 scale = node.GetWorldScale();

            // We need to scale the collision shapes. Unfortunately the only way I could figure this out is to completely delete
            // the children and recreate them. The loop below does just that.
            // TODO: Have a look into this problem. If it's a bug, it needs to be reported.
            int meshCount = metadata->pickingShape.getNumChildShapes();
            for (int i = 0; i < meshCount; ++i)
            {
                auto oldChild = static_cast<btGImpactMeshShape*>(metadata->pickingShape.getChildShape(0));
                metadata->pickingShape.removeChildShapeByIndex(0);

                auto newChild = new btGImpactMeshShape(oldChild->getMeshInterface());
                newChild->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
                newChild->updateBound();

                metadata->pickingShape.addChildShape(btTransform::getIdentity(), newChild);

                delete oldChild;
            }

            this->occlusionCollisionWorld.updateSingleAabb(&metadata->pickingObject);
        }

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





    void DefaultScene::OnSceneNodeComponentAttached(SceneNode&, Component& component)
    {
        if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->ambientLightComponents.Append(static_cast<AmbientLightComponent*>(&component));
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->directionalLightComponents.Append(static_cast<DirectionalLightComponent*>(&component));
        }
        // TEMP: Point.
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->pointLightComponents.Append(static_cast<PointLightComponent*>(&component));
        }

#if 0
        // If the component is a camera component, we need to ensure the node is in the camera nodes list.
        if (GTCore::Strings::Equal(component.GetName(), CameraComponent::Name) && this->cameraNodes.Find(&node) == nullptr)
        {
            this->cameraNodes.Append(&node);
        }
#endif
    }

    void DefaultScene::OnSceneNodeComponentDetached(SceneNode&, Component& component)
    {
        if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->ambientLightComponents.Remove(this->ambientLightComponents.Find(static_cast<AmbientLightComponent*>(&component)));
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->directionalLightComponents.Remove(this->directionalLightComponents.Find(static_cast<DirectionalLightComponent*>(&component)));
        }
        // TEMP: Point.
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->pointLightComponents.Remove(this->pointLightComponents.Find(static_cast<PointLightComponent*>(&component)));
        }

#if 0
        // If the component is a camera component, we need to ensure the node is removed from the camera nodes list.
        if (GTCore::Strings::Equal(component.GetName(), CameraComponent::Name))
        {
            this->cameraNodes.Remove(this->cameraNodes.Find(&node));
        }
#endif
    }




#if 0
    void DefaultScene::OnSceneNodeStaticChanged(SceneNode& node)
    {
#if 0
        // If the node is now static, it needs to be moved out from the dynamic list and into the static list.
        if (node.IsStatic())
        {
            this->dynamicNodes.Remove(this->dynamicNodes.Find(&node));
            this->staticNodes.Append(&node);
        }
        else
        {
            this->staticNodes.Remove(this->staticNodes.Find(&node));
            this->dynamicNodes.Append(&node);
        }
#endif
    }

#endif
}