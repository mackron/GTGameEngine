
#include <GTEngine/DefaultSceneRenderer.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

// In this file, DSR = DefaultSceneRenderer.

// TODO: Make this a config variable.
#define SHADOW_MAP_SIZE     1024

namespace GTEngine
{
    /// The callback used during the material pass.
    class DefaultSceneRenderer_MaterialPassCallback : public SceneCullingManager::VisibleCallback
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_MaterialPassCallback(DefaultSceneRenderer &rendererIn)
            : SceneCullingManager::VisibleCallback(), renderer(rendererIn)
        {
        }

        /// Destructor.
        ~DefaultSceneRenderer_MaterialPassCallback()
        {
        }


        /// SceneCullingManager::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object)
        {
            this->renderer.__MaterialPass_Model(object);
        }


        /// SceneCullingManager::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(SceneObject &object)
        {
            this->renderer.__AmbientLight(object);
        }

        /// SceneCullingManager::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(SceneObject &object)
        {
            this->renderer.__DirectionalLight(object);
        }

        /// SceneCullingManager::ProcessObjectPointLight().
        void ProcessObjectPointLight(SceneObject &object)
        {
            this->renderer.__PointLight(object);
        }

        /// SceneCullingManager::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(SceneObject &object)
        {
            this->renderer.__SpotLight(object);
        }



    private:

        /// The renderer that owns this callback.
        DefaultSceneRenderer &renderer;


    private:    // No copying.
        DefaultSceneRenderer_MaterialPassCallback(const DefaultSceneRenderer_MaterialPassCallback &);
        DefaultSceneRenderer_MaterialPassCallback & operator=(const DefaultSceneRenderer_MaterialPassCallback &);
    };



    // Culling callback for shadow map passes.
    class DefaultSceneRenderer_ShadowPassCallback : public SceneCullingManager::VisibleCallback
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_ShadowPassCallback(DefaultSceneRenderer &rendererIn, const glm::mat4 &projection, const glm::mat4 &view)
            : SceneCullingManager::VisibleCallback(), renderer(rendererIn), projection(projection), view(view)
        {
        }

        /// Destructor.
        ~DefaultSceneRenderer_ShadowPassCallback()
        {
        }


        /// SceneCullingManager::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object)
        {
            this->renderer.__ShadowPass_Model(object, projection, view);
        }


        /// SceneCullingManager::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(SceneObject &)
        {
        }

        /// SceneCullingManager::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(SceneObject &)
        {
        }

        /// SceneCullingManager::ProcessObjectPointLight().
        void ProcessObjectPointLight(SceneObject &)
        {
        }

        /// SceneCullingManager::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(SceneObject &)
        {
        }



    private:

        /// The renderer that owns this callback.
        DefaultSceneRenderer &renderer;

        glm::mat4 projection;
        glm::mat4 view;


    private:    // No copying.
        DefaultSceneRenderer_ShadowPassCallback(const DefaultSceneRenderer_ShadowPassCallback &);
        DefaultSceneRenderer_ShadowPassCallback & operator=(const DefaultSceneRenderer_ShadowPassCallback &);
    };
}


namespace GTEngine
{
    DefaultSceneRenderer::DefaultSceneRenderer()
        : ambientLights(), directionalLights(), pointLights(), spotLights(),
          directionalLights_NoShadows(), pointLights_NoShadows(), spotLights_NoShadows(),
          viewportFramebuffers(),
          clearColourBuffer(true), clearColour(0.25f, 0.25f, 0.25f),
          rcBegin(),
          Shaders(),
          materialMetadatas(),
          pointLightShadowMap(), pointLightShadowMapDepthBuffer(), pointLightShadowMapFramebuffer()
    {
        unsigned int shadowMapSize = SHADOW_MAP_SIZE;
        pointLightShadowMap.PositiveX->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMap.NegativeX->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMap.PositiveY->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMap.NegativeY->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMap.PositiveZ->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMap.NegativeZ->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        pointLightShadowMapDepthBuffer.SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_Depth24_Stencil8);

        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveX, 0);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeX, 1);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveY, 2);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeY, 3);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveZ, 4);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeZ, 5);
        this->pointLightShadowMapFramebuffer.AttachDepthStencilBuffer(&pointLightShadowMapDepthBuffer);
        this->pointLightShadowMapFramebuffer.CheckStatus();


        this->Shaders.Lighting_NoShadow_A1           = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_NoShadow_A1");
        this->Shaders.Lighting_NoShadow_D1           = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_NoShadow_D1");
        this->Shaders.Lighting_NoShadow_P1           = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_NoShadow_P1");
        this->Shaders.Lighting_P1                    = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_P1");
        this->Shaders.Lighting_NoShadow_S1           = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_NoShadow_S1");
        this->Shaders.Lighting_ShadowMap             = ShaderLibrary::Acquire("Engine_ShadowVS",          "Engine_LightingPass_ShadowMap");
        this->Shaders.Compositor_DiffuseOnly         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_Compositor_DiffuseOnly");
        this->Shaders.Compositor_NormalsOnly         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_Compositor_NormalsOnly");
        this->Shaders.Compositor_DiffuseLightingOnly = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_Compositor_DiffuseLightingOnly");
        this->Shaders.Compositor_FinalOutput         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_Compositor_FinalOutput");

        this->Shaders.Lighting_P1->SetParameter("ShadowMap", &this->pointLightShadowMap);
    }

    DefaultSceneRenderer::~DefaultSceneRenderer()
    {
        // We need to remove all of the framebuffers.
        for (size_t i = 0; i < this->viewportFramebuffers.count; ++i)
        {
            delete this->viewportFramebuffers.buffer[i]->value;
        }

        // Shaders need to be unacquired
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_A1);
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_D1);
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_P1);
        ShaderLibrary::Unacquire(Shaders.Lighting_P1);
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_S1);
        ShaderLibrary::Unacquire(Shaders.Lighting_ShadowMap);
        ShaderLibrary::Unacquire(Shaders.Compositor_DiffuseOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_NormalsOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_DiffuseLightingOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_FinalOutput);

        /// The material shaders need to be deleted.
        auto &shaders = this->Shaders.materialPassShaders.GetShaders();
        for (size_t i = 0; i < shaders.count; ++i)
        {
            delete shaders.buffer[i]->value;
        }

        /// We need to make sure any existing material metadata is removed for tidyness.
        while (this->materialMetadatas.root != nullptr)
        {
            delete this->materialMetadatas.root->value;
            this->materialMetadatas.RemoveRoot();
        }
    }


    void DefaultSceneRenderer::Begin(Scene &)
    {
        this->rcBegin[Renderer::BackIndex].Reset();
        this->rcEnd[Renderer::BackIndex].Reset();
        this->rcBeginLayer[Renderer::BackIndex].Reset();
        this->rcEndLayer[Renderer::BackIndex].Reset();
        this->rcDrawVA[Renderer::BackIndex].Reset();
        this->rcSetFaceCulling[Renderer::BackIndex].Reset();
        this->rcBeginLighting[Renderer::BackIndex].Reset();
        this->rcControlBlending[Renderer::BackIndex].Reset();
        this->rcLighting_SetShader[Renderer::BackIndex].Reset();
        this->rcLighting_DrawGeometry[Renderer::BackIndex].Reset();
        this->rcLighting_BeginPointShadowMap[Renderer::BackIndex].Reset();
        this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Reset();
        this->rcLighting_EndShadowMap[Renderer::BackIndex].Reset();
        this->rcLighting_DrawShadowPassGeometry[Renderer::BackIndex].Reset();
    }

    void DefaultSceneRenderer::End(Scene &)
    {
    }


    void DefaultSceneRenderer::RenderViewport(Scene &scene, SceneViewport &viewport)
    {
        // TODO: Should be able remove these calls once lighting is done properly.
        this->ambientLights.Clear();
        this->directionalLights.Clear();
        this->pointLights.Clear();
        this->spotLights.Clear();

        this->directionalLights_NoShadows.Clear();
        this->pointLights_NoShadows.Clear();
        this->spotLights_NoShadows.Clear();



        this->usedMaterials.Clear();
        this->lightingDrawRCs.Clear();


        // We render layer-by-layer, starting at the top and working our way down.
        auto &layers = viewport.GetLayerCameraMap();
        if (layers.count > 0)
        {
            auto framebuffer = this->GetViewportFramebuffer(viewport);

            // The first thing we do is set the framebuffer to the current viewport. This also where the framebuffer will be resized if required.
            auto &rcBegin = this->rcBegin[Renderer::BackIndex].Acquire();
            rcBegin.framebuffer = framebuffer;
            Renderer::BackRCQueue->Append(rcBegin);


            for (size_t i = layers.count; i > 0; --i)
            {
                auto cameraNode = layers.buffer[i - 1]->value;
                if (cameraNode != nullptr)
                {
                    auto camera = cameraNode->GetComponent<CameraComponent>();
                    assert(camera != nullptr);

                    this->projection = camera->GetProjectionMatrix();
                    this->view       = camera->GetViewMatrix();
                }
                else
                {
                    this->projection = glm::mat4();
                    this->view       = glm::mat4();
                }


                // We first need to begin the layer.
                auto &rcBeginLayer = this->rcBeginLayer[Renderer::BackIndex].Acquire();
                rcBeginLayer.isFirstLayer = (i == layers.count);
                Renderer::BackRCQueue->Append(rcBeginLayer);


                // We're going to start with the material pass. This pass will acquire the ambient lights that we'll use for the lighting pass.
                this->MaterialPass(scene);

                // The lighting pass must come after the material pass, since it depends on things like normals. Also, the material pass will retrieve the visible light objects.
                this->LightingPass(scene, *framebuffer);


                // Now we end the layer.
                auto &rcEndLayer = this->rcEndLayer[Renderer::BackIndex].Acquire();
                rcEndLayer.framebuffer       = framebuffer;
                rcEndLayer.compositingShader = this->Shaders.Compositor_FinalOutput;
                Renderer::BackRCQueue->Append(rcEndLayer);
            }


            // At this point the viewport will be finished and we can finish up.
            auto &rcEnd = this->rcEnd[Renderer::BackIndex].Acquire();
            rcEnd.drawBackground   = this->clearColourBuffer;
            rcEnd.backgroundColour = this->clearColour;
            Renderer::BackRCQueue->Append(rcEnd);
        }
    }


    void DefaultSceneRenderer::AddViewport(SceneViewport &viewport)
    {
        auto framebuffer = new DefaultSceneRenderer::Framebuffer;
        viewport.__SetColourBuffer(framebuffer->GetFinalOutputColourBuffer());

        this->viewportFramebuffers.Add(&viewport, framebuffer);
    }

    void DefaultSceneRenderer::RemoveViewport(SceneViewport &viewport)
    {
        auto framebuffer = this->GetViewportFramebuffer(viewport);

        viewport.__SetColourBuffer(nullptr);
        this->viewportFramebuffers.Remove(&viewport);

        delete framebuffer;
    }

    void DefaultSceneRenderer::OnViewportResized(SceneViewport &viewport)
    {
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);

        framebuffer->Resize(viewport.GetWidth(), viewport.GetHeight());
    }


    void DefaultSceneRenderer::EnableBackgroundColourClearing(float r, float g, float b)
    {
        this->clearColourBuffer = true;
        this->clearColour       = glm::vec3(r, g, b);
    }

    void DefaultSceneRenderer::DisableBackgroundColourClearing()
    {
        this->clearColourBuffer = false;
    }


    void DefaultSceneRenderer::__MaterialPass_Model(const SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto modelComponent = static_cast<const SceneNode &>(object).GetComponent<GTEngine::ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelMatrix     = modelComponent->GetNode().GetWorldTransform();
                glm::mat4 ModelViewMatrix = this->view * ModelMatrix;
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    auto material = mesh->GetMaterial();
                    if (material != nullptr)
                    {
                        auto meshVA = this->GetMeshGeometry(*mesh, model->IsAnimating());

                        auto &rcDrawVA = this->rcDrawVA[Renderer::BackIndex].Acquire();
                        rcDrawVA.SetVertexArray(meshVA);
                        rcDrawVA.SetParameter("MVPMatrix",       MVPMatrix);
                        rcDrawVA.SetParameter("NormalMatrix",    NormalMatrix);
                        rcDrawVA.SetParameter("ModelViewMatrix", ModelViewMatrix);
                        rcDrawVA.SetParameter("ModelMatrix",     ModelMatrix);

                        
                        // This is the pass where the animated geometry needs to be applied, if applicable. To do this, we first check that
                        // the model is being animated. If so, we need to create a skinned geometry vertex array and calculate the blended
                        // vertex information.
                        if (model->IsAnimating())
                        {
                            auto skinnedGeometry = mesh->GetSkinnedGeometry();
                            if (skinnedGeometry != nullptr)
                            {
                                // Now that we have skinned geometry vertex array, we need to apply the skinning to it.
                                mesh->ApplySkinning();
                            }

                            rcDrawVA.SetVertexArray(skinnedGeometry);
                        }
                        else
                        {
                            rcDrawVA.SetVertexArray(mesh->GetGeometry());
                        }


                        // Here is where we need to retrieve a shader for the material. This managed completely by the renderer
                        auto &materialMetadata = this->GetMaterialMetadata(*material);

                        // If we don't have a shader, it needs to be created.
                        if (materialMetadata.materialPassShader == nullptr)
                        {
                            materialMetadata.materialPassShader = this->CreateMaterialPassShader(*material);
                        }

                        this->usedMaterials.Insert(&materialMetadata);


                        // Now that we have the shader, we can set some properties and set it on the render command.
                        rcDrawVA.SetShader(materialMetadata.materialPassShader);

                        // The material may have pending properties. These need to be set on the shader also.
                        auto &materialParams = material->GetParameters();
                        for (size_t iProperty = 0; iProperty < materialParams.count; ++iProperty)
                        {
                            auto iParam = materialParams.buffer[iProperty];
                            assert(iParam        != nullptr);
                            assert(iParam->value != nullptr);

                            rcDrawVA.SetParameter(iParam->key, iParam->value);
                        }

                        auto &rcSetFaceCulling = this->rcSetFaceCulling[Renderer::BackIndex].Acquire();
                        rcSetFaceCulling.SetCullingMode(modelComponent->CullFrontFaces(), modelComponent->CullBackFaces());

                        // Here is where we determine where the render commands are added. We don't append the commands straight onto the renderer directly. Instead, we just
                        // append to local RC queues which will be mapped to a material definition. At the end, we just append those local queues to the main one. This is
                        // how we group commands by material.
                        
                        materialMetadata.materialPassRCs.Append(rcSetFaceCulling);
                        materialMetadata.materialPassRCs.Append(rcDrawVA);


                        // Here we need to create the render command that will draw the geometry of the mesh in each of the lighting passes.
                        auto &rcLighting_DrawGeometry = this->rcLighting_DrawGeometry[Renderer::BackIndex].Acquire();
                        rcLighting_DrawGeometry.va                = meshVA;
                        rcLighting_DrawGeometry.mvpMatrix         = MVPMatrix;
                        rcLighting_DrawGeometry.normalMatrix      = NormalMatrix;
                        rcLighting_DrawGeometry.modelViewMatrix   = ModelViewMatrix;
                        rcLighting_DrawGeometry.modelMatrix       = ModelMatrix;
                        rcLighting_DrawGeometry.changeFaceCulling = !(modelComponent->CullBackFaces() == true && modelComponent->CullFrontFaces() == false);
                        rcLighting_DrawGeometry.cullBackFace      = modelComponent->CullBackFaces();
                        rcLighting_DrawGeometry.cullFrontFace     = modelComponent->CullFrontFaces();
                        this->lightingDrawRCs.Append(rcLighting_DrawGeometry);
                    }
                }
            }
        }
    }

    void DefaultSceneRenderer::__ShadowPass_Model(const SceneObject &object, const glm::mat4 &projection, const glm::mat4 &view)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto modelComponent = static_cast<const SceneNode &>(object).GetComponent<GTEngine::ModelComponent>();
            assert(modelComponent != nullptr);

            if (modelComponent->IsShadowCastingEnabled())
            {
                auto model = modelComponent->GetModel();
                if (model != nullptr)
                {
                    glm::mat4 ModelViewMatrix = view       * modelComponent->GetNode().GetWorldTransform();
                    glm::mat4 MVPMatrix       = projection * ModelViewMatrix;

                    for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                    {
                        auto mesh = model->meshes[iMesh];
                        assert(mesh != nullptr);

                        auto &rcDrawGeometry = this->rcLighting_DrawShadowPassGeometry[Renderer::BackIndex].Acquire();
                        rcDrawGeometry.va              = this->GetMeshGeometry(*mesh, model->IsAnimating());
                        rcDrawGeometry.mvpMatrix       = MVPMatrix;
                        rcDrawGeometry.modelViewMatrix = ModelViewMatrix;
                        Renderer::BackRCQueue->Append(rcDrawGeometry);
                    }
                }
            }
        }
    }

    void DefaultSceneRenderer::__AmbientLight(const SceneObject &object)
    {
        this->ambientLights.PushBack(&object);
    }

    void DefaultSceneRenderer::__DirectionalLight(const SceneObject &object)
    {
        this->directionalLights_NoShadows.PushBack(&object);
    }

    void DefaultSceneRenderer::__PointLight(const SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            if (static_cast<const SceneNode &>(object).GetComponent<PointLightComponent>()->IsShadowCastingEnabled())
            {
                this->pointLights.PushBack(&object);
            }
            else
            {
                this->pointLights_NoShadows.PushBack(&object);
            }
        }
    }

    void DefaultSceneRenderer::__SpotLight(const SceneObject &object)
    {
        this->spotLights_NoShadows.PushBack(&object);
    }



    ///////////////////////////////////////////////
    // Private

    DefaultSceneRenderer::Framebuffer* DefaultSceneRenderer::GetViewportFramebuffer(SceneViewport &viewport)
    {
        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);

        return iFramebuffer->value;
    }


    void DefaultSceneRenderer::MaterialPass(Scene &scene)
    {
        DefaultSceneRenderer_MaterialPassCallback materialPassCallback(*this);
        scene.QueryVisibleObjects(this->projection * this->view, materialPassCallback);

        // We will have render commands waiting to be added to the main RC queue. This is where we should do this.
        while (this->usedMaterials.root != nullptr)
        {
            auto &queue = this->usedMaterials.root->value->materialPassRCs;

            Renderer::BackRCQueue->Append(queue);
            queue.Clear();

            this->usedMaterials.RemoveRoot();
        }
    }

    void DefaultSceneRenderer::LightingPass(Scene &scene, DefaultSceneRenderer::Framebuffer &framebuffer)
    {
        // We begin with the lights that are not casting shadows. We can do an optimized pass here where we can group lights into a single pass.
        auto &rcBeginLighting = this->rcBeginLighting[Renderer::BackIndex].Acquire();
        rcBeginLighting.framebuffer = &framebuffer;
        Renderer::BackRCQueue->Append(rcBeginLighting);



        while (this->ambientLights.count > 0)
        {
            auto &light = *this->ambientLights.GetBack();

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_A1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                rcSetShader.SetParameter("ALights0.Colour", static_cast<const SceneNode &>(light).GetComponent<GTEngine::AmbientLightComponent>()->GetColour());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            Renderer::BackRCQueue->Append(this->lightingDrawRCs);


            this->ambientLights.PopBack();
        }

        while (this->directionalLights_NoShadows.count > 0)
        {
            auto &light = *this->directionalLights_NoShadows.GetBack();

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_D1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::DirectionalLightComponent>();

                rcSetShader.SetParameter("DLights0.Colour",    component->GetColour());
                rcSetShader.SetParameter("DLights0.Direction", glm::normalize(glm::mat3(this->view) * component->GetNode().GetWorldForwardVector()));
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            Renderer::BackRCQueue->Append(this->lightingDrawRCs);


            this->directionalLights_NoShadows.PopBack();
        }

        while (this->pointLights_NoShadows.count > 0)
        {
            auto &light = *this->pointLights_NoShadows.GetBack();

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_P1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::PointLightComponent>();

                rcSetShader.SetParameter("PLight0.Position",             glm::vec3(this->view * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("PLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("PLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("PLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("PLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            Renderer::BackRCQueue->Append(this->lightingDrawRCs);


            this->pointLights_NoShadows.PopBack();
        }

        while (this->spotLights_NoShadows.count > 0)
        {
            auto &light = *this->spotLights_NoShadows.GetBack();

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_S1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::SpotLightComponent>();

                rcSetShader.SetParameter("SLight0.Position",             glm::vec3(this->view * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("SLight0.Direction",            glm::normalize(glm::mat3(this->view) * component->GetNode().GetWorldForwardVector()));
                rcSetShader.SetParameter("SLight0.CosAngleInner",        glm::cos(glm::radians(component->GetInnerAngle())));
                rcSetShader.SetParameter("SLight0.CosAngleOuter",        glm::cos(glm::radians(component->GetOuterAngle())));
                rcSetShader.SetParameter("SLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("SLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("SLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("SLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            Renderer::BackRCQueue->Append(this->lightingDrawRCs);


            this->spotLights_NoShadows.PopBack();
        }



        // With the shadowless lights done, we can now do the lights with shadow.
        while (this->pointLights.count > 0)
        {
            auto &light = *this->pointLights.GetBack();

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_P1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::PointLightComponent>();


                // This will build the light's shadow map.
                this->LightingPass_BuildPointLightShadowMap(scene, framebuffer, component->GetNode().GetWorldPosition(), component->GetApproximateRadius());



                rcSetShader.SetParameter("PLight0.Position",             glm::vec3(this->view * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("PLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("PLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("PLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("PLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());

                rcSetShader.SetParameter("PLight0_PositionWS",           component->GetNode().GetWorldPosition());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            Renderer::BackRCQueue->Append(this->lightingDrawRCs);


            this->pointLights.PopBack();
        }
    }

    void DefaultSceneRenderer::LightingPass_BuildPointLightShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::vec3 &position, float radius)
    {
        glm::mat4 positiveXViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);;
        glm::mat4 negativeXViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);;
        glm::mat4 positiveYViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-position);;
        glm::mat4 negativeYViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-position);;
        glm::mat4 positiveZViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(  0.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);;
        glm::mat4 negativeZViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);;
        
        glm::mat4 projectionMatrix = glm::perspective(90.0f, 1.0f, 0.1f, radius);



        auto &rcBeginPointShadowMap = this->rcLighting_BeginPointShadowMap[Renderer::BackIndex].Acquire();
        rcBeginPointShadowMap.shader      = this->Shaders.Lighting_ShadowMap;
        rcBeginPointShadowMap.framebuffer = &this->pointLightShadowMapFramebuffer;
        Renderer::BackRCQueue->Append(rcBeginPointShadowMap);



        // Positive X
        auto &rcBeginPositiveX = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginPositiveX.colourBufferIndex = 0;
        Renderer::BackRCQueue->Append(rcBeginPositiveX);

        DefaultSceneRenderer_ShadowPassCallback positiveXCallback(*this, projectionMatrix, positiveXViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * positiveXViewMatrix, positiveXCallback);


        // Negative X
        auto &rcBeginNegativeX = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginNegativeX.colourBufferIndex = 1;
        Renderer::BackRCQueue->Append(rcBeginNegativeX);

        DefaultSceneRenderer_ShadowPassCallback negativeXCallback(*this, projectionMatrix, negativeXViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * negativeXViewMatrix, negativeXCallback);

        

        // Positive Y
        auto &rcBeginPositiveY = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginPositiveY.colourBufferIndex = 2;
        Renderer::BackRCQueue->Append(rcBeginPositiveY);

        DefaultSceneRenderer_ShadowPassCallback positiveYCallback(*this, projectionMatrix, positiveYViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * positiveYViewMatrix, positiveYCallback);


        // Negative Y
        auto &rcBeginNegativeY = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginNegativeY.colourBufferIndex = 3;
        Renderer::BackRCQueue->Append(rcBeginNegativeY);

        DefaultSceneRenderer_ShadowPassCallback negativeYCallback(*this, projectionMatrix, negativeYViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * negativeYViewMatrix, negativeYCallback);



        // Positive Z
        auto &rcBeginPositiveZ = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginPositiveZ.colourBufferIndex = 4;
        Renderer::BackRCQueue->Append(rcBeginPositiveZ);

        DefaultSceneRenderer_ShadowPassCallback positiveZCallback(*this, projectionMatrix, positiveZViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * positiveZViewMatrix, positiveZCallback);


        // Negative Z
        auto &rcBeginNegativeZ = this->rcLighting_BeginPointShadowMapFace[Renderer::BackIndex].Acquire();
        rcBeginNegativeZ.colourBufferIndex = 5;
        Renderer::BackRCQueue->Append(rcBeginNegativeZ);

        DefaultSceneRenderer_ShadowPassCallback negativeZCallback(*this, projectionMatrix, negativeZViewMatrix);
        scene.QueryVisibleObjects(projectionMatrix * negativeZViewMatrix, negativeZCallback);



        // Now we need to mark the shadow map generation as complete.
        auto &rcEndShadowMap = this->rcLighting_EndShadowMap[Renderer::BackIndex].Acquire();
        rcEndShadowMap.newFramebuffer = &mainFramebuffer;
        Renderer::BackRCQueue->Append(rcEndShadowMap);
    }



    DefaultSceneRenderer::MaterialMetadata & DefaultSceneRenderer::GetMaterialMetadata(Material &material)
    {
        auto &definition = const_cast<MaterialDefinition &>(material.GetDefinition());

        // The key for the metadata will be 'this'.
        auto metadata = static_cast<MaterialMetadata*>(definition.GetMetadata(reinterpret_cast<size_t>(this)));
        if (metadata == nullptr)
        {
            metadata = new MaterialMetadata;
            definition.SetMetadata(reinterpret_cast<size_t>(this), metadata);

            this->materialMetadatas.Append(metadata);
        }

        return *metadata;
    }

    /*
    void DefaultSceneRenderer::DeleteMaterialMetadata(Material &material)
    {
        auto &definition = const_cast<MaterialDefinition &>(material.GetDefinition());

        auto metadata = static_cast<MaterialMetadata*>(definition.GetMetadata(reinterpret_cast<size_t>(this)));
        if (metadata == nullptr)
        {
            this->materialMetadatas.Remove(this->materialMetadatas.Find(metadata));
            delete metadata;
        }
    }
    */



    Shader* DefaultSceneRenderer::CreateMaterialPassShader(Material &material)
    {
        auto &materialDefinition = material.GetDefinition();

        // If a shader already exists for this material, we just reuse that.
        Shader* shader = this->Shaders.materialPassShaders.GetShader(materialDefinition);
        if (shader == nullptr)
        {
            // If we get here, it means the shader does not exist. We need to create it and then add it to the cache.
            // There are two parts to the shader. There is the base shader and then the material components.
            const char* baseShaderVS      = ShaderLibrary::GetShaderString("Engine_MaterialPass_VS");
            const char* baseShaderFS      = ShaderLibrary::GetShaderString("Engine_MaterialPass_FS");
            const char* materialDiffuse   = ShaderLibrary::GetShaderString(material.GetDiffuseShaderID());
            const char* materialEmissive  = ShaderLibrary::GetShaderString(material.GetEmissiveShaderID());
            const char* materialShininess = ShaderLibrary::GetShaderString(material.GetShininessShaderID());
            const char* materialNormal    = ShaderLibrary::GetShaderString(material.GetNormalShaderID());

            // With the shader strings retrieved, we need to concatinate the shaders and create the shader object.
            GTCore::Strings::List<char> fragmentShaderString;
            fragmentShaderString.Append(baseShaderFS);
            fragmentShaderString.Append("\n");
            fragmentShaderString.Append(materialDiffuse);
            fragmentShaderString.Append("\n");
            fragmentShaderString.Append(materialEmissive);
            fragmentShaderString.Append("\n");
            fragmentShaderString.Append(materialShininess);
            fragmentShaderString.Append("\n");
            fragmentShaderString.Append(materialNormal);

            shader = new Shader(baseShaderVS, fragmentShaderString.c_str());

            // With the shader created, we now add it to the cache.
            this->Shaders.materialPassShaders.SetShader(materialDefinition, *shader);
        }

        return shader;
    }

    VertexArray* DefaultSceneRenderer::GetMeshGeometry(Mesh &mesh, bool animating)
    {
        if (animating)
        {
            auto skinnedGeometry = mesh.GetSkinnedGeometry();
            if (skinnedGeometry != nullptr)
            {
                return skinnedGeometry;
            }
        }
        
        return mesh.GetGeometry();
    }




    ///////////////////////////////////////////////
    // Render Commands

    void DefaultSceneRenderer::RCBegin::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
        Renderer::SetViewport(0, 0, this->framebuffer->GetWidth(), this->framebuffer->GetHeight());
    }

    void DefaultSceneRenderer::RCEnd::Execute()
    {
        // If we are drawing a background colour, we will draw a solid colour full screen quad. We should be in a good rendering state
        // here with depth testing and writing disabled and stencil testing enabled.
        if (this->drawBackground)
        {
            Renderer::SetStencilFunc(RendererFunction_Equal, 0, 255);
            Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);

            Renderer::SetShader(ShaderLibrary::GetColouredBGQuadShader());
            Renderer::SetShaderParameter("Colour", this->backgroundColour);

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
        }

        // The default state is stencil testing disabled. Thus, we need to disable here.
        Renderer::DisableStencilTest();
    }

    void DefaultSceneRenderer::RCBeginLayer::Execute()
    {
        int drawBuffers[] = {1, 2, 3};      // Material Buffers 0/1/2
        Renderer::SetDrawBuffers(3, drawBuffers);

        // The first thing we need to do is clear the depth and stencil buffer, and also the colour buffer if applicable.
        unsigned int clearbuffers = GTEngine::DepthBuffer;
        if (this->isFirstLayer)
        {
            clearbuffers |= GTEngine::StencilBuffer;
        }

        Renderer::EnableDepthTest();
        Renderer::SetDepthFunc(RendererFunction_LEqual);
        Renderer::EnableDepthWrites();
        
        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_GEqual, 1, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Replace);

        Renderer::ClearDepth(1.0f);
        Renderer::ClearStencil(0);
        Renderer::Clear(clearbuffers);
    }

    void DefaultSceneRenderer::RCEndLayer::Execute()
    {
        assert(this->framebuffer != nullptr);


        int drawBuffers[] = {0};        // Final output buffer.
        Renderer::SetDrawBuffers(1, drawBuffers);

        Renderer::SetShader(this->compositingShader);
        Renderer::SetShaderParameter("LightingBuffer0", this->framebuffer->lightingBuffer0);
        Renderer::SetShaderParameter("LightingBuffer1", this->framebuffer->lightingBuffer1);
        Renderer::SetShaderParameter("MaterialBuffer0", this->framebuffer->materialBuffer0);
        Renderer::SetShaderParameter("MaterialBuffer1", this->framebuffer->materialBuffer1);
        Renderer::SetShaderParameter("MaterialBuffer2", this->framebuffer->materialBuffer2);

        Renderer::DisableDepthTest();
        Renderer::DisableBlending();
        Renderer::SetFaceCulling(false, true);

        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_Equal, 1, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Increment);
        
        Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
    }

    void DefaultSceneRenderer::RCBeginLighting::Execute()
    {
        assert(this->framebuffer != nullptr);

        int drawBuffers[] = {4, 5};     // Lighting Buffers 0/1
        Renderer::SetDrawBuffers(2, drawBuffers);

        // Clearing to black is important here.
        Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::Clear(GTEngine::ColourBuffer);

        Renderer::SetDepthFunc(RendererFunction_Equal);
        Renderer::DisableDepthWrites();

        // We combine lighting passes using standard blending.
        Renderer::EnableBlending();
        Renderer::SetBlendEquation(BlendEquation_Add);
        Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
    }

    void DefaultSceneRenderer::RCControlBlending::Execute()
    {
        if (this->enable)
        {
            Renderer::EnableBlending();
            Renderer::SetBlendFunc(this->sourceFactor, this->destFactor);
        }
        else
        {
            Renderer::DisableBlending();
        }
    }

    void DefaultSceneRenderer::RCLighting_SetShader::Execute()
    {
        Renderer::SetShader(this->shader);
        Renderer::SetShaderParameter("Lighting_Normals", this->materialBuffer2);
        Renderer::SetShaderParameter("ScreenSize",       this->screenSize);

        for (size_t i = 0; i < this->parameters.GetCount(); ++i)
        {
            auto iParamName  = this->parameters.GetNameByIndex(i);
            auto iParamValue = this->parameters.GetByIndex(i);

            iParamValue->SetOnCurrentShader(iParamName);
        }
    }

    void DefaultSceneRenderer::RCLighting_DrawGeometry::Execute()
    {
        if (this->changeFaceCulling)
        {
            Renderer::SetFaceCulling(this->cullFrontFace, this->cullBackFace);
        }

        Renderer::SetShaderParameter("MVPMatrix",       this->mvpMatrix);
        Renderer::SetShaderParameter("NormalMatrix",    this->normalMatrix);
        Renderer::SetShaderParameter("ModelViewMatrix", this->modelViewMatrix);
        Renderer::SetShaderParameter("ModelMatrix",     this->modelMatrix);
        Renderer::Draw(this->va);


        // Default back to back-face culling.
        if (this->changeFaceCulling)
        {
            Renderer::SetFaceCulling(false, true);
        }
    }


    void DefaultSceneRenderer::RCLighting_BeginPointShadowMap::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
        Renderer::SetShader(this->shader);
        Renderer::SetViewport(0, 0, this->framebuffer->GetColourBuffer(0)->GetWidth(), this->framebuffer->GetColourBuffer(0)->GetHeight());
    }

    void DefaultSceneRenderer::RCLighting_BeginPointShadowMapFace::Execute()
    {
        int drawBuffers[] = {this->colourBufferIndex};
        Renderer::SetDrawBuffers(1, drawBuffers);

        Renderer::DisableBlending();

        Renderer::SetDepthFunc(RendererFunction_LEqual);
        Renderer::EnableDepthWrites();

        Renderer::DisableStencilTest();

        Renderer::ClearDepth(1.0f);
        Renderer::Clear(GTEngine::DepthBuffer);
    }

    void DefaultSceneRenderer::RCLighting_EndShadowMap::Execute()
    {
        Renderer::SetFramebuffer(this->newFramebuffer);
        Renderer::SetViewport(0, 0, this->newFramebuffer->width, this->newFramebuffer->height);
        
        Renderer::EnableBlending();

        Renderer::SetDepthFunc(RendererFunction_Equal);
        Renderer::DisableDepthWrites();

        Renderer::EnableStencilTest();
    }

    void DefaultSceneRenderer::RCLighting_DrawShadowPassGeometry::Execute()
    {
        Renderer::SetShaderParameter("MVPMatrix",       this->mvpMatrix);
        Renderer::SetShaderParameter("ModelViewMatrix", this->modelViewMatrix);

        Renderer::Draw(this->va);
    }
}