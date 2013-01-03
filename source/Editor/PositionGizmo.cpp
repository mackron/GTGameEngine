
#include <GTEngine/Editor/PositionGizmo.hpp>
#include <GTEngine/ModelLibrary.hpp>

namespace GTEngine
{
    PositionGizmo::PositionGizmo()
        : sceneNode(),
          xArrowSceneNode(), yArrowSceneNode(), zArrowSceneNode(),
          xArrowModel(),     yArrowModel(),     zArrowModel(),
          arrowLineVA(nullptr), arrowHeadVA(nullptr)
    {
    }

    PositionGizmo::~PositionGizmo()
    {
        delete this->arrowLineVA;
        delete this->arrowHeadVA;
    }


    void PositionGizmo::Initialise()
    {
        this->arrowLineVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->arrowHeadVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);


        // Arrow line.
        glm::vec3 arrowLineVertices[2] =
        {
            glm::vec3(0.0f, 0.0f,  0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        };
        unsigned int arrowLineIndices[2] = {0, 1};


        // Arrow head.
        float        arrowHeadLength       = 0.3f;
        float        arrowHeadRadius       = 0.075f;
        unsigned int arrowHeadSegments     = 16;
        float        arrowHeadSegmentAngle = glm::radians(360.0f / static_cast<float>(arrowHeadSegments));
        
        GTCore::Vector<glm::vec3>    arrowHeadVertices;
        GTCore::Vector<unsigned int> arrowHeadIndices;

        arrowHeadVertices.PushBack(glm::vec3(0.0f, 0.0f, -1.0f));                       // First vertex is the point.
        arrowHeadVertices.PushBack(glm::vec3(0.0f, 0.0f, -1.0f + arrowHeadLength));     // Second vertex is the center of the base.

        // Now we do the base vertices. There will be arrowHeadSegments vertices on the base.
        for (unsigned int i = 0; i < arrowHeadSegments; ++i)
        {
            glm::vec3 position;

            position.x = std::cos(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.y = std::sin(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.z = -1.0f + arrowHeadLength;

            arrowHeadVertices.PushBack(position);
        }

        // At this point we'll have all of the arrow head vertices setup. We now need to do the indices. We'll start with the base
        unsigned int pointIndex          = 0;
        unsigned int baseCenterIndex     = 1;
        unsigned int firstOuterBaseIndex = 2;

        for (unsigned int i = 0; i < arrowHeadSegments; ++i)
        {
            arrowHeadIndices.PushBack(baseCenterIndex);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + i);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + ((i + 1) % arrowHeadSegments));

            arrowHeadIndices.PushBack(pointIndex);
            arrowHeadIndices.PushBack(firstOuterBaseIndex + ((i + 1) % arrowHeadSegments));
            arrowHeadIndices.PushBack(firstOuterBaseIndex + i);
        }

        



        this->arrowLineVA->SetData(&arrowLineVertices[0].x, 2, arrowLineIndices, 2);
        this->arrowHeadVA->SetData(&arrowHeadVertices[0].x, arrowHeadVertices.count, &arrowHeadIndices[0], arrowHeadIndices.count);




        this->xArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->xArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        this->xArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->xArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);

        this->yArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->yArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        this->yArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->yArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);

        this->zArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->zArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        this->zArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        this->zArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);



        // All of the arrow scene nodes use the same model, only we orientate them to point in a particular direction. By default, it's pointing down -Z.
        this->xArrowSceneNode.RotateY(-90.0f);
        this->yArrowSceneNode.RotateX( 90.0f);
        this->zArrowSceneNode.RotateX( 180.0f);



        this->xArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->xArrowModel);
        this->yArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->yArrowModel);
        this->zArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->zArrowModel);

        this->xArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->yArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->zArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();


        auto metadata = this->sceneNode.AddComponent<EditorMetadataComponent>();

        metadata = this->xArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);

        metadata = this->yArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);

        metadata = this->zArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);


        this->sceneNode.AttachChild(this->xArrowSceneNode);
        this->sceneNode.AttachChild(this->yArrowSceneNode);
        this->sceneNode.AttachChild(this->zArrowSceneNode);


        this->UpdatePickingVolumes();
    }


    void PositionGizmo::SetPosition(const glm::vec3 &position)
    {
        this->sceneNode.SetPosition(position);
    }

    const glm::vec3 & PositionGizmo::GetPosition() const
    {
        return this->sceneNode.GetPosition();
    }

    
    void PositionGizmo::SetScale(const glm::vec3 &scale)
    {
        this->sceneNode.SetScale(scale);
        this->UpdatePickingVolumes();
    }

    const glm::vec3 & PositionGizmo::GetScale() const
    {
        return this->sceneNode.GetScale();
    }


    void PositionGizmo::Show()
    {
        this->sceneNode.Show();
    }

    void PositionGizmo::Hide()
    {
        this->sceneNode.Hide();
    }


    void PositionGizmo::RestoreColours()
    {
        this->xArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->xArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);

        this->yArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->yArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);

        this->zArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        this->zArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
    }

    void PositionGizmo::ChangeAxisColour(SceneNode &axisSceneNode, float r, float g, float b)
    {
        assert(&axisSceneNode == &this->xArrowSceneNode || &axisSceneNode == &this->yArrowSceneNode || &axisSceneNode == &this->zArrowSceneNode);
        {
            auto modelComponent = axisSceneNode.GetComponent<ModelComponent>();
            if (modelComponent != nullptr)
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)
                {
                    assert(model->meshes.count == 2);

                    model->meshes[0]->GetMaterial()->SetParameter("EmissiveColour", r, g, b);
                    model->meshes[1]->GetMaterial()->SetParameter("EmissiveColour", r, g, b);
                }
            }
        }
    }


    void PositionGizmo::UpdatePickingVolumes()
    {
        // The models are actually unit length which makes determining the length of the picking volume super easy - it's the scale of the z axis. The
        // scale of a gizmo should be uniform, so you could also use x or y.
        glm::vec3 halfExtents;
        halfExtents.x = 0.075f;
        halfExtents.y = 0.075f;
        halfExtents.z = 0.5f;
        halfExtents *= this->GetScale();

        glm::vec3 offset;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = -halfExtents.z;

        
        auto metadata = this->xArrowSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }

        metadata = this->yArrowSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }

        metadata = this->zArrowSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }
    }
}