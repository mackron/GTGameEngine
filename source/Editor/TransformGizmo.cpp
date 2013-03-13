// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/TransformGizmo.hpp>
#include <GTEngine/ModelLibrary.hpp>            // <-- Can delete this.
#include <GTEngine/MaterialLibrary.hpp>

namespace GTEngine
{
    TransformGizmo::TransformGizmo()
        : position(), orientation(), scale(1.0f, 1.0f, 1.0f),
          isVisible(false), showingTranslationHandles(false), showingRotationHandles(false), showingScaleHandles(false),
          xTranslateHandle(HandleAxis_X), yTranslateHandle(HandleAxis_Y), zTranslateHandle(HandleAxis_Z),
          xScaleHandle(HandleAxis_X),     yScaleHandle(HandleAxis_Y),     zScaleHandle(HandleAxis_Z),
          xRotateHandle(HandleAxis_X),    yRotateHandle(HandleAxis_Y),    zRotateHandle(HandleAxis_Z), cameraFacingRotateHandle(HandleAxis_FrontFacing)
    {
    }

    TransformGizmo::~TransformGizmo()
    {
    }


    void TransformGizmo::SetPosition(const glm::vec3 &newPosition, const SceneNode &cameraNode)
    {
        this->position = newPosition;
        this->UpdateHandleTransforms(cameraNode);
    }

    const glm::vec3 & TransformGizmo::GetPosition() const
    {
        return this->position;
    }


    void TransformGizmo::SetRotation(const glm::quat &rotation, const SceneNode &cameraNode)
    {
        this->orientation = rotation;
        this->UpdateHandleTransforms(cameraNode);
    }


    void TransformGizmo::SetScale(const glm::vec3 &newScale, const SceneNode &cameraNode)
    {
        this->scale = newScale;
        this->UpdateHandleTransforms(cameraNode);
    }

    const glm::vec3 & TransformGizmo::GetScale() const
    {
        return this->scale;
    }



    void TransformGizmo::UpdateHandleTransforms(const SceneNode &cameraNode)
    {
        glm::mat4 viewMatrix;
        if (cameraNode.HasComponent<CameraComponent>())
        {
            viewMatrix = cameraNode.GetComponent<CameraComponent>()->GetViewMatrix();
        }

        this->xTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->yTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->zTranslateHandle.UpdateTransform(this->position, this->orientation, this->scale);

        this->xRotateHandle.UpdateTransform(this->position, this->orientation, this->scale, viewMatrix);
        this->yRotateHandle.UpdateTransform(this->position, this->orientation, this->scale, viewMatrix);
        this->zRotateHandle.UpdateTransform(this->position, this->orientation, this->scale, viewMatrix);
        this->cameraFacingRotateHandle.UpdateTransform(this->position, this->orientation, this->scale, viewMatrix);

        this->xScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->yScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
        this->zScaleHandle.UpdateTransform(this->position, this->orientation, this->scale);
    }


    void TransformGizmo::Show(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->isVisible = true;

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
    }

    void TransformGizmo::Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->isVisible = false;

        this->xTranslateHandle.Hide(renderer, pickingWorld);
        this->yTranslateHandle.Hide(renderer, pickingWorld);
        this->zTranslateHandle.Hide(renderer, pickingWorld);

        this->xRotateHandle.Hide(renderer, pickingWorld);
        this->yRotateHandle.Hide(renderer, pickingWorld);
        this->zRotateHandle.Hide(renderer, pickingWorld);
        this->cameraFacingRotateHandle.Hide(renderer, pickingWorld);

        this->xScaleHandle.Hide(renderer, pickingWorld);
        this->yScaleHandle.Hide(renderer, pickingWorld);
        this->zScaleHandle.Hide(renderer, pickingWorld);

        // Important: Don't modify the 'this->showing*Handles' members.
    }


    void TransformGizmo::ShowTranslationHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        this->xTranslateHandle.Show(renderer, pickingWorld);
        this->yTranslateHandle.Show(renderer, pickingWorld);
        this->zTranslateHandle.Show(renderer, pickingWorld);

        this->xRotateHandle.Hide(renderer, pickingWorld);
        this->yRotateHandle.Hide(renderer, pickingWorld);
        this->zRotateHandle.Hide(renderer, pickingWorld);
        this->cameraFacingRotateHandle.Hide(renderer, pickingWorld);

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

        this->xRotateHandle.Show(renderer, pickingWorld);
        this->yRotateHandle.Show(renderer, pickingWorld);
        this->zRotateHandle.Show(renderer, pickingWorld);
        this->cameraFacingRotateHandle.Show(renderer, pickingWorld);

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

        this->xRotateHandle.Hide(renderer, pickingWorld);
        this->yRotateHandle.Hide(renderer, pickingWorld);
        this->zRotateHandle.Hide(renderer, pickingWorld);
        this->cameraFacingRotateHandle.Hide(renderer, pickingWorld);

        this->xScaleHandle.Show(renderer, pickingWorld);
        this->yScaleHandle.Show(renderer, pickingWorld);
        this->zScaleHandle.Show(renderer, pickingWorld);


        this->showingTranslationHandles = false;
        this->showingRotationHandles    = false;
        this->showingScaleHandles       = true;
    }


    void TransformGizmo::RestoreColours()
    {
        this->xTranslateHandle.RestoreBaseColour();
        this->yTranslateHandle.RestoreBaseColour();
        this->zTranslateHandle.RestoreBaseColour();

        this->xRotateHandle.RestoreBaseColour();
        this->yRotateHandle.RestoreBaseColour();
        this->zRotateHandle.RestoreBaseColour();
        this->cameraFacingRotateHandle.RestoreBaseColour();

        this->xScaleHandle.RestoreBaseColour();
        this->yScaleHandle.RestoreBaseColour();
        this->zScaleHandle.RestoreBaseColour();
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
        if ((axisIn & HandleAxis_FrontFacing))
        {
            this->baseColour = glm::vec3(1.0f, 1.0f, 1.0f);
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
        : Handle(HandleType_Translate, axis), localOrientation(), headMesh(), lineMesh(), pickingShape(), pickingShapeBox(btVector3(1.0f, 1.0f, 1.0f))
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
        this->pickingShape.addChildShape(btTransform::getIdentity(), &this->pickingShapeBox);
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


            // We set the shape dimensions explicitly because Bullet does weird things with scaling. In particular, if the scale is set to 0,0,0 the
            // next scaling operations won't work properly.
            this->pickingShapeBox.setImplicitShapeDimensions(halfExtents);


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
    // RotateHandle

    TransformGizmo::RotateHandle::RotateHandle(HandleAxis axis)
        : Handle(HandleType_Rotate, axis), localOrientation(), mesh(), pickingShape(), pickingShapeSegment(0.0f, 0.0f)
    {
        switch (axis)
        {
        case HandleAxis_X:           this->localOrientation = glm::mat3_cast(glm::angleAxis(-90.0f,  glm::vec3(0.0f, 1.0f, 0.0f))); break;
        case HandleAxis_Y:           this->localOrientation = glm::mat3_cast(glm::angleAxis( 90.0f,  glm::vec3(1.0f, 0.0f, 0.0f))); break;
        case HandleAxis_Z:           this->localOrientation = glm::mat3_cast(glm::angleAxis( 180.0f, glm::vec3(1.0f, 0.0f, 0.0f))); break;
        case HandleAxis_FrontFacing: break;

        default:
            {
                assert(false);
                break;
            }
        }



        // Mesh
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

            circleVertices.PushBack(this->localOrientation * position);
        }

        GTCore::Vector<unsigned int> circleIndices;
        for (unsigned int i = 0; i < circleSegmentCount; ++i)
        {
            circleIndices.PushBack(i);
            circleIndices.PushBack((i + 1) % circleSegmentCount);
        }

        this->mesh.vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3);
        this->mesh.vertexArray->SetData(&circleVertices[0].x, circleVertices.count, &circleIndices[0], circleIndices.count);
        this->mesh.drawMode = DrawMode_Lines;


        // Need to draw this on top of everything else.
        this->mesh.flags = SceneRendererMesh::DrawLast | SceneRendererMesh::NoDepthTest | SceneRendererMesh::NoDepthWrite;


        // Materials.
        this->mesh.material = this->material;



        // Picking.
        this->pickingObject.setCollisionShape(&this->pickingShape);
    }

    TransformGizmo::RotateHandle::~RotateHandle()
    {
        Renderer2::DeleteVertexArray(this->mesh.vertexArray);
    }


    glm::vec3 TransformGizmo::RotateHandle::GetForwardVector() const
    {
        return this->forwardVector;
    }


    void TransformGizmo::RotateHandle::UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, const glm::mat4 &viewMatrix)
    {
        // Updates mesh transform.
        glm::mat4 transform;
        Math::CalculateTransformMatrix(position, (this->axis & HandleAxis_FrontFacing) ? glm::quat_cast(glm::inverse(viewMatrix)) : orientation, scale, transform);

        this->mesh.transform = transform;



        // Updat the mesh geometry to hide the back-facing lines segments.
        assert(this->mesh.vertexArray != nullptr);
        {
            size_t vertexCount = this->mesh.vertexArray->GetVertexCount();

            auto circlePosition  = glm::vec3(viewMatrix * glm::vec4(position, 1.0f));
            auto circleTransform = transform;

            // All we do is start from the start and work our way around. If a line segment has both vertices facing away from the camera,
            // we'll ignore it and move on.
            GTCore::Vector<unsigned int> circleIndices;
            for (unsigned int i = 0; i < vertexCount; ++i)
            {
                size_t index0 = i;
                size_t index1 = (i + 1) % vertexCount;

                auto vertexPtr0 = reinterpret_cast<const glm::vec3*>(this->mesh.vertexArray->GetVertexDataPtr()) + index0;
                auto vertexPtr1 = reinterpret_cast<const glm::vec3*>(this->mesh.vertexArray->GetVertexDataPtr()) + index1;

                glm::vec3 vertex0(viewMatrix * circleTransform * glm::vec4(*vertexPtr0, 1.0f));
                glm::vec3 vertex1(viewMatrix * circleTransform * glm::vec4(*vertexPtr1, 1.0f));

                if (glm::vec3(circlePosition - vertex0).z <= 0.0f ||
                    glm::vec3(circlePosition - vertex1).z <= 0.0f)
                {
                    circleIndices.PushBack(index0);
                    circleIndices.PushBack(index1);
                }
            }


            if (circleIndices.count > 0)
            {
                this->mesh.vertexArray->SetIndexData(&circleIndices[0], circleIndices.count);
            }
        }



        // Update forward vector.
        this->forwardVector = glm::normalize(orientation * (this->localOrientation * glm::vec3(0.0f, 0.0f, -1.0f)));



        // Update picking volumes. Only do this for the non-forward-facing handles.
        if (!(this->axis & HandleAxis_FrontFacing))
        {
            auto pickingWorld = this->pickingObject.GetWorld();
            if (pickingWorld != nullptr)
            {
                pickingWorld->RemoveCollisionObject(this->pickingObject);
            }


            // Shapes are going to be completely recreated.
            while (this->pickingShape.getNumChildShapes() > 0)
            {
                auto shape = this->pickingShape.getChildShape(0);
                assert(shape != nullptr);
                {
                    this->pickingShape.removeChildShapeByIndex(0);
                }
            }

            float        outerRadius   = 1.0f * scale.y;              // 1.0 is the radius of the circles.
            float        innerRadius   = 0.1f * scale.y;              // This is the radius of the geometry making up the ring.
            unsigned int subdivisions  = 32;

            float        segmentLength = 2.0f * SIMD_PI * outerRadius / static_cast<float>(subdivisions);
            float        segmentAngle  = 2.0f * SIMD_PI               / static_cast<float>(subdivisions);

            this->pickingShapeSegment.~btCapsuleShape();
            new (&this->pickingShapeSegment) btCapsuleShape(innerRadius, segmentLength);

            btTransform segmentTransform;
            for (unsigned int i = 0; i < subdivisions; ++i)
            {
                float angle = segmentAngle * static_cast<float>(i);

                btVector3 position;
                position.setX(std::cos(angle) * outerRadius);
                position.setY(std::sin(angle) * outerRadius);
                position.setZ(0.0f);

                btQuaternion rotation(btVector3(0.0f, 0.0f, -1.0f), angle);

                segmentTransform.setIdentity();
                segmentTransform.setOrigin(position);
                segmentTransform.setRotation(rotation);
                this->pickingShape.addChildShape(segmentTransform, &this->pickingShapeSegment);
            }



            // Position/Rotation.
            btTransform bulletTransform;
            Math::CalculateTransformMatrix(position, orientation * glm::quat_cast(this->localOrientation), bulletTransform);
            this->pickingObject.setWorldTransform(bulletTransform);

            if (pickingWorld != nullptr)
            {
                pickingWorld->AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
            }
        }
    }


    void TransformGizmo::RotateHandle::Show(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.AddExternalMesh(this->mesh);

        // We don't want to add a picking object for the front-facing handles.
        if (!(this->axis & HandleAxis_FrontFacing))
        {
            pickingWorld.AddCollisionObject(this->pickingObject, CollisionGroups::EditorGizmo, CollisionGroups::EditorSelectionRay);
        }
    }

    void TransformGizmo::RotateHandle::Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld)
    {
        renderer.RemoveExternalMesh(this->mesh);

        // We didn't add a picking object for the front-facing handles.
        if (!(this->axis & HandleAxis_FrontFacing))
        {
            pickingWorld.RemoveCollisionObject(this->pickingObject);
        }
    }




    ////////////////////////////////////////////////////
    // ScaleHandle

    TransformGizmo::ScaleHandle::ScaleHandle(HandleAxis axis)
        : Handle(HandleType_Scale, axis), localOrientation(), headMesh(), lineMesh(), pickingShape(), pickingShapeBox(btVector3(1.0f, 1.0f, 1.0f))
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
        this->pickingShape.addChildShape(btTransform::getIdentity(), &this->pickingShapeBox);
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


            // We set the shape dimensions explicitly because Bullet does weird things with scaling. In particular, if the scale is set to 0,0,0 the
            // next scaling operations won't work properly.
            this->pickingShapeBox.setImplicitShapeDimensions(halfExtents);


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
