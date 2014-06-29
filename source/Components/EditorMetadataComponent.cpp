// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/EditorMetadataComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/MeshBuilder.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(EditorMetadataComponent, "EditorMetadata")

    const uint32_t EditorMetadataComponent::ChangeFlag_Sprite                = (1 << 0);
    const uint32_t EditorMetadataComponent::ChangeFlag_DirectionArrow        = (1 << 1);
    const uint32_t EditorMetadataComponent::ChangeFlag_WireframeColour       = (1 << 2);
    const uint32_t EditorMetadataComponent::ChangeFlag_PickingCollisionGroup = (1 << 3);
    const uint32_t EditorMetadataComponent::ChangeFlag_PickingMesh           = (1 << 4);
    const uint32_t EditorMetadataComponent::ChangeFlag_AlwaysShowOnTop       = (1 << 5);
    const uint32_t EditorMetadataComponent::ChangeFlag_IsSelected            = (1 << 6);
    const uint32_t EditorMetadataComponent::ChangeFlag_Prefab                = (1 << 7);

    EditorMetadataComponent::EditorMetadataComponent(SceneNode &node)
        : Component(node),
          alwaysShowOnTop(false), useModelForPickingShape(true),
          isSelected(false), selectionWireframeColour(1.0f, 0.75f, 0.5f),
          pickingCollisionObject(), pickingCollisionShape(nullptr), pickingCollisionShapeType(PickingCollisionShapeType_None), pickingCollisionGroup(CollisionGroups::EditorSelectionVolume),
          spritePickingCollisionObject(nullptr), spritePickingCollisionShape(nullptr), spriteTexturePath(), spriteTexture(nullptr), spriteMesh(),
          directionArrowMesh(),
          collisionShapeMesh(), isShowingCollisionShapeMesh(false), isCollisionShapeMeshDirty(true),
          proximityShapeMesh(), isShowingProximityShapeMesh(false), isProximityShapeMeshDirty(true),
          prefabRelativePath(), prefabID(0),
          sceneNodeEventHandler()
    {
        pickingCollisionObject.setUserPointer(this);

        // The collision shape meshes need to use wireframe mode.
        this->collisionShapeMesh.drawMode = DrawMode_Lines;
        this->proximityShapeMesh.drawMode = DrawMode_Lines;

        // Need an event handler for transformations.
        node.AttachEventHandler(this->sceneNodeEventHandler);
    }

    EditorMetadataComponent::~EditorMetadataComponent()
    {
        this->DeleteCollisionShape();
        this->HideSprite();             // <-- This will make sure everything to do with the sprite is deallocated.
        this->HideDirectionArrow();     // <-- This will delete the model.

        VertexArrayLibrary::Delete(this->collisionShapeMesh.vertexArray);
        MaterialLibrary::Delete(this->collisionShapeMesh.material);
    }


    bool EditorMetadataComponent::IsSelected() const
    {
        return this->isSelected;
    }

    void EditorMetadataComponent::Select()
    {
        this->isSelected = true;
    }

    void EditorMetadataComponent::Deselect()
    {
        this->isSelected = false;
    }


    bool EditorMetadataComponent::IsAncestorSelected() const
    {
        if (this->node.GetParent() != nullptr)
        {
            auto parentMetadata = this->node.GetParent()->GetComponent<EditorMetadataComponent>();
            if (parentMetadata != nullptr)
            {
                if (parentMetadata->IsSelected())
                {
                    return true;
                }
                else
                {
                    return parentMetadata->IsAncestorSelected();
                }
            }
        }

        return false;
    }


    const glm::vec3 & EditorMetadataComponent::GetSelectionWireframeColour() const
    {
        return this->selectionWireframeColour;
    }

    void EditorMetadataComponent::SetSelectionWireframeColour(const glm::vec3 &newColour)
    {
        this->selectionWireframeColour = newColour;

        this->OnChanged(ChangeFlag_WireframeColour);
    }


    void EditorMetadataComponent::SetPickingCollisionShapeToModel()
    {
        auto world = this->pickingCollisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->pickingCollisionObject);
        }


        // We'll be completely recreating the shape so we'll need to delete the old one.
        this->DeleteCollisionShape();


        auto modelComponent = this->GetNode().GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                this->pickingCollisionShape = new StaticMeshCollisionShape(*model);
                this->pickingCollisionShape->setLocalScaling(ToBulletVector3(this->node.GetWorldScale()));
            }
        }


        this->pickingCollisionObject.setCollisionShape(this->pickingCollisionShape);


        // Transform.
        btTransform transform;
        this->node.GetWorldTransform(transform);
        this->pickingCollisionObject.setWorldTransform(transform);



        if (world != nullptr && this->pickingCollisionShape != nullptr)
        {
            world->AddCollisionObject(this->pickingCollisionObject, this->pickingCollisionGroup, CollisionGroups::EditorSelectionRay);
        }


        this->pickingCollisionShapeType = PickingCollisionShapeType_Model;
    }


    void EditorMetadataComponent::ClearPickingCollisionShape()
    {
        auto world = this->pickingCollisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->pickingCollisionObject);
        }

        this->DeleteCollisionShape();
    }


    void EditorMetadataComponent::SetPickingCollisionGroup(short group)
    {
        if (this->pickingCollisionGroup != group)
        {
            this->pickingCollisionGroup = group;

            // We will need to remove and re-add the collision objcet.
            auto world = this->pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(this->pickingCollisionObject);
                world->AddCollisionObject(this->pickingCollisionObject, group, CollisionGroups::EditorSelectionRay);
            }


            this->OnChanged(ChangeFlag_PickingCollisionGroup);
        }
    }

    short EditorMetadataComponent::GetPickingCollisionGroup() const
    {
        return this->pickingCollisionGroup;
    }


    bool EditorMetadataComponent::IsAlwaysShownOnTop() const
    {
        return this->alwaysShowOnTop;
    }

    void EditorMetadataComponent::SetAlwaysShowOnTop(bool alwaysShowOnTopIn)
    {
        this->alwaysShowOnTop = alwaysShowOnTopIn;
    }



    void EditorMetadataComponent::ShowSprite(const char* texturePath, const glm::vec3 &colour)
    {
        if (!this->IsShowingSprite())
        {
            // Vertex Array.
            if (this->spriteMesh.vertexArray == nullptr)
            {
                this->spriteMesh.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2);

                // X/Y plane facing +Z.
                float vertices[] =
                {
                    -0.25f, -0.25f, 0.0f,
                     0.0f,   0.0f,

                     0.25f, -0.25f, 0.0f,
                     1.0f,   0.0f,

                     0.25f,  0.25f, 0.0f,
                     1.0f,   1.0f,

                    -0.25f,  0.25f, 0.0f,
                     0.0f,   1.0f,
                };

                unsigned int indices[] =
                {
                    0, 1, 2,
                    2, 3, 0
                };

                this->spriteMesh.vertexArray->SetData(vertices, 4, indices, 6);


                // Picking Shapes.
                assert(this->spritePickingCollisionShape  == nullptr);
                assert(this->spritePickingCollisionObject == nullptr);
                {
                    this->spritePickingCollisionShape = new btBoxShape(btVector3(0.25f, 0.25f, 0.0f));

                    this->spritePickingCollisionObject = new CollisionObject;
                    this->spritePickingCollisionObject->setUserPointer(this);
                    this->spritePickingCollisionObject->setCollisionShape(this->spritePickingCollisionShape);
                }
            }


            // Material.
            if (this->spriteMesh.material == nullptr)
            {
                this->spriteMesh.material = MaterialLibrary::Create("engine/materials/editor-sprite.material");
            }

            auto newSpriteTexture = Texture2DLibrary::Acquire(texturePath);
            if (newSpriteTexture != this->spriteTexture)
            {
                Texture2DLibrary::Unacquire(this->spriteTexture);
                this->spriteTexture = newSpriteTexture;

                this->spriteMesh.material->SetParameter("SpriteTexture", this->spriteTexture);
            }

            this->spriteMesh.material->SetParameter("SpriteColour", colour);



            // Transform.
            this->ApplyTransformToSprite();
            this->ApplyScaleToSprite();



            this->spriteTexturePath = texturePath;


            this->OnChanged(ChangeFlag_Sprite);
        }
    }

    void EditorMetadataComponent::ShowSprite(const char* texturePath, float colourR, float colourG, float colourB)
    {
        this->ShowSprite(texturePath, glm::vec3(colourR, colourG, colourB));
    }

    void EditorMetadataComponent::HideSprite()
    {
        if (this->IsShowingSprite())
        {
            delete this->spritePickingCollisionObject;      // <-- the destructor will remove it from the scene.
            delete this->spritePickingCollisionShape;
            Renderer::DeleteVertexArray(this->spriteMesh.vertexArray);
            MaterialLibrary::Delete(this->spriteMesh.material);
            Texture2DLibrary::Unacquire(this->spriteTexture);


            this->spritePickingCollisionObject = nullptr;
            this->spritePickingCollisionShape  = nullptr;
            this->spriteMesh.vertexArray       = nullptr;
            this->spriteMesh.material          = nullptr;
            this->spriteTexture                = nullptr;

            this->spriteTexturePath = "";

            this->OnChanged(ChangeFlag_Sprite);
        }
    }

    void EditorMetadataComponent::ApplyTransformToSprite()
    {
        if (this->spritePickingCollisionShape != nullptr && this->spritePickingCollisionObject != nullptr)
        {
            btTransform transform;
            transform.setFromOpenGLMatrix(&this->spriteMesh.transform[0][0]);
            this->spritePickingCollisionObject->setWorldTransform(transform);


            auto world = this->spritePickingCollisionObject->GetWorld();
            if (world != nullptr)
            {
                world->UpdateAABB(*this->spritePickingCollisionObject);
            }
        }
    }

    void EditorMetadataComponent::ApplyScaleToSprite()
    {
        if (this->spritePickingCollisionShape != nullptr && this->spritePickingCollisionObject != nullptr)
        {
            auto world = this->spritePickingCollisionObject->GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(*this->spritePickingCollisionObject);
            }


            this->spritePickingCollisionShape->setImplicitShapeDimensions(btVector3(0.25f, 0.25f, 0.0f) * ToBulletVector3(this->node.GetWorldScale()));


            if (world != nullptr)
            {
                world->AddCollisionObject(*this->spritePickingCollisionObject, this->pickingCollisionGroup, CollisionGroups::EditorSelectionRay);
            }
        }
    }

    bool EditorMetadataComponent::IsUsingSprite() const
    {
        return this->spriteMesh.vertexArray != nullptr;
    }


    CollisionObject* EditorMetadataComponent::GetSpritePickingCollisionObject()
    {
        return this->spritePickingCollisionObject;
    }

    const char* EditorMetadataComponent::GetSpriteTexturePath() const
    {
        return this->spriteTexturePath.c_str();
    }

    void EditorMetadataComponent::UpdateSpriteTransform(const SceneNode &cameraNode)
    {
        // The sprite mesh will be facing +Z. We just set the orientation to that of the camera and the position and scale to that of the sprite.
        this->spriteMesh.transform    = glm::mat4_cast(cameraNode.GetWorldOrientation());
        this->spriteMesh.transform[3] = glm::vec4(this->node.GetWorldPosition(), 1.0f);
        this->spriteMesh.transform   *= glm::scale(this->node.GetWorldScale());

        this->ApplyTransformToSprite();
        this->ApplyScaleToSprite();
    }



    void EditorMetadataComponent::ShowDirectionArrow()
    {
        if (!this->IsShowingDirectionArrow())
        {
            if (this->directionArrowMesh.vertexArray == nullptr)
            {
                this->directionArrowMesh.drawMode = DrawMode_Lines;


                // Just a simple wirefram model here. Unit length, facing -Z.
                float arrowHeadLength = 0.3f;
                float arrowHeadWidth  = 0.2f;

                glm::vec3 vertices[4];
                vertices[0].x =  0.0f;                  vertices[0].y = 0.0f; vertices[0].z = -1.0f;
                vertices[1].x =  0.0f;                  vertices[1].y = 0.0f; vertices[1].z =  0.0f;
                vertices[2].x = -arrowHeadWidth * 0.5f; vertices[2].y = 0.0f; vertices[2].z = -1.0f + arrowHeadLength;
                vertices[3].x =  arrowHeadWidth * 0.5f; vertices[3].y = 0.0f; vertices[3].z = -1.0f + arrowHeadLength;

                unsigned int indices[6] =
                {
                    0, 1,
                    0, 2,
                    0, 3
                };

                this->directionArrowMesh.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
                this->directionArrowMesh.vertexArray->SetData(&vertices[0].x, 4, indices, 6);


                // Material.
                this->directionArrowMesh.material = MaterialLibrary::Create("engine/materials/simple-emissive.material");
                this->directionArrowMesh.material->SetParameter("EmissiveColour", 1.0f, 1.0f, 0.0f);


                // Transform.
                this->UpdateDirectionArrowTransform();


                this->OnChanged(ChangeFlag_DirectionArrow);
            }
        }
    }

    void EditorMetadataComponent::HideDirectionArrow()
    {
        if (this->IsShowingDirectionArrow())
        {
            Renderer::DeleteVertexArray(this->directionArrowMesh.vertexArray);
            MaterialLibrary::Delete(this->directionArrowMesh.material);

            this->directionArrowMesh.vertexArray = nullptr;
            this->directionArrowMesh.material    = nullptr;


            this->OnChanged(ChangeFlag_DirectionArrow);
        }
    }

    bool EditorMetadataComponent::IsShowingDirectionArrow() const
    {
        return this->directionArrowMesh.vertexArray != nullptr;
    }

    void EditorMetadataComponent::UpdateDirectionArrowTransform()
    {
        this->directionArrowMesh.transform = this->node.GetWorldTransform();
    }



    void EditorMetadataComponent::ShowCollisionShapeMesh()
    {
        this->isShowingCollisionShapeMesh = true;
        this->UpdateCollisionShapeMeshTransform();
    }

    void EditorMetadataComponent::HideCollisionShapeMesh()
    {
        this->isShowingCollisionShapeMesh = false;
    }

    bool EditorMetadataComponent::IsShowingCollisionShapeMesh() const
    {
        return this->isShowingCollisionShapeMesh;
    }

    void EditorMetadataComponent::UpdateCollisionShapeMeshGeometry(const SceneNode &cameraNode, const btCollisionShape &shape)
    {
        if (this->isCollisionShapeMeshDirty)
        {
            auto cameraComponent = cameraNode.GetComponent<CameraComponent>();
            if (cameraComponent != nullptr)
            {
                // Geometry.
                if (this->collisionShapeMesh.vertexArray == nullptr)
                {
                    this->collisionShapeMesh.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3);
                }

                WireframeCollisionShapeMeshBuilder mesh(64);
                mesh.Build(shape, cameraComponent->GetViewMatrix());

                this->collisionShapeMesh.vertexArray->SetVertexData(mesh.GetVertexData(), mesh.GetVertexCount());
                this->collisionShapeMesh.vertexArray->SetIndexData( mesh.GetIndexData(),  mesh.GetIndexCount());


                // Material.
                if (this->collisionShapeMesh.material == nullptr)
                {
                    this->collisionShapeMesh.material = MaterialLibrary::Create("engine/materials/simple-emissive.material");
                    this->collisionShapeMesh.material->SetParameter("EmissiveColour", 0.25f, 0.75f, 0.25f);
                }
            }

            this->isCollisionShapeMeshDirty = false;
        }
    }

    void EditorMetadataComponent::UpdateCollisionShapeMeshGeometry(const SceneNode &cameraNode)
    {
        auto dynamicsComponent = this->node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            this->UpdateCollisionShapeMeshGeometry(cameraNode, dynamicsComponent->GetCollisionShape());
        }
    }

    void EditorMetadataComponent::UpdateCollisionShapeMeshTransform()
    {
        this->collisionShapeMesh.transform = this->node.GetWorldTransformWithoutScale();
    }

    void EditorMetadataComponent::MarkCollisionShapeMeshAsDirty()
    {
        this->isCollisionShapeMeshDirty = true;
    }



    void EditorMetadataComponent::ShowProximityShapeMesh()
    {
        this->isShowingProximityShapeMesh = true;
        this->UpdateProximityShapeMeshTransform();
    }

    void EditorMetadataComponent::HideProximityShapeMesh()
    {
        this->isShowingProximityShapeMesh = false;
    }

    bool EditorMetadataComponent::IsShowingProximityShapeMesh() const
    {
        return this->isShowingProximityShapeMesh;
    }

    void EditorMetadataComponent::UpdateProximityShapeMeshGeometry(const SceneNode &cameraNode, const btCollisionShape &shape)
    {
        if (this->isProximityShapeMeshDirty)
        {
            auto cameraComponent = cameraNode.GetComponent<CameraComponent>();
            if (cameraComponent != nullptr)
            {
                // Geometry.
                if (this->proximityShapeMesh.vertexArray == nullptr)
                {
                    this->proximityShapeMesh.vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3);
                }

                WireframeCollisionShapeMeshBuilder mesh(64);
                mesh.Build(shape, cameraComponent->GetViewMatrix());

                this->proximityShapeMesh.vertexArray->SetVertexData(mesh.GetVertexData(), mesh.GetVertexCount());
                this->proximityShapeMesh.vertexArray->SetIndexData( mesh.GetIndexData(),  mesh.GetIndexCount());


                // Material.
                if (this->proximityShapeMesh.material == nullptr)
                {
                    this->proximityShapeMesh.material = MaterialLibrary::Create("engine/materials/simple-emissive.material");
                    this->proximityShapeMesh.material->SetParameter("EmissiveColour", 0.75f, 0.75f, 0.25f);
                }
            }

            this->isProximityShapeMeshDirty = false;
        }
    }

    void EditorMetadataComponent::UpdateProximityShapeMeshGeometry(const SceneNode &cameraNode)
    {
        auto proximityComponent = this->node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            this->UpdateProximityShapeMeshGeometry(cameraNode, proximityComponent->GetCollisionShape());
        }
    }

    void EditorMetadataComponent::MarkProximityShapeMeshAsDirty()
    {
        this->isProximityShapeMeshDirty = true;
    }


    void EditorMetadataComponent::UpdateProximityShapeMeshTransform()
    {
        this->proximityShapeMesh.transform = this->node.GetWorldTransformWithoutScale();
    }



    const char* EditorMetadataComponent::GetPrefabRelativePath() const
    {
        return this->prefabRelativePath.c_str();
    }

    void EditorMetadataComponent::SetPrefabRelativePath(const char* newPrefabRelativePath)
    {
        this->prefabRelativePath = newPrefabRelativePath;
    }

    uint64_t EditorMetadataComponent::GetPrefabID() const
    {
        return this->prefabID;
    }

    void EditorMetadataComponent::SetPrefabID(uint64_t newPrefabID)
    {
        this->prefabID = newPrefabID;
    }

    bool EditorMetadataComponent::IsLinkedToPrefab() const
    {
        return !this->prefabRelativePath.IsEmpty();
    }

    void EditorMetadataComponent::LinkToPrefab(const char* newPrefabRelativePath, uint64_t id)
    {
        this->prefabRelativePath = newPrefabRelativePath;
        this->prefabID           = id;
    }

    void EditorMetadataComponent::UnlinkFromPrefab()
    {
        this->prefabRelativePath = "";
        this->prefabID           = 0;
    }



    ////////////////////////////////////////////////////////
    // Events

    void EditorMetadataComponent::OnSceneNodeTransform()
    {
        // The transform of the external meshes needs to be updated.
        this->UpdateDirectionArrowTransform();
        this->UpdateCollisionShapeMeshTransform();
        this->UpdateProximityShapeMeshTransform();
    }

    void EditorMetadataComponent::OnSceneNodeScale()
    {
        this->UpdateCollisionShapeMeshTransform();
        this->UpdateProximityShapeMeshTransform();
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void EditorMetadataComponent::Serialize(GTLib::Serializer &serializer, unsigned int flags) const
    {
        GTLib::BasicSerializer intermediarySerializer;

        intermediarySerializer.Write(this->alwaysShowOnTop);
        intermediarySerializer.Write(this->useModelForPickingShape);
        if ((flags & SceneNode::ForceDeselected) != 0)
        {
            intermediarySerializer.Write(false);
        }
        else
        {
            intermediarySerializer.Write(this->isSelected);
        }
        intermediarySerializer.Write(this->selectionWireframeColour);
        intermediarySerializer.Write(static_cast<uint32_t>(this->pickingCollisionShapeType));
        intermediarySerializer.Write(this->pickingCollisionGroup);

        if (this->IsUsingSprite())
        {
            intermediarySerializer.Write(true);
            intermediarySerializer.WriteString(this->spriteTexturePath);
        }
        else
        {
            intermediarySerializer.Write(false);
        }

        intermediarySerializer.Write(this->IsShowingDirectionArrow());

        intermediarySerializer.WriteString(this->prefabRelativePath);
        intermediarySerializer.Write(this->prefabID);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_EditorMetadataComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void EditorMetadataComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        uint32_t whatChanged = 0;
        this->LockOnChanged();


        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_EditorMetadataComponent_Main);
        {
            switch (header.version)
            {
            case 1:
                {
                    bool alwaysShowOnTop_New;
                    deserializer.Read(alwaysShowOnTop_New);
                    if (alwaysShowOnTop_New != this->alwaysShowOnTop)
                    {
                        this->alwaysShowOnTop = alwaysShowOnTop_New;
                        whatChanged |= ChangeFlag_AlwaysShowOnTop;
                    }

                    bool useModelForPickingShape_New;
                    deserializer.Read(useModelForPickingShape_New);
                    if (useModelForPickingShape_New != this->useModelForPickingShape)
                    {
                        this->useModelForPickingShape = useModelForPickingShape_New;
                        whatChanged |= ChangeFlag_PickingMesh;
                    }

                    bool isSelected_New;
                    deserializer.Read(isSelected_New);
                    if (isSelected_New != this->isSelected)
                    {
                        this->isSelected = isSelected_New;
                        whatChanged |= ChangeFlag_IsSelected;
                    }

                    glm::vec3 selectionWireframeColour_New;
                    deserializer.Read(selectionWireframeColour_New);
                    if (selectionWireframeColour_New != this->selectionWireframeColour)
                    {
                        this->selectionWireframeColour = selectionWireframeColour_New;
                        whatChanged |= ChangeFlag_WireframeColour;
                    }


                    uint32_t pickingCollisionShapeTypeIn;
                    deserializer.Read(pickingCollisionShapeTypeIn);
                    this->pickingCollisionShapeType = static_cast<PickingCollisionShapeType>(pickingCollisionShapeTypeIn);


                    int16_t pickingCollisionGroup_New;
                    deserializer.Read(pickingCollisionGroup_New);
                    if (pickingCollisionGroup_New != this->pickingCollisionGroup)
                    {
                        this->pickingCollisionGroup = pickingCollisionGroup_New;
                        whatChanged |= ChangeFlag_PickingCollisionGroup;
                    }



                    bool isUsingSprite;
                    deserializer.Read(isUsingSprite);

                    if (isUsingSprite)
                    {
                        deserializer.ReadString(this->spriteTexturePath);
                        this->ShowSprite(this->spriteTexturePath.c_str());
                    }
                    else
                    {
                        this->HideSprite();
                    }

                    whatChanged |= ChangeFlag_Sprite;



                    bool isShowingDirectionArrow;
                    deserializer.Read(isShowingDirectionArrow);

                    if (isShowingDirectionArrow)
                    {
                        this->ShowDirectionArrow();
                    }
                    else
                    {
                        this->HideDirectionArrow();
                    }

                    whatChanged |= ChangeFlag_DirectionArrow;



                    GTLib::String prefabRelativePath_New;
                    deserializer.ReadString(prefabRelativePath_New);
                    if (prefabRelativePath_New != this->prefabRelativePath)
                    {
                        this->prefabRelativePath = prefabRelativePath_New;
                        whatChanged |= ChangeFlag_Prefab;
                    }

                    uint64_t prefabID_New;
                    deserializer.Read(prefabID_New);
                    if (prefabID_New != this->prefabID)
                    {
                        this->prefabID = prefabID_New;
                        whatChanged |= ChangeFlag_Prefab;
                    }



                    // We'll try setting the model for the picking shape now.
                    if (this->useModelForPickingShape && (whatChanged & ChangeFlag_PickingMesh))
                    {
                        this->SetPickingCollisionShapeToModel();
                    }


                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing EditorMetadataComponent. Main chunk has an unsupported version (%d).", header.version);
                    break;
                }
            }
        }


        this->UnlockOnChanged();
        if (whatChanged != 0)
        {
            this->OnChanged(whatChanged);
        }
    }




    void EditorMetadataComponent::DeleteCollisionShape()
    {
        if (this->pickingCollisionShape != nullptr)
        {
            if (this->pickingCollisionShapeType == PickingCollisionShapeType_Box)
            {
                auto compoundShape = static_cast<btCompoundShape*>(this->pickingCollisionShape);

                while (compoundShape->getNumChildShapes() > 0)
                {
                    delete compoundShape->getChildShape(0);
                    compoundShape->removeChildShapeByIndex(0);
                }
            }
            else if (this->pickingCollisionShapeType == PickingCollisionShapeType_Torus)
            {
                auto compoundShape = static_cast<btCompoundShape*>(this->pickingCollisionShape);

                // We will be re-using a single cylinder shape here, so we only want to delete the first one.
                if (compoundShape->getNumChildShapes() > 0)
                {
                    delete compoundShape->getChildShape(0);
                }
            }



            delete this->pickingCollisionShape;
            this->pickingCollisionShape = nullptr;


            this->pickingCollisionObject.setCollisionShape(nullptr);
        }
    }



    ///////////////////////////////////////////////
    // SceneNodeEventHandler

    void EditorMetadataComponent::SceneNodeEventHandler::OnTransform(SceneNode &node)
    {
        auto editorMetadata = node.GetComponent<EditorMetadataComponent>();
        assert(editorMetadata != nullptr);
        {
            editorMetadata->OnSceneNodeTransform();
        }
    }

    void EditorMetadataComponent::SceneNodeEventHandler::OnScale(SceneNode &node)
    {
        auto editorMetadata = node.GetComponent<EditorMetadataComponent>();
        assert(editorMetadata != nullptr);
        {
            editorMetadata->OnSceneNodeScale();
        }
    }
}
