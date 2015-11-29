// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_VisibilityProcessor.hpp>
#include <GTEngine/Scene.hpp>

namespace GT
{
    static const glm::vec3 HighlightColour      = glm::vec3(1.0f, 0.66f, 0.33f);
    static const glm::vec3 ChildHighlightColour = glm::vec3(1.0f, 0.8f, 0.6f);

    DefaultSceneRenderer_VisibilityProcessor::DefaultSceneRenderer_VisibilityProcessor(Scene &sceneIn, SceneViewport &viewportIn)
        : scene(sceneIn),
          opaqueObjects(), transparentObjects(), opaqueObjectsLast(), transparentObjectsLast(),
          lightManager(),
          visibleMeshes(),
          visibleModels(), modelsToAnimate(),
          visibleParticleSystems(),
          allLights(),
          projectionMatrix(), viewMatrix(), projectionViewMatrix()
    {
        auto cameraNode = viewportIn.GetCameraNode();
        if (cameraNode != nullptr)
        {
            auto camera = cameraNode->GetComponent<CameraComponent>();
            if (camera != nullptr)
            {
                this->projectionMatrix     = camera->GetProjectionMatrix();
                this->viewMatrix           = camera->GetViewMatrix();
                this->projectionViewMatrix = projectionMatrix * viewMatrix;
            }
        }
    }

    DefaultSceneRenderer_VisibilityProcessor::~DefaultSceneRenderer_VisibilityProcessor()
    {
        for (size_t i = 0; i < this->opaqueObjects.count; ++i)
        {
            delete this->opaqueObjects.buffer[i]->value;
        }

        for (size_t i = 0; i < this->opaqueObjectsLast.count; ++i)
        {
            delete this->opaqueObjectsLast.buffer[i]->value;
        }



        for (size_t i = 0; i < this->visibleMeshes.count; ++i)
        {
            delete this->visibleMeshes.buffer[i]->value;
        }


        for (size_t i = 0; i < this->visibleModels.count; ++i)
        {
            delete this->visibleModels.buffer[i]->value;
        }

        for (size_t i = 0; i < this->visibleParticleSystems.count; ++i)
        {
            delete this->visibleParticleSystems.buffer[i]->value;
        }
    }


    void DefaultSceneRenderer_VisibilityProcessor::ProcessModel(const SceneNode &sceneNode)
    {
        auto modelComponent = sceneNode.GetComponent<ModelComponent>();
        assert(modelComponent != nullptr);
        {
            if (!this->visibleModels.Exists(modelComponent))
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)
                {
                    this->visibleModels.Add(modelComponent, new DefaultSceneRenderer_LightGroup);

                    // If the model is animated, we don't want to mark it as such, but not actually apply the animation yet.
                    if (model->IsAnimating() && !model->IsAnimationPaused())
                    {
                        this->modelsToAnimate.PushBack(modelComponent);
                    }


                    for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                    {
                        auto mesh = model->meshes[iMesh];
                        assert(mesh != nullptr);
                        {
                            this->visibleMeshes.Add(mesh, new DefaultSceneRendererMesh);
                        }
                    }
                }
            }
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::ProcessPointLight(const SceneNode &sceneNode)
    {
        auto lightComponent = sceneNode.GetComponent<PointLightComponent>();
        assert(lightComponent != nullptr);
        {
            this->lightManager.AddPointLight(lightComponent);
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::ProcessSpotLight(const SceneNode &sceneNode)
    {
        auto lightComponent = sceneNode.GetComponent<SpotLightComponent>();
        assert(lightComponent != nullptr);
        {
            this->lightManager.AddSpotLight(lightComponent);
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::ProcessAmbientLight(const SceneNode &sceneNode)
    {
        auto lightComponent = sceneNode.GetComponent<AmbientLightComponent>();
        assert(lightComponent != nullptr);
        {
            this->lightManager.AddAmbientLight(lightComponent);
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::ProcessDirectionalLight(const SceneNode &sceneNode)
    {
        auto lightComponent = sceneNode.GetComponent<DirectionalLightComponent>();
        assert(lightComponent != nullptr);
        {
            this->lightManager.AddDirectionalLight(lightComponent);
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::ProcessParticleSystem(const SceneNode &sceneNode)
    {
        auto particleSystemComponent = sceneNode.GetComponent<ParticleSystemComponent>();
        assert(particleSystemComponent != nullptr);
        {
            if (!this->visibleParticleSystems.Exists(particleSystemComponent))
            {
                this->visibleParticleSystems.Add(particleSystemComponent, new DefaultSceneRenderer_LightGroup);
            }
        }
    }



    void DefaultSceneRenderer_VisibilityProcessor::AddMesh(const Mesh &mesh, const glm::mat4 &transform, const DefaultSceneRenderer_LightGroup* lights, bool drawHighlight, const glm::vec3 &highlightColour)
    {
        // TODO: Consider ways to remove these const_casts. Don't want to make the pointers in SceneRendererMesh constant because
        //       the user of that structure probably won't want a constant pointer.

        DefaultSceneRendererMesh object;
        object.vertexArray    = const_cast<VertexArray*>(mesh.GetSkinnedGeometry());
        object.drawMode       = mesh.GetDrawMode();
        object.material       = const_cast<Material*>(mesh.GetMaterial());
        object.transform      = transform;
        object.touchingLights = lights;

        if (drawHighlight)
        {
            object.flags |= SceneRendererMesh::DrawHighlight;
            object.highlightColour = highlightColour;
        }

        this->AddMesh(object);
    }

    void DefaultSceneRenderer_VisibilityProcessor::AddMesh(const DefaultSceneRendererMesh &mesh)
    {
        if (mesh.material != nullptr)
        {
            auto &materialDefinition = mesh.material->GetDefinition();
            {
                Vector<DefaultSceneRendererMesh>* objectList = nullptr;

                if (mesh.material->IsBlended() || mesh.material->IsRefractive())
                {
                    if (!(mesh.flags & SceneRendererMesh::DrawLast))
                    {
                        objectList = &this->transparentObjects;
                    }
                    else
                    {
                        objectList = &this->transparentObjectsLast;
                    }
                }
                else
                {
                    if (!(mesh.flags & SceneRendererMesh::DrawLast))
                    {
                        auto iObjectList = this->opaqueObjects.Find(&materialDefinition);
                        if (iObjectList != nullptr)
                        {
                            objectList = iObjectList->value;
                        }
                        else
                        {
                            objectList = new Vector<DefaultSceneRendererMesh>(100);
                            this->opaqueObjects.Add(&materialDefinition, objectList);
                        }
                    }
                    else
                    {
                        auto iObjectList = this->opaqueObjectsLast.Find(&materialDefinition);
                        if (iObjectList != nullptr)
                        {
                            objectList = iObjectList->value;
                        }
                        else
                        {
                            objectList = new Vector<DefaultSceneRendererMesh>(100);
                            this->opaqueObjectsLast.Add(&materialDefinition, objectList);
                        }
                    }
                }

                assert(objectList != nullptr);
                {
                    objectList->PushBack(mesh);
                }
            }
        }
    }

    void DefaultSceneRenderer_VisibilityProcessor::PostProcess()
    {
        // We need to now query the objects contained inside the volumes of the point and spot lights.
        const auto &cullingManager = scene.GetCullingManager();
        {
            // Point Lights.
            for (size_t iPointLight = 0; iPointLight < this->lightManager.pointLights.count; ++iPointLight)
            {
                auto lightComponent = this->lightManager.pointLights.buffer[iPointLight]->key;
                auto light          = this->lightManager.pointLights.buffer[iPointLight]->value;

                assert(lightComponent != nullptr);
                assert(light          != nullptr);
                {
                    PointLightContactsCallback callback(*this, iPointLight, light->IsShadowCasting());
                    cullingManager.QueryPointLightContacts(lightComponent->GetNode(), callback);
                }
            }


            // Spot Lights.
            for (size_t iSpotLight = 0; iSpotLight < this->lightManager.spotLights.count; ++iSpotLight)
            {
                auto lightComponent = this->lightManager.spotLights.buffer[iSpotLight]->key;
                auto light          = this->lightManager.spotLights.buffer[iSpotLight]->value;

                assert(lightComponent != nullptr);
                assert(light          != nullptr);
                {
                    SpotLightContactsCallback callback(*this, iSpotLight, light->IsShadowCasting());
                    cullingManager.QuerySpotLightContacts(lightComponent->GetNode(), callback);
                }
            }
        }


        // We need to now determines which lights are touching which meshes. Ambient and directional lights are easy, but point and
        // spot lights need to be determined by the scene.
        for (size_t iModel = 0; iModel < this->visibleModels.count; ++iModel)
        {
            auto modelLights = this->visibleModels.buffer[iModel]->value;
            assert(modelLights != nullptr);
            {
                for (size_t iAmbientLight = 0; iAmbientLight < this->lightManager.ambientLights.count; ++iAmbientLight)
                {
                    modelLights->AddAmbientLight(static_cast<uint32_t>(iAmbientLight));
                }


                // Directional lights.
                for (size_t iDirectionalLight = 0; iDirectionalLight < this->lightManager.directionalLights.count; ++iDirectionalLight)
                {
                    auto light = this->lightManager.directionalLights.buffer[iDirectionalLight]->value;
                    assert(light != nullptr);
                    {
                        if (light->IsShadowCasting())
                        {
                            modelLights->AddShadowDirectionalLight(static_cast<uint32_t>(iDirectionalLight));
                        }
                        else
                        {
                            modelLights->AddDirectionalLight(static_cast<uint32_t>(iDirectionalLight));
                        }
                    }
                }
            }
        }

        // As above, but now with particle systems.
        for (size_t iParticleSystem = 0; iParticleSystem < this->visibleParticleSystems.count; ++iParticleSystem)
        {
            auto particleSystemLights = this->visibleParticleSystems.buffer[iParticleSystem]->value;
            assert(particleSystemLights != nullptr);
            {
                for (size_t iAmbientLight = 0; iAmbientLight < this->lightManager.ambientLights.count; ++iAmbientLight)
                {
                    particleSystemLights->AddAmbientLight(static_cast<uint32_t>(iAmbientLight));
                }


                // Directional lights.
                for (size_t iDirectionalLight = 0; iDirectionalLight < this->lightManager.directionalLights.count; ++iDirectionalLight)
                {
                    auto light = this->lightManager.directionalLights.buffer[iDirectionalLight]->value;
                    assert(light != nullptr);
                    {
                        if (light->IsShadowCasting())
                        {
                            particleSystemLights->AddShadowDirectionalLight(static_cast<uint32_t>(iDirectionalLight));
                        }
                        else
                        {
                            particleSystemLights->AddDirectionalLight(static_cast<uint32_t>(iDirectionalLight));
                        }
                    }
                }
            }
        }



        


        // We now need to create the mesh objects that will be drawn. These are created from the model components.
        for (size_t iModel = 0; iModel < this->visibleModels.count; ++iModel)
        {
            auto modelComponent = this->visibleModels.buffer[iModel]->key;
            auto modelLights    = this->visibleModels.buffer[iModel]->value;

            assert(modelComponent != nullptr);
            assert(modelLights    != nullptr);
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)                           // <-- Is allowed to be null. Perhaps due to a bad path?
                {
                    for (size_t i = 0; i < model->meshes.count; ++i)
                    {
                        auto mesh = model->meshes[i];
                        assert(mesh != nullptr);
                        {
                            // If the mesh needs to be animated we don't want to add the mesh straight away. Instead we want to wait until
                            // after it's been animated. If it's not animated, we just add it straight away.
                            if (model->IsAnimating() && !model->IsAnimationPaused())
                            {
                                if (!this->modelsToAnimate.Exists(modelComponent))
                                {
                                    this->modelsToAnimate.PushBack(modelComponent);
                                }
                            }
                            else
                            {
                                /// If the node has an editor component and is selected, we'll draw a highlight.
                                bool      drawHighlight   = false;
                                glm::vec3 highlightColour = HighlightColour;

                                auto editorMetadata = modelComponent->GetNode().GetComponent<EditorMetadataComponent>();
                                if (editorMetadata != nullptr)
                                {
                                    if (editorMetadata->IsSelected())
                                    {
                                        drawHighlight = true;
                                    }
                                    else if (editorMetadata->IsAncestorSelected())
                                    {
                                        drawHighlight   = true;
                                        highlightColour = ChildHighlightColour;
                                    }
                                }

                                this->AddMesh(*mesh, modelComponent->GetNode().GetWorldTransform(), modelLights, drawHighlight, highlightColour);
                            }
                        }
                    }
                }
            }
        }



        // Now we need to build the mesh to draw for each particle system.
        glm::quat inverseView = glm::quat(glm::inverse(glm::quat_cast(this->viewMatrix)));

        for (size_t iParticleSystem = 0; iParticleSystem < this->visibleParticleSystems.count; ++iParticleSystem)
        {
            auto particleSystemComponent = this->visibleParticleSystems.buffer[iParticleSystem]->key;
            auto particleSystemLights    = this->visibleParticleSystems.buffer[iParticleSystem]->value;

            assert(particleSystemComponent != nullptr);
            assert(particleSystemLights    != nullptr);
            {
                auto particleSystem = particleSystemComponent->GetParticleSystem();
                if (particleSystem != nullptr)
                {
                    for (size_t iEmitter = 0; iEmitter < particleSystem->GetEmitterCount(); ++iEmitter)
                    {
                        auto emitter = particleSystem->GetEmitter(iEmitter);
                        assert(emitter != nullptr);
                        {
                            auto &vertexArray = emitter->GetVertexArray();
                            {
                                size_t particleCount = emitter->GetParticleCount();

                                if (particleCount > 0)
                                {
                                    // Pre-allocate.
                                    vertexArray.SetVertexData(nullptr, particleCount * 4);
                                    vertexArray.SetIndexData( nullptr, particleCount * 6);


                                    // Build the data.
                                    auto vertices = vertexArray.MapVertexData();
                                    auto indices  = vertexArray.MapIndexData();
                                
                                    assert(vertices != nullptr);
                                    assert(indices  != nullptr);
                                    {
                                        for (size_t iParticle = 0; iParticle < particleCount; ++iParticle)
                                        {
                                            auto &particle = emitter->GetParticle(iParticle);
                                            {
                                                // Vertices.
                                                size_t vertexSize = vertexArray.GetFormat().GetSize();

                                                auto vertex0 = vertices + (iParticle * 4 * vertexSize);
                                                auto vertex1 = vertex0 + vertexSize;
                                                auto vertex2 = vertex1 + vertexSize;
                                                auto vertex3 = vertex2 + vertexSize;

                                                glm::quat absoluteOrientation = inverseView * particle.orientation;


                                                glm::mat4 transform = glm::mat4_cast(absoluteOrientation);
                                                transform[0] *= particle.scale.x;
                                                transform[1] *= particle.scale.y;
                                                transform[2] *= particle.scale.z;
                                                transform[3]  = particle.position;

                                                glm::vec4 position0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
                                                glm::vec4 position1 = transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
                                                glm::vec4 position2 = transform * glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
                                                glm::vec4 position3 = transform * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

                                                glm::vec4 texcoord0     = glm::vec4(particle.uTexCoordMin0, particle.vTexCoordMin0, particle.uTexCoordMin1, particle.vTexCoordMin1);
                                                glm::vec4 texcoord1     = glm::vec4(particle.uTexCoordMax0, particle.vTexCoordMin0, particle.uTexCoordMax1, particle.vTexCoordMin1);
                                                glm::vec4 texcoord2     = glm::vec4(particle.uTexCoordMax0, particle.vTexCoordMax0, particle.uTexCoordMax1, particle.vTexCoordMax1);
                                                glm::vec4 texcoord3     = glm::vec4(particle.uTexCoordMin0, particle.vTexCoordMax0, particle.uTexCoordMin1, particle.vTexCoordMax1);

                                                glm::vec4 normal0   = absoluteOrientation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); normal0.w = particle.uvTileInterpolationFactor;
                                                glm::vec4 normal1   = normal0;
                                                glm::vec4 normal2   = normal0;
                                                glm::vec4 normal3   = normal0;

                                                glm::vec4 colour0   = particle.colour;
                                                glm::vec4 colour1   = particle.colour;
                                                glm::vec4 colour2   = particle.colour;
                                                glm::vec4 colour3   = particle.colour;

                                                vertex0[0]  = position0.x; vertex0[1]  = position0.y; vertex0[2]  = position0.z; vertex0[3]  = 1.0f;
                                                vertex0[4]  = texcoord0.x; vertex0[5]  = texcoord0.y; vertex0[6]  = texcoord0.z; vertex0[7]  = texcoord0.w;
                                                vertex0[8]  = normal0.x;   vertex0[9]  = normal0.y;   vertex0[10] = normal0.z;   vertex0[11] = normal0.w;
                                                vertex0[12] = colour0.x;   vertex0[13] = colour0.y;   vertex0[14] = colour0.z;   vertex0[15] = colour0.w;

                                                vertex1[0]  = position1.x; vertex1[1]  = position1.y; vertex1[2]  = position1.z; vertex1[3]  = 1.1f;
                                                vertex1[4]  = texcoord1.x; vertex1[5]  = texcoord1.y; vertex1[6]  = texcoord1.z; vertex1[7]  = texcoord1.w;
                                                vertex1[8]  = normal1.x;   vertex1[9]  = normal1.y;   vertex1[10] = normal1.z;   vertex1[11] = normal1.w;
                                                vertex1[12] = colour1.x;   vertex1[13] = colour1.y;   vertex1[14] = colour1.z;   vertex1[15] = colour1.w;

                                                vertex2[0]  = position2.x; vertex2[1]  = position2.y; vertex2[2]  = position2.z; vertex2[3]  = 1.2f;
                                                vertex2[4]  = texcoord2.x; vertex2[5]  = texcoord2.y; vertex2[6]  = texcoord2.z; vertex2[7]  = texcoord2.w;
                                                vertex2[8]  = normal2.x;   vertex2[9]  = normal2.y;   vertex2[10] = normal2.z;   vertex2[11] = normal2.w;
                                                vertex2[12] = colour2.x;   vertex2[13] = colour2.y;   vertex2[14] = colour2.z;   vertex2[15] = colour2.w;

                                                vertex3[0]  = position3.x; vertex3[1]  = position3.y; vertex3[2]  = position3.z; vertex3[3]  = 1.3f;
                                                vertex3[4]  = texcoord3.x; vertex3[5]  = texcoord3.y; vertex3[6]  = texcoord3.z; vertex3[7]  = texcoord3.w;
                                                vertex3[8]  = normal3.x;   vertex3[9]  = normal3.y;   vertex3[10] = normal3.z;   vertex3[11] = normal3.w;
                                                vertex3[12] = colour3.x;   vertex3[13] = colour3.y;   vertex3[14] = colour3.z;   vertex3[15] = colour3.w;



                                                // Indices.
                                                indices[(iParticle * 6) + 0] = (iParticle * 4) + 0;
                                                indices[(iParticle * 6) + 1] = (iParticle * 4) + 1;
                                                indices[(iParticle * 6) + 2] = (iParticle * 4) + 2;
                                                indices[(iParticle * 6) + 3] = (iParticle * 4) + 2;
                                                indices[(iParticle * 6) + 4] = (iParticle * 4) + 3;
                                                indices[(iParticle * 6) + 5] = (iParticle * 4) + 0;
                                            }
                                        }
                                    }
                                
                                    vertexArray.UnmapVertexData();
                                    vertexArray.UnmapIndexData();
                                


                                    DefaultSceneRendererMesh mesh;
                                    mesh.vertexArray    = &vertexArray;
                                    mesh.drawMode       = DrawMode_Triangles;
                                    mesh.material       = emitter->GetMaterial();
                                    mesh.transform      = glm::mat4();
                                    mesh.touchingLights = particleSystemLights;
                                    mesh.flags          = SceneRendererMesh::NoNormalMapping;   // No normal mapping on particles.
                                    this->AddMesh(mesh);
                                }
                            }
                        }
                    }
                }
            }
        }




        // We need to retrieve the visible objects of each shadow casting light.

        // Shadow-Casting Directional Lights.
        for (size_t i = 0; i < this->lightManager.shadowDirectionalLights.count; ++i)
        {
            auto iLight = this->lightManager.directionalLights.Find(this->lightManager.shadowDirectionalLights[i]);
            if (iLight != nullptr)
            {
                auto light = iLight->value;
                assert(light != nullptr);
                {
                    scene.QueryVisibleSceneNodes(light->projection * light->view, light->containedMeshes);
                }
            }
        }


        // Shadow-Casting Point Lights.
        for (size_t i = 0; i < this->lightManager.shadowPointLights.count; ++i)
        {
            auto iLight = this->lightManager.pointLights.Find(this->lightManager.shadowPointLights[i]);
            if (iLight != nullptr)
            {
                auto light = iLight->value;
                assert(light != nullptr);
                {
                    // We need to do 6 queries here - one for each face.
                    scene.QueryVisibleSceneNodes(light->projection * light->positiveXView, light->containedMeshesPositiveX);
                    scene.QueryVisibleSceneNodes(light->projection * light->negativeXView, light->containedMeshesNegativeX);
                    scene.QueryVisibleSceneNodes(light->projection * light->positiveYView, light->containedMeshesPositiveY);
                    scene.QueryVisibleSceneNodes(light->projection * light->negativeYView, light->containedMeshesNegativeY);
                    scene.QueryVisibleSceneNodes(light->projection * light->positiveZView, light->containedMeshesPositiveZ);
                    scene.QueryVisibleSceneNodes(light->projection * light->negativeZView, light->containedMeshesNegativeZ);
                }
            }
        }


        // Shadow-Casting Spot Lights.
        for (size_t i = 0; i < this->lightManager.shadowSpotLights.count; ++i)
        {
            auto iLight = this->lightManager.spotLights.Find(this->lightManager.shadowSpotLights[i]);
            if (iLight != nullptr)
            {
                auto light = iLight->value;
                assert(light != nullptr);
                {
                    scene.QueryVisibleSceneNodes(light->projection * light->view, light->containedMeshes);
                }
            }
        }



        // Apply skinning.
        for (size_t i = 0; i < this->modelsToAnimate.count; ++i)
        {
            auto modelComponent = this->modelsToAnimate[i];
            assert(modelComponent != nullptr);
            {
                auto model = modelComponent->GetModel();
                assert(model != nullptr);
                {
                    for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                    {
                        auto mesh = model->meshes[iMesh];
                        assert(mesh != nullptr);
                        {
                            mesh->ApplySkinning();

                            // Now we now give the renderer the mesh so it can render it.
                            auto iModelLights = this->visibleModels.Find(modelComponent);
                            assert(iModelLights != nullptr);
                            {
                                /// If the node has an editor component and is selected, we'll draw a highlight.
                                bool      drawHighlight   = false;
                                glm::vec3 highlightColour = HighlightColour;

                                auto editorMetadata = modelComponent->GetNode().GetComponent<EditorMetadataComponent>();
                                if (editorMetadata != nullptr)
                                {
                                    if (editorMetadata->IsSelected())
                                    {
                                        drawHighlight = true;
                                    }
                                    else if (editorMetadata->IsAncestorSelected())
                                    {
                                        drawHighlight   = true;
                                        highlightColour = ChildHighlightColour;
                                    }
                                }

                                this->AddMesh(*mesh, modelComponent->GetNode().GetWorldTransform(), iModelLights->value, drawHighlight, highlightColour);
                            }
                        }
                    }
                }
            }
        }
    }
}
