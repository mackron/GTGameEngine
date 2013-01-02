
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
          pickingCollisionObject(), pickingCollisionShape(nullptr), pickingCollisionGroup(CollisionGroups::EditorSelectionVolume),
          spriteModel(nullptr), spritePickingCollisionObject(nullptr), spritePickingCollisionShape(nullptr), spriteTransform()
    {
        pickingCollisionObject.setUserPointer(this);
    }

    EditorMetadataComponent::~EditorMetadataComponent()
    {
        this->DeleteCollisionShape();
        this->HideSprite();         // <-- This will make sure everything to do with the sprite is deallocated.
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
                    this->spritePickingCollisionShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f) * ToBulletVector3(this->node.GetWorldScale()));
                    
                    this->spritePickingCollisionObject = new CollisionObject;
                    this->spritePickingCollisionObject->setUserPointer(this);
                    this->spritePickingCollisionObject->setCollisionShape(this->spritePickingCollisionShape);
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


            this->spritePickingCollisionShape->setImplicitShapeDimensions(btVector3(0.5f, 0.5f, 0.5f) * ToBulletVector3(this->node.GetWorldScale()));


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



    void EditorMetadataComponent::DeleteCollisionShape()
    {
        if (this->pickingCollisionShape != nullptr)
        {
            if (this->pickingCollisionShape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
            {
                auto compoundShape = static_cast<btCompoundShape*>(this->pickingCollisionShape);
                
                while (compoundShape->getNumChildShapes() > 0)
                {
                    delete compoundShape->getChildShape(0);
                    compoundShape->removeChildShapeByIndex(0);
                }
            }


            delete this->pickingCollisionShape;
            this->pickingCollisionShape = nullptr;
        }
    }
}