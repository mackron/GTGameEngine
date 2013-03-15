// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer2.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ///////////////////////////////////////////////////////////
    // DefaultSceneRendererShadowObjects

    DefaultSceneRendererShadowObjects::DefaultSceneRendererShadowObjects()
        : meshes()
    {
    }

    DefaultSceneRendererShadowObjects::~DefaultSceneRendererShadowObjects()
    {
    }

    void DefaultSceneRendererShadowObjects::ProcessObjectModel(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    if (modelComponent->IsShadowCastingEnabled())       // <-- Ignore the model if it is not casting shadows.
                    {
                        auto model = modelComponent->GetModel();
                        if (model != nullptr)                           // <-- Is allowed to be null. Perhaps due to a bad path?
                        {
                            for (size_t i = 0; i < model->meshes.count; ++i)
                            {
                                auto mesh = model->meshes[i];
                                assert(mesh != nullptr);
                                {
                                    SceneRendererMesh object;
                                    object.vertexArray = mesh->GetSkinnedGeometry();
                                    object.drawMode    = mesh->GetDrawMode();
                                    object.material    = mesh->GetMaterial();
                                    object.transform   = sceneNode.GetWorldTransform();
                                    this->meshes.PushBack(object);
                                }
                            }
                        }
                    }
                }
            }
        }
    }




    ///////////////////////////////////////////////////////////
    // DefaultSceneRendererVisibleObjects

    DefaultSceneRendererVisibleObjects::DefaultSceneRendererVisibleObjects(Scene &sceneIn, SceneViewport &viewportIn)
        : scene(sceneIn),
          opaqueObjects(),     alphaTransparentObjects(),     refractiveTransparentObjects(),
          opaqueObjectsLast(), alphaTransparentObjectsLast(), refractiveTransparentObjectsLast(),
          ambientLights(), directionalLights(), pointLights(), spotLights(),
          shadowDirectionalLights(), shadowPointLights(), shadowSpotLights(),
          meshesToAnimate(),
          projectionMatrix(), viewMatrix(), projectionViewMatrix()
    {
        auto cameraNode = viewportIn.GetCameraNode();
        assert(cameraNode != nullptr);
        {
            auto camera = cameraNode->GetComponent<CameraComponent>();
            assert(camera != nullptr);
            {
                this->projectionMatrix     = camera->GetProjectionMatrix();
                this->viewMatrix           = camera->GetViewMatrix();
                this->projectionViewMatrix = projectionMatrix * viewMatrix;
            }
        }
    }

    DefaultSceneRendererVisibleObjects::~DefaultSceneRendererVisibleObjects()
    {
        for (size_t i = 0; i < this->opaqueObjects.count; ++i)
        {
            delete this->opaqueObjects.buffer[i]->value;
        }

        for (size_t i = 0; i < this->alphaTransparentObjects.count; ++i)
        {
            delete this->alphaTransparentObjects.buffer[i]->value;
        }
        
        for (size_t i = 0; i < this->refractiveTransparentObjects.count; ++i)
        {
            delete this->refractiveTransparentObjects.buffer[i]->value;
        }



        for (size_t i = 0; i < this->opaqueObjectsLast.count; ++i)
        {
            delete this->opaqueObjectsLast.buffer[i]->value;
        }

        for (size_t i = 0; i < this->alphaTransparentObjectsLast.count; ++i)
        {
            delete this->alphaTransparentObjectsLast.buffer[i]->value;
        }
        
        for (size_t i = 0; i < this->refractiveTransparentObjectsLast.count; ++i)
        {
            delete this->refractiveTransparentObjectsLast.buffer[i]->value;
        }


        for (size_t i = 0; i < this->ambientLights.count; ++i)
        {
            delete this->ambientLights[i];
        }

        for (size_t i = 0; i < this->directionalLights.count; ++i)
        {
            delete this->directionalLights[i];
        }

        for (size_t i = 0; i < this->pointLights.count; ++i)
        {
            delete this->pointLights[i];
        }

        for (size_t i = 0; i < this->spotLights.count; ++i)
        {
            delete this->spotLights[i];
        }


        for (size_t i = 0; i < this->shadowDirectionalLights.count; ++i)
        {
            delete this->shadowDirectionalLights[i];
        }

        for (size_t i = 0; i < this->shadowPointLights.count; ++i)
        {
            delete this->shadowPointLights[i];
        }

        for (size_t i = 0; i < this->shadowSpotLights.count; ++i)
        {
            delete this->shadowSpotLights[i];
        }
    }


    void DefaultSceneRendererVisibleObjects::ProcessObjectModel(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto modelComponent = sceneNode.GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);
                {
                    auto model = modelComponent->GetModel();
                    if (model != nullptr)                           // <-- Is allowed to be null. Perhaps due to a bad path?
                    {
                        for (size_t i = 0; i < model->meshes.count; ++i)
                        {
                            auto mesh = model->meshes[i];
                            assert(mesh != nullptr);
                            {
                                glm::mat4 transform = sceneNode.GetWorldTransform();

                                // If the mesh needs to be animated we don't want to add the mesh straight away. Instead we want to wait until
                                // after it's been animated. If it's not animated, we just add it straight away.
                                if (model->IsAnimating() && mesh->IsAnimated())
                                {
                                    if (!this->meshesToAnimate.Exists(mesh))
                                    {
                                        this->meshesToAnimate.Add(mesh, transform);
                                    }
                                }
                                else
                                {
                                    this->AddMesh(*mesh, transform);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void DefaultSceneRendererVisibleObjects::ProcessObjectPointLight(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto lightComponent = sceneNode.GetComponent<PointLightComponent>();
                assert(lightComponent != nullptr);
                {
                    SceneRendererPointLight* light = nullptr;

                    if (!lightComponent->IsShadowCastingEnabled())
                    {
                        light = new SceneRendererPointLight;
                        this->pointLights.PushBack(light);
                    }
                    else
                    {
                        light = new DefaultSceneRendererShadowPointLight;
                        this->shadowPointLights.PushBack(static_cast<DefaultSceneRendererShadowPointLight*>(light));
                    }

                    light->colour               = lightComponent->GetColour();
                    light->position             = sceneNode.GetWorldPosition();
                    light->constantAttenuation  = lightComponent->GetConstantAttenuation();
                    light->linearAttenuation    = lightComponent->GetLinearAttenuation();
                    light->quadraticAttenuation = lightComponent->GetQuadraticAttenuation();

                    if (lightComponent->IsShadowCastingEnabled())
                    {
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->projection    = glm::perspective(90.0f, 1.0f, 0.1f, Math::Lighting::ApproximateAttenuationRadius(light->constantAttenuation, light->linearAttenuation, light->quadraticAttenuation));
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->positiveXView = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-light->position);
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->negativeXView = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-light->position);
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->positiveYView = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-light->position);
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->negativeYView = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-light->position);
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->positiveZView = glm::mat4_cast(glm::inverse(glm::angleAxis(  0.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-light->position);
                        static_cast<DefaultSceneRendererShadowPointLight*>(light)->negativeZView = glm::mat4_cast(glm::inverse(glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-light->position);
                    }
                }
            }
        }
    }

    void DefaultSceneRendererVisibleObjects::ProcessObjectSpotLight(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto lightComponent = sceneNode.GetComponent<SpotLightComponent>();
                assert(lightComponent != nullptr);
                {
                    SceneRendererSpotLight* light = nullptr;

                    if (!lightComponent->IsShadowCastingEnabled())
                    {
                        light = new SceneRendererSpotLight;
                        this->spotLights.PushBack(light);
                    }
                    else
                    {
                        light = new DefaultSceneRendererShadowSpotLight;
                        this->shadowSpotLights.PushBack(static_cast<DefaultSceneRendererShadowSpotLight*>(light));
                    }


                    light->colour               = lightComponent->GetColour();
                    light->position             = sceneNode.GetWorldPosition();
                    light->orientation          = sceneNode.GetWorldOrientation();
                    light->constantAttenuation  = lightComponent->GetConstantAttenuation();
                    light->linearAttenuation    = lightComponent->GetLinearAttenuation();
                    light->quadraticAttenuation = lightComponent->GetQuadraticAttenuation();
                    light->innerAngle           = lightComponent->GetInnerAngle();
                    light->outerAngle           = lightComponent->GetOuterAngle();

                    // A few additional properties need to be set if the light is casting a shadow.
                    if (lightComponent->IsShadowCastingEnabled())
                    {
                        float cAtt = light->constantAttenuation; 
                        float lAtt = light->linearAttenuation;
                        float qAtt = light->quadraticAttenuation;

                        static_cast<DefaultSceneRendererShadowSpotLight*>(light)->projection = glm::perspective(light->outerAngle * 2.0f, 1.0f, 0.1f, Math::Lighting::ApproximateAttenuationRadius(cAtt, lAtt, qAtt));
                        static_cast<DefaultSceneRendererShadowSpotLight*>(light)->view       = glm::mat4_cast(glm::inverse(light->orientation)) * glm::translate(-light->position);
                    }
                }
            }
        }
    }

    void DefaultSceneRendererVisibleObjects::ProcessObjectAmbientLight(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto lightComponent = sceneNode.GetComponent<AmbientLightComponent>();
                assert(lightComponent != nullptr);
                {
                    auto light = new SceneRendererAmbientLight;
                    light->colour = lightComponent->GetColour();

                    this->ambientLights.PushBack(light);
                }
            }
        }
    }

    void DefaultSceneRendererVisibleObjects::ProcessObjectDirectionalLight(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &sceneNode = static_cast<SceneNode &>(object);
            {
                auto lightComponent = sceneNode.GetComponent<DirectionalLightComponent>();
                assert(lightComponent != nullptr);
                {
                    SceneRendererDirectionalLight* light = nullptr;

                    if (!lightComponent->IsShadowCastingEnabled())
                    {
                        light = new SceneRendererDirectionalLight;
                        this->directionalLights.PushBack(light);
                    }
                    else
                    {
                        light = new DefaultSceneRendererShadowDirectionalLight;
                        this->shadowDirectionalLights.PushBack(static_cast<DefaultSceneRendererShadowDirectionalLight*>(light));
                    }


                    light->colour      = lightComponent->GetColour();
                    light->position    = sceneNode.GetWorldPosition();
                    light->orientation = sceneNode.GetWorldOrientation();

                    if (lightComponent->IsShadowCastingEnabled())
                    {
                        static_cast<DefaultSceneRendererShadowDirectionalLight*>(light)->projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 500.0f);
                        static_cast<DefaultSceneRendererShadowDirectionalLight*>(light)->view       = glm::mat4_cast(glm::inverse(light->orientation)) * glm::translate(-light->position);
                    }
                }
            }
        }
    }



    void DefaultSceneRendererVisibleObjects::AddMesh(Mesh &mesh, const glm::mat4 &transform)
    {
        SceneRendererMesh object;
        object.vertexArray = mesh.GetSkinnedGeometry();
        object.drawMode    = mesh.GetDrawMode();
        object.material    = mesh.GetMaterial();
        object.transform   = transform;
        this->AddMesh(object);
    }

    void DefaultSceneRendererVisibleObjects::AddMesh(const SceneRendererMesh &mesh)
    {
        if (mesh.material != nullptr)
        {
            GTCore::Vector<SceneRendererMesh>* objectList = nullptr;

            auto &materialDefinition = mesh.material->GetDefinition();
            if (mesh.material->IsRefractive())
            {
                if (!(mesh.flags & SceneRendererMesh::DrawLast))
                {
                    auto iObjectList = this->refractiveTransparentObjects.Find(&materialDefinition);
                    if (iObjectList != nullptr)
                    {
                        objectList = iObjectList->value;
                    }
                    else
                    {
                        objectList = new GTCore::Vector<SceneRendererMesh>;
                        this->refractiveTransparentObjects.Add(&materialDefinition, objectList);
                    }
                }
                else
                {
                    auto iObjectList = this->refractiveTransparentObjectsLast.Find(&materialDefinition);
                    if (iObjectList != nullptr)
                    {
                        objectList = iObjectList->value;
                    }
                    else
                    {
                        objectList = new GTCore::Vector<SceneRendererMesh>;
                        this->refractiveTransparentObjectsLast.Add(&materialDefinition, objectList);
                    }
                }
            }
            //else if (material->IsAlphaTransparent())
            //{
            //}
            else    // Opaque
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
                        objectList = new GTCore::Vector<SceneRendererMesh>(100);
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
                        objectList = new GTCore::Vector<SceneRendererMesh>(100);
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

    void DefaultSceneRendererVisibleObjects::PostProcess()
    {
        // We need to retrieve the visible objects of each shadow casting light.

        // Shadow-Casting Directional Lights.
        for (size_t i = 0; i < this->shadowDirectionalLights.count; ++i)
        {
            auto light = this->shadowDirectionalLights[i];
            assert(light != nullptr);
            {
                scene.QueryVisibleObjects(light->projection * light->view, light->containedMeshes);
            }
        }


        // Shadow-Casting Point Lights.
        for (size_t i = 0; i < this->shadowPointLights.count; ++i)
        {
            auto light = this->shadowPointLights[i];
            assert(light != nullptr);
            {
                // We need to do 6 queries here - one for each face.
                scene.QueryVisibleObjects(light->projection * light->positiveXView, light->containedMeshesPositiveX);
                scene.QueryVisibleObjects(light->projection * light->negativeXView, light->containedMeshesNegativeX);
                scene.QueryVisibleObjects(light->projection * light->positiveYView, light->containedMeshesPositiveY);
                scene.QueryVisibleObjects(light->projection * light->negativeYView, light->containedMeshesNegativeY);
                scene.QueryVisibleObjects(light->projection * light->positiveZView, light->containedMeshesPositiveZ);
                scene.QueryVisibleObjects(light->projection * light->negativeZView, light->containedMeshesNegativeZ);
            }
        }


        // Shadow-Casting Spot Lights.
        for (size_t i = 0; i < this->shadowSpotLights.count; ++i)
        {
            auto light = this->shadowSpotLights[i];
            assert(light != nullptr);
            {
                scene.QueryVisibleObjects(light->projection * light->view, light->containedMeshes);
            }
        }



        // Apply skinning.
        for (size_t i = 0; i < this->meshesToAnimate.count; ++i)
        {
            auto mesh       = this->meshesToAnimate.buffer[i]->key;
            auto &transform = this->meshesToAnimate.buffer[i]->value;
            assert(mesh != nullptr);
            {
                mesh->ApplySkinning();

                // Here is where we need to add the mesh.
                this->AddMesh(*mesh, transform);
            }
        }
    }




    ///////////////////////////////////////////////////////////
    // DefaultSceneRenderer

    DefaultSceneRenderer2::DefaultSceneRenderer2()
        : viewportFramebuffers(), materialShadersToDelete(), depthPassShader(nullptr), externalMeshes(),
          shadowMapFramebuffer(), shadowMapShader(nullptr), pointShadowMapFramebuffer(512, 512), pointShadowMapShader(nullptr),
          fullscreenTriangleVA(nullptr), finalCompositionShader(nullptr),
          materialLibraryEventHandler(*this)
    {
        this->depthPassShader        = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassVS"),        ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassFS"));
        this->shadowMapShader        = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowMapVS"),        ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowMapFS"));
        this->pointShadowMapShader   = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointShadowMapVS"),   ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointShadowMapFS"));
        this->finalCompositionShader = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionFS"));
        this->bloomShader            = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_BloomFS"));


        // Shadow Map Framebuffer.
        this->shadowMapFramebuffer.framebuffer        = Renderer2::CreateFramebuffer();
        this->shadowMapFramebuffer.depthStencilBuffer = Renderer2::CreateTexture2D();
        this->shadowMapFramebuffer.colourBuffer       = Renderer2::CreateTexture2D();

        // We just resize to setup the texture formats and whatnot.
        this->shadowMapFramebuffer.Resize(512, 512);        // TODO: Make these a configurable variable.

        // Now we can setup the filtering and attach the textures to the framebuffer itself.
        Renderer2::SetTexture2DFilter(  *this->shadowMapFramebuffer.colourBuffer, TextureFilter_LinearLinear, TextureFilter_Linear);
        Renderer2::SetTexture2DWrapMode(*this->shadowMapFramebuffer.colourBuffer, TextureWrapMode_ClampToEdge);

        this->shadowMapFramebuffer.framebuffer->AttachDepthStencilBuffer(this->shadowMapFramebuffer.depthStencilBuffer);
        this->shadowMapFramebuffer.framebuffer->AttachColourBuffer(this->shadowMapFramebuffer.colourBuffer, 0);

        Renderer2::PushAttachments(*this->shadowMapFramebuffer.framebuffer);



        /// Fullscreen Triangle Vertex Array.
        this->fullscreenTriangleVA = Renderer2::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2);
        
        float triangleVertices[] =
        {
            -3.0f, -1.0f, 0.0f,
            -1.0f,  0.0f,

             1.0f, -1.0f, 0.0f,
             1.0f,  0.0f,

             1.0f,  3.0f, 0.0f,
             1.0f,  2.0f,
        };

        unsigned int triangleIndices[] =
        {
            0, 1, 2
        };

        this->fullscreenTriangleVA->SetData(triangleVertices, 3, triangleIndices, 3);




        // The event handler needs to be attached to the material library so we can catch when a material definition is deleted an
        // in turn delete the shaders we've associated with that material.
        MaterialLibrary::AttachEventHandler(this->materialLibraryEventHandler);
    }

    DefaultSceneRenderer2::~DefaultSceneRenderer2()
    {
        Renderer2::DeleteShader(this->depthPassShader);
        Renderer2::DeleteShader(this->shadowMapShader);
        Renderer2::DeleteShader(this->pointShadowMapShader);
        Renderer2::DeleteShader(this->finalCompositionShader);
        Renderer2::DeleteShader(this->bloomShader);

        for (size_t i = 0; i < this->materialShadersToDelete.count; ++i)
        {
            auto materialDefinition = this->materialShadersToDelete.buffer[i]->key;
            auto materialShaders    = this->materialShadersToDelete.buffer[i]->value;

            assert(materialDefinition != nullptr);
            assert(materialShaders    != nullptr);
            {
                materialDefinition->RemoveMetadata(reinterpret_cast<size_t>(this));
                delete materialShaders;
            }
        }

        Renderer2::DeleteVertexArray(this->fullscreenTriangleVA);

        MaterialLibrary::RemoveEventHandler(this->materialLibraryEventHandler);
    }


    void DefaultSceneRenderer2::Begin(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer2::End(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer2::RenderViewport(Scene &scene, SceneViewport &viewport)
    {
        // 0) Retrieve visible objects.
        DefaultSceneRendererVisibleObjects visibleObjects(scene, viewport);
        scene.QueryVisibleObjects(viewport.GetMVPMatrix(), visibleObjects);
        
        // All external meshes are considered visible. Not going to do any frustum culling here.
        for (size_t i = 0; i < this->externalMeshes.count; ++i)
        {
            auto externalMesh = this->externalMeshes[i];
            assert(externalMesh != nullptr);
            {
                visibleObjects.AddMesh(*externalMesh);
            }
        }

        // Post-processing needs to be done after everything has been added.
        visibleObjects.PostProcess();



        // 1) Make everything current.
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            Renderer2::SetCurrentFramebuffer(framebuffer->framebuffer);
            Renderer2::SetViewport(0, 0, framebuffer->width, framebuffer->height);

            //int drawBuffers[] = {0};
            //Renderer2::SetDrawBuffers(1, drawBuffers);


            // Clear.
            //Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            //Renderer2::SetClearDepth(1.0f);
            //Renderer2::SetClearStencil(0);
            //Renderer2::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);
        }





        // 2) The opaque pass.
        this->RenderOpaquePass(framebuffer, visibleObjects);

        // 3) The alpha transparent pass (for things like sprites and particle effects).
        //this->RenderAlphaTransparentPass(scene, framebuffer);

        // 4) The refractive transparent pass (for things with refractive properties).
        //this->RenderRefractiveTransparentPass(scene, framebuffer);


        // 5) The final composition.
        this->RenderFinalComposition(framebuffer);
    }

    void DefaultSceneRenderer2::AddViewport(SceneViewport &viewport)
    {
        // We create a framebuffer for every viewport. We map these to viewports with a simple GTCore::Map container.
        auto framebuffer = this->CreateFramebuffer(viewport.GetWidth(), viewport.GetHeight());
        assert(framebuffer != nullptr);
        {
            viewport.SetColourBuffer(framebuffer->finalColourBuffer);
            this->viewportFramebuffers.Add(&viewport, framebuffer);
        }
    }

    void DefaultSceneRenderer2::RemoveViewport(SceneViewport &viewport)
    {
        // When a viewport is removed, the framebuffer needs to be deleted.
        viewport.SetColourBuffer(nullptr);

        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);
        {
            auto framebuffer = iFramebuffer->value;
            assert(framebuffer != nullptr);
            {
                this->DeleteFramebuffer(framebuffer);
                this->viewportFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }
    }

    void DefaultSceneRenderer2::OnViewportResized(SceneViewport &viewport)
    {
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            framebuffer->Resize(viewport.GetWidth(), viewport.GetHeight());
        }
    }


    void DefaultSceneRenderer2::AddExternalMesh(const SceneRendererMesh &meshToAdd)
    {
        if (!this->externalMeshes.Exists(&meshToAdd))
        {
            this->externalMeshes.PushBack(&meshToAdd);
        }
    }

    void DefaultSceneRenderer2::RemoveExternalMesh(const SceneRendererMesh &meshToRemove)
    {
        this->externalMeshes.RemoveFirstOccuranceOf(&meshToRemove);
    }


    ////////////////////////////////////////////////////////////////
    // Event Handlers from MaterialLibrary.

    void DefaultSceneRenderer2::OnDeleteMaterialDefinition(MaterialDefinition &definition)
    {
        auto iShaders = this->materialShadersToDelete.Find(&definition);
        if (iShaders != nullptr)
        {
            delete iShaders->value;
            this->materialShadersToDelete.RemoveByIndex(iShaders->index);
        }
    }



    /////////////////////////////////////////////////////////
    // Private

    DefaultSceneRendererFramebuffer* DefaultSceneRenderer2::CreateFramebuffer(unsigned int width, unsigned int height)
    {
        auto newFramebuffer = new DefaultSceneRendererFramebuffer;
        newFramebuffer->framebuffer        = Renderer2::CreateFramebuffer();
        newFramebuffer->depthStencilBuffer = Renderer2::CreateTexture2D();
        newFramebuffer->colourBuffer       = Renderer2::CreateTexture2D();
        newFramebuffer->lightingBuffer0    = Renderer2::CreateTexture2D();
        newFramebuffer->lightingBuffer1    = Renderer2::CreateTexture2D();
        newFramebuffer->finalColourBuffer  = Renderer2::CreateTexture2D();

        newFramebuffer->bloomFramebuffer   = Renderer2::CreateFramebuffer();
        newFramebuffer->bloomBuffer        = Renderer2::CreateTexture2D();

        // Sizes and formats need to be set. All we need to do is call the Resize() method.
        newFramebuffer->Resize(width, height);


        // Filters.
        Renderer2::SetTexture2DFilter(*newFramebuffer->colourBuffer,      TextureFilter_NearestNearest, TextureFilter_Nearest);
        Renderer2::SetTexture2DFilter(*newFramebuffer->bloomBuffer,       TextureFilter_LinearNearest, TextureFilter_Linear);
        Renderer2::SetTexture2DFilter(*newFramebuffer->lightingBuffer0,   TextureFilter_Nearest,       TextureFilter_Nearest);
        Renderer2::SetTexture2DFilter(*newFramebuffer->lightingBuffer1  , TextureFilter_Nearest,       TextureFilter_Nearest);
        Renderer2::SetTexture2DFilter(*newFramebuffer->finalColourBuffer, TextureFilter_Nearest,       TextureFilter_Nearest);

        // Wrap Modes.
        Renderer2::SetTexture2DWrapMode(*newFramebuffer->colourBuffer, TextureWrapMode_ClampToEdge);
        Renderer2::SetTexture2DWrapMode(*newFramebuffer->bloomBuffer,  TextureWrapMode_ClampToEdge);


        // Attach to the main framebuffer.
        newFramebuffer->framebuffer->AttachDepthStencilBuffer(newFramebuffer->depthStencilBuffer);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->colourBuffer,      0);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->lightingBuffer0,   2);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->lightingBuffer1,   3);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->finalColourBuffer, 4);
        Renderer2::PushAttachments(*newFramebuffer->framebuffer);

        newFramebuffer->bloomFramebuffer->AttachColourBuffer(newFramebuffer->bloomBuffer, 0);
        Renderer2::PushAttachments(*newFramebuffer->bloomFramebuffer);
        


        return newFramebuffer;
    }

    void DefaultSceneRenderer2::DeleteFramebuffer(DefaultSceneRendererFramebuffer* framebufferToDelete)
    {
        Renderer2::DeleteTexture2D(framebufferToDelete->depthStencilBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->colourBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->bloomBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->lightingBuffer0);
        Renderer2::DeleteTexture2D(framebufferToDelete->lightingBuffer1);
        Renderer2::DeleteTexture2D(framebufferToDelete->finalColourBuffer);
        
        Renderer2::DeleteFramebuffer(framebufferToDelete->framebuffer);
        Renderer2::DeleteFramebuffer(framebufferToDelete->bloomFramebuffer);

        delete framebufferToDelete;
    }

    DefaultSceneRendererFramebuffer* DefaultSceneRenderer2::GetViewportFramebuffer(SceneViewport &viewport)
    {
        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        if (iFramebuffer != nullptr)
        {
            return iFramebuffer->value;
        }

        return nullptr;
    }


    ///////////////////////
    // Rendering.

    void DefaultSceneRenderer2::RenderOpaquePass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        this->RenderOpaqueLightingPass(framebuffer, visibleObjects);
        this->RenderOpaqueMaterialPass(framebuffer, visibleObjects);
    }

    void DefaultSceneRenderer2::RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // We need to do a depth pre-pass.
        Renderer2::DisableColourWrites();

        // Shader needs to be made current, once.
        Renderer2::SetCurrentShader(this->depthPassShader);


        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderDepthPass(framebuffer, visibleObjects, *meshList);
            }
        }


        // Last
        for (size_t iObjectList = 0; iObjectList < visibleObjects.opaqueObjectsLast.count; ++iObjectList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iObjectList]->value;
            assert(meshList != nullptr);
            {
                this->RenderDepthPass(framebuffer, visibleObjects, *meshList);
            }
        }


        Renderer2::EnableColourWrites();
    }

    void DefaultSceneRenderer2::RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        (void)framebuffer;

        for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
        {
            auto &mesh = meshes.Get(iMesh);
            {
                // Shader Properties.
                this->depthPassShader->SetParameter("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
                {
                    Renderer2::PushShaderPendingProperties(*this->depthPassShader);
                }
                this->depthPassShader->ClearPendingParameters();

                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer2::DisableDepthTest();
                if ((mesh.flags & SceneRendererMesh::NoDepthWrite)) Renderer2::DisableDepthWrites();
                {
                    Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer2::EnableDepthTest();
                if ((mesh.flags & SceneRendererMesh::NoDepthWrite)) Renderer2::EnableDepthWrites();
            }
        }
    }


    void DefaultSceneRenderer2::RenderOpaqueLightingPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // The lighting buffers must be cleared to black. Also need to clear the depth/stencil buffer.
        int lightingBuffers[] = {2, 3};
        Renderer2::SetDrawBuffers(2, lightingBuffers);

        Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::SetClearStencil(0);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);

        Renderer2::DisableBlending();
        Renderer2::EnableDepthWrites();
        Renderer2::EnableDepthTest();
        Renderer2::SetDepthFunction(RendererFunction_LEqual);

        // Depth Pass first.
        this->RenderDepthPass(framebuffer, visibleObjects);


        // Depth is laid down. No need to write.
        Renderer2::DisableDepthWrites();
        Renderer2::SetDepthFunction(RendererFunction_Equal);

        // We use additive blending here.
        Renderer2::EnableBlending();
        Renderer2::SetBlendEquation(BlendEquation_Add);
        Renderer2::SetBlendFunction(BlendFunc_One, BlendFunc_One);


        size_t ambientLightsRemaining     = visibleObjects.ambientLights.count;
        size_t directionalLightsRemaining = visibleObjects.directionalLights.count;
        size_t pointLightsRemaining       = visibleObjects.pointLights.count;
        size_t spotLightsRemaining        = visibleObjects.spotLights.count;

        // Ambient Lights.
        while (ambientLightsRemaining > 0)
        {
            this->RenderOpaqueAmbientLightingPass(ambientLightsRemaining - 1, visibleObjects);
            ambientLightsRemaining -= 1;
        }

        // Directional Lights.
        while (directionalLightsRemaining > 0)
        {
            this->RenderOpaqueDirectionalLightingPass(directionalLightsRemaining - 1, visibleObjects);
            directionalLightsRemaining -= 1;
        }

        // Point Lights.
        while (pointLightsRemaining > 0)
        {
            this->RenderOpaquePointLightingPass(pointLightsRemaining - 1, visibleObjects);
            pointLightsRemaining -= 1;
        }

        // Spot Lights.
        while (spotLightsRemaining > 0)
        {
            this->RenderOpaqueSpotLightingPass(spotLightsRemaining - 1, visibleObjects);
            spotLightsRemaining -= 1;
        }



        /// Shadow-Casting Spot Lights.
        size_t shadowDirectionalLightsRemaining = visibleObjects.shadowDirectionalLights.count;
        size_t shadowPointLightsRemaining       = visibleObjects.shadowPointLights.count;
        size_t shadowSpotLightsRemaining        = visibleObjects.shadowSpotLights.count;

        while (shadowDirectionalLightsRemaining > 0)
        {
            this->RenderOpaqueShadowDirectionalLightingPass(shadowDirectionalLightsRemaining - 1, visibleObjects, framebuffer);
            shadowDirectionalLightsRemaining -= 1;
        }

        while (shadowPointLightsRemaining > 0)
        {
            this->RenderOpaqueShadowPointLightingPass(shadowPointLightsRemaining - 1, visibleObjects, framebuffer);
            shadowPointLightsRemaining -= 1;
        }
        
        while (shadowSpotLightsRemaining > 0)
        {
            this->RenderOpaqueShadowSpotLightingPass(shadowSpotLightsRemaining - 1, visibleObjects, framebuffer);
            shadowSpotLightsRemaining -= 1;
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // This pass draws the objects like normal and grab the lighting information from the lighting buffers.
        Renderer2::DisableBlending();
        

        int outputBuffer[] = {0};
        Renderer2::SetDrawBuffers(1, outputBuffer);

        if (this->IsBackgroundColourClearingEnabled())
        {
            auto &colour = this->GetBackgroundClearColour();
            Renderer2::SetClearColour(colour.x, colour.y, colour.z, 1.0f);
            Renderer2::Clear(BufferType_Colour);
        }


        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueMaterialPass(framebuffer, visibleObjects, *meshList);
            }
        }
        
        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueMaterialPass(framebuffer, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
        {
            auto &mesh = meshes.Get(iMesh);
            {
                auto material = mesh.material;
                assert(material != nullptr);
                {
                    // Shader Setup.
                    auto shader = this->GetMaterialMaterialShader(*material);
                    assert(shader != nullptr);
                    {
                        glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                        glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                        Renderer2::SetCurrentShader(shader);

                        shader->SetParametersFromMaterial(*material);
                        shader->SetParameter("ViewModelMatrix",  viewModelMatrix);
                        shader->SetParameter("NormalMatrix",     normalMatrix);
                        shader->SetParameter("PVMMatrix",        visibleObjects.projectionViewMatrix * mesh.transform);
                        shader->SetParameter("DiffuseLighting",  framebuffer->lightingBuffer0);
                        shader->SetParameter("SpecularLighting", framebuffer->lightingBuffer1);
                        {
                            Renderer2::PushShaderPendingProperties(*shader);
                        }
                        shader->ClearPendingParameters();
                    }

                    // Draw.
                    if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                    {
                        Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                    }
                    if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                }
            }
        }
    }

    


    void DefaultSceneRenderer2::RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueAmbientLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueAmbientLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.ambientLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iObject = 0; iObject < meshes.count; ++iObject)
            {
                auto &mesh = meshes.Get(iObject);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialAmbientLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix", viewModelMatrix);
                            shader->SetParameter("NormalMatrix",    normalMatrix);
                            shader->SetParameter("Colour",          light->colour);
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer2::RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueDirectionalLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueDirectionalLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.directionalLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iObject = 0; iObject < meshes.count; ++iObject)
            {
                auto &mesh = meshes.Get(iObject);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialDirectionalLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix", viewModelMatrix);
                            shader->SetParameter("NormalMatrix",    normalMatrix);
                            shader->SetParameter("Colour",          light->colour);
                            shader->SetParameter("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }


    void DefaultSceneRenderer2::RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer2::DisableBlending();
        Renderer2::SetDepthFunction(RendererFunction_LEqual);
        Renderer2::EnableDepthWrites();

        Renderer2::SetCurrentFramebuffer(this->shadowMapFramebuffer.framebuffer);
        Renderer2::SetCurrentShader(this->shadowMapShader);        
        Renderer2::SetViewport(0, 0, this->shadowMapFramebuffer.width, this->shadowMapFramebuffer.height);

        Renderer2::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth);

        int shadowBufferIndex = 0;
        Renderer2::SetDrawBuffers(1, &shadowBufferIndex);



        auto light = visibleObjects.shadowDirectionalLights[lightIndex];
        assert(light != nullptr);
        {
            glm::mat4 projectionView = light->projection * light->view;

            for (size_t i = 0; i < light->containedMeshes.meshes.count; ++i)
            {
                auto &mesh = light->containedMeshes.meshes[i];
                assert(mesh.vertexArray != nullptr);
                {
                    // Shader setup.
                    shadowMapShader->SetParameter("PVMMatrix", projectionView * mesh.transform);
                    {
                        Renderer2::PushShaderPendingProperties(*this->shadowMapShader);
                    }
                    shadowMapShader->ClearPendingParameters();


                    // Draw.
                    Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                }
            }
        }

        Renderer2::GenerateTexture2DMipmaps(*this->shadowMapFramebuffer.colourBuffer);



        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer2::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {2, 3};
        Renderer2::SetDrawBuffers(2, lightingBuffers);
        Renderer2::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);
        Renderer2::EnableBlending();
        Renderer2::DisableDepthWrites();
        Renderer2::SetDepthFunction(RendererFunction_Equal);



        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowDirectionalLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowDirectionalLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowDirectionalLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iObject = 0; iObject < meshes.count; ++iObject)
            {
                auto &mesh = meshes.Get(iObject);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialShadowDirectionalLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix", viewModelMatrix);
                            shader->SetParameter("NormalMatrix",    normalMatrix);
                            shader->SetParameter("LightPVMMatrix",  light->projection * light->view * mesh.transform);
                            shader->SetParameter("Colour",          light->colour);
                            shader->SetParameter("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetParameter("ShadowMap",       this->shadowMapFramebuffer.colourBuffer);
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer2::RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaquePointLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaquePointLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.pointLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
            {
                auto &mesh = meshes.Get(iMesh);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialPointLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix",      viewModelMatrix);
                            shader->SetParameter("NormalMatrix",         normalMatrix);
                            shader->SetParameter("Colour",               light->colour);
                            shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetParameter("ConstantAttenuation",  light->constantAttenuation);
                            shader->SetParameter("LinearAttenuation",    light->linearAttenuation);
                            shader->SetParameter("QuadraticAttenuation", light->quadraticAttenuation);
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer2::RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer2::DisableBlending();
        Renderer2::EnableDepthWrites();
        Renderer2::SetDepthFunction(RendererFunction_LEqual);
        
        Renderer2::SetCurrentFramebuffer(this->pointShadowMapFramebuffer.framebuffer);
        Renderer2::SetCurrentShader(this->pointShadowMapShader);
        Renderer2::SetViewport(0, 0, this->pointShadowMapFramebuffer.width, this->pointShadowMapFramebuffer.height);

        
        auto light = visibleObjects.shadowPointLights[lightIndex];
        assert(light != nullptr);
        {
            this->RenderPointShapowMapFace(*light, light->positiveXView, 0, light->containedMeshesPositiveX.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeXView, 1, light->containedMeshesNegativeX.meshes);
            this->RenderPointShapowMapFace(*light, light->positiveYView, 2, light->containedMeshesPositiveY.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeYView, 3, light->containedMeshesNegativeY.meshes);
            this->RenderPointShapowMapFace(*light, light->positiveZView, 4, light->containedMeshesPositiveZ.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeZView, 5, light->containedMeshesNegativeZ.meshes);
        }



        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer2::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {2, 3};
        Renderer2::SetDrawBuffers(2, lightingBuffers);
        Renderer2::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);
        Renderer2::EnableBlending();
        Renderer2::DisableDepthWrites();
        Renderer2::SetDepthFunction(RendererFunction_Equal);



        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowPointLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowPointLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowPointLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
            {
                auto &mesh = meshes.Get(iMesh);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialShadowPointLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix",      viewModelMatrix);
                            shader->SetParameter("ModelMatrix",          mesh.transform);
                            shader->SetParameter("NormalMatrix",         normalMatrix);
                            shader->SetParameter("Colour",               light->colour);
                            shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetParameter("ConstantAttenuation",  light->constantAttenuation);
                            shader->SetParameter("LinearAttenuation",    light->linearAttenuation);
                            shader->SetParameter("QuadraticAttenuation", light->quadraticAttenuation);
                            shader->SetParameter("ShadowMap",            this->pointShadowMapFramebuffer.colourBuffer);
                            shader->SetParameter("PositionWS",           light->position);
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }

    void DefaultSceneRenderer2::RenderPointShapowMapFace(const DefaultSceneRendererShadowPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        // The draw buffer needs to be set. The appropriate framebuffer will have already been set.
        Renderer2::SetDrawBuffers(1, &faceIndex);
        
        // We need to clear both depth and colour. The colour needs to be cleared to the radius of the light.
        float radius = Math::Lighting::ApproximateAttenuationRadius(light.constantAttenuation, light.linearAttenuation, light.quadraticAttenuation);
        Renderer2::SetClearColour(radius, radius * radius, 0.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth);



        glm::mat4 projectionView = light.projection * faceViewMatrix;

        for (size_t i = 0; i < meshes.count; ++i)
        {
            auto &mesh = meshes[i];
            assert(mesh.vertexArray != nullptr);
            {
                // Shader setup.
                this->pointShadowMapShader->SetParameter("PVMMatrix",       projectionView * mesh.transform);
                this->pointShadowMapShader->SetParameter("ViewModelMatrix", faceViewMatrix * mesh.transform);
                {
                    Renderer2::PushShaderPendingProperties(*this->pointShadowMapShader);
                }
                this->pointShadowMapShader->ClearPendingParameters();


                // Draw.
                Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
            }
        }
    }



    void DefaultSceneRenderer2::RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueSpotLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueSpotLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.spotLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
            {
                auto &mesh = meshes.Get(iMesh);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialSpotLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix",      viewModelMatrix);
                            shader->SetParameter("NormalMatrix",         normalMatrix);
                            shader->SetParameter("Colour",               light->colour);
                            shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetParameter("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetParameter("ConstantAttenuation",  light->constantAttenuation);
                            shader->SetParameter("LinearAttenuation",    light->linearAttenuation);
                            shader->SetParameter("QuadraticAttenuation", light->quadraticAttenuation);
                            shader->SetParameter("CosAngleInner",        glm::cos(glm::radians(light->innerAngle)));
                            shader->SetParameter("CosAngleOuter",        glm::cos(glm::radians(light->outerAngle)));
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer2::RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer2::DisableBlending();
        Renderer2::SetDepthFunction(RendererFunction_LEqual);
        Renderer2::EnableDepthWrites();

        Renderer2::SetCurrentFramebuffer(this->shadowMapFramebuffer.framebuffer);
        Renderer2::SetCurrentShader(this->shadowMapShader);        
        Renderer2::SetViewport(0, 0, this->shadowMapFramebuffer.width, this->shadowMapFramebuffer.height);

        Renderer2::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth);

        int shadowBufferIndex = 0;
        Renderer2::SetDrawBuffers(1, &shadowBufferIndex);



        auto light = visibleObjects.shadowSpotLights[lightIndex];
        assert(light != nullptr);
        {
            glm::mat4 projectionView = light->projection * light->view;

            for (size_t i = 0; i < visibleObjects.shadowSpotLights[lightIndex]->containedMeshes.meshes.count; ++i)
            {
                auto &mesh = visibleObjects.shadowSpotLights[lightIndex]->containedMeshes.meshes[i];
                assert(mesh.vertexArray != nullptr);
                {
                    // Shader setup.
                    shadowMapShader->SetParameter("PVMMatrix", projectionView * mesh.transform);
                    {
                        Renderer2::PushShaderPendingProperties(*this->shadowMapShader);
                    }
                    shadowMapShader->ClearPendingParameters();


                    // Draw.
                    Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                }
            }
        }

        Renderer2::GenerateTexture2DMipmaps(*this->shadowMapFramebuffer.colourBuffer);



        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer2::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {2, 3};
        Renderer2::SetDrawBuffers(2, lightingBuffers);
        Renderer2::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);
        Renderer2::EnableBlending();
        Renderer2::DisableDepthWrites();
        Renderer2::SetDepthFunction(RendererFunction_Equal);


        // First.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjects.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjects.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowSpotLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }

        // Last.
        for (size_t iMeshList = 0; iMeshList < visibleObjects.opaqueObjectsLast.count; ++iMeshList)
        {
            auto meshList = visibleObjects.opaqueObjectsLast.buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                this->RenderOpaqueShadowSpotLightingPass(lightIndex, visibleObjects, *meshList);
            }
        }
    }

    void DefaultSceneRenderer2::RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRendererVisibleObjects &visibleObjects, const GTCore::Vector<SceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowSpotLights[lightIndex];
        assert(light != nullptr);
        {
            for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
            {
                auto &mesh = meshes.Get(iMesh);
                {
                    auto material = mesh.material;
                    assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                    {
                        // Setup Shader.
                        auto shader = this->GetMaterialShadowSpotLightShader(*material);
                        assert(shader != nullptr);
                        {
                            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                            Renderer2::SetCurrentShader(shader);

                            shader->SetParametersFromMaterial(*material);
                            shader->SetParameter("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetParameter("ViewModelMatrix",      viewModelMatrix);
                            shader->SetParameter("NormalMatrix",         normalMatrix);
                            shader->SetParameter("LightPVMMatrix",       light->projection * light->view * mesh.transform);
                            shader->SetParameter("Colour",               light->colour);
                            shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetParameter("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetParameter("ConstantAttenuation",  light->constantAttenuation);
                            shader->SetParameter("LinearAttenuation",    light->linearAttenuation);
                            shader->SetParameter("QuadraticAttenuation", light->quadraticAttenuation);
                            shader->SetParameter("CosAngleInner",        glm::cos(glm::radians(light->innerAngle)));
                            shader->SetParameter("CosAngleOuter",        glm::cos(glm::radians(light->outerAngle)));
                            shader->SetParameter("ShadowMap",            this->shadowMapFramebuffer.colourBuffer);
                            {
                                Renderer2::PushShaderPendingProperties(*shader);
                            }
                            shader->ClearPendingParameters();
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::DisableDepthTest();
                        {
                            Renderer2::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer2::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer2::RenderAlphaTransparentPass(Scene &scene, DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        (void)scene;
        (void)framebuffer;
        (void)visibleObjects;
    }

    void DefaultSceneRenderer2::RenderRefractiveTransparentPass(Scene &scene, DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        (void)scene;
        (void)framebuffer;
        (void)visibleObjects;
    }


    void DefaultSceneRenderer2::RenderFinalComposition(DefaultSceneRendererFramebuffer* framebuffer)
    {
        // Bloom buffer needs to be build. This will set some shader state for us.
        this->RenderBloomMap(framebuffer);


        // 0) Generate mipmaps on the colour buffer.
        Renderer2::GenerateTexture2DMipmaps(*framebuffer->colourBuffer);


        // 1) Setup.
        Renderer2::SetCurrentFramebuffer(framebuffer->framebuffer);

        int finalOutputBufferIndex = 4;
        Renderer2::SetDrawBuffers(1, &finalOutputBufferIndex);

        Renderer2::SetViewport(0, 0, framebuffer->finalColourBuffer->GetWidth(), framebuffer->finalColourBuffer->GetHeight());


        // 2) Draw a fullscreen triangle using the composition shader.
        Renderer2::SetCurrentShader(this->finalCompositionShader);
        this->finalCompositionShader->SetParameter("ColourBuffer", framebuffer->colourBuffer);
        this->finalCompositionShader->SetParameter("BloomBuffer",  framebuffer->bloomBuffer);
        this->finalCompositionShader->SetParameter("Exposure",     1.0f);
        this->finalCompositionShader->SetParameter("BloomFactor",  1.0f);
        {
            Renderer2::PushShaderPendingProperties(*this->finalCompositionShader);
        }
        this->finalCompositionShader->ClearPendingParameters();


        Renderer2::Draw(*this->fullscreenTriangleVA);
    }

    void DefaultSceneRenderer2::RenderBloomMap(DefaultSceneRendererFramebuffer* framebuffer)
    {
        // 1) Setup.
        Renderer2::SetCurrentFramebuffer(framebuffer->bloomFramebuffer);

        int bufferIndex = 0;
        Renderer2::SetDrawBuffers(1, &bufferIndex);

        Renderer2::DisableDepthTest();
        Renderer2::DisableDepthWrites();
        Renderer2::DisableStencilTest();
        Renderer2::DisableBlending();
        Renderer2::DisableScissorTest();

        Renderer2::SetViewport(0, 0, framebuffer->bloomBuffer->GetWidth(), framebuffer->bloomBuffer->GetHeight());

        Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 0.0f);
        Renderer2::Clear(BufferType_Colour);


        // 2) Draw a fullscreen triangle using the composition shader.
        Renderer2::SetCurrentShader(this->bloomShader);
        this->bloomShader->SetParameter("ColourBuffer", framebuffer->colourBuffer);
        {
            Renderer2::PushShaderPendingProperties(*this->bloomShader);
        }
        this->bloomShader->ClearPendingParameters();


        Renderer2::Draw(*this->fullscreenTriangleVA);


        // 2) Generate mipmaps.
        Renderer2::GenerateTexture2DMipmaps(*framebuffer->bloomBuffer);
    }



    ///////////////////////
    // Materials.

    DefaultSceneRendererMaterialShaders* DefaultSceneRenderer2::GetMaterialShaders(Material &material)
    {
        // The shaders are created per-definition. This means 1 shader for each definition. If we don't do this, we'll be creating many more shaders
        // than is required. Also, since we're modifying the definition (by setting a metadata property), we'll be evil and do a const_cast.
        auto &materialDefinition = const_cast<MaterialDefinition &>(material.GetDefinition());
        {
            auto shaders = static_cast<DefaultSceneRendererMaterialShaders*>(materialDefinition.GetMetadata(reinterpret_cast<size_t>(this)));
            if (shaders == nullptr)
            {
                shaders = new DefaultSceneRendererMaterialShaders;
                materialDefinition.SetMetadata(reinterpret_cast<size_t>(this), shaders);

                this->materialShadersToDelete.Add(&materialDefinition, shaders);
            }

            return shaders;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialAmbientLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->ambientLightShader == nullptr)
            {
                // The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_AmbientLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->ambientLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->ambientLightShader;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialDirectionalLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->directionalLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_DirectionalLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->directionalLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->directionalLightShader;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialPointLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->pointLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->pointLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->pointLightShader;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialSpotLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->spotLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_SpotLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->spotLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->spotLightShader;
        }
    }


    Shader* DefaultSceneRenderer2::GetMaterialShadowDirectionalLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->shadowDirectionalLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowDirectionalLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->shadowDirectionalLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowDirectionalLightShader;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialShadowPointLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->shadowPointLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowPointLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->shadowPointLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowPointLightShader;
        }
    }

    Shader* DefaultSceneRenderer2::GetMaterialShadowSpotLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->shadowSpotLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowSpotLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->shadowSpotLightShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowSpotLightShader;
        }
    }



    Shader* DefaultSceneRenderer2::GetMaterialMaterialShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->materialShader == nullptr)
            {
                // The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_MaterialVS"));

                GTCore::Strings::List<char> fragmentSource;
                if (!material.IsRefractive())
                {
                    fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_MaterialFS"));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetDiffuseShaderID()));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetEmissiveShaderID()));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetShininessShaderID()));
                }
                else
                {
                    fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_MaterialRefractiveFS"));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetDiffuseShaderID()));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetEmissiveShaderID()));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetShininessShaderID()));
                    fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetRefractionShaderID()));
                }

                shaders->materialShader = Renderer2::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->materialShader;
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
