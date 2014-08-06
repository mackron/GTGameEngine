// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer.hpp>
#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_SinglePassPipeline.hpp>
#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_MultiPassPipeline.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    static const bool SplitShadowLights = true;


    DefaultSceneRenderer::DefaultSceneRenderer()
        : viewportFramebuffers(), materialShaders(), depthPassShader(nullptr), externalMeshes(),
          directionalShadowMapFramebuffer(1, 1), pointShadowMapFramebuffer(1, 1), spotShadowMapFramebuffer(1, 1),
          fullscreenTriangleVA(nullptr),
          shadowMapShader(nullptr), pointShadowMapShader(nullptr),
          finalCompositionShaderHDR(nullptr), finalCompositionShaderHDRNoBloom(nullptr), finalCompositionShaderLDR(nullptr),
          bloomShader(nullptr), highlightShader(nullptr),
          bloomBlurShaderX(nullptr), bloomBlurShaderY(nullptr), shadowBlurShaderX(nullptr), shadowBlurShaderY(nullptr),
          shaderBuilder(),
          luminanceChain(),
          materialUniformNames(),
          isHDREnabled(true), isBloomEnabled(true), hdrExposure(1.0f), bloomFactor(1.0f),
          directionalShadowMapSize(1024), pointShadowMapSize(256), spotShadowMapSize(512),
          materialLibraryEventHandler(*this)
    {
        this->directionalShadowMapFramebuffer.Resize(this->directionalShadowMapSize, this->directionalShadowMapSize);
        this->pointShadowMapFramebuffer.Resize(this->pointShadowMapSize, this->pointShadowMapSize);
        this->spotShadowMapFramebuffer.Resize(this->spotShadowMapSize, this->spotShadowMapSize);

        this->depthPassShader                  = this->shaderBuilder.CreateDepthPassShader();
        this->highlightShader                  = this->shaderBuilder.CreateHighlightShader();
        this->bloomBlurShaderX                 = this->shaderBuilder.CreateXGaussianBlurShader(21, 8.0f);
        this->bloomBlurShaderY                 = this->shaderBuilder.CreateYGaussianBlurShader(21, 8.0f);
        this->shadowBlurShaderX                = this->shaderBuilder.CreateXGaussianBlurShader(11, 2.0f);
        this->shadowBlurShaderY                = this->shaderBuilder.CreateYGaussianBlurShader(11, 2.0f);
        this->shadowMapShader                  = this->shaderBuilder.CreateShadowMapShader();
        this->pointShadowMapShader             = this->shaderBuilder.CreatePointShadowMapShader();
        this->bloomShader                      = this->shaderBuilder.CreateBloomShader();
        this->finalCompositionShaderLDR        = this->shaderBuilder.CreateLDRFinalCompositionShader();
        this->finalCompositionShaderHDR        = this->shaderBuilder.CreateHDRFinalCompositionShader();
        this->finalCompositionShaderHDRNoBloom = this->shaderBuilder.CreateHDRNoBloomFinalCompositionShader();


        /// Fullscreen Triangle Vertex Array.
        this->fullscreenTriangleVA = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2);

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

    DefaultSceneRenderer::~DefaultSceneRenderer()
    {
        Renderer::DeleteShader(this->depthPassShader);
        Renderer::DeleteShader(this->shadowMapShader);
        Renderer::DeleteShader(this->pointShadowMapShader);
        Renderer::DeleteShader(this->finalCompositionShaderHDR);
        Renderer::DeleteShader(this->finalCompositionShaderHDRNoBloom);
        Renderer::DeleteShader(this->finalCompositionShaderLDR);
        Renderer::DeleteShader(this->bloomShader);
        Renderer::DeleteShader(this->highlightShader);
        Renderer::DeleteShader(this->bloomBlurShaderX);
        Renderer::DeleteShader(this->bloomBlurShaderY);
        Renderer::DeleteShader(this->shadowBlurShaderX);
        Renderer::DeleteShader(this->shadowBlurShaderY);

        for (size_t i = 0; i < this->materialShaders.count; ++i)
        {
            auto materialShaders = this->materialShaders.buffer[i]->value;
            assert(materialShaders != nullptr);
            {
                delete materialShaders;
            }
        }

        Renderer::DeleteVertexArray(this->fullscreenTriangleVA);

        MaterialLibrary::RemoveEventHandler(this->materialLibraryEventHandler);
    }


    void DefaultSceneRenderer::Begin(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer::End(Scene &scene)
    {
        (void)scene;
    }

    void DefaultSceneRenderer::RenderViewport(Scene &scene, SceneViewport &viewport)
    {
        // 0) Retrieve visible objects.
        DefaultSceneRenderer_VisibilityProcessor visibleObjects(scene, viewport);
        scene.QueryVisibleSceneNodes(viewport.GetMVPMatrix(), visibleObjects);

        // All external meshes are considered visible. Not going to do any frustum culling here.
        for (size_t i = 0; i < this->externalMeshes.count; ++i)
        {
            auto externalMesh = this->externalMeshes[i];
            assert(externalMesh != nullptr);
            {
                DefaultSceneRendererMesh meshToAdd;
                meshToAdd.vertexArray = externalMesh->vertexArray;
                meshToAdd.drawMode    = externalMesh->drawMode;
                meshToAdd.material    = externalMesh->material;
                meshToAdd.transform   = externalMesh->transform;
                meshToAdd.flags       = externalMesh->flags;
                visibleObjects.AddMesh(meshToAdd);
            }
        }

        // Post-processing needs to be done after everything has been added.
        visibleObjects.PostProcess();


        // We'll want to grab the framebuffer and set a few defaults.
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            Renderer::SetCurrentFramebuffer(framebuffer->framebuffer);
            Renderer::SetViewport(0, 0, framebuffer->width, framebuffer->height);
        }

        Renderer::DisableBlending();
        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_LEqual);


        // With the visible objects determined we can now choose a pipeline to run rendering operations through. We want to use fast paths where
        // they're available.
        if (visibleObjects.lightManager.shadowDirectionalLights.count <= 1 && 
            visibleObjects.lightManager.shadowPointLights.count       <= 1 &&
            visibleObjects.lightManager.shadowSpotLights.count        <= 1)
        {
            // Shadow-casting lights can be combined into the main lighting pass. If the total number of lights is small enough, we can combine
            // everything into a single pass and do an even faster path.
            //
            // At the moment we need to be conservative, but this will likely be improved later.
            if (visibleObjects.lightManager.GetTotalLightCount() <= 4)
            {
                // Single pass.
                //DefaultSceneRenderer_SinglePassPipeline pipeline(*this, framebuffer, visibleObjects);
                DefaultSceneRenderer_MultiPassPipeline pipeline(*this, *framebuffer, visibleObjects, !SplitShadowLights);
                pipeline.Execute();
            }
            else
            {
                // Multi-pass.
                DefaultSceneRenderer_MultiPassPipeline pipeline(*this, *framebuffer, visibleObjects, !SplitShadowLights);
                pipeline.Execute();
            }
        }
        else
        {
            // Multi-pass, slower due to too many shadow-casting lights.
            DefaultSceneRenderer_MultiPassPipeline pipeline(*this, *framebuffer, visibleObjects, SplitShadowLights);
            pipeline.Execute();
        }


        // Here we just render the final image. Basically, this is just the post-process step. This will eventually be made into a proper
        // pipeline stage.
        this->RenderFinalComposition(framebuffer, framebuffer->colourBuffer0);
    }

    void DefaultSceneRenderer::AddViewport(SceneViewport &viewport)
    {
        // We create a framebuffer for every viewport. We map these to viewports with a simple GTLib::Map container.
        auto framebuffer = new DefaultSceneRendererFramebuffer(viewport.GetWidth(), viewport.GetHeight());
        assert(framebuffer != nullptr);
        {
            viewport.SetColourBuffer(framebuffer->finalColourBuffer);
            this->viewportFramebuffers.Add(&viewport, framebuffer);
        }
    }

    void DefaultSceneRenderer::RemoveViewport(SceneViewport &viewport)
    {
        // When a viewport is removed, the framebuffer needs to be deleted.
        viewport.SetColourBuffer(nullptr);

        auto iFramebuffer = this->viewportFramebuffers.Find(&viewport);
        assert(iFramebuffer != nullptr);
        {
            auto framebuffer = iFramebuffer->value;
            assert(framebuffer != nullptr);
            {
                delete framebuffer;
                this->viewportFramebuffers.RemoveByIndex(iFramebuffer->index);
            }
        }
    }

    void DefaultSceneRenderer::OnViewportResized(SceneViewport &viewport)
    {
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            framebuffer->Resize(viewport.GetWidth(), viewport.GetHeight());
        }
    }


    void DefaultSceneRenderer::AddExternalMesh(const SceneRendererMesh &meshToAdd)
    {
        if (!this->externalMeshes.Exists(&meshToAdd))
        {
            this->externalMeshes.PushBack(&meshToAdd);
        }
    }

    void DefaultSceneRenderer::RemoveExternalMesh(const SceneRendererMesh &meshToRemove)
    {
        this->externalMeshes.RemoveFirstOccuranceOf(&meshToRemove);
    }


    ////////////////////////////////////////////////////////////////
    // Properties

    void DefaultSceneRenderer::SetProperty(const char* name, const char* value)
    {
        (void)name;
        (void)value;
    }

    void DefaultSceneRenderer::SetProperty(const char* name, int value)
    {
        (void)name;
        (void)value;
    }

    void DefaultSceneRenderer::SetProperty(const char* name, float value)
    {
        if (GTLib::Strings::Equal(name, "HDRExposure"))
        {
            this->SetHDRExposure(value);
        }
        else if (GTLib::Strings::Equal(name, "BloomFactor"))
        {
            this->SetBloomFactor(value);
        }
    }

    void DefaultSceneRenderer::SetProperty(const char* name, bool value)
    {
        if (GTLib::Strings::Equal(name, "IsBackgroundClearEnabled"))
        {
            if (value)
            {
                this->EnableBackgroundColourClearing(this->GetBackgroundClearColour());
            }
            else
            {
                this->DisableBackgroundColourClearing();
            }
        }
        else if (GTLib::Strings::Equal(name, "IsHDREnabled"))
        {
            if (value)
            {
                this->EnableHDR();
            }
            else
            {
                this->DisableHDR();
            }
        }
        else if (GTLib::Strings::Equal(name, "IsBloomEnabled"))
        {
            if (value)
            {
                this->EnableBloom();
            }
            else
            {
                this->DisableBloom();
            }
        }
    }

    void DefaultSceneRenderer::SetProperty(const char* name, const glm::vec2 &value)
    {
        (void)name;
        (void)value;
    }

    void DefaultSceneRenderer::SetProperty(const char* name, const glm::vec3 &value)
    {
        if (GTLib::Strings::Equal(name, "BackgroundClearColour"))
        {
            bool isEnabled = this->IsBackgroundColourClearingEnabled();
            this->EnableBackgroundColourClearing(value);

            if (!isEnabled)
            {
                this->DisableBackgroundColourClearing();
            }
        }
    }

    void DefaultSceneRenderer::SetProperty(const char* name, const glm::vec4 &value)
    {
        (void)name;
        (void)value;
    }


    GTLib::String DefaultSceneRenderer::GetStringProperty(const char* name) const
    {
        (void)name;

        return "";
    }

    int DefaultSceneRenderer::GetIntegerProperty(const char* name) const
    {
        (void)name;

        return 0;
    }

    float DefaultSceneRenderer::GetFloatProperty(const char* name) const
    {
        if (GTLib::Strings::Equal(name, "HDRExposure"))
        {
            return this->GetHDRExposure();
        }
        else if (GTLib::Strings::Equal(name, "BloomFactor"))
        {
            return this->GetBloomFactor();
        }

        return 0.0f;
    }

    bool DefaultSceneRenderer::GetBooleanProperty(const char* name) const
    {
        if (GTLib::Strings::Equal(name, "IsBackgroundClearEnabled"))
        {
            return this->IsBackgroundColourClearingEnabled();
        }
        else if (GTLib::Strings::Equal(name, "IsHDREnabled"))
        {
            return this->IsHDREnabled();
        }
        else if (GTLib::Strings::Equal(name, "IsBloomEnabled"))
        {
            return this->IsBloomEnabled();
        }

        return false;
    }

    glm::vec2 DefaultSceneRenderer::GetVector2Property(const char* name) const
    {
        (void)name;

        return glm::vec2();
    }

    glm::vec3 DefaultSceneRenderer::GetVector3Property(const char* name) const
    {
        if (GTLib::Strings::Equal(name, "BackgroundClearColour"))
        {
            return this->GetBackgroundClearColour();
        }

        return glm::vec3();
    }

    glm::vec4 DefaultSceneRenderer::GetVector4Property(const char* name) const
    {
        (void)name;

        return glm::vec4();
    }



    ////////////////////////////////////////////////////////////////
    // Settings.

    void DefaultSceneRenderer::EnableHDR()
    {
        this->isHDREnabled = true;
    }

    void DefaultSceneRenderer::DisableHDR()
    {
        this->isHDREnabled = false;
    }

    bool DefaultSceneRenderer::IsHDREnabled() const
    {
        return this->isHDREnabled;
    }


    void DefaultSceneRenderer::EnableBloom()
    {
        this->isBloomEnabled = true;
    }

    void DefaultSceneRenderer::DisableBloom()
    {
        this->isBloomEnabled = false;
    }

    bool DefaultSceneRenderer::IsBloomEnabled() const
    {
        return this->isBloomEnabled;
    }


    void DefaultSceneRenderer::SetHDRExposure(float newExposure)
    {
        this->hdrExposure = newExposure;
    }

    float DefaultSceneRenderer::GetHDRExposure() const
    {
        return this->hdrExposure;
    }


    void DefaultSceneRenderer::SetBloomFactor(float newBloomFactor)
    {
        this->bloomFactor = newBloomFactor;
    }

    float DefaultSceneRenderer::GetBloomFactor() const
    {
        return this->bloomFactor;
    }



    ////////////////////////////////////////////////////////////////
    // Resource Access.

    Shader* DefaultSceneRenderer::GetDepthPassShader() const
    {
        return this->depthPassShader;
    }

    Texture2D* DefaultSceneRenderer::GetDirectionalShadowMapByIndex(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until multiple shadow maps are supported.
        (void)index;

        return this->directionalShadowMapFramebuffer.colourBuffer;
    }

    TextureCube* DefaultSceneRenderer::GetPointShadowMapByIndex(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until multiple shadow maps are supported.
        (void)index;

        return this->pointShadowMapFramebuffer.colourBuffer;
    }

    Texture2D* DefaultSceneRenderer::GetSpotShadowMapByIndex(size_t index)
    {
        assert(index == 0);     // <-- Temp assert until multiple shadow maps are supported.
        (void)index;

        return this->spotShadowMapFramebuffer.colourBuffer;
    }

    Shader* DefaultSceneRenderer::GetMaterialShader(Material &material, const DefaultSceneRenderer_LightGroupID &lightGroupID, uint32_t flags)
    {
        auto materialShaders = this->GetMaterialShaders(material);
        assert(materialShaders != nullptr);
        {
            DefaultSceneRenderer_MaterialShaderID shaderID(lightGroupID, flags);

            auto iShader = materialShaders->shaders.Find(shaderID);
            if (iShader != nullptr)
            {
                return iShader->value;
            }
            else
            {
                auto shader = this->shaderBuilder.CreateShader(shaderID, material.GetDefinition());
                if (shader != nullptr)
                {
                    materialShaders->shaders.Add(shaderID, shader);
                }

                return shader;
            }
        }
    }

    Shader* DefaultSceneRenderer::GetHighlightShader() const
    {
        return this->highlightShader;
    }

    Shader* DefaultSceneRenderer::GetFullscreenTriangleCopyShader() const
    {
        return this->finalCompositionShaderLDR;
    }

    const VertexArray & DefaultSceneRenderer::GetFullscreenTriangleVA() const
    {
        assert(this->fullscreenTriangleVA != nullptr);
        {
            return *this->fullscreenTriangleVA;
        }
    }


    ////////////////////////////////////////////////////////////////
    // Rendering Helpers.

    Texture2D* DefaultSceneRenderer::RenderDirectionalLightShadowMap(DefaultSceneRendererDirectionalLight &light, size_t shadowMapIndex)
    {
        assert(shadowMapIndex == 0);    // <-- temp assert until we add support for multiple shadow maps.
        (void)shadowMapIndex;

        // State setup.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();

        // Framebuffer setup.
        Renderer::SetCurrentFramebuffer(this->directionalShadowMapFramebuffer.framebuffer);
        Renderer::SetViewport(0, 0, this->directionalShadowMapFramebuffer.width, this->directionalShadowMapFramebuffer.height);

        int colourBufferIndex = 0;
        int blurBufferIndex   = 1;
        Renderer::SetDrawBuffers(1, &colourBufferIndex);

        // Clear.
        Renderer::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::Clear(BufferType_Colour | BufferType_Depth);

        // Shader setup.
        Renderer::SetCurrentShader(this->shadowMapShader);


        glm::mat4 lightProjectionView = light.projection * light.view;

        for (size_t iMesh = 0; iMesh < light.containedMeshes.meshes.count; ++iMesh)
        {
            auto &mesh = light.containedMeshes.meshes[iMesh];
            assert(mesh.vertexArray != nullptr);
            {
                // Shader setup.
                this->shadowMapShader->SetUniform("PVMMatrix", lightProjectionView * mesh.transform);
                Renderer::PushPendingUniforms(*this->shadowMapShader);


                // Draw.
                Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
            }
        }


        // Here is where we perform the blurring of the shadow map. This renders a fullscreen quad, so we don't want depth testing here.
        Renderer::DisableDepthWrites();
        Renderer::DisableDepthTest();

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderX);
            this->shadowBlurShaderX->SetUniform("Texture", this->directionalShadowMapFramebuffer.colourBuffer);
            this->shadowBlurShaderX->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->directionalShadowMapFramebuffer.colourBuffer->GetWidth()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &colourBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderY);
            this->shadowBlurShaderY->SetUniform("Texture", this->directionalShadowMapFramebuffer.blurBuffer);
            this->shadowBlurShaderY->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->directionalShadowMapFramebuffer.colourBuffer->GetHeight()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // The blurring is now done, but we want to restore depth testing.
        Renderer::EnableDepthTest();



        return this->directionalShadowMapFramebuffer.colourBuffer;
    }

    TextureCube* DefaultSceneRenderer::RenderPointLightShadowMap(DefaultSceneRendererPointLight &light, size_t shadowMapIndex)
    {
        assert(shadowMapIndex == 0);    // <-- temp assert until we add support for multiple shadow maps.
        (void)shadowMapIndex;

        // State setup.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();

        // Framebuffer setup.
        Renderer::SetCurrentFramebuffer(this->pointShadowMapFramebuffer.framebuffer);
        Renderer::SetViewport(0, 0, this->pointShadowMapFramebuffer.width, this->pointShadowMapFramebuffer.height);

        this->RenderPointShapowMapFace(light, light.positiveXView, 0, light.containedMeshesPositiveX.meshes);
        this->RenderPointShapowMapFace(light, light.negativeXView, 1, light.containedMeshesNegativeX.meshes);
        this->RenderPointShapowMapFace(light, light.positiveYView, 2, light.containedMeshesPositiveY.meshes);
        this->RenderPointShapowMapFace(light, light.negativeYView, 3, light.containedMeshesNegativeY.meshes);
        this->RenderPointShapowMapFace(light, light.positiveZView, 4, light.containedMeshesPositiveZ.meshes);
        this->RenderPointShapowMapFace(light, light.negativeZView, 5, light.containedMeshesNegativeZ.meshes);


        return this->pointShadowMapFramebuffer.colourBuffer;
    }

    Texture2D* DefaultSceneRenderer::RenderSpotLightShadowMap(DefaultSceneRendererSpotLight &light, size_t shadowMapIndex)
    {
        assert(shadowMapIndex == 0);    // <-- temp assert until we add support for multiple shadow maps.
        (void)shadowMapIndex;

        // State setup.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();

        // Framebuffer setup.
        Renderer::SetCurrentFramebuffer(this->spotShadowMapFramebuffer.framebuffer);
        Renderer::SetViewport(0, 0, this->spotShadowMapFramebuffer.width, this->spotShadowMapFramebuffer.height);

        int colourBufferIndex = 0;
        int blurBufferIndex   = 1;
        Renderer::SetDrawBuffers(1, &colourBufferIndex);

        // Clear.
        Renderer::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::Clear(BufferType_Colour | BufferType_Depth);

        // Shader setup.
        Renderer::SetCurrentShader(this->shadowMapShader);


        glm::mat4 lightProjectionView = light.projection * light.view;

        for (size_t iMesh = 0; iMesh < light.containedMeshes.meshes.count; ++iMesh)
        {
            auto &mesh = light.containedMeshes.meshes[iMesh];
            assert(mesh.vertexArray != nullptr);
            {
                // Shader setup.
                this->shadowMapShader->SetUniform("PVMMatrix", lightProjectionView * mesh.transform);
                Renderer::PushPendingUniforms(*this->shadowMapShader);


                // Draw.
                Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
            }
        }


        // Here is where we perform the blurring of the shadow map. This renders a fullscreen quad, so we don't want depth testing here.
        Renderer::DisableDepthWrites();
        Renderer::DisableDepthTest();

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderX);
            this->shadowBlurShaderX->SetUniform("Texture", this->spotShadowMapFramebuffer.colourBuffer);
            this->shadowBlurShaderX->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->spotShadowMapFramebuffer.colourBuffer->GetWidth()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &colourBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderY);
            this->shadowBlurShaderY->SetUniform("Texture", this->spotShadowMapFramebuffer.blurBuffer);
            this->shadowBlurShaderY->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->spotShadowMapFramebuffer.colourBuffer->GetHeight()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // The blurring is now done, but we want to restore depth testing.
        Renderer::EnableDepthTest();


        return this->spotShadowMapFramebuffer.colourBuffer;
    }


    void DefaultSceneRenderer::RenderPointShapowMapFace(const DefaultSceneRendererPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTLib::Vector<DefaultSceneRendererMesh> &meshes)
    {
        // The draw buffer needs to be set. The appropriate framebuffer will have already been set.
        int blurBuffer0Index = 6;
        int blurBuffer1Index = 7;
        Renderer::SetDrawBuffers(1, &blurBuffer0Index);
        Renderer::SetCurrentShader(this->pointShadowMapShader);

        // We need to clear both depth and colour. The colour needs to be cleared to the radius of the light.
        float radius = light.radius;
        Renderer::SetClearColour(radius, radius * radius, 0.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::Clear(BufferType_Colour | BufferType_Depth);


        glm::mat4 projectionView = light.projection * faceViewMatrix;
        for (size_t i = 0; i < meshes.count; ++i)
        {
            auto &mesh = meshes[i];
            assert(mesh.vertexArray != nullptr);
            {
                // Shader setup.
                this->pointShadowMapShader->SetUniform("PVMMatrix",       projectionView * mesh.transform);
                this->pointShadowMapShader->SetUniform("ViewModelMatrix", faceViewMatrix * mesh.transform);
                Renderer::PushPendingUniforms(*this->pointShadowMapShader);


                // Draw.
                Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
            }
        }



        // TODO: Make this optional.

        // Now we want to do a gaussian blur on the face. The way we do it is we first blur on the X axis and then do the same on the Y axis. We use
        // an intermediary buffer that is bound to index 6 in the framebuffer. Not sure how seams will work here.
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBuffer1Index);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderX);
            this->shadowBlurShaderX->SetUniform("Texture", this->pointShadowMapFramebuffer.blurBuffer0);
            this->shadowBlurShaderX->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->pointShadowMapFramebuffer.blurBuffer0->GetWidth()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y
        {
            Renderer::SetDrawBuffers(1, &faceIndex);

            // Shader.
            Renderer::SetCurrentShader(this->shadowBlurShaderY);
            this->shadowBlurShaderY->SetUniform("Texture", this->pointShadowMapFramebuffer.blurBuffer1);
            this->shadowBlurShaderY->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(this->pointShadowMapFramebuffer.blurBuffer1->GetHeight()));
            Renderer::PushPendingUniforms(*this->shadowBlurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
    }


    void DefaultSceneRenderer::SetMaterialShaderUniforms(Shader &shader, const Material &material, const DefaultSceneRenderer_LightGroup &lightGroup, uint32_t flags, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        (void)flags;

        uint16_t ambientLightCount            = lightGroup.GetAmbientLightCount();
        uint16_t directionalLightCount        = lightGroup.GetDirectionalLightCount();
        uint16_t pointLightCount              = lightGroup.GetPointLightCount();
        uint16_t spotLightCount               = lightGroup.GetSpotLightCount();
        uint16_t shadowDirectionalLightCount  = lightGroup.GetShadowDirectionalLightCount();
        uint16_t shadowPointLightCount        = lightGroup.GetShadowPointLightCount();
        uint16_t shadowSpotLightCount         = lightGroup.GetShadowSpotLightCount();

        auto ambientLightStartIndex           = lightGroup.GetAmbientLightStartIndex();
        auto directionalLightStartIndex       = lightGroup.GetDirectionalLightStartIndex();
        auto pointLightStartIndex             = lightGroup.GetPointLightStartIndex();
        auto spotLightStartIndex              = lightGroup.GetSpotLightStartIndex();
        auto shadowDirectionalLightStartIndex = lightGroup.GetShadowDirectionalLightStartIndex();
        auto shadowPointLightStartIndex       = lightGroup.GetShadowPointLightStartIndex();
        auto shadowSpotLightStartIndex        = lightGroup.GetShadowSpotLightStartIndex();

        // Ambient Lights.
        for (int i = 0; i < ambientLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.ambientLights.buffer[lightGroup.lightIDs[ambientLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_AmbientLightFS_Colour, i), light->colour);
            }
        }

        // Directional Lights.
        for (int i = 0; i < directionalLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.directionalLights.buffer[lightGroup.lightIDs[directionalLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_DirectionalLightFS_Colour,    i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_DirectionalLightFS_Direction, i), glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
            }
        }

        // Point Lights.
        for (int i = 0; i < pointLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.pointLights.buffer[lightGroup.lightIDs[pointLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_PointLightVS_PositionVS, i), glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_PointLightFS_Colour,     i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_PointLightFS_Radius,     i), light->radius);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_PointLightFS_Falloff,    i), light->falloff);
            }
        }

        // Spot Lights.
        for (int i = 0; i < spotLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.spotLights.buffer[lightGroup.lightIDs[spotLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_Position,      i), glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_Colour,        i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_Direction,     i), glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_Length,        i), light->length);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_Falloff,       i), light->falloff);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_CosAngleInner, i), glm::cos(glm::radians(light->innerAngle)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_SpotLightFS_CosAngleOuter, i), glm::cos(glm::radians(light->outerAngle)));
            }
        }

        // Shadow-Casting Directional Lights.
        for (int i = 0; i < shadowDirectionalLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.directionalLights.buffer[lightGroup.lightIDs[shadowDirectionalLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowDirectionalLightVS_ProjectionView, i), light->projection * light->view);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShaderDirectionalLightFS_Colour,         i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShaderDirectionalLightFS_Direction,      i), glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                
                // TODO: set the shadow map.
            }
        }

        // Shadow-Casting Point Lights.
        for (int i = 0; i < shadowPointLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.pointLights.buffer[lightGroup.lightIDs[shadowPointLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowPointLightFS_PositionVS, i), glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowPointLightFS_PositionWS, i), light->position);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowPointLightFS_Colour,     i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowPointLightFS_Radius,     i), light->radius);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniform_ShadowPointLightFS_Falloff,    i), light->falloff);

                // TODO: set the shadow map.
            }
        }

        // Shadow-Casting Spot Lights.
        for (int i = 0; i < shadowSpotLightCount; ++i)
        {
            auto light = visibleObjects.lightManager.spotLights.buffer[lightGroup.lightIDs[shadowSpotLightStartIndex + i]]->value;
            assert(light != nullptr);
            {
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightVS_ProjectionView, i), light->projection * light->view);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_Position,       i), glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_Colour,         i), light->colour);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_Direction,      i), glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_Length,         i), light->length);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_Falloff,        i), light->falloff);
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_CosAngleInner,  i), glm::cos(glm::radians(light->innerAngle)));
                shader.SetUniform(this->materialUniformNames.GetLightUniformName(MaterialUniforms_ShadowSpotLightFS_CosAngleOuter,  i), glm::cos(glm::radians(light->outerAngle)));

                // TODO: set the shadow map.
            }
        }

        shader.SetUniformsFromMaterial(material);
    }



    ////////////////////////////////////////////////////////////////
    // Event Handlers from MaterialLibrary.

    void DefaultSceneRenderer::OnDeleteMaterialDefinition(MaterialDefinition &definition)
    {
        auto iShaders = this->materialShaders.Find(&definition);
        if (iShaders != nullptr)
        {
            delete iShaders->value;
            this->materialShaders.RemoveByIndex(iShaders->index);
        }
    }

    void DefaultSceneRenderer::OnReloadMaterialDefinition(MaterialDefinition &definition)
    {
        // All we want to do is delete the shaders. This will force the renderer to recreated them when the material is used next.
        auto iShaders = this->materialShaders.Find(&definition);
        if (iShaders != nullptr)
        {
            delete iShaders->value;
            this->materialShaders.RemoveByIndex(iShaders->index);
        }
    }



    /////////////////////////////////////////////////////////
    // Private

    DefaultSceneRendererFramebuffer* DefaultSceneRenderer::GetViewportFramebuffer(SceneViewport &viewport)
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

    void DefaultSceneRenderer::RenderFinalComposition(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer)
    {
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();
        Renderer::DisableStencilTest();

        if (this->IsHDREnabled())
        {
            // HDR requires luminocity.
            this->luminanceChain.ComputeLuminance(*sourceColourBuffer, this->hdrExposure);


            // Might need a bloom buffer.
            if (this->IsBloomEnabled())
            {
                this->RenderBloomMap(framebuffer, sourceColourBuffer);
            }



            // Framebuffer Setup.
            int finalOutputBufferIndex = 4;

            Renderer::SetCurrentFramebuffer(framebuffer->framebuffer);
            Renderer::SetDrawBuffers(1, &finalOutputBufferIndex);
            Renderer::SetViewport(0, 0, framebuffer->width, framebuffer->height);



            // Shader Setup.
            if (this->IsBloomEnabled())
            {
                Renderer::SetCurrentShader(this->finalCompositionShaderHDR);
                this->finalCompositionShaderHDR->SetUniform("ColourBuffer",    sourceColourBuffer);
                this->finalCompositionShaderHDR->SetUniform("LuminanceBuffer", this->luminanceChain.GetLuminanceBuffer());
                this->finalCompositionShaderHDR->SetUniform("BloomBuffer",     framebuffer->bloomBuffer);
                this->finalCompositionShaderHDR->SetUniform("BloomFactor",     this->bloomFactor);
                Renderer::PushPendingUniforms(*this->finalCompositionShaderHDR);
            }
            else
            {
                Renderer::SetCurrentShader(this->finalCompositionShaderHDRNoBloom);
                this->finalCompositionShaderHDRNoBloom->SetUniform("ColourBuffer",    sourceColourBuffer);
                this->finalCompositionShaderHDRNoBloom->SetUniform("LuminanceBuffer", this->luminanceChain.GetLuminanceBuffer());
                Renderer::PushPendingUniforms(*this->finalCompositionShaderHDRNoBloom);
            }
        }
        else
        {
            Renderer::SetTexture2DFilter(*sourceColourBuffer, TextureFilter_Nearest, TextureFilter_Nearest);


            // Framebuffer Setup.
            int finalOutputBufferIndex = 4;

            Renderer::SetCurrentFramebuffer(framebuffer->framebuffer);
            Renderer::SetDrawBuffers(1, &finalOutputBufferIndex);
            Renderer::SetViewport(0, 0, framebuffer->width, framebuffer->height);


            // Shader Setup.
            Renderer::SetCurrentShader(this->finalCompositionShaderLDR);
            this->finalCompositionShaderLDR->SetUniform("ColourBuffer", sourceColourBuffer);
            Renderer::PushPendingUniforms(*this->finalCompositionShaderLDR);
        }


        // Draw.
        Renderer::Draw(*this->fullscreenTriangleVA);
    }

    void DefaultSceneRenderer::RenderBloomMap(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer)
    {
        // Framebuffer Setup.
        int bufferIndex     = 0;
        int blurBufferIndex = 1;

        Renderer::SetCurrentFramebuffer(framebuffer->bloomFramebuffer);
        Renderer::SetDrawBuffers(1, &bufferIndex);
        Renderer::SetViewport(0, 0, framebuffer->bloomBuffer->GetWidth(), framebuffer->bloomBuffer->GetHeight());


        // Shader Setup.
        Renderer::SetCurrentShader(this->bloomShader);
        this->bloomShader->SetUniform("ColourBuffer", sourceColourBuffer);
        Renderer::PushPendingUniforms(*this->bloomShader);

        // Draw.
        Renderer::Draw(*this->fullscreenTriangleVA);



        // Gaussian Blur.

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->bloomBlurShaderX);
            this->bloomBlurShaderX->SetUniform("Texture",               framebuffer->bloomBuffer);
            this->bloomBlurShaderX->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(framebuffer->bloomBlurBuffer->GetWidth()));
            Renderer::PushPendingUniforms(*this->bloomBlurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &bufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->bloomBlurShaderY);
            this->bloomBlurShaderY->SetUniform("Texture",               framebuffer->bloomBlurBuffer);
            this->bloomBlurShaderY->SetUniform("TextureSizeReciprocal", 1.0f / static_cast<float>(framebuffer->bloomBlurBuffer->GetHeight()));
            Renderer::PushPendingUniforms(*this->bloomBlurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }
    }



    ///////////////////////
    // Materials.

    DefaultSceneRenderer_MaterialShaders* DefaultSceneRenderer::GetMaterialShaders(Material &material)
    {
        // A single set of shaders is created for each definition. We map the shaders to the definition, with the definition acting as the key.
        auto iMaterialShaders = this->materialShaders.Find(&material.GetDefinition());
        if (iMaterialShaders == nullptr)
        {
            // The shaders structure has not yet been created, so it needs to be created now.
            auto shaders = new DefaultSceneRenderer_MaterialShaders;
            this->materialShaders.Add(&material.GetDefinition(), shaders);

            return shaders;
        }
        else
        {
            // The shaders structure has already been created, so we just return that.
            return iMaterialShaders->value;
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
