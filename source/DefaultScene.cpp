
#include <GTEngine/DefaultScene.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/Components/SpriteComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/SceneNodeFactory.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTCore/Strings/Equal.hpp>

#if 0
namespace GTEngine
{
    class Clear : public RenderCommand
    {
    public:

        Clear()
        {
        }

        void Execute()
        {
            Renderer::ClearColour(0.5f, 0.25f, 0.25f, 1.0f);
            Renderer::ClearDepth(1.0f);
            Renderer::Clear(GTEngine::ColourBuffer | GTEngine::DepthBuffer);
        }

        void OnExecuted()
        {
            Clear::Deallocate(this);
        }


    public: // Allocation/Deallocation.

        static Clear * Allocate()
        {
            return new Clear();
        }

        static void Deallocate(Clear *call)
        {
            delete call;
        }
    };

    class DrawHDRQuad : public RenderCommand
    {
    public:

        DrawHDRQuad(DefaultScene *scene)
            : scene(scene)
        {
        }

        void Execute()
        {
            Renderer::SetFramebuffer(nullptr);
            Renderer::DisableDepthTest();

            Renderer::SetShader(scene->hdrQuadShader);
            Renderer::SetShaderParameter("Texture",   scene->defaultFramebuffer->GetColourBuffer(0));

            if (Renderer::SupportFloatTextures())
            {
                Renderer::SetShaderParameter("Exposure",  1.0f);
                Renderer::SetShaderParameter("MaxBright", 4.0f);
            }

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());

            Renderer::EnableDepthTest();
        }

        void OnExecuted()
        {
            DrawHDRQuad::Deallocate(this);
        }


    public: // Allocation/Deallocation.

        static DrawHDRQuad * Allocate(DefaultScene *scene)
        {
            return new DrawHDRQuad(scene);
        }

        static void Deallocate(DrawHDRQuad *call)
        {
            delete call;
        }

    private:

        DefaultScene *scene;


    private:    // No copying.
        DrawHDRQuad(const DrawHDRQuad &);
        DrawHDRQuad & operator=(const DrawHDRQuad &);
    };
}
#endif

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
        : /*rootNode(), */viewports(), nodes(), dynamicsWorld(), occlusionCollisionWorld(),
          /*defaultCameraNode(nullptr), colourBuffer(nullptr), depthStencilBuffer(nullptr), defaultShader(nullptr), hdrQuadShader(nullptr), defaultTexture(nullptr), defaultFramebuffer(nullptr),*/
          cameraNodes(), staticNodes(), dynamicNodes()
    {
        /*
        // The very first thing we need to do is let rootNode know that this is it's scene.
        rootNode.SetScene(this);

        this->defaultCameraNode = SceneNodeFactory::Create3DCameraNode(90.0f, 1280.0f / 720.0f, 0.001f, 1000.0f);
        this->AddSceneNode(*this->defaultCameraNode);

        this->defaultShader     = new Shader(ShaderLibrary::GetShaderString("SimpleMVP_P3T2N3_Vertex"), ShaderLibrary::GetShaderString("TextureLight_Fragment"));
        this->defaultTexture    = new Texture2D("textures/default.png");

        if (Renderer::SupportFloatTextures())
        {
            this->colourBuffer  = new Texture2D(1280, 720, GTImage::ImageFormat_RGBA16F);
            this->hdrQuadShader = new Shader(ShaderLibrary::GetShaderString("FullscreenQuad_Vertex"), ShaderLibrary::GetShaderString("HDRQuad_Fragment"));
        }
        else
        {
            this->colourBuffer  = new Texture2D(1280, 720, GTImage::ImageFormat_RGBA8);
            this->hdrQuadShader = new Shader(ShaderLibrary::GetShaderString("FullscreenQuad_Vertex"), ShaderLibrary::GetShaderString("NoHDRQuad_Fragment"));
        }

        this->depthStencilBuffer = new Texture2D(1280, 720, GTImage::ImageFormat_Depth24_Stencil8);
        
        this->defaultFramebuffer = new Framebuffer();
        this->defaultFramebuffer->AttachColourBuffer(this->colourBuffer, 0);
        this->defaultFramebuffer->AttachDepthStencilBuffer(this->depthStencilBuffer);
        */
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
        //this->rootNode.AttachChild(node);
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
            this->dynamicsWorld.stepSimulation(static_cast<btScalar>(deltaTimeInSeconds));
        }


        // TODO: Look into using Bullet for frustum and occlusion culling. For now we'll do a horribly unoptimized implementation.
        //
        // We first need to loop over all of our nodes and update them. As we update, we will check if it's a drawable node. If it
        // is, it will be added to each viewports visible nodes cache.
        for (auto iNode = this->nodes.root; iNode != nullptr; iNode = iNode->next)
        {
            if (iNode->value != nullptr)
            {
                auto &node = *iNode->value;

                if (!this->IsPaused())
                {
                    this->UpdateNode(node, deltaTimeInSeconds);
                }

                // if node is visible.
                {
                    for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
                    {
                        assert(iViewport->value != nullptr);

                        iViewport->value->AddVisibleNode(node);
                    }
                }
            }
        }


        // Now we need to draw everything on every attached viewport.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            assert(iViewport->value != nullptr);

            iViewport->value->Render();
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


    void DefaultScene::UpdateNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // TODO: Step animations.

        // The node needs to know that it's being updated.
        node.OnUpdate(deltaTimeInSeconds);
    }

    void DefaultScene::DoPreUpdateClean()
    {
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            assert(iViewport->value != nullptr);

            iViewport->value->ClearVisibleNodes();
        }
    }




    void DefaultScene::OnSceneNodeAdded(SceneNode &node)
    {
        this->nodes.Append(&node);

        // If the scene node has a dynamics component, we need to add it's rigid body.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            auto &rigidBody = dynamicsComponent->GetRigidBody();

            btTransform transform;
            if (rigidBody.getMotionState() != nullptr)
            {
                rigidBody.getMotionState()->getWorldTransform(transform);
                rigidBody.setWorldTransform(transform);
            }

            this->dynamicsWorld.addRigidBody(&rigidBody, dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
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

        // TODO: Need to handle cases where we may be in the middle of a simulation...
        // If the node has a dynamics component, the rigid body needs to be removed.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            this->dynamicsWorld.removeRigidBody(&dynamicsComponent->GetRigidBody());
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

    void DefaultScene::OnSceneNodeScale(SceneNode &node, const glm::vec3 &)
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
    }




#if 0

    void DefaultScene::OnSceneNodeComponentAttached(SceneNode& node, Component& component)
    {
#if 0
        // If the component is a camera component, we need to ensure the node is in the camera nodes list.
        if (GTCore::Strings::Equal(component.GetName(), CameraComponent::Name) && this->cameraNodes.Find(&node) == nullptr)
        {
            this->cameraNodes.Append(&node);
        }
#endif
    }

    void DefaultScene::OnSceneNodeComponentDetached(SceneNode& node, Component& component)
    {
#if 0
        // If the component is a camera component, we need to ensure the node is removed from the camera nodes list.
        if (GTCore::Strings::Equal(component.GetName(), CameraComponent::Name))
        {
            this->cameraNodes.Remove(this->cameraNodes.Find(&node));
        }
#endif
    }

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