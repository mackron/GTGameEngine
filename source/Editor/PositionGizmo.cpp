
#include <GTEngine/Editor/PositionGizmo.hpp>

namespace GTEngine
{
    PositionGizmo::PositionGizmo()
        : sceneNode(), xArrowSceneNode(), yArrowSceneNode(), zArrowSceneNode()
    {
    }

    PositionGizmo::~PositionGizmo()
    {
    }


    void PositionGizmo::Initialise()
    {
        // All of the arrow scene nodes use the same model, only we orientate them to point in a particular direction. By default, it's pointing down -Z.
        this->xArrowSceneNode.RotateY(-90.0f);
        this->yArrowSceneNode.RotateX( 90.0f);
        this->zArrowSceneNode.RotateX( 180.0f);


        auto model = this->xArrowSceneNode.AddComponent<ModelComponent>()->SetModel("engine/models/gizmo-arrow.dae");
        assert(model != nullptr);
        {
            model->meshes[0]->SetMaterial("engine/materials/simple-emissive.material");
            model->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        }

        model = this->yArrowSceneNode.AddComponent<ModelComponent>()->SetModel("engine/models/gizmo-arrow.dae");
        assert(model != nullptr);
        {
            model->meshes[0]->SetMaterial("engine/materials/simple-emissive.material");
            model->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        }

        model = this->zArrowSceneNode.AddComponent<ModelComponent>()->SetModel("engine/models/gizmo-arrow.dae");
        assert(model != nullptr);
        {
            model->meshes[0]->SetMaterial("engine/materials/simple-emissive.material");
            model->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        }


        auto metadata = this->sceneNode.AddComponent<EditorMetadataComponent>();

        metadata = this->xArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetPickingCollisionShapeToModel();

        metadata = this->yArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetPickingCollisionShapeToModel();

        metadata = this->zArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetPickingCollisionShapeToModel();


        this->sceneNode.AttachChild(this->xArrowSceneNode);
        this->sceneNode.AttachChild(this->yArrowSceneNode);
        this->sceneNode.AttachChild(this->zArrowSceneNode);
    }


    void PositionGizmo::SetPosition(const glm::vec3 &position)
    {
        this->sceneNode.SetPosition(position);
    }

    const glm::vec3 & PositionGizmo::GetPosition() const
    {
        return this->sceneNode.GetPosition();
    }
}