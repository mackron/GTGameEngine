
#include <GTEngine/Components/EditorMetadataComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Texture2DLibrary.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(EditorMetadataComponent, "EditorMetadata");

    EditorMetadataComponent::EditorMetadataComponent(SceneNode &node)
        : Component(node),
          alwaysShowOnTop(false), useModelForPickingShape(true), deleteOnClose(false),
          isSelected(false), selectionWireframeColour(1.0f, 0.75f, 0.5f),
          pickingCollisionObject(), pickingCollisionShape(nullptr), pickingCollisionShapeType(PickingCollisionShapeType_None), pickingCollisionGroup(CollisionGroups::EditorSelectionVolume),
          spriteModel(nullptr), spritePickingCollisionObject(nullptr), spritePickingCollisionShape(nullptr), spriteTransform(), spriteTexturePath(),
          directionArrowModel(nullptr), directionArrowVA(nullptr)
    {
        pickingCollisionObject.setUserPointer(this);
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


    const glm::vec3 & EditorMetadataComponent::GetSelectionWireframeColour() const
    {
        return this->selectionWireframeColour;
    }

    void EditorMetadataComponent::SetSelectionWireframeColour(const glm::vec3 &newColour)
    {
        this->selectionWireframeColour = newColour;
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
            }
        }


        this->pickingCollisionObject.setCollisionShape(this->pickingCollisionShape);


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


        btVector3 forward(0.0f,        0.0f, -1.0f);
        btVector3 side(   outerRadius, 0.0f,  0.0f);
        float gap = std::sqrt(2.0f * innerRadius * innerRadius - 2.0f * innerRadius * innerRadius * std::cos((2.0f * SIMD_PI) / static_cast<float>(subdivisions)));

        btCylinderShape* cylinderShape = new btCylinderShape(btVector3(innerRadius, (SIMD_PI / static_cast<float>(subdivisions)) + 0.5f * gap, innerRadius));

        btTransform transform;
        for (unsigned int i = 0; i < subdivisions; ++i)
        {
            float angle = (i * 2.0f * SIMD_PI) / static_cast<float>(subdivisions);
            btVector3 position = side.rotate(forward, angle);
            btQuaternion q(forward, angle);

            transform.setIdentity();
            transform.setOrigin(position);
            transform.setRotation(q);
            compoundShape->addChildShape(transform, cylinderShape);
        }



        



        this->pickingCollisionShape = compoundShape;
        this->pickingCollisionObject.setCollisionShape(this->pickingCollisionShape);

        if (world != nullptr && this->pickingCollisionShape != nullptr)
        {
            world->AddCollisionObject(this->pickingCollisionObject, this->pickingCollisionGroup, CollisionGroups::EditorSelectionRay);
        }


        this->pickingCollisionShapeType = PickingCollisionShapeType_Torus;
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
        if (this->spriteModel == nullptr)       // <-- No need to re-create the model if we already have one.
        {
            this->spriteModel = ModelLibrary::CreatePlaneXY(0.5f, 0.5f);
            assert(this->spriteModel != nullptr);
            assert(this->spriteModel->meshes.count == 1);
            {
                this->spriteModel->meshes[0]->SetMaterial("engine/materials/editor-sprite.material");

                assert(this->spritePickingCollisionShape  == nullptr);
                assert(this->spritePickingCollisionObject == nullptr);
                {
                    this->spritePickingCollisionShape = new btBoxShape(btVector3(0.25f, 0.25f, 0.0f));
                    
                    this->spritePickingCollisionObject = new CollisionObject;
                    this->spritePickingCollisionObject->setUserPointer(this);
                    this->spritePickingCollisionObject->setCollisionShape(this->spritePickingCollisionShape);


                    this->ApplyTransformToSprite();
                    this->ApplyScaleToSprite();
                }
            }
        }


        assert(this->spriteModel != nullptr);
        assert(this->spriteModel->meshes.count == 1);
        assert(this->spriteModel->meshes[0]->GetMaterial() != nullptr);
        {
            this->spriteModel->meshes[0]->GetMaterial()->SetParameter("SpriteTexture", Texture2DLibrary::Acquire(texturePath));
            this->spriteModel->meshes[0]->GetMaterial()->SetParameter("SpriteColour",  colour);
        }


        this->spriteTexturePath = texturePath;
    }

    void EditorMetadataComponent::ShowSprite(const char* texturePath, float colourR, float colourG, float colourB)
    {
        this->ShowSprite(texturePath, glm::vec3(colourR, colourG, colourB));
    }

    void EditorMetadataComponent::HideSprite()
    {
        delete this->spritePickingCollisionObject;      // <-- the destructor will remove it from the scene.
        delete this->spritePickingCollisionShape;
        ModelLibrary::Delete(this->spriteModel);

        this->spritePickingCollisionObject = nullptr;
        this->spritePickingCollisionShape  = nullptr;
        this->spriteModel                  = nullptr;

        this->spriteTexturePath = "";
    }

    void EditorMetadataComponent::ApplyTransformToSprite()
    {
        if (this->spritePickingCollisionShape != nullptr && this->spritePickingCollisionObject != nullptr)
        {
            btTransform transform;
            transform.setFromOpenGLMatrix(&this->spriteTransform[0][0]);
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
                world->AddCollisionObject(*this->spritePickingCollisionObject);
            }
        }
    }

    bool EditorMetadataComponent::IsUsingSprite() const
    {
        return this->spriteModel != nullptr;
    }

    const Model* EditorMetadataComponent::GetSpriteModel() const
    {
        return this->spriteModel;
    }

    CollisionObject* EditorMetadataComponent::GetSpritePickingCollisionObject()
    {
        return this->spritePickingCollisionObject;
    }

    const char* EditorMetadataComponent::GetSpriteTexturePath() const
    {
        return this->spriteTexturePath.c_str();
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

                this->directionArrowVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
                this->directionArrowVA->SetData(&vertices[0].x, 4, indices, 6);

                this->directionArrowModel->AttachMesh(this->directionArrowVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
                this->directionArrowModel->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 1.0f, 0.0f);
            }
        }
    }

    void EditorMetadataComponent::HideDirectionArrow()
    {
        ModelLibrary::Delete(this->directionArrowModel);
        GarbageCollector::MarkForCollection(this->directionArrowVA);

        this->directionArrowModel = nullptr;
        this->directionArrowVA    = nullptr;
    }

    bool EditorMetadataComponent::IsShowingDirectionArrow() const
    {
        return this->directionArrowModel != nullptr;
    }

    const Model* EditorMetadataComponent::GetDirectionArrowModel() const
    {
        return this->directionArrowModel;
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
                // We will be re-using a single cylinder shape here, so we only want to delete the first one.
                auto compoundShape = static_cast<btCompoundShape*>(this->pickingCollisionShape);

                if (compoundShape->getNumChildShapes() > 0)
                {
                    delete compoundShape->getChildShape(0);
                }
            }



            delete this->pickingCollisionShape;
            this->pickingCollisionShape = nullptr;
        }
    }
}