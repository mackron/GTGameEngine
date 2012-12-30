
#include <GTEngine/Components/EditorMetadataComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(EditorMetadataComponent, "EditorMetadata");

    EditorMetadataComponent::EditorMetadataComponent(SceneNode &node)
        : Component(node),
          alwaysShowOnTop(false), useModelForPickingShape(true),
          isSelected(false), selectionWireframeColour(1.0f, 0.75f, 0.5f),
          pickingCollisionObject(), pickingCollisionShape(nullptr), pickingCollisionGroup(CollisionGroups::EditorSelectionVolume)
    {
        pickingCollisionObject.setUserPointer(this);
    }

    EditorMetadataComponent::~EditorMetadataComponent()
    {
        this->DeleteCollisionShape();
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