
#include <GTEngine/Components/EditorMetadataComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/CollisionGroups.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(EditorMetadataComponent, "EditorMetadata");

    EditorMetadataComponent::EditorMetadataComponent(SceneNode &node)
        : Component(node),
          isSelected(false), selectionWireframeColour(1.0f, 0.75f, 0.5f),
          pickingCollisionObject(), pickingCollisionShape(nullptr)
    {
        pickingCollisionObject.setUserPointer(this);
    }

    EditorMetadataComponent::~EditorMetadataComponent()
    {
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


        if (this->pickingCollisionShape != nullptr)
        {
            delete this->pickingCollisionShape;
            this->pickingCollisionShape = nullptr;
        }

        
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
            world->AddCollisionObject(this->pickingCollisionObject, CollisionGroups::EditorSelectionVolume, CollisionGroups::EditorSelectionRay);
        }
    }
}