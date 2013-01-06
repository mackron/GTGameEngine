
#include <GTEngine/Editor/TransformGizmo.hpp>
#include <GTEngine/ModelLibrary.hpp>

namespace GTEngine
{
    TransformGizmo::TransformGizmo()
        : sceneNode(),
          xArrowSceneNode(), yArrowSceneNode(), zArrowSceneNode(), xCircleSceneNode(), yCircleSceneNode(), zCircleSceneNode(), cameraFacingCircleSceneNode(),
          xArrowModel(),     yArrowModel(),     zArrowModel(),     xCircleModel(),     yCircleModel(),     zCircleModel(),     cameraFacingCircleModel(),
          arrowLineVA(nullptr), arrowHeadVA(nullptr),
          xCircleVA(nullptr), yCircleVA(nullptr), zCircleVA(nullptr), cameraFacingCircleVA(nullptr)
    {
    }

    TransformGizmo::~TransformGizmo()
    {
        GarbageCollector::MarkForCollection(this->arrowLineVA);
        GarbageCollector::MarkForCollection(this->arrowHeadVA);
        
        GarbageCollector::MarkForCollection(this->xCircleVA);
        GarbageCollector::MarkForCollection(this->yCircleVA);
        GarbageCollector::MarkForCollection(this->zCircleVA);
        GarbageCollector::MarkForCollection(this->cameraFacingCircleVA);
    }


    void TransformGizmo::Initialise()
    {
        /////////////////////////////////////////
        // Arrow Mesh
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




        /////////////////////////////////////////
        // Circle Mesh
        float        circleRadius       = 1.5f;
        unsigned int circleSegmentCount = 64;
        float        circleSegmentAngle = glm::radians(360.0f / static_cast<float>(circleSegmentCount));

        GTCore::Vector<glm::vec3> circleVertices;
        for (unsigned int i = 0; i < circleSegmentCount; ++i)
        {
            glm::vec3 position;

            position.x = std::cos(circleSegmentAngle * i) * circleRadius;
            position.y = std::sin(circleSegmentAngle * i) * circleRadius;
            position.z = 0.0f;

            circleVertices.PushBack(position);
        }

        GTCore::Vector<unsigned int> circleIndices;
        for (unsigned int i = 0; i < circleSegmentCount; ++i)
        {
            circleIndices.PushBack(i);
            circleIndices.PushBack((i + 1) % circleSegmentCount);
        }


        this->xCircleVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->xCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);
        
        this->yCircleVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->yCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);
        
        this->zCircleVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->zCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);

        this->cameraFacingCircleVA = new VertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->cameraFacingCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);





        this->xArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->xArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        this->yArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->yArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        this->zArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->zArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");

        this->xCircleModel.AttachMesh(this->xCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->yCircleModel.AttachMesh(this->yCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->zCircleModel.AttachMesh(this->zCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->cameraFacingCircleModel.AttachMesh(this->cameraFacingCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);



        // All of the arrow scene nodes use the same model, only we orientate them to point in a particular direction. By default, it's pointing down -Z.
        this->xArrowSceneNode.RotateY(-90.0f);
        this->yArrowSceneNode.RotateX( 90.0f);
        this->zArrowSceneNode.RotateX( 180.0f);

        this->xCircleSceneNode.RotateY(-90.0f);
        this->yCircleSceneNode.RotateX( 90.0f);
        this->zCircleSceneNode.RotateX(  0.0f);



        this->xArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->xArrowModel);
        this->yArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->yArrowModel);
        this->zArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->zArrowModel);

        this->xCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->xCircleModel);
        this->yCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->yCircleModel);
        this->zCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->zCircleModel);
        this->cameraFacingCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->cameraFacingCircleModel);


        this->xArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->yArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->zArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();

        this->xCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->yCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->zCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->cameraFacingCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();



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


        metadata = this->xCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);

        metadata = this->yCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);

        metadata = this->zCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);

        metadata = this->cameraFacingCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);



        this->sceneNode.AttachChild(this->xArrowSceneNode);
        this->sceneNode.AttachChild(this->yArrowSceneNode);
        this->sceneNode.AttachChild(this->zArrowSceneNode);

        this->sceneNode.AttachChild(this->cameraFacingCircleSceneNode);
        this->sceneNode.AttachChild(this->xCircleSceneNode);
        this->sceneNode.AttachChild(this->yCircleSceneNode);
        this->sceneNode.AttachChild(this->zCircleSceneNode);

        this->UpdatePickingVolumes();
        this->RestoreColours();


        // We don't want to do serialization here.
        this->sceneNode.DisableSerialization();



        // We will give these scene nodes names to make debugging a bit easier.
        this->sceneNode.SetName("TransformGizmo");
        this->xArrowSceneNode.SetName("TransformGizmo_xArrowSceneNode");
        this->yArrowSceneNode.SetName("TransformGizmo_yArrowSceneNode");
        this->zArrowSceneNode.SetName("TransformGizmo_zArrowSceneNode");
        this->xCircleSceneNode.SetName("TransformGizmo_xCircleSceneNode");
        this->yCircleSceneNode.SetName("TransformGizmo_yCircleSceneNode");
        this->zCircleSceneNode.SetName("TransformGizmo_zCircleSceneNode");
    }


    void TransformGizmo::SetPosition(const glm::vec3 &position)
    {
        this->sceneNode.SetPosition(position);
    }

    const glm::vec3 & TransformGizmo::GetPosition() const
    {
        return this->sceneNode.GetPosition();
    }


    void TransformGizmo::SetRotation(const glm::quat &rotation, const SceneNode &cameraNode)
    {
        this->xCircleSceneNode.SetWorldOrientation(rotation);
        this->yCircleSceneNode.SetWorldOrientation(rotation);
        this->zCircleSceneNode.SetWorldOrientation(rotation);

        this->xCircleSceneNode.RotateY(-90.0f);
        this->yCircleSceneNode.RotateX( 90.0f);
        this->zCircleSceneNode.RotateX(  0.0f);

        // We need to update the geometry of the circles so that only the pieces that are actually facing the camera are shown.
        this->UpdateCircleVertexArray(this->xCircleVA, this->xCircleSceneNode, cameraNode);
        this->UpdateCircleVertexArray(this->yCircleVA, this->yCircleSceneNode, cameraNode);
        this->UpdateCircleVertexArray(this->zCircleVA, this->zCircleSceneNode, cameraNode);


        // The camera-facing scene node needs to be facing the camera, like a sprite.
        this->cameraFacingCircleSceneNode.SetWorldOrientation(cameraNode.GetWorldOrientation() * glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    
    void TransformGizmo::SetScale(const glm::vec3 &scale)
    {
        this->sceneNode.SetScale(scale);
        this->UpdatePickingVolumes();
    }

    const glm::vec3 & TransformGizmo::GetScale() const
    {
        return this->sceneNode.GetScale();
    }


    void TransformGizmo::Show()
    {
        this->sceneNode.Show();
    }

    void TransformGizmo::Hide()
    {
        this->sceneNode.Hide();
    }


    void TransformGizmo::RestoreColours()
    {
        this->xArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->xArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->yArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->yArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->zArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        this->zArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);

        this->xCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f,  0.35f, 0.35f);
        this->yCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.35f, 1.0f,  0.35f);
        this->zCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.35f, 0.35f, 1.0f);
        this->cameraFacingCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.75f, 0.75f, 0.75f);
    }

    void TransformGizmo::ChangeAxisColour(SceneNode &axisSceneNode, float r, float g, float b)
    {
        assert(&axisSceneNode == &this->xArrowSceneNode  || &axisSceneNode == &this->yArrowSceneNode  || &axisSceneNode == &this->zArrowSceneNode ||
               &axisSceneNode == &this->xCircleSceneNode || &axisSceneNode == &this->yCircleSceneNode || &axisSceneNode == &this->zCircleSceneNode);
        {
            auto modelComponent = axisSceneNode.GetComponent<ModelComponent>();
            if (modelComponent != nullptr)
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)
                {
                    for (size_t i = 0; i < model->meshes.count; ++i)
                    {
                        model->meshes[i]->GetMaterial()->SetParameter("EmissiveColour", r, g, b);
                    }
                }
            }
        }
    }


    void TransformGizmo::UpdatePickingVolumes()
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



        // Now for the circles. We use a torus for these. We'll use the Y scale for this, but since the scale is uniform, shouldn't really matter what we use.
        float yScale      = this->GetScale().y;
        float outerRadius = 1.5f * yScale;              // 1.5 is the radius of the circles.
        float innerRadius = 0.1f * yScale;              // This is the radius of the geometry making up the ring.

        metadata = this->xCircleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToTorus(outerRadius, innerRadius, 32);
        }

        metadata = this->yCircleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToTorus(outerRadius, innerRadius, 32);
        }

        metadata = this->zCircleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToTorus(outerRadius, innerRadius, 32);
        }
    }



    void TransformGizmo::UpdateCircleVertexArray(VertexArray* vertexArray, const SceneNode &circleNode, const SceneNode &cameraNode)
    {
        assert(vertexArray != nullptr);
        {
            size_t vertexCount = vertexArray->GetVertexCount();

            auto circlePosition = circleNode.GetWorldPosition();
            auto cameraPosition = cameraNode.GetWorldPosition();

            auto cameraTransform = cameraNode.GetComponent<CameraComponent>()->GetViewMatrix();
            
            circlePosition = glm::vec3(cameraTransform * glm::vec4(circlePosition, 1.0f));

            // All we do is start from the start and work our way around. If a line segment has both vertices facing away from the camera,
            // we'll ignore it and move on.
            GTCore::Vector<unsigned int> circleIndices;
            for (unsigned int i = 0; i < vertexCount; ++i)
            {
                size_t index0 = i;
                size_t index1 = (i + 1) % vertexCount;

                auto vertexPtr0 = reinterpret_cast<const glm::vec3*>(vertexArray->GetVertexDataPtr()) + index0;
                auto vertexPtr1 = reinterpret_cast<const glm::vec3*>(vertexArray->GetVertexDataPtr()) + index1;

                glm::vec3 vertex0(cameraTransform * circleNode.GetWorldTransform() * glm::vec4(*vertexPtr0, 1.0f));
                glm::vec3 vertex1(cameraTransform * circleNode.GetWorldTransform() * glm::vec4(*vertexPtr1, 1.0f));

                if (glm::vec3(circlePosition - vertex0).z <= 0.0f ||
                    glm::vec3(circlePosition - vertex1).z <= 0.0f)
                {
                    circleIndices.PushBack(index0);
                    circleIndices.PushBack(index1);
                }
            }


            if (circleIndices.count > 0)
            {
                vertexArray->SetIndexData(&circleIndices[0], circleIndices.count);
            }
        }
    }
}