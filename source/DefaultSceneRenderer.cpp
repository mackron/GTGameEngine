
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
        DefaultSceneRenderer_MaterialPassCallback(DefaultSceneRenderer &rendererIn, const glm::mat4 &cameraProjectionIn, const glm::mat4 &cameraViewIn, DefaultSceneRenderer::LayerState &layerStateIn, bool opaquePassIn)
            : SceneCullingManager::VisibleCallback(), renderer(rendererIn),
              cameraProjection(cameraProjectionIn), cameraView(cameraViewIn),
              layerState(layerStateIn),
              opaquePass(opaquePassIn)
        {
        }

        /// Destructor.
        ~DefaultSceneRenderer_MaterialPassCallback()
        {
        }


        /// SceneCullingManager::ProcessObjectModel().
        void ProcessObjectModel(SceneObject &object)
        {
            this->renderer.__MaterialPass_Model(object, this->cameraProjection, this->cameraView, this->layerState);
        }


        /// SceneCullingManager::ProcessObjectAmbientLight().
        void ProcessObjectAmbientLight(SceneObject &object)
        {
            this->layerState.ambientLights.PushBack(&object);
        }

        /// SceneCullingManager::ProcessObjectDirectionalLight().
        void ProcessObjectDirectionalLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                if (static_cast<const SceneNode &>(object).GetComponent<DirectionalLightComponent>()->IsShadowCastingEnabled())
                {
                    this->layerState.directionalLights.PushBack(&object);
                }
                else
                {
                    this->layerState.directionalLights_NoShadows.PushBack(&object);
                }
            }
        }

        /// SceneCullingManager::ProcessObjectPointLight().
        void ProcessObjectPointLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                if (static_cast<const SceneNode &>(object).GetComponent<PointLightComponent>()->IsShadowCastingEnabled())
                {
                    this->layerState.pointLights.PushBack(&object);
                }
                else
                {
                    this->layerState.pointLights_NoShadows.PushBack(&object);
                }
            }
        }

        /// SceneCullingManager::ProcessObjectSpotLight().
        void ProcessObjectSpotLight(SceneObject &object)
        {
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                if (static_cast<const SceneNode &>(object).GetComponent<SpotLightComponent>()->IsShadowCastingEnabled())
                {
                    this->layerState.spotLights.PushBack(&object);
                }
                else
                {
                    this->layerState.spotLights_NoShadows.PushBack(&object);
                }
            }
        }



    private:

        /// The renderer that owns this callback.
        DefaultSceneRenderer &renderer;

        /// The projection matrix of the camera.
        const glm::mat4 &cameraProjection;

        /// The view matrix of the camera.
        const glm::mat4 &cameraView;

        /// The layer state.
        DefaultSceneRenderer::LayerState &layerState;

        /// Whether or not we are rendering the opaque pass.
        bool opaquePass;


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
        : viewportFramebuffers(),
          clearColourBuffer(true), clearColour(0.25f, 0.25f, 0.25f),
          rcBegin(),
          Shaders(),
          materialMetadatas(),
          shadowMap(), shadowMapDepthBuffer(), shadowMapFramebuffer(), pointLightShadowMap(), pointLightShadowMapDepthBuffer(), pointLightShadowMapFramebuffer()
    {
        unsigned int shadowMapSize = SHADOW_MAP_SIZE;

        this->shadowMap.SetWrapMode(TextureWrapMode_Clamp);
        this->shadowMap.SetData(           shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->shadowMapDepthBuffer.SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_Depth24_Stencil8);
        
        this->shadowMapFramebuffer.AttachColourBuffer(&this->shadowMap, 0);
        this->shadowMapFramebuffer.AttachDepthStencilBuffer(&this->shadowMapDepthBuffer);
        this->shadowMapFramebuffer.CheckStatus();


        this->pointLightShadowMap.PositiveX->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMap.NegativeX->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMap.PositiveY->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMap.NegativeY->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMap.PositiveZ->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMap.NegativeZ->SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_R32F, nullptr);
        this->pointLightShadowMapDepthBuffer.SetData(shadowMapSize, shadowMapSize, GTImage::ImageFormat_Depth24_Stencil8);

        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveX, 0);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeX, 1);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveY, 2);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeY, 3);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.PositiveZ, 4);
        this->pointLightShadowMapFramebuffer.AttachColourBuffer(pointLightShadowMap.NegativeZ, 5);
        this->pointLightShadowMapFramebuffer.AttachDepthStencilBuffer(&pointLightShadowMapDepthBuffer);
        this->pointLightShadowMapFramebuffer.CheckStatus();


        this->Shaders.Lighting_NoShadow_A1           = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_NoShadow_A1");
        this->Shaders.Lighting_NoShadow_D1           = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_NoShadow_D1");
        this->Shaders.Lighting_D1                    = ShaderLibrary::Acquire("Engine_DefaultShadowVS",    "Engine_LightingPass_D1");
        this->Shaders.Lighting_NoShadow_P1           = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_NoShadow_P1");
        this->Shaders.Lighting_P1                    = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_P1");
        this->Shaders.Lighting_NoShadow_S1           = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_NoShadow_S1");
        this->Shaders.Lighting_S1                    = ShaderLibrary::Acquire("Engine_DefaultShadowVS",    "Engine_LightingPass_S1");
        this->Shaders.Lighting_ShadowMap             = ShaderLibrary::Acquire("Engine_ShadowVS",           "Engine_LightingPass_ShadowMap");
        this->Shaders.Lighting_PointLightShadowMap   = ShaderLibrary::Acquire("Engine_PointLightShadowVS", "Engine_LightingPass_PointLightShadowMap");
        this->Shaders.Lighting_ColourClear           = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_LightingPass_ColourClear");
        this->Shaders.Compositor_DiffuseOnly         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_Compositor_DiffuseOnly");
        this->Shaders.Compositor_NormalsOnly         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_Compositor_NormalsOnly");
        this->Shaders.Compositor_DiffuseLightingOnly = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_Compositor_DiffuseLightingOnly");
        this->Shaders.Compositor_OpaqueFinalOutput   = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_Compositor_OpaqueFinalOutput");
        this->Shaders.Compositor_FinalOutput         = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_Compositor_FinalOutput");
        this->Shaders.MaterialPass_ClearBackground   = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_MaterialPass_ClearBackground");

        this->Shaders.Lighting_D1->SetParameter("ShadowMap", &this->shadowMap);
        this->Shaders.Lighting_P1->SetParameter("ShadowMap", &this->pointLightShadowMap);
        this->Shaders.Lighting_S1->SetParameter("ShadowMap", &this->shadowMap);
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
        ShaderLibrary::Unacquire(Shaders.Lighting_D1);
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_P1);
        ShaderLibrary::Unacquire(Shaders.Lighting_P1);
        ShaderLibrary::Unacquire(Shaders.Lighting_NoShadow_S1);
        ShaderLibrary::Unacquire(Shaders.Lighting_S1);
        ShaderLibrary::Unacquire(Shaders.Lighting_ShadowMap);
        ShaderLibrary::Unacquire(Shaders.Lighting_PointLightShadowMap);
        ShaderLibrary::Unacquire(Shaders.Lighting_ColourClear);
        ShaderLibrary::Unacquire(Shaders.Compositor_DiffuseOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_NormalsOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_DiffuseLightingOnly);
        ShaderLibrary::Unacquire(Shaders.Compositor_OpaqueFinalOutput);
        ShaderLibrary::Unacquire(Shaders.MaterialPass_ClearBackground);

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
        this->rcBeginBackground[Renderer::BackIndex].Reset();
        this->rcBackgroundColourClear[Renderer::BackIndex].Reset();
        this->rcBuildFinalImage[Renderer::BackIndex].Reset();
        this->rcBeginLighting[Renderer::BackIndex].Reset();
        this->rcBeginTransparency[Renderer::BackIndex].Reset();
        this->rcBeginForegroundTransparency[Renderer::BackIndex].Reset();
        this->rcBeginTransparentMaterialPass[Renderer::BackIndex].Reset();
        this->rcControlBlending[Renderer::BackIndex].Reset();
        this->rcSetShader[Renderer::BackIndex].Reset();
        this->rcDrawGeometry[Renderer::BackIndex].Reset();
        this->rcLighting_SetShader[Renderer::BackIndex].Reset();
        this->rcLighting_BeginDirectionalShadowMap[Renderer::BackIndex].Reset();
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
        this->mainLayerState.Reset();
        this->backgroundLayerState.Reset();


        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);


        // Here is the general workflow of the renderer:
        //
        // 1) Clear depth to 1.0 and stencil to 0.
        // 2) Draw opaque objects.
        //      2a) Draw opaque objects in the main layer, setting a value of 1 in the stencil buffer.
        //      2b) Draw opaque objects in the background layer, ignoring fragments where the stencil buffer is 1 (from the main layer).
        // 3) Build a composited image from the opaque objects, which will then be used as input for transparent objects.
        // 4) Draw transparent objects.
        //      4a) Draw transparent objects in the background layer.
        //      4b) Clear the depth buffer where the background was drawn by drawing a fullscreen quad with colour writes disabled. This is required so that any transparent objects are drawn on top of the background layer.
        //      4c) Draw transparent objects in the main layer.
        // 5) Build the final image like normal.



        // The first thing to do is setup.
        auto &rcBegin = this->rcBegin[Renderer::BackIndex].Acquire();
        rcBegin.framebuffer = framebuffer;
        Renderer::BackRCQueue->Append(rcBegin);


        
        // Main layer, opaque.
        auto cameraNode = viewport.GetCameraNode(ViewportLayer::Main);
        if (cameraNode != nullptr)
        {
            auto camera = cameraNode->GetComponent<CameraComponent>();
            assert(camera != nullptr);

            auto &cameraProjection = camera->GetProjectionMatrix();
            auto  cameraView       = camera->GetViewMatrix();

            this->MaterialPass(scene, cameraProjection, cameraView, *framebuffer, this->mainLayerState, false);
            this->LightingPass(scene, cameraProjection, cameraView, *framebuffer, 1, this->mainLayerState, false);
        }


        
        // Now we begin the background.
        auto &rcBeginBackground = this->rcBeginBackground[Renderer::BackIndex].Acquire();
        Renderer::BackRCQueue->Append(rcBeginBackground);

        
        // Background clear colour, if applicable.
        if (this->clearColourBuffer)
        {
            auto &rcBackgroundColourClear = this->rcBackgroundColourClear[Renderer::BackIndex].Acquire();
            rcBackgroundColourClear.colour            = this->clearColour;
            rcBackgroundColourClear.colourClearShader = this->Shaders.MaterialPass_ClearBackground;
            Renderer::BackRCQueue->Append(rcBackgroundColourClear);
        }


        // Background layer, opaque.
        cameraNode = viewport.GetCameraNode(ViewportLayer::Background);
        if (cameraNode != nullptr)
        {
            auto camera = cameraNode->GetComponent<CameraComponent>();
            assert(camera != nullptr);

            auto &cameraProjection = camera->GetProjectionMatrix();
            auto  cameraView       = camera->GetViewMatrix();

            this->MaterialPass(scene, cameraProjection, cameraView, *framebuffer,  this->backgroundLayerState, false);
            this->LightingPass(scene, cameraProjection, cameraView, *framebuffer, 0, this->backgroundLayerState, false);
        }
        

        if (this->mainLayerState.HasRefractiveGeometry() || this->backgroundLayerState.HasRefractiveGeometry())
        {
            // Now we need to build an image that will be used for refractions.
            auto &rcBuildRefractionImage = this->rcBuildFinalImage[Renderer::BackIndex].Acquire();
            rcBuildRefractionImage.colourBufferIndex = 6;
            rcBuildRefractionImage.framebuffer       = framebuffer;
            rcBuildRefractionImage.compositingShader = this->Shaders.Compositor_OpaqueFinalOutput;
            Renderer::BackRCQueue->Append(rcBuildRefractionImage);


            // Now we need to mark the beginning of our transparency operations.
            auto &rcBeginTransparency = this->rcBeginTransparency[Renderer::BackIndex].Acquire();
            Renderer::BackRCQueue->Append(rcBeginTransparency);

            
            if (this->backgroundLayerState.HasRefractiveGeometry())
            {
                cameraNode = viewport.GetCameraNode(ViewportLayer::Background);
                if (cameraNode != nullptr)
                {
                    auto camera = cameraNode->GetComponent<CameraComponent>();
                    assert(camera != nullptr);

                    auto &cameraProjection = camera->GetProjectionMatrix();
                    auto  cameraView       = camera->GetViewMatrix();

                    this->MaterialPass(scene, cameraProjection, cameraView, *framebuffer, this->backgroundLayerState, true);
                    this->LightingPass(scene, cameraProjection, cameraView, *framebuffer, 0, this->backgroundLayerState, true);
                }
            }
            

            if (this->mainLayerState.HasRefractiveGeometry())
            {
                cameraNode = viewport.GetCameraNode(ViewportLayer::Main);
                if (cameraNode != nullptr)
                {
                    auto &rcBeginForegroundTransparency = this->rcBeginForegroundTransparency[Renderer::BackIndex].Acquire();
                    rcBeginForegroundTransparency.depthClearShader = ShaderLibrary::GetDepthClearShader();
                    Renderer::BackRCQueue->Append(rcBeginForegroundTransparency);


                    auto camera = cameraNode->GetComponent<CameraComponent>();
                    assert(camera != nullptr);

                    auto &cameraProjection = camera->GetProjectionMatrix();
                    auto  cameraView       = camera->GetViewMatrix();

                    this->MaterialPass(scene, cameraProjection, cameraView, *framebuffer, this->mainLayerState, true);
                    this->LightingPass(scene, cameraProjection, cameraView, *framebuffer, 1, this->mainLayerState, true);
                }
            }


            // Now we need to end the frame by simply building the final image and placing it in colour buffer 0.
            auto &rcBuildFinalImage = this->rcBuildFinalImage[Renderer::BackIndex].Acquire();
            rcBuildFinalImage.colourBufferIndex = 0;
            rcBuildFinalImage.framebuffer       = framebuffer;
            rcBuildFinalImage.compositingShader = this->Shaders.Compositor_FinalOutput;
            Renderer::BackRCQueue->Append(rcBuildFinalImage);
        }
        else
        {
            // Now we need to end the frame by simply building the final image and placing it in colour buffer 0.
            auto &rcBuildFinalImage = this->rcBuildFinalImage[Renderer::BackIndex].Acquire();
            rcBuildFinalImage.colourBufferIndex = 0;
            rcBuildFinalImage.framebuffer       = framebuffer;
            rcBuildFinalImage.compositingShader = this->Shaders.Compositor_OpaqueFinalOutput;
            Renderer::BackRCQueue->Append(rcBuildFinalImage);
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


    void DefaultSceneRenderer::__MaterialPass_Model(const SceneObject &object, const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, LayerState &state)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto modelComponent = static_cast<const SceneNode &>(object).GetComponent<GTEngine::ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr && modelComponent->IsModelVisible())
            {
                glm::mat4 ModelMatrix     = modelComponent->GetNode().GetWorldTransform();
                glm::mat4 ModelViewMatrix = cameraView * ModelMatrix;
                glm::mat4 MVPMatrix       = cameraProjection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    auto material = mesh->GetMaterial();
                    if (material != nullptr)
                    {
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
                        }


                        // Here is where we need to retrieve a shader for the material. This is stored as metadata. If the shader has not yet been created,
                        // it will be created now.
                        auto &materialMetadata = this->GetMaterialMetadata(*material);

                        if (materialMetadata.materialPassShader == nullptr)
                        {
                            materialMetadata.materialPassShader = this->CreateMaterialPassShader(*material);
                        }



                        auto &rcDrawGeometry = this->rcDrawGeometry[Renderer::BackIndex].Acquire();
                        rcDrawGeometry.va                = this->GetMeshGeometry(*mesh, model->IsAnimating());
                        rcDrawGeometry.mvpMatrix         = MVPMatrix;
                        rcDrawGeometry.normalMatrix      = NormalMatrix;
                        rcDrawGeometry.modelViewMatrix   = ModelViewMatrix;
                        rcDrawGeometry.modelMatrix       = ModelMatrix;
                        rcDrawGeometry.changeFaceCulling = !(modelComponent->IsCullingBackFaces() == true && modelComponent->IsCullingFrontFaces() == false);
                        rcDrawGeometry.cullBackFace      = modelComponent->IsCullingBackFaces();
                        rcDrawGeometry.cullFrontFace     = modelComponent->IsCullingFrontFaces();

                        // The material may have pending properties. These need to be set on the shader also.
                        auto &materialParams = material->GetParameters();
                        for (size_t iProperty = 0; iProperty < materialParams.count; ++iProperty)
                        {
                            auto iParam = materialParams.buffer[iProperty];
                            assert(iParam        != nullptr);
                            assert(iParam->value != nullptr);

                            rcDrawGeometry.materialParameters.Set(iParam->key, iParam->value);
                        }



                        // We need to add the rendering command to a couple of queues. The first queue is the queue for the material being used
                        // by the mesh. The other queue contains the commands to call during the lighting pass.
                        if (material->IsRefractive())
                        {
                            state.usedRefractiveMaterials.Insert(&materialMetadata);
                            state.refractiveLightingDrawRCs.Append(rcDrawGeometry);
                        }
                        else
                        {
                            state.usedMaterials.Insert(&materialMetadata);
                            state.lightingDrawRCs.Append(rcDrawGeometry);
                        }

                        materialMetadata.materialPassRCs.Append(rcDrawGeometry);
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
                if (model != nullptr && modelComponent->IsModelVisible())
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



    ///////////////////////////////////////////////
    // Private

    DefaultSceneRenderer::Framebuffer* DefaultSceneRenderer::GetViewportFramebuffer(SceneViewport &viewport)
    {
        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);

        return iFramebuffer->value;
    }


    void DefaultSceneRenderer::MaterialPass(Scene &scene, const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, DefaultSceneRenderer::Framebuffer &framebuffer, LayerState &state, bool refractive)
    {
        // We will have render commands waiting to be added to the main RC queue. This is where we should do this.
        if (!refractive)
        {
            DefaultSceneRenderer_MaterialPassCallback materialPassCallback(*this, cameraProjection, cameraView, state, !refractive);        // <-- last parameter is 'opaquePass'
            scene.QueryVisibleObjects(cameraProjection * cameraView, materialPassCallback);

            while (state.usedMaterials.root != nullptr)
            {
                auto &queue = state.usedMaterials.root->value->materialPassRCs;

                auto &rcSetShader = this->rcSetShader[Renderer::BackIndex].Acquire();
                rcSetShader.shader = state.usedMaterials.root->value->materialPassShader;
                Renderer::BackRCQueue->Append(rcSetShader);

                Renderer::BackRCQueue->Append(queue);
                queue.Clear();

                state.usedMaterials.RemoveRoot();
            }
        }
        else
        {
            while (state.usedRefractiveMaterials.root != nullptr)
            {
                auto &queue = state.usedRefractiveMaterials.root->value->materialPassRCs;

                auto &rcBeginTransparentMaterialPass = this->rcBeginTransparentMaterialPass[Renderer::BackIndex].Acquire();
                rcBeginTransparentMaterialPass.shader            = state.usedRefractiveMaterials.root->value->materialPassShader;
                rcBeginTransparentMaterialPass.backgroundTexture = framebuffer.opaqueColourBuffer;
                Renderer::BackRCQueue->Append(rcBeginTransparentMaterialPass);


                Renderer::BackRCQueue->Append(queue);
                queue.Clear();

                state.usedRefractiveMaterials.RemoveRoot();
            }
        }
    }

    void DefaultSceneRenderer::LightingPass(Scene &scene, const glm::mat4 &cameraProjection, const glm::mat4 &cameraView, DefaultSceneRenderer::Framebuffer &framebuffer, int stencilIndex, LayerState &state, bool refractive)
    {
        (void)cameraProjection;


        // We begin with the lights that are not casting shadows. We can do an optimized pass here where we can group lights into a single pass.
        auto &rcBeginLighting = this->rcBeginLighting[Renderer::BackIndex].Acquire();
        rcBeginLighting.framebuffer       = &framebuffer;
        rcBeginLighting.colourClearShader = this->Shaders.Lighting_ColourClear;
        rcBeginLighting.stencilIndex      = stencilIndex;
        Renderer::BackRCQueue->Append(rcBeginLighting);


        for (size_t i = 0; i < state.ambientLights.count; ++i)
        {
            auto &light = *state.ambientLights[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_A1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                rcSetShader.SetParameter("ALights0.Colour", static_cast<const SceneNode &>(light).GetComponent<GTEngine::AmbientLightComponent>()->GetColour());
            }


            Renderer::BackRCQueue->Append(rcSetShader);

            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }

        for (size_t i = 0; i < state.directionalLights_NoShadows.count; ++i)
        {
            auto &light = *state.directionalLights_NoShadows[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_D1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::DirectionalLightComponent>();

                rcSetShader.SetParameter("DLights0.Colour",    component->GetColour());
                rcSetShader.SetParameter("DLights0.Direction", glm::normalize(glm::mat3(cameraView) * component->GetNode().GetWorldForwardVector()));
            }


            Renderer::BackRCQueue->Append(rcSetShader);

            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }

        for (size_t i = 0; i < state.pointLights_NoShadows.count; ++i)
        {
            auto &light = *state.pointLights_NoShadows[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_P1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::PointLightComponent>();

                rcSetShader.SetParameter("PLight0.Position",             glm::vec3(cameraView * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("PLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("PLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("PLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("PLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            
            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }

        for (size_t i = 0; i < state.spotLights_NoShadows.count; ++i)
        {
            auto &light = *state.spotLights_NoShadows[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_NoShadow_S1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::SpotLightComponent>();

                rcSetShader.SetParameter("SLight0.Position",             glm::vec3(cameraView * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("SLight0.Direction",            glm::normalize(glm::mat3(cameraView) * component->GetNode().GetWorldForwardVector()));
                rcSetShader.SetParameter("SLight0.CosAngleInner",        glm::cos(glm::radians(component->GetInnerAngle())));
                rcSetShader.SetParameter("SLight0.CosAngleOuter",        glm::cos(glm::radians(component->GetOuterAngle())));
                rcSetShader.SetParameter("SLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("SLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("SLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("SLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            
            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }



        // With the shadowless lights done, we can now do the lights with shadow.
        for (size_t i = 0; i < state.directionalLights.count; ++i)
        {
            auto &light = *state.directionalLights[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_D1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::DirectionalLightComponent>();

                glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 10000.0f);
                glm::mat4 lightView       = glm::mat4_cast(glm::inverse(component->GetNode().GetWorldOrientation())) * glm::translate(-component->GetNode().GetWorldPosition());

                this->LightingPass_BuildShadowMap(scene, framebuffer, lightProjection, lightView);


                rcSetShader.SetParameter("DLights0.Colour",    component->GetColour());
                rcSetShader.SetParameter("DLights0.Direction", glm::normalize(glm::mat3(cameraView) * component->GetNode().GetWorldForwardVector()));

                rcSetShader.SetParameter("ShadowProjectionMatrix", lightProjection);
                rcSetShader.SetParameter("ShadowViewMatrix",       lightView);
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            
            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }

        for (size_t i = 0; i < state.pointLights.count; ++i)
        {
            auto &light = *state.pointLights[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_P1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::PointLightComponent>();


                // This will build the light's shadow map.
                this->LightingPass_BuildPointLightShadowMap(scene, framebuffer, component->GetNode().GetWorldPosition(), component->GetApproximateRadius());



                rcSetShader.SetParameter("PLight0.Position",             glm::vec3(cameraView * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("PLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("PLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("PLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("PLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());

                rcSetShader.SetParameter("PLight0_PositionWS",           component->GetNode().GetWorldPosition());
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            
            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }

        for (size_t i = 0; i < state.spotLights.count; ++i)
        {
            auto &light = *state.spotLights[i];

            auto &rcSetShader = this->rcLighting_SetShader[Renderer::BackIndex].Acquire();
            rcSetShader.shader          = this->Shaders.Lighting_S1;
            rcSetShader.materialBuffer2 = framebuffer.materialBuffer2;
            rcSetShader.screenSize      = glm::vec2(static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height));

            if (light.GetType() == SceneObjectType_SceneNode)
            {
                auto component = static_cast<const SceneNode &>(light).GetComponent<GTEngine::SpotLightComponent>();

                glm::mat4 lightProjection = glm::perspective(component->GetOuterAngle() * 2.0f, 1.0f, 0.1f, component->GetApproximateLength());
                glm::mat4 lightView       = glm::mat4_cast(glm::inverse(component->GetNode().GetWorldOrientation())) * glm::translate(-component->GetNode().GetWorldPosition());


                this->LightingPass_BuildShadowMap(scene, framebuffer, lightProjection, lightView);


                rcSetShader.SetParameter("SLight0.Position",             glm::vec3(cameraView * glm::vec4(component->GetNode().GetWorldPosition(), 1.0f)));
                rcSetShader.SetParameter("SLight0.Direction",            glm::normalize(glm::mat3(cameraView) * component->GetNode().GetWorldForwardVector()));
                rcSetShader.SetParameter("SLight0.CosAngleInner",        glm::cos(glm::radians(component->GetInnerAngle())));
                rcSetShader.SetParameter("SLight0.CosAngleOuter",        glm::cos(glm::radians(component->GetOuterAngle())));
                rcSetShader.SetParameter("SLight0.Colour",               component->GetColour());
                rcSetShader.SetParameter("SLight0.ConstantAttenuation",  component->GetConstantAttenuation());
                rcSetShader.SetParameter("SLight0.LinearAttenuation",    component->GetLinearAttenuation());
                rcSetShader.SetParameter("SLight0.QuadraticAttenuation", component->GetQuadraticAttenuation());


                glm::mat4 bias(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.5f, 0.0f,
                    0.5f, 0.5f, 0.5f, 1.0f);

                rcSetShader.SetParameter("ShadowProjectionMatrix", /*bias * */lightProjection);
                rcSetShader.SetParameter("ShadowViewMatrix",       lightView);
            }


            Renderer::BackRCQueue->Append(rcSetShader);
            
            if (refractive)
            {
                Renderer::BackRCQueue->Append(state.refractiveLightingDrawRCs);
            }
            else
            {
                Renderer::BackRCQueue->Append(state.lightingDrawRCs);
            }
        }
    }


    void DefaultSceneRenderer::LightingPass_BuildShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::mat4 &lightProjection, const glm::mat4 &lightView)
    {
        auto &rcBeginShadowMap = this->rcLighting_BeginDirectionalShadowMap[Renderer::BackIndex].Acquire();
        rcBeginShadowMap.shader      = this->Shaders.Lighting_ShadowMap;
        rcBeginShadowMap.framebuffer = &this->shadowMapFramebuffer;
        rcBeginShadowMap.mvpMatrix   = lightProjection * lightView;
        Renderer::BackRCQueue->Append(rcBeginShadowMap);


        DefaultSceneRenderer_ShadowPassCallback shadowPassCallback(*this, lightProjection, lightView);
        scene.QueryVisibleObjects(rcBeginShadowMap.mvpMatrix, shadowPassCallback);


        // Now we need to mark the shadow map generation as complete.
        auto &rcEndShadowMap = this->rcLighting_EndShadowMap[Renderer::BackIndex].Acquire();
        rcEndShadowMap.newFramebuffer = &mainFramebuffer;
        Renderer::BackRCQueue->Append(rcEndShadowMap);
    }


    void DefaultSceneRenderer::LightingPass_BuildPointLightShadowMap(Scene &scene, DefaultSceneRenderer::Framebuffer &mainFramebuffer, const glm::vec3 &position, float radius)
    {
        glm::mat4 positiveXViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);
        glm::mat4 negativeXViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);
        glm::mat4 positiveYViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-position);
        glm::mat4 negativeYViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(+90.0f, glm::vec3(1.0f, 0.0f, 0.0f)))) * glm::translate(-position);
        glm::mat4 positiveZViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(  0.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);
        glm::mat4 negativeZViewMatrix = glm::mat4_cast(glm::inverse(glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f)))) * glm::translate(-position);
        
        glm::mat4 projectionMatrix = glm::perspective(90.0f, 1.0f, 0.1f, radius);



        auto &rcBeginPointShadowMap = this->rcLighting_BeginPointShadowMap[Renderer::BackIndex].Acquire();
        rcBeginPointShadowMap.shader      = this->Shaders.Lighting_PointLightShadowMap;
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



    Shader* DefaultSceneRenderer::CreateMaterialPassShader(Material &material)
    {
        auto &materialDefinition = material.GetDefinition();

        // If a shader already exists for this material, we just reuse that.
        Shader* shader = this->Shaders.materialPassShaders.GetShader(materialDefinition);
        if (shader == nullptr)
        {
            // If we get here, it means the shader does not exist. We need to create it and then add it to the cache.
            // There are two parts to the shader. There is the base shader and then the material components.
            if (material.IsRefractive())
            {
                const char* baseShaderVS       = ShaderLibrary::GetShaderString("Engine_MaterialPass_VS");
                const char* baseShaderFS       = ShaderLibrary::GetShaderString("Engine_MaterialPass_Refraction_FS");
                const char* materialDiffuse    = ShaderLibrary::GetShaderString(material.GetDiffuseShaderID());
                const char* materialEmissive   = ShaderLibrary::GetShaderString(material.GetEmissiveShaderID());
                const char* materialShininess  = ShaderLibrary::GetShaderString(material.GetShininessShaderID());
                const char* materialNormal     = ShaderLibrary::GetShaderString(material.GetNormalShaderID());
                const char* materialRefraction = ShaderLibrary::GetShaderString(material.GetRefractionShaderID());

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
                fragmentShaderString.Append("\n");
                fragmentShaderString.Append(materialRefraction);

                shader = new Shader(baseShaderVS, fragmentShaderString.c_str());
            }
            else
            {
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
            }

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




    // TODO: Might need to make it so the main layer renders to stencil index 2 so that the background layer can use index 1 for doing lighting clears for transparency. The main
    //       layer will use index 3 for transparent lighting clears.


    ///////////////////////////////////////////////
    // Render Commands

    void DefaultSceneRenderer::RCBegin::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
        
        int lightsDrawBuffers[] = {1, 2, 3, 4, 5};      // Material Buffers 0/1/2, Lighting Buffers 0/1
        Renderer::SetDrawBuffers(5, lightsDrawBuffers);


        Renderer::SetViewport(0, 0, this->framebuffer->GetWidth(), this->framebuffer->GetHeight());


        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunc(RendererFunction_LEqual);
        
        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_GEqual, 1, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Replace);


        Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::ClearDepth(1.0f);
        Renderer::ClearStencil(0);
        Renderer::Clear(GTEngine::ColourBuffer | GTEngine::DepthBuffer | GTEngine::StencilBuffer);


        int drawBuffers[] = {1, 2, 3};      // Material Buffers 0/1/2
        Renderer::SetDrawBuffers(3, drawBuffers);
    }


    void DefaultSceneRenderer::RCBeginBackground::Execute()
    {
        int drawBuffers[] = {1, 2, 3};      // Material Buffers 0/1/2
        Renderer::SetDrawBuffers(3, drawBuffers);

        // Standard depth testing.
        Renderer::SetDepthFunc(RendererFunction_LEqual);
        Renderer::EnableDepthWrites();
        
        // Stencil test against everything that's not 0 and leave the stencil buffer as-is.
        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_Equal, 0, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);


        // The previous lighting pass will have enabled blending.
        Renderer::DisableBlending();
    }


    void DefaultSceneRenderer::RCBeginLighting::Execute()
    {
        assert(this->framebuffer != nullptr);

        int drawBuffers[] = {4, 5};     // Lighting Buffers 0/1
        Renderer::SetDrawBuffers(2, drawBuffers);

        // The depth buffer will have been layed down.
        Renderer::SetDepthFunc(RendererFunction_Equal);
        Renderer::DisableDepthWrites();

        // We combine lighting passes using standard blending.
        Renderer::EnableBlending();
        Renderer::SetBlendEquation(BlendEquation_Add);
        Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
    }


    void DefaultSceneRenderer::RCBeginTransparency::Execute()
    {
        int drawBuffers[] = {1, 2, 3, 4, 5};      // Material Buffers 0/1/2, Lighting Buffers 0/1
        Renderer::SetDrawBuffers(5, drawBuffers);

        // Standard depth testing, but no writing.
        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunc(RendererFunction_LEqual);

        // Stencil testing needs to be enabled.
        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_GEqual, 0, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);
    }

    void DefaultSceneRenderer::RCBeginForegroundTransparency::Execute()
    {
        int drawBuffers[] = {1, 2, 3, 4, 5};      // Material Buffers 0/1/2, Lighting Buffers 0/1
        Renderer::SetDrawBuffers(5, drawBuffers);

        // Standard depth testing, but no writing.
        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();


        // We need to clear the background's depth so that foreground elements are always drawn on top of it.
        Renderer::DisableColourWrites();
        Renderer::SetDepthFunc(RendererFunction_Always);
        Renderer::SetShader(this->depthClearShader);


        float vertices[] =
        {
            -1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, 1.0f,
        };

        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0,
        };

        Renderer::Draw(vertices, indices, 6, VertexFormat::P3);


        Renderer::EnableColourWrites();
        Renderer::SetDepthFunc(RendererFunction_LEqual);


        // Stencil testing needs to be enabled.
        Renderer::DisableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_GEqual, 1, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);
    }

    void DefaultSceneRenderer::RCBeginTransparentMaterialPass::Execute()
    {
        Renderer::SetShader(this->shader);
        Renderer::SetShaderParameter("BackgroundTexture", this->backgroundTexture);
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


    void DefaultSceneRenderer::RCSetShader::Execute()
    {
        Renderer::SetShader(this->shader);
    }


    void DefaultSceneRenderer::RCDrawGeometry::Execute()
    {
        // If we're doing the material pass, we need to 
        if (this->doingMaterialPass)
        {
            for (size_t i = 0; i < this->materialParameters.GetCount(); ++i)
            {
                auto iParamName  = this->materialParameters.GetNameByIndex(i);
                auto iParamValue = this->materialParameters.GetByIndex(i);

                iParamValue->SetOnCurrentShader(iParamName);
            }

            this->doingMaterialPass = false;
        }



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


    void DefaultSceneRenderer::RCLighting_BeginDirectionalShadowMap::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
        Renderer::SetShader(this->shader);
        Renderer::SetViewport(0, 0, this->framebuffer->GetDepthStencilBuffer()->GetWidth(), this->framebuffer->GetDepthStencilBuffer()->GetHeight());


        int drawBuffers[] = {0};
        Renderer::SetDrawBuffers(1, drawBuffers);

        Renderer::DisableBlending();

        Renderer::SetDepthFunc(RendererFunction_LEqual);
        Renderer::EnableDepthWrites();

        Renderer::DisableStencilTest();

        Renderer::ClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::ClearDepth(1.0f);
        Renderer::Clear(GTEngine::ColourBuffer | GTEngine::DepthBuffer);
    }

    void DefaultSceneRenderer::RCLighting_BeginPointShadowMap::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
        Renderer::SetShader(this->shader);
        Renderer::SetViewport(0, 0, this->framebuffer->GetDepthStencilBuffer()->GetWidth(), this->framebuffer->GetDepthStencilBuffer()->GetHeight());
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


    void DefaultSceneRenderer::RCBackgroundColourClear::Execute()
    {
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();

        Renderer::SetShader(this->colourClearShader);
        Renderer::SetShaderParameter("ClearColour", this->colour);

        Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
    }

    void DefaultSceneRenderer::RCBuildFinalImage::Execute()
    {
        assert(this->framebuffer != nullptr);

        Renderer::SetDrawBuffers(1, &this->colourBufferIndex);

        Renderer::SetShader(this->compositingShader);
        Renderer::SetShaderParameter("LightingBuffer0", this->framebuffer->lightingBuffer0);
        Renderer::SetShaderParameter("LightingBuffer1", this->framebuffer->lightingBuffer1);
        Renderer::SetShaderParameter("MaterialBuffer0", this->framebuffer->materialBuffer0);
        Renderer::SetShaderParameter("MaterialBuffer1", this->framebuffer->materialBuffer1);
        Renderer::SetShaderParameter("MaterialBuffer2", this->framebuffer->materialBuffer2);

        Renderer::DisableDepthTest();
        Renderer::DisableBlending();

        Renderer::DisableStencilTest();
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);
        
        Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
    }
}