// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/TransformGizmo.hpp>
#include <GTEngine/ModelLibrary.hpp>            // <-- Can delete this.
#include <GTEngine/MaterialLibrary.hpp>

namespace GTEngine
{
    TransformGizmo::TransformGizmo()
        : position(), orientation(), scale(1.0f, 1.0f, 1.0f),
          sceneNode(),
          //xArrowSceneNode(),  yArrowSceneNode(),  zArrowSceneNode(),
          xCircleSceneNode(), yCircleSceneNode(), zCircleSceneNode(), cameraFacingCircleSceneNode(),
          //xScaleSceneNode(),  yScaleSceneNode(),  zScaleSceneNode(),
          //xArrowModel(),      yArrowModel(),      zArrowModel(),
          xCircleModel(),     yCircleModel(),     zCircleModel(),     cameraFacingCircleModel(),
          //xScaleModel(),      yScaleModel(),      zScaleModel(),
          xTranslateHandle(HandleAxis_X), yTranslateHandle(HandleAxis_Y), zTranslateHandle(HandleAxis_Z),
          xScaleHandle(HandleAxis_X), yScaleHandle(HandleAxis_Y), zScaleHandle(HandleAxis_Z),
          //arrowLineVA(nullptr), arrowHeadVA(nullptr),
          xCircleVA(nullptr), yCircleVA(nullptr), zCircleVA(nullptr), cameraFacingCircleVA(nullptr)/*,
          scaleLineVA(nullptr), scaleHeadVA(nullptr)*/
    {
    }

    TransformGizmo::~TransformGizmo()
    {
        //Renderer2::DeleteVertexArray(this->arrowLineVA);
        //Renderer2::DeleteVertexArray(this->arrowHeadVA);

        Renderer2::DeleteVertexArray(this->xCircleVA);
        Renderer2::DeleteVertexArray(this->yCircleVA);
        Renderer2::DeleteVertexArray(this->zCircleVA);
        Renderer2::DeleteVertexArray(this->cameraFacingCircleVA);

        //Renderer2::DeleteVertexArray(this->scaleLineVA);
        //Renderer2::DeleteVertexArray(this->scaleHeadVA);
    }


    void TransformGizmo::Initialise()
    {
        /*
        /////////////////////////////////////////
        // Arrow Mesh
        this->arrowLineVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->arrowHeadVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);

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
        */



        /////////////////////////////////////////
        // Circle Mesh
        float        circleRadius       = 1.0f;
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


        this->xCircleVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->xCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);

        this->yCircleVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->yCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);

        this->zCircleVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->zCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);

        this->cameraFacingCircleVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->cameraFacingCircleVA->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);


        /*
        /////////////////////////////////////////
        // Scale Handle Mesh
        this->scaleLineVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->scaleHeadVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);

        // Scale line.
        glm::vec3 scaleLineVertices[2] =
        {
            glm::vec3(0.0f, 0.0f,  0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        };
        unsigned int scaleLineIndices[2] = {0, 1};


        // Scale head.
        float scaleHeadHalfExtent = 0.075f;

        glm::vec3 scaleHeadVertices[8] =
        {
            // Front face (facing +Z)
            glm::vec3(-scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            glm::vec3(+scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            glm::vec3(+scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            glm::vec3(-scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),

            // Back face (facing -Z)
            glm::vec3(+scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f),
            glm::vec3(-scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f),
            glm::vec3(-scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f),
            glm::vec3(+scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f)
        };

        unsigned int scaleHeadIndices[36] =
        {
            // +Z
            0, 1, 2,
            2, 3, 0,

            // -Z
            4, 5, 6,
            6, 7, 4,

            // +Y
            3, 2, 7,
            7, 6, 3,

            // -Y
            5, 4, 1,
            1, 0, 5,

            // +X
            1, 4, 7,
            7, 2, 1,

            // -X
            5, 0, 3,
            3, 6, 5,
        };

        this->scaleLineVA->SetData(&scaleLineVertices[0].x, 2, scaleLineIndices, 2);
        this->scaleHeadVA->SetData(&scaleHeadVertices[0].x, 8, scaleHeadIndices, 36);
        */






        //this->xArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        //this->xArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        //this->yArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        //this->yArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");
        //this->zArrowModel.AttachMesh(arrowLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        //this->zArrowModel.AttachMesh(arrowHeadVA, "engine/materials/simple-emissive.material");

        this->xCircleModel.AttachMesh(this->xCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->yCircleModel.AttachMesh(this->yCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->zCircleModel.AttachMesh(this->zCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->cameraFacingCircleModel.AttachMesh(this->cameraFacingCircleVA, "engine/materials/simple-emissive.material", DrawMode_Lines);

        /*
        this->xScaleModel.AttachMesh(scaleLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->xScaleModel.AttachMesh(scaleHeadVA, "engine/materials/simple-emissive.material");
        this->yScaleModel.AttachMesh(scaleLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->yScaleModel.AttachMesh(scaleHeadVA, "engine/materials/simple-emissive.material");
        this->zScaleModel.AttachMesh(scaleLineVA, "engine/materials/simple-emissive.material", DrawMode_Lines);
        this->zScaleModel.AttachMesh(scaleHeadVA, "engine/materials/simple-emissive.material");
        */


        // All of the arrow scene nodes use the same model, only we orientate them to point in a particular direction. By default, it's pointing down -Z.
        //this->xArrowSceneNode.RotateY(-90.0f);
        //this->yArrowSceneNode.RotateX( 90.0f);
        //this->zArrowSceneNode.RotateX( 180.0f);

        this->xCircleSceneNode.RotateY(-90.0f);
        this->yCircleSceneNode.RotateX( 90.0f);
        this->zCircleSceneNode.RotateX(  0.0f);

        /*
        this->xScaleSceneNode.RotateY(-90.0f);
        this->yScaleSceneNode.RotateX( 90.0f);
        this->zScaleSceneNode.RotateX( 180.0f);
        */



        //this->xArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->xArrowModel);
        //this->yArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->yArrowModel);
        //this->zArrowSceneNode.AddComponent<ModelComponent>()->SetModel(this->zArrowModel);

        this->xCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->xCircleModel);
        this->yCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->yCircleModel);
        this->zCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->zCircleModel);
        this->cameraFacingCircleSceneNode.AddComponent<ModelComponent>()->SetModel(this->cameraFacingCircleModel);

        /*
        this->xScaleSceneNode.AddComponent<ModelComponent>()->SetModel(this->xScaleModel);
        this->yScaleSceneNode.AddComponent<ModelComponent>()->SetModel(this->yScaleModel);
        this->zScaleSceneNode.AddComponent<ModelComponent>()->SetModel(this->zScaleModel);
        */



        //this->xArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        //this->yArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        //this->zArrowSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();

        this->xCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->yCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->zCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->cameraFacingCircleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();

        /*
        this->xScaleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->yScaleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        this->zScaleSceneNode.GetComponent<ModelComponent>()->DisableShadowCasting();
        */




        auto metadata = this->sceneNode.AddComponent<EditorMetadataComponent>();
        metadata->IsSystemNode(true);

        /*
        metadata = this->xArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->yArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->zArrowSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);
        */

        metadata = this->xCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->yCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->zCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->cameraFacingCircleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);


        /*
        metadata = this->xScaleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->yScaleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);

        metadata = this->zScaleSceneNode.AddComponent<EditorMetadataComponent>();
        metadata->SetPickingCollisionGroup(CollisionGroups::EditorGizmo);
        metadata->SetAlwaysShowOnTop(true);
        metadata->UseModelForPickingShape(false);
        metadata->IsSystemNode(true);
        */





        //this->sceneNode.AttachChild(this->xArrowSceneNode);
        //this->sceneNode.AttachChild(this->yArrowSceneNode);
        //this->sceneNode.AttachChild(this->zArrowSceneNode);

        this->sceneNode.AttachChild(this->cameraFacingCircleSceneNode);
        this->sceneNode.AttachChild(this->xCircleSceneNode);
        this->sceneNode.AttachChild(this->yCircleSceneNode);
        this->sceneNode.AttachChild(this->zCircleSceneNode);

        /*
        this->sceneNode.AttachChild(this->xScaleSceneNode);
        this->sceneNode.AttachChild(this->yScaleSceneNode);
        this->sceneNode.AttachChild(this->zScaleSceneNode);
        */

        this->UpdatePickingVolumes();
        this->RestoreColours();


        // We don't want to do serialization and state stack staging here.
        this->sceneNode.DisableSerialization();
        this->sceneNode.DisableStateStackStaging();



        // We will give these scene nodes names to make debugging a bit easier.
        this->sceneNode.SetName("TransformGizmo");
        //this->xArrowSceneNode.SetName("TransformGizmo_xArrowSceneNode");
        //this->yArrowSceneNode.SetName("TransformGizmo_yArrowSceneNode");
        //this->zArrowSceneNode.SetName("TransformGizmo_zArrowSceneNode");
        this->xCircleSceneNode.SetName("TransformGizmo_xCircleSceneNode");
        this->yCircleSceneNode.SetName("TransformGizmo_yCircleSceneNode");
        this->zCircleSceneNode.SetName("TransformGizmo_zCircleSceneNode");
        //this->xScaleSceneNode.SetName("TransformGizmo_xScaleSceneNode");
        //this->yScaleSceneNode.SetName("TransformGizmo_yScaleSceneNode");
        //this->zScaleSceneNode.SetName("TransformGizmo_zScaleSceneNode");
    }


    void TransformGizmo::SetPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;
        this->UpdateHandleTransforms();

        this->sceneNode.SetPosition(newPosition);
    }

    const glm::vec3 & TransformGizmo::GetPosition() const
    {
        //return this->sceneNode.GetPosition();
        return this->position;
    }


    void TransformGizmo::SetRotation(const glm::quat &rotation, const SceneNode &cameraNode)
    {
        this->orientation = rotation;
        this->UpdateHandleTransforms();

        this->sceneNode.SetWorldOrientation(rotation);

        // We need to update the geometry of the circles so that only the pieces that are actually facing the camera are shown.
        this->UpdateCircleVertexArray(this->xCircleVA, this->xCircleSceneNode, cameraNode);
        this->UpdateCircleVertexArray(this->yCircleVA, this->yCircleSceneNode, cameraNode);
        this->UpdateCircleVertexArray(this->zCircleVA, this->zCircleSceneNode, cameraNode);


        // The camera-facing scene node needs to be facing the camera, like a sprite.
        this->cameraFacingCircleSceneNode.SetWorldOrientation(cameraNode.GetWorldOrientation() * glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    }


    void TransformGizmo::SetScale(const glm::vec3 &newScale)
    {
        this->scale = newScale;
        this->UpdateHandleTransforms();


        //this->sceneNode.SetScale(scale);
        //this->UpdatePickingVolumes();
    }

    const glm::vec3 & TransformGizmo::GetScale() const
    {
        //return this->sceneNode.GetScale();
        return this->scale;
    }



    void TransformGizmo::UpdateHandleTransforms()
    {
        this->xTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->yTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->zTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);

        // TODO: Rotation.


        this->xScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->yScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->zScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
    }


    void TransformGizmo::Show(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        if (this->showingTranslationHandles)
        {
            this->ShowTranslationHandles(renderer, pickingWorld);
        }
        else if (this->showingRotationHandles)
        {
            this->ShowRotationHandles(renderer, pickingWorld);
        }
        else if (this->showingScaleHandles)
        {
            this->ShowScaleHandles(renderer, pickingWorld);
        }


        this->sceneNode.Show();
    }

    void TransformGizmo::Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->xTranslateHandle.Hide(renderer, pickingWorld);
        this->yTranslateHandle.Hide(renderer, pickingWorld);
        this->zTranslateHandle.Hide(renderer, pickingWorld);

        // TODO: Rotation.

        this->xScaleHandle.Hide(renderer, pickingWorld);
        this->yScaleHandle.Hide(renderer, pickingWorld);
        this->zScaleHandle.Hide(renderer, pickingWorld);

        this->sceneNode.Hide();
    }


    void TransformGizmo::ShowTranslationHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->xTranslateHandle.Show(renderer, pickingWorld);
        this->yTranslateHandle.Show(renderer, pickingWorld);
        this->zTranslateHandle.Show(renderer, pickingWorld);

        this->xCircleSceneNode.Hide();
        this->yCircleSceneNode.Hide();
        this->zCircleSceneNode.Hide();
        this->cameraFacingCircleSceneNode.Hide();

        this->xScaleHandle.Hide(renderer, pickingWorld);
        this->yScaleHandle.Hide(renderer, pickingWorld);
        this->zScaleHandle.Hide(renderer, pickingWorld);


        this->showingTranslationHandles = true;
        this->showingRotationHandles    = false;
        this->showingScaleHandles       = false;
    }

    void TransformGizmo::ShowRotationHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->xTranslateHandle.Hide(renderer, pickingWorld);
        this->yTranslateHandle.Hide(renderer, pickingWorld);
        this->zTranslateHandle.Hide(renderer, pickingWorld);

        this->xCircleSceneNode.Show();
        this->yCircleSceneNode.Show();
        this->zCircleSceneNode.Show();
        this->cameraFacingCircleSceneNode.Show();

        this->xScaleHandle.Hide(renderer, pickingWorld);
        this->yScaleHandle.Hide(renderer, pickingWorld);
        this->zScaleHandle.Hide(renderer, pickingWorld);


        this->showingTranslationHandles = false;
        this->showingRotationHandles    = true;
        this->showingScaleHandles       = false;
    }

    void TransformGizmo::ShowScaleHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->xTranslateHandle.Hide(renderer, pickingWorld);
        this->yTranslateHandle.Hide(renderer, pickingWorld);
        this->zTranslateHandle.Hide(renderer, pickingWorld);

        this->xCircleSceneNode.Hide();
        this->yCircleSceneNode.Hide();
        this->zCircleSceneNode.Hide();
        this->cameraFacingCircleSceneNode.Hide();

        this->xScaleHandle.Show(renderer, pickingWorld);
        this->yScaleHandle.Show(renderer, pickingWorld);
        this->zScaleHandle.Show(renderer, pickingWorld);


        this->showingTranslationHandles = false;
        this->showingRotationHandles    = false;
        this->showingScaleHandles       = true;
    }


    void TransformGizmo::RestoreColours()
    {
        //this->xArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        //this->xArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        //this->yArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        //this->yArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        //this->zArrowModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        //this->zArrowModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);

        this->xTranslateHandle.RestoreBaseColour();
        this->yTranslateHandle.RestoreBaseColour();
        this->zTranslateHandle.RestoreBaseColour();

        this->xCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f,  0.35f, 0.35f);
        this->yCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.35f, 1.0f,  0.35f);
        this->zCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.35f, 0.35f, 1.0f);
        this->cameraFacingCircleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.75f, 0.75f, 0.75f);

        this->xScaleHandle.RestoreBaseColour();
        this->yScaleHandle.RestoreBaseColour();
        this->zScaleHandle.RestoreBaseColour();

        /*
        this->xScaleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->xScaleModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 1.0f, 0.0f, 0.0f);
        this->yScaleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->yScaleModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 1.0f, 0.0f);
        this->zScaleModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        this->zScaleModel.meshes[1]->GetMaterial()->SetParameter("EmissiveColour", 0.0f, 0.0f, 1.0f);
        */
    }

    void TransformGizmo::ChangeAxisColour(SceneNode &axisSceneNode, float r, float g, float b)
    {
        assert(/*&axisSceneNode == &this->xArrowSceneNode  || &axisSceneNode == &this->yArrowSceneNode  || &axisSceneNode == &this->zArrowSceneNode  ||*/
               &axisSceneNode == &this->xCircleSceneNode || &axisSceneNode == &this->yCircleSceneNode || &axisSceneNode == &this->zCircleSceneNode/* ||
               &axisSceneNode == &this->xScaleSceneNode  || &axisSceneNode == &this->yScaleSceneNode  || &axisSceneNode == &this->zScaleSceneNode*/);
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

        /*
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
        */

        /*
        auto metadata = this->xScaleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }

        metadata = this->yScaleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }

        metadata = this->zScaleSceneNode.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            metadata->SetPickingCollisionShapeToBox(halfExtents, offset);
        }
        */



        // Now for the circles. We use a torus for these. We'll use the Y scale for this, but since the scale is uniform, shouldn't really matter what we use.
        float yScale      = this->GetScale().y;
        float outerRadius = 1.0f * yScale;              // 1.0 is the radius of the circles.
        float innerRadius = 0.1f * yScale;              // This is the radius of the geometry making up the ring.

        auto metadata = this->xCircleSceneNode.GetComponent<EditorMetadataComponent>();
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
        // NOTE:
        //
        // We are changing the index data here, but it is safe in this case because the data will never be greater than it's initial allocation. Thus, the
        // vertex array will not be doing any deletions. Typically one should be aware that the rendering thread may be using the data, but in this case
        // since we will not be forcing a reallocation we can get away with it.


        assert(vertexArray != nullptr);
        {
            size_t vertexCount = vertexArray->GetVertexCount();

            auto circlePosition  = circleNode.GetWorldPosition();
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



    ////////////////////////////////////////////////////
    // HandleMesh

    TransformGizmo::Handle::Handle(HandleType typeIn, uint32_t axisIn)
        : type(typeIn), axis(axisIn), material(MaterialLibrary::Create("engine/materials/simple-emissive.material")), baseColour(), pickingObject()
    {
        if ((axisIn & HandleAxis_X))
        {
            this->baseColour.x = 1.0f;
        }
        if ((axisIn & HandleAxis_Y))
        {
            this->baseColour.y = 1.0f;
        }
        if ((axisIn & HandleAxis_Z))
        {
            this->baseColour.z = 1.0f;
        }
        this->SetColour(this->baseColour);


        // We assign a pointer to 'this' as the user pointer in the picking object. This will allow editting tools to retrieve the handle so it
        // know what to do.
        this->pickingObject.setUserPointer(this);
    }

    TransformGizmo::Handle::~Handle()
    {
        MaterialLibrary::Delete(this->material);
    }


    void TransformGizmo::Handle::SetColour(const glm::vec3 &colour)
    {
        this->material->SetParameter("EmissiveColour", colour);
    }

    void TransformGizmo::Handle::RestoreBaseColour()
    {
        this->SetColour(this->baseColour);
    }




    ////////////////////////////////////////////////////
    // TranslateHandle

    TransformGizmo::TranslateHandle::TranslateHandle(HandleAxis axis)
        : Handle(HandleType_Translate, axis), localOrientation(), headMesh(), lineMesh(), pickingShape()
    {
        switch (axis)
        {
        case HandleAxis_X: this->localOrientation = glm::mat3_cast(glm::angleAxis(-90.0f,  glm::vec3(0.0f, 1.0f, 0.0f))); break;
        case HandleAxis_Y: this->localOrientation = glm::mat3_cast(glm::angleAxis( 90.0f,  glm::vec3(1.0f, 0.0f, 0.0f))); break;
        case HandleAxis_Z: this->localOrientation = glm::mat3_cast(glm::angleAxis( 180.0f, glm::vec3(1.0f, 0.0f, 0.0f))); break;

        case HandleAxis_FrontFacing:
        default:
            {
                assert(false);
                break;
            }
        }



        this->headMesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->lineMesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);


        // Head.
        float        arrowHeadLength       = 0.3f;
        float        arrowHeadRadius       = 0.075f;
        unsigned int arrowHeadSegments     = 16;
        float        arrowHeadSegmentAngle = glm::radians(360.0f / static_cast<float>(arrowHeadSegments));

        GTCore::Vector<glm::vec3>    arrowHeadVertices;
        GTCore::Vector<unsigned int> arrowHeadIndices;

        arrowHeadVertices.PushBack(this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f));                       // First vertex is the point.
        arrowHeadVertices.PushBack(this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f + arrowHeadLength));     // Second vertex is the center of the base.

        // Now we do the base vertices. There will be arrowHeadSegments vertices on the base.
        for (unsigned int i = 0; i < arrowHeadSegments; ++i)
        {
            glm::vec3 position;

            position.x = std::cos(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.y = std::sin(arrowHeadSegmentAngle * i) * arrowHeadRadius;
            position.z = -1.0f + arrowHeadLength;

            arrowHeadVertices.PushBack(this->localOrientation * position);
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

        // Now we just set the data in the vertex array.
        this->headMesh.vertexArray->SetData(&arrowHeadVertices[0].x, arrowHeadVertices.count, &arrowHeadIndices[0], arrowHeadIndices.count);


        // Line.
        glm::vec3 arrowLineVertices[2] =
        {
            glm::vec3(this->localOrientation * glm::vec3(0.0f, 0.0f,  0.0f)),
            glm::vec3(this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f))
        };
        unsigned int arrowLineIndices[2] = {0, 1};

        this->lineMesh.vertexArray->SetData(&arrowLineVertices[0].x, 2, arrowLineIndices, 2);
        this->lineMesh.drawMode = DrawMode_Lines;


        // Need to draw this on top of everything else.
        this->headMesh.flags = this->lineMesh.flags = SceneRendererMesh::DrawLast | SceneRendererMesh::NoDepthTest | SceneRendererMesh::NoDepthWrite;




        // Materials.
        this->headMesh.material = this->material;
        this->lineMesh.material = this->material;




        // Picking.
        this->pickingShape.addChildShape(btTransform::getIdentity(), new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
        this->pickingObject.setCollisionShape(&this->pickingShape);
    }

    TransformGizmo::TranslateHandle::~TranslateHandle()
    {
        Renderer2::DeleteVertexArray(this->headMesh.vertexArray);
        Renderer2::DeleteVertexArray(this->lineMesh.vertexArray);
    }


    glm::vec3 TransformGizmo::TranslateHandle::GetForwardVector() const
    {
        return this->forwardVector;
    }


    void TransformGizmo::TranslateHandle::UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale)
    {
        // Updates mesh transform.
        glm::mat4 transform;
        Math::CalculateTransformMatrix(position, orientation, scale, transform);

        this->headMesh.transform = transform;
        this->lineMesh.transform = transform;



        // Update forward vector.
        this->forwardVector = glm::normalize(orientation * (this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f)));



        // Update picking volumes.
        auto pickingWorld = this->pickingObject.GetWorld();
        if (pickingWorld != nullptr)
        {
            pickingWorld->RemoveCollisionObject(this->pickingObject);
        }


        // Scale.
        assert(this->pickingShape.getNumChildShapes() == 1);
        {
            btVector3 halfExtents;
            halfExtents.setX(0.075f * scale.x);
            halfExtents.setY(0.075f * scale.y);
            halfExtents.setZ(0.5f   * scale.z);


            auto boxShape = static_cast<btBoxShape*>(this->pickingShape.getChildShape(0));
            assert(boxShape != nullptr);
            {
                boxShape->setImplicitShapeDimensions(halfExtents);
            }


            btMatrix3x3 basis;
            basis.setFromOpenGLSubMatrix(&glm::mat4(this->localOrientation)[0][0]);

            btVector3 offset;
            offset.setX(0.0f);
            offset.setY(0.0f);
            offset.setZ(-halfExtents.z());
            offset = basis * offset;

            // The box shape needs to have an offset applied.
            btTransform transform;
            transform.setBasis(basis);
            transform.setOrigin(offset);

            this->pickingShape.updateChildTransform(0, transform);
        }


        // Position/Rotation.
        btTransform bulletTransform;
        Math::CalculateTransformMatrix(position, orientation, bulletTransform);
        this->pickingObject.setWorldTransform(bulletTransform);

        if (pickingWorld != nullptr)
        {
            pickingWorld->AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
        }
    }


    void TransformGizmo::TranslateHandle::Show(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.AddExternalMesh(this->headMesh);
        renderer.AddExternalMesh(this->lineMesh);

        pickingWorld.AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
    }

    void TransformGizmo::TranslateHandle::Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.RemoveExternalMesh(this->headMesh);
        renderer.RemoveExternalMesh(this->lineMesh);

        pickingWorld.RemoveCollisionObject(this->pickingObject);
    }



    ////////////////////////////////////////////////////
    // ScaleHandle

    TransformGizmo::ScaleHandle::ScaleHandle(HandleAxis axis)
        : Handle(HandleType_Scale, axis), localOrientation(), headMesh(), lineMesh(), pickingShape()
    {
        switch (axis)
        {
        case HandleAxis_X: this->localOrientation = glm::mat3_cast(glm::angleAxis(-90.0f,  glm::vec3(0.0f, 1.0f, 0.0f))); break;
        case HandleAxis_Y: this->localOrientation = glm::mat3_cast(glm::angleAxis( 90.0f,  glm::vec3(1.0f, 0.0f, 0.0f))); break;
        case HandleAxis_Z: this->localOrientation = glm::mat3_cast(glm::angleAxis( 180.0f, glm::vec3(1.0f, 0.0f, 0.0f))); break;

        case HandleAxis_FrontFacing:
        default:
            {
                assert(false);
                break;
            }
        }



        this->headMesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->lineMesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);



        // Scale head.
        float scaleHeadHalfExtent = 0.075f;

        glm::vec3 scaleHeadVertices[8] =
        {
            // Front face (facing +Z)
            this->localOrientation * glm::vec3(-scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            this->localOrientation * glm::vec3(+scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            this->localOrientation * glm::vec3(+scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),
            this->localOrientation * glm::vec3(-scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f + scaleHeadHalfExtent * 2.0f),

            // Back face (facing -Z)
            this->localOrientation * glm::vec3(+scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f),
            this->localOrientation * glm::vec3(-scaleHeadHalfExtent, -scaleHeadHalfExtent, -1.0f),
            this->localOrientation * glm::vec3(-scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f),
            this->localOrientation * glm::vec3(+scaleHeadHalfExtent, +scaleHeadHalfExtent, -1.0f)
        };

        unsigned int scaleHeadIndices[36] =
        {
            // +Z
            0, 1, 2,
            2, 3, 0,

            // -Z
            4, 5, 6,
            6, 7, 4,

            // +Y
            3, 2, 7,
            7, 6, 3,

            // -Y
            5, 4, 1,
            1, 0, 5,

            // +X
            1, 4, 7,
            7, 2, 1,

            // -X
            5, 0, 3,
            3, 6, 5,
        };

        this->headMesh.vertexArray->SetData(&scaleHeadVertices[0].x, 8, scaleHeadIndices, 36);


        // Scale line.
        glm::vec3 scaleLineVertices[2] =
        {
            this->localOrientation * glm::vec3(0.0f, 0.0f,  0.0f),
            this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f)
        };
        unsigned int scaleLineIndices[2] = {0, 1};

        this->lineMesh.vertexArray->SetData(&scaleLineVertices[0].x, 2, scaleLineIndices, 2);
        this->lineMesh.drawMode = DrawMode_Lines;


        // Need to draw this on top of everything else.
        this->headMesh.flags = this->lineMesh.flags = SceneRendererMesh::DrawLast | SceneRendererMesh::NoDepthTest | SceneRendererMesh::NoDepthWrite;




        // Materials.
        this->headMesh.material = this->material;
        this->lineMesh.material = this->material;




        // Picking.
        this->pickingShape.addChildShape(btTransform::getIdentity(), new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
        this->pickingObject.setCollisionShape(&this->pickingShape);
    }

    TransformGizmo::ScaleHandle::~ScaleHandle()
    {
        Renderer2::DeleteVertexArray(this->headMesh.vertexArray);
        Renderer2::DeleteVertexArray(this->lineMesh.vertexArray);
    }


    glm::vec3 TransformGizmo::ScaleHandle::GetForwardVector() const
    {
        return this->forwardVector;
    }


    void TransformGizmo::ScaleHandle::UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale)
    {
        // Updates mesh transform.
        glm::mat4 transform;
        Math::CalculateTransformMatrix(position, orientation, scale, transform);

        this->headMesh.transform = transform;
        this->lineMesh.transform = transform;



        // Update forward vector.
        this->forwardVector = glm::normalize(orientation * (this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f)));



        // Update picking volumes.
        auto pickingWorld = this->pickingObject.GetWorld();
        if (pickingWorld != nullptr)
        {
            pickingWorld->RemoveCollisionObject(this->pickingObject);
        }


        // Scale.
        assert(this->pickingShape.getNumChildShapes() == 1);
        {
            btVector3 halfExtents;
            halfExtents.setX(0.075f * scale.x);
            halfExtents.setY(0.075f * scale.y);
            halfExtents.setZ(0.5f   * scale.z);


            auto boxShape = static_cast<btBoxShape*>(this->pickingShape.getChildShape(0));
            assert(boxShape != nullptr);
            {
                boxShape->setImplicitShapeDimensions(halfExtents);
            }


            btMatrix3x3 basis;
            basis.setFromOpenGLSubMatrix(&glm::mat4(this->localOrientation)[0][0]);

            btVector3 offset;
            offset.setX(0.0f);
            offset.setY(0.0f);
            offset.setZ(-halfExtents.z());
            offset = basis * offset;

            // The box shape needs to have an offset applied.
            btTransform transform;
            transform.setBasis(basis);
            transform.setOrigin(offset);

            this->pickingShape.updateChildTransform(0, transform);
        }


        // Position/Rotation.
        btTransform bulletTransform;
        Math::CalculateTransformMatrix(position, orientation, bulletTransform);
        this->pickingObject.setWorldTransform(bulletTransform);

        if (pickingWorld != nullptr)
        {
            pickingWorld->AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
        }
    }


    void TransformGizmo::ScaleHandle::Show(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.AddExternalMesh(this->headMesh);
        renderer.AddExternalMesh(this->lineMesh);

        pickingWorld.AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
    }

    void TransformGizmo::ScaleHandle::Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.RemoveExternalMesh(this->headMesh);
        renderer.RemoveExternalMesh(this->lineMesh);

        pickingWorld.RemoveCollisionObject(this->pickingObject);
    }
}
