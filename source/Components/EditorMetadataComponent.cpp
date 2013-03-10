// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/EditorMetadataComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(EditorMetadataComponent, "EditorMetadata");

    EditorMetadataComponent::EditorMetadataComponent(SceneNode &node)
        : Component(node),
          alwaysShowOnTop(false), useModelForPickingShape(true), isSystemNode(false),
          isSelected(false), selectionWireframeColour(1.0f, 0.75f, 0.5f),
          pickingCollisionObject(), pickingCollisionShape(nullptr), pickingCollisionShapeType(PickingCollisionShapeType_None), pickingCollisionGroup(CollisionGroups::EditorSelectionVolume),
          spritePickingCollisionObject(nullptr), spritePickingCollisionShape(nullptr), spriteTexturePath(), spriteTexture(nullptr), spriteMesh(),
          directionArrowModel(nullptr), directionArrowVA(nullptr),
          prefabRelativePath(), prefabID(0),
          sceneNodeEventHandler()
    {
        pickingCollisionObject.setUserPointer(this);

        // Need an event handler for transformations.
        node.AttachEventHandler(this->sceneNodeEventHandler);
    }

    EditorMetadataComponent::~EditorMetadataComponent()
    {
        this->DeleteCollisionShape();
        this->HideSprite();             // <-- This will make sure everything to do with the sprite is deallocated.
        this->HideDirectionArrow();     // <-- This will delete the model.
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
        this->OnChanged();
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

    void EditorMetadataComponent::SetPickingCollisionShapeToBox(const glm::vec3 &halfExtents, const glm::vec3 &offset)
    {
        auto world = this->pickingCollisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->pickingCollisionObject);
        }


        // The old shape needs to be deleted.
        this->DeleteCollisionShape();


        // Since we want to apply an offset to the box, we'll need to use a compund shape.

        btTransform transform;
        transform.setBasis(btMatrix3x3::getIdentity());
        transform.setOrigin(ToBulletVector3(offset));

        auto compoundShape = new btCompoundShape;
        compoundShape->addChildShape(transform, new btBoxShape(ToBulletVector3(halfExtents)));


        this->pickingCollisionShape = compoundShape;
        this->pickingCollisionObject.setCollisionShape(this->pickingCollisionShape);



        // Transform.
        this->node.GetWorldTransform(transform);
        this->pickingCollisionObject.setWorldTransform(transform);



        if (world != nullptr && this->pickingCollisionShape != nullptr)
        {
            world->AddCollisionObject(this->pickingCollisionObject, this->pickingCollisionGroup, CollisionGroups::EditorSelectionRay);
        }


        this->pickingCollisionShapeType = PickingCollisionShapeType_Box;
    }

    void EditorMetadataComponent::SetPickingCollisionShapeToTorus(float outerRadius, float innerRadius, unsigned int subdivisions)
    {
        auto world = this->pickingCollisionObject.GetWorld();
        if (world != nullptr)
        {
            world->RemoveCollisionObject(this->pickingCollisionObject);
        }


        // The old shape needs to be deleted.
        this->DeleteCollisionShape();


        auto compoundShape = new btCompoundShape;

        float segmentLength = 2.0f * SIMD_PI * outerRadius / static_cast<float>(subdivisions);
        float segmentAngle  = 2.0f * SIMD_PI               / static_cast<float>(subdivisions);

        btCapsuleShape* cylinderShape = new btCapsuleShape(innerRadius, segmentLength);


        btTransform transform;
        for (unsigned int i = 0; i < subdivisions; ++i)
        {
            float angle = segmentAngle * static_cast<float>(i);

            btVector3 position;
            position.setX(std::cos(angle) * outerRadius);
            position.setY(std::sin(angle) * outerRadius);
            position.setZ(0.0f);

            btQuaternion rotation(btVector3(0.0f, 0.0f, -1.0f), angle);

            transform.setIdentity();
            transform.setOrigin(position);
            transform.setRotation(rotation);
            compoundShape->addChildShape(transform, cylinderShape);
        }



        this->pickingCollisionShape = compoundShape;
        this->pickingCollisionObject.setCollisionShape(this->pickingCollisionShape);


        // Transform.
        this->node.GetWorldTransform(transform);
        this->pickingCollisionObject.setWorldTransform(transform);


        if (world != nullptr && this->pickingCollisionShape != nullptr)
        {
            world->AddCollisionObject(this->pickingCollisionObject, this->pickingCollisionGroup, CollisionGroups::EditorSelectionRay);
        }


        this->pickingCollisionShapeType = PickingCollisionShapeType_Torus;
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


            this->OnChanged();
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
        // Vertex Array.
        if (this->spriteMesh.vertexArray == nullptr)
        {
            this->spriteMesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2);

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

        this->spriteMesh.material->SetParameter("SpriteColour",  colour);


        // Transform.
        this->ApplyTransformToSprite();
        this->ApplyScaleToSprite();



        this->spriteTexturePath = texturePath;
        this->OnChanged();
    }

    void EditorMetadataComponent::ShowSprite(const char* texturePath, float colourR, float colourG, float colourB)
    {
        this->ShowSprite(texturePath, glm::vec3(colourR, colourG, colourB));
    }

    void EditorMetadataComponent::HideSprite()
    {
        delete this->spritePickingCollisionObject;      // <-- the destructor will remove it from the scene.
        delete this->spritePickingCollisionShape;
        Renderer2::DeleteVertexArray(this->spriteMesh.vertexArray);


        this->spritePickingCollisionObject = nullptr;
        this->spritePickingCollisionShape  = nullptr;
        this->spriteMesh.vertexArray       = nullptr;


        this->spriteTexturePath = "";
        this->OnChanged();
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
        if (this->directionArrowModel == nullptr)
        {
            this->directionArrowModel = ModelLibrary::CreateEmpty();
            assert(this->directionArrowModel != nullptr);
            {
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

                this->directionArrowVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
                this->directionArrowVA->SetData(&vertices[0].x, 4, indices, 6);

                this->directionArrowModel->AttachMesh(this->directionArrowVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
                this->directionArrowModel->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 1.0f, 0.0f);
            }


            this->OnChanged();
        }
    }

    void EditorMetadataComponent::HideDirectionArrow()
    {
        ModelLibrary::Delete(this->directionArrowModel);
        Renderer2::DeleteVertexArray(this->directionArrowVA);

        this->directionArrowModel = nullptr;
        this->directionArrowVA    = nullptr;


        this->OnChanged();
    }

    bool EditorMetadataComponent::IsShowingDirectionArrow() const
    {
        return this->directionArrowModel != nullptr;
    }

    const Model* EditorMetadataComponent::GetDirectionArrowModel() const
    {
        return this->directionArrowModel;
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
    }

    void EditorMetadataComponent::OnSceneNodeScale()
    {
        // We just do a full OnSceneNodeTransform() here. That will handle scaling for us.
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void EditorMetadataComponent::Serialize(GTCore::Serializer &serializer) const
    {
        GTCore::BasicSerializer intermediarySerializer;

        intermediarySerializer.Write(this->alwaysShowOnTop);
        intermediarySerializer.Write(this->useModelForPickingShape);
        intermediarySerializer.Write(this->isSelected);
        intermediarySerializer.Write(this->selectionWireframeColour);
        intermediarySerializer.Write(static_cast<uint32_t>(this->pickingCollisionShapeType));
        intermediarySerializer.Write(this->pickingCollisionGroup);

        if (this->IsUsingSprite())
        {
            intermediarySerializer.Write(true);
            intermediarySerializer.Write(this->spriteTexturePath);
        }
        else
        {
            intermediarySerializer.Write(false);
        }

        intermediarySerializer.Write(this->IsShowingDirectionArrow());

        intermediarySerializer.Write(this->prefabRelativePath);
        intermediarySerializer.Write(this->prefabID);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_EditorMetadataComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void EditorMetadataComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_EditorMetadataComponent_Main);
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(this->alwaysShowOnTop);
                    deserializer.Read(this->useModelForPickingShape);
                    deserializer.Read(this->isSelected);
                    deserializer.Read(this->selectionWireframeColour);

                    uint32_t pickingCollisionShapeTypeIn;
                    deserializer.Read(pickingCollisionShapeTypeIn);
                    this->pickingCollisionShapeType = static_cast<PickingCollisionShapeType>(pickingCollisionShapeTypeIn);

                    deserializer.Read(this->pickingCollisionGroup);

                    bool isUsingSprite;
                    deserializer.Read(isUsingSprite);

                    if (isUsingSprite)
                    {
                        deserializer.Read(this->spriteTexturePath);
                        this->ShowSprite(this->spriteTexturePath.c_str());
                    }


                    bool isShowingDirectionArrow;
                    deserializer.Read(isShowingDirectionArrow);

                    if (isShowingDirectionArrow)
                    {
                        this->ShowDirectionArrow();
                    }


                    deserializer.Read(this->prefabRelativePath);
                    deserializer.Read(this->prefabID);



                    // We'll try setting the model for the picking shape now.
                    if (this->useModelForPickingShape)
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


        this->OnChanged();
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
