
#include <GTEngine/DefaultViewportRenderer.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    //////////////////////////////////////////////////////
    // DVRFramebuffer

    void DVRFramebuffer_RendererEventHandler::OnSwapRCQueues()
    {
        if (this->framebuffer.__NeedsResize())
        {
            this->framebuffer.__DoResize();
        }
    }



    //////////////////////////////////////////////////////
    // DVR_RCBegin

    DVR_RCBegin::DVR_RCBegin()
        : framebuffer(nullptr),
          viewportWidth(0), viewportHeight(0)
    {
    }

    void DVR_RCBegin::Init(DVRFramebuffer &framebuffer)
    {
        this->framebuffer    = &framebuffer;
        this->viewportWidth  = framebuffer.width;
        this->viewportHeight = framebuffer.height;
    }

    void DVR_RCBegin::Execute()
    {
        if (this->framebuffer != nullptr)
        {
            Renderer::SetFramebuffer(this->framebuffer);
            Renderer::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);
        }
    }


    DVR_RCEnd::DVR_RCEnd()
        : drawBackground(false), backgroundColour(0.5f, 0.5f, 0.5f)
    {
    }

    void DVR_RCEnd::Execute()
    {
        // If we are drawing a background colour, we will draw a solid colour full screen quad. We should be in a good rendering state
        // here with depth testing and writing disable and stencil testing enabled.
        if (this->drawBackground)
        {
            Renderer::SetStencilFunc(RendererFunction_Equal, 0, 255);
            Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Keep);

            Renderer::SetShader(ShaderLibrary::GetColouredBGQuadShader());
            Renderer::SetShaderParameter("Colour", this->backgroundColour);

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
        }

        // We best disable stencil testing here.
        Renderer::DisableStencilTest();
    }



    DVR_RCBeginLayer::DVR_RCBeginLayer()
        : isFirstLayer(true)
    {
    }

    void DVR_RCBeginLayer::Execute()
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



    DVR_RCEndLayer::DVR_RCEndLayer()
        : framebuffer(nullptr), combinerShader(nullptr),
          finalOutputBuffer(nullptr),
          lightingBuffer0(nullptr), lightingBuffer1(nullptr),
          materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr)
    {
    }

    void DVR_RCEndLayer::Init(DVRFramebuffer &framebuffer, Shader* combinerShader)
    {
        this->framebuffer       = &framebuffer;
        this->finalOutputBuffer = framebuffer.finalOutput;
        this->lightingBuffer0   = framebuffer.lightingBuffer0;
        this->lightingBuffer1   = framebuffer.lightingBuffer1;
        this->materialBuffer0   = framebuffer.materialBuffer0;
        this->materialBuffer1   = framebuffer.materialBuffer1;
        this->materialBuffer2   = framebuffer.materialBuffer2;

        this->combinerShader = combinerShader;
    }

    void DVR_RCEndLayer::Execute()
    {
        assert(this->framebuffer != nullptr);


        int drawBuffers[] = {0};        // Final output buffer.
        Renderer::SetDrawBuffers(1, drawBuffers);

        Renderer::SetShader(this->combinerShader);
        Renderer::SetShaderParameter("LightingBuffer0", this->lightingBuffer0);
        Renderer::SetShaderParameter("LightingBuffer1", this->lightingBuffer1);
        Renderer::SetShaderParameter("MaterialBuffer0", this->materialBuffer0);
        Renderer::SetShaderParameter("MaterialBuffer1", this->materialBuffer1);

        Renderer::DisableDepthTest();
        Renderer::DisableBlending();
        Renderer::SetFaceCulling(false, true);

        Renderer::EnableStencilTest();
        Renderer::SetStencilFunc(RendererFunction_Equal, 1, 255);
        Renderer::SetStencilOp(StencilOp_Keep, StencilOp_Keep, StencilOp_Increment);
        
        Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
    }





    DVR_RCBeginLighting::DVR_RCBeginLighting()
        : framebuffer(nullptr),
          lightingBuffer0(nullptr), lightingBuffer1(nullptr)
    {
    }

    void DVR_RCBeginLighting::Execute()
    {
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


    DVR_RCControlBlending::DVR_RCControlBlending()
        : enable(false), sourceFactor(BlendFunc_One), destFactor(BlendFunc_Zero)
    {
    }

    void DVR_RCControlBlending::Execute()
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



    DVR_RCBeginLightingPass::DVR_RCBeginLightingPass()
        : framebuffer(nullptr), materialBuffer2(nullptr),
          shader(nullptr), screenSize()
    {
    }

    void DVR_RCBeginLightingPass::Execute()
    {
        Renderer::SetShader(this->shader);

        Renderer::SetShaderParameter("Lighting_Normals", this->materialBuffer2);
        Renderer::SetShaderParameter("ScreenSize",       this->screenSize);
    }
}

namespace GTEngine
{
    DefaultViewportRenderer::DefaultViewportRenderer()
        : owner(nullptr), framebuffer(), Shaders(), RenderCommands(),
          materialLibraryEventHandler(*this),
          materialMetadatas(),
          clearColourBuffer(false), clearColour(0.5f, 0.5f, 0.5f),
          projection(), view(),
          screenSize(),
          framebufferNeedsResize(false)
    {
        Shaders.lightingD1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_D1");
        Shaders.lightingA1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1");
        Shaders.lightingP1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_P1");
        Shaders.lightingS1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_S1");
        Shaders.lightingA1D1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1D1");
        Shaders.lightingA1P1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1P1");
        Shaders.combiner     = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_LightingMaterialCombiner");

        RenderCommands.rcBegin[0].framebuffer = &this->framebuffer;
        RenderCommands.rcBegin[1].framebuffer = &this->framebuffer;

        // We need catch the deletion of materials so we can delete any metadata this renderer may have created. To do this, we attach an event
        // handler to the material library.
        MaterialLibrary::AttachEventHandler(this->materialLibraryEventHandler);
    }

    DefaultViewportRenderer::~DefaultViewportRenderer()
    {
        ShaderLibrary::Unacquire(Shaders.lightingD1);
        ShaderLibrary::Unacquire(Shaders.lightingA1);
        ShaderLibrary::Unacquire(Shaders.lightingP1);
        ShaderLibrary::Unacquire(Shaders.lightingS1);
        ShaderLibrary::Unacquire(Shaders.lightingA1D1);
        ShaderLibrary::Unacquire(Shaders.lightingA1P1);
        ShaderLibrary::Unacquire(Shaders.combiner);

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


        /// The event handler needs to be removed from the material library.
        MaterialLibrary::RemoveEventHandler(this->materialLibraryEventHandler);
    }

    void DefaultViewportRenderer::SetOwnerViewport(SceneViewport* owner)
    {
        this->owner = owner;
    }

    SceneViewport* DefaultViewportRenderer::GetOwnerViewport()
    {
        return this->owner;
    }

    Framebuffer & DefaultViewportRenderer::GetFramebuffer()
    {
        return this->framebuffer;
    }

    void DefaultViewportRenderer::ResizeFramebuffer(unsigned int newWidth, unsigned int newHeight)
    {
        // We don't do the actual resize yet. Instead we delay it until the next call to Render().
        this->framebufferNeedsResize = true;

        this->screenSize.x = static_cast<float>(newWidth);
        this->screenSize.y = static_cast<float>(newHeight);
    }

    void DefaultViewportRenderer::Render()
    {
        // The new back RC caches need to be reset in preparation for the next frame.
        this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Reset();
        this->RenderCommands.rcDrawVA[Renderer::BackIndex].Reset();

        // If the framebuffer needs to be resized, we best do that now. Resizing the framebuffer leaves 
        if (this->framebufferNeedsResize)
        {
            this->framebuffer.Resize(static_cast<unsigned int>(this->screenSize.x), static_cast<unsigned int>(this->screenSize.y));
            this->framebufferNeedsResize = false;
        }


        if (this->owner != nullptr)
        {
            // Step 1: Begin the frame.
            auto &rcBegin = this->RenderCommands.rcBegin[Renderer::BackIndex];
            rcBegin.Init(this->framebuffer);
            Renderer::BackRCQueue->Append(rcBegin);

            // We need to render layer-by-layer. We start from the layer with the lowest index and move upwards. At the beginning of
            // every layer we need to ensure the depth and stencil buffer is cleared.
            auto &layerCameras = this->owner->GetLayerCameraMap();
            if (layerCameras.count > 0)
            {
                for (size_t i = layerCameras.count; i > 0; --i)
                {
                    auto cameraNode = layerCameras.buffer[i - 1]->value;
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


                    // Here we retrieve the visible components.
                    this->modelComponents.count            = 0;
                    this->ambientLightComponents.count     = 0;
                    this->directionalLightComponents.count = 0;
                    this->pointLightComponents.count       = 0;
                    this->spotLightComponents.count        = 0;

                    this->owner->GetScene()->GetVisibleComponents(this->projection * this->view,
                        this->modelComponents,
                        this->ambientLightComponents,
                        this->directionalLightComponents,
                        this->pointLightComponents,
                        this->spotLightComponents);



                    // Step 2a: Begin the layer.
                    auto &rcBeginLayer = this->RenderCommands.rcBeginLayer[Renderer::BackIndex].Acquire();
                    rcBeginLayer.isFirstLayer      = (i == layerCameras.count);
                    Renderer::BackRCQueue->Append(rcBeginLayer);


                    // Step 2b: Material pass. This will fill the depth and stencil buffers.
                    this->DoMaterialPass(this->modelComponents);

                    // Step 2c: Lighting pass.
                    this->DoLightingPass(this->modelComponents, this->ambientLightComponents, this->directionalLightComponents, this->pointLightComponents, this->spotLightComponents);


                    // Step 2d: End the layer.
                    auto &rcEndLayer = this->RenderCommands.rcEndLayer[Renderer::BackIndex].Acquire();
                    rcEndLayer.Init(this->framebuffer, this->Shaders.combiner);
                    Renderer::BackRCQueue->Append(rcEndLayer);
                }
            }

            // Step 3: End the frame.
            auto &rcEnd   = this->RenderCommands.rcEnd[Renderer::BackIndex];
            rcEnd.drawBackground   = this->clearColourBuffer;
            rcEnd.backgroundColour = this->clearColour;
            Renderer::BackRCQueue->Append(rcEnd);
        }
    }


    Texture2D* DefaultViewportRenderer::GetFinalColourOutputBuffer()
    {
        return this->framebuffer.finalOutput;
    }

    Texture2D* DefaultViewportRenderer::GetFinalDepthStencilOutputBuffer()
    {
        return this->framebuffer.depthStencil;
    }


    void DefaultViewportRenderer::SetClearColour(float r, float g, float b)
    {
        this->clearColour = glm::vec3(r, g, b);
    }

    void DefaultViewportRenderer::DisableColourClears()
    {
        this->clearColourBuffer = false;
    }

    void DefaultViewportRenderer::EnableColourClears()
    {
        this->clearColourBuffer = true;
    }



    DefaultViewportRenderer::MaterialMetadata & DefaultViewportRenderer::GetMaterialMetadata(Material &material)
    {
        // The key for the metadata will be 'this'.
        auto metadata = static_cast<MaterialMetadata*>(material.GetMetadata(reinterpret_cast<size_t>(this)));
        if (metadata == nullptr)
        {
            metadata = new MaterialMetadata;
            material.SetMetadata(reinterpret_cast<size_t>(this), metadata);

            this->materialMetadatas.Append(metadata);
        }

        return *metadata;
    }

    void DefaultViewportRenderer::DeleteMaterialMetadata(Material &material)
    {
        auto metadata = static_cast<MaterialMetadata*>(material.GetMetadata(reinterpret_cast<size_t>(this)));
        if (metadata == nullptr)
        {
            this->materialMetadatas.Remove(this->materialMetadatas.Find(metadata));
            delete metadata;
        }
    }



    Shader* DefaultViewportRenderer::CreateMaterialPassShader(Material &material)
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

    void DefaultViewportRenderer::DoMaterialPass(const GTCore::Vector<ModelComponent*> &models)
    {
        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    auto material = mesh->GetMaterial();

                    if (material != nullptr)
                    {
                        RCDrawVA*         rcDrawVA;
                        RCSetFaceCulling* rcSetFaceCulling;
                        this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, rcDrawVA, rcSetFaceCulling);

                        assert(rcDrawVA         != nullptr);
                        assert(rcSetFaceCulling != nullptr);

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

                            rcDrawVA->SetVertexArray(skinnedGeometry);
                        }
                        else
                        {
                            rcDrawVA->SetVertexArray(mesh->GetGeometry());
                        }

                        // Here is where we need to retrieve a shader for the material. This managed completely by the viewport
                        // renderer. For now we are using a simple unlit shader, but this will need to change as we add things
                        // like multiple lights.
                        auto &materialMetadata = this->GetMaterialMetadata(*material);

                        // If we don't have a shader, it needs to be created.
                        if (materialMetadata.materialPassShader == nullptr)
                        {
                            materialMetadata.materialPassShader = this->CreateMaterialPassShader(*material);
                        }

                        // Now that we have the shader, we can set some properties and set it on the render command.
                        rcDrawVA->SetShader(materialMetadata.materialPassShader);

                        // The material may have pending properties. These need to be set on the shader also.
                        auto &materialParams = material->GetParameters();
                        for (size_t iProperty = 0; iProperty < materialParams.count; ++iProperty)
                        {
                            auto iParam = materialParams.buffer[iProperty];
                            assert(iParam        != nullptr);
                            assert(iParam->value != nullptr);

                            rcDrawVA->SetParameter(iParam->key, iParam->value);
                        }


                        // Here we need to append the render command. If we have transparency enabled, we don't want to append it to the back RC queue straight away. Instead
                        // we want to hold on to it and append it later.
                        if (material->IsTransparencyEnabled())
                        {
                            this->rcSetFaceCulling_Transparent.PushBack(rcSetFaceCulling);
                            this->rcDrawVA_Transparent.PushBack(rcDrawVA);
                        }
                        else
                        {
                            Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                            Renderer::BackRCQueue->Append(*rcDrawVA);
                        }
                    }
                }
            }
        }

        // Now we need to append all of the commands from transparent materials. We need to enable transparency here.
        auto &rcControlBlending_Enable = this->RenderCommands.rcControlBlending[Renderer::BackIndex].Acquire();
        rcControlBlending_Enable.EnableAlphaBlending();
        Renderer::BackRCQueue->Append(rcControlBlending_Enable);

        // At this point blending will be enabled. Now we just want to render like normal.
        for (size_t i = 0; i < rcDrawVA_Transparent.count; ++i)
        {
            Renderer::BackRCQueue->Append(*this->rcSetFaceCulling_Transparent[i]);
            Renderer::BackRCQueue->Append(*this->rcDrawVA_Transparent[i]);
        }

        this->rcSetFaceCulling_Transparent.Clear();
        this->rcDrawVA_Transparent.Clear();

        // Now we want to disable blending.
        auto &rcControlBlending_Disable = this->RenderCommands.rcControlBlending[Renderer::BackIndex].Acquire();
        rcControlBlending_Disable.Disable();
        Renderer::BackRCQueue->Append(rcControlBlending_Disable);
    }

    void DefaultViewportRenderer::DoLightingPass(const GTCore::Vector<ModelComponent*> &modelNodes,
                                                 const GTCore::Vector<AmbientLightComponent*> &ambientLightNodes,
                                                 const GTCore::Vector<DirectionalLightComponent*> &directionalLightNodes,
                                                 const GTCore::Vector<PointLightComponent*> &pointLightNodes,
                                                 const GTCore::Vector<SpotLightComponent*> &spotLightNodes)
    {
        auto &rc = this->RenderCommands.rcBeginLighting[Renderer::BackIndex];
        rc.SetFramebuffer(this->framebuffer);

        Renderer::BackRCQueue->Append(rc);


        size_t iALight = 0;
        size_t iDLight = 0;
        size_t iPLight = 0;
        size_t iSLight = 0;


        
        // A1D1 passes.
        while (ambientLightNodes.count     - iALight >= 1 &&
               directionalLightNodes.count - iDLight >= 1)
        {
            this->DoLightingPass_A1D1(ambientLightNodes[iALight], directionalLightNodes[iDLight], modelNodes);
            ++iALight;
            ++iDLight;
        }

        // A1P1 passes.
        while (ambientLightNodes.count - iALight >= 1 &&
               pointLightNodes.count   - iPLight >= 1)
        {
            this->DoLightingPass_A1P1(ambientLightNodes[iALight], pointLightNodes[iPLight], modelNodes);
            ++iALight;
            ++iPLight;
        }
        
        

        // Now we just cycle through any remaining lights. Hopefully there won't be too many of these...
        for ( ; iALight < ambientLightNodes.count; ++iALight)
        {
            this->DoLightingPass_A1(ambientLightNodes[iALight], modelNodes);
        }

        for ( ; iDLight < directionalLightNodes.count; ++iDLight)
        {
            this->DoLightingPass_D1(directionalLightNodes[iDLight], modelNodes);
        }

        for ( ; iPLight < pointLightNodes.count; ++iPLight)
        {
            this->DoLightingPass_P1(pointLightNodes[iPLight], modelNodes);
        }

        for ( ; iSLight < spotLightNodes.count; ++iSLight)
        {
            this->DoLightingPass_S1(spotLightNodes[iSLight], modelNodes);
        }
    }

    void DefaultViewportRenderer::DoLightingPass_A1(AmbientLightComponent* A0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(A0 != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 MVPMatrix    = this->projection * (this->view * modelComponent->GetNode().GetWorldTransform());
                glm::mat3 NormalMatrix = glm::inverse(glm::transpose(glm::mat3(MVPMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingA1);

                    rcDrawVA->SetParameter("ALights0.Colour", A0->GetColour());

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_D1(DirectionalLightComponent *D0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(D0 != nullptr);


        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingD1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, ModelViewMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingD1);

                    rcDrawVA->SetParameter("DLights0.Colour",    D0->GetColour());
                    rcDrawVA->SetParameter("DLights0.Direction", glm::normalize(glm::mat3(this->view) * D0->GetNode().GetWorldForwardVector()));

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_P1(PointLightComponent* P0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(P0 != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingP1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, ModelViewMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingP1);

                    rcDrawVA->SetParameter("PLights0.Position",             glm::vec3(this->view * glm::vec4(P0->GetNode().GetWorldPosition(), 1.0f)));
                    rcDrawVA->SetParameter("PLights0.Colour",               P0->GetColour());
                    rcDrawVA->SetParameter("PLights0.ConstantAttenuation",  P0->GetConstantAttenuation());
                    rcDrawVA->SetParameter("PLights0.LinearAttenuation",    P0->GetLinearAttenuation());
                    rcDrawVA->SetParameter("PLights0.QuadraticAttenuation", P0->GetQuadraticAttenuation());

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_S1(SpotLightComponent* S0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(S0 != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingS1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, ModelViewMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingS1);

                    rcDrawVA->SetParameter("SLights0.Position",             glm::vec3(this->view * glm::vec4(S0->GetNode().GetWorldPosition(), 1.0f)));
                    rcDrawVA->SetParameter("SLights0.Direction",            glm::normalize(glm::mat3(this->view) * S0->GetNode().GetWorldForwardVector()));
                    rcDrawVA->SetParameter("SLights0.CosAngleInner",        glm::cos(glm::radians(S0->GetInnerAngle())));
                    rcDrawVA->SetParameter("SLights0.CosAngleOuter",        glm::cos(glm::radians(S0->GetOuterAngle())));
                    rcDrawVA->SetParameter("SLights0.Colour",               S0->GetColour());
                    rcDrawVA->SetParameter("SLights0.ConstantAttenuation",  S0->GetConstantAttenuation());
                    rcDrawVA->SetParameter("SLights0.LinearAttenuation",    S0->GetLinearAttenuation());
                    rcDrawVA->SetParameter("SLights0.QuadraticAttenuation", S0->GetQuadraticAttenuation());

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }


    void DefaultViewportRenderer::DoLightingPass_A1D1(AmbientLightComponent* A0, DirectionalLightComponent* D0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(A0 != nullptr);
        assert(D0 != nullptr);


        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1D1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, ModelViewMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingA1D1);

                    rcDrawVA->SetParameter("ALights0.Colour",    A0->GetColour());
                    rcDrawVA->SetParameter("DLights0.Colour",    D0->GetColour());
                    rcDrawVA->SetParameter("DLights0.Direction", glm::normalize(glm::mat3(this->view) * D0->GetNode().GetWorldForwardVector()));

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_A1P1(AmbientLightComponent* A0, PointLightComponent* P0, const GTCore::Vector<ModelComponent*> &models)
    {
        assert(A0 != nullptr);
        assert(P0 != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[Renderer::BackIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1P1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iComponent = 0; iComponent < models.count; ++iComponent)
        {
            auto modelComponent = models[iComponent];
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelComponent->GetNode().GetWorldTransform();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    RCDrawVA*         rcDrawVA;
                    RCSetFaceCulling* rcSetFaceCulling;
                    this->AcquireMeshRCs(*modelComponent, iMesh, MVPMatrix, NormalMatrix, ModelViewMatrix, rcDrawVA, rcSetFaceCulling);

                    assert(rcDrawVA         != nullptr);
                    assert(rcSetFaceCulling != nullptr);

                    rcDrawVA->SetShader(this->Shaders.lightingA1P1);

                    rcDrawVA->SetParameter("ALights0.Colour", A0->GetColour());
                    rcDrawVA->SetParameter("PLights0.Position",             glm::vec3(this->view * glm::vec4(P0->GetNode().GetWorldPosition(), 1.0f)));
                    rcDrawVA->SetParameter("PLights0.Colour",               P0->GetColour());
                    rcDrawVA->SetParameter("PLights0.ConstantAttenuation",  P0->GetConstantAttenuation());
                    rcDrawVA->SetParameter("PLights0.LinearAttenuation",    P0->GetLinearAttenuation());
                    rcDrawVA->SetParameter("PLights0.QuadraticAttenuation", P0->GetQuadraticAttenuation());

                    Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                    Renderer::BackRCQueue->Append(*rcDrawVA);
                }
            }
        }
    }


    VertexArray* DefaultViewportRenderer::GetMeshGeometry(Mesh &mesh, bool animating)
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


    bool DefaultViewportRenderer::AcquireMeshRCs(ModelComponent &modelComponent, size_t meshIndex, const glm::mat4 &mvpMatrix, const glm::mat3 &normalMatrix, RCDrawVA* &rcDrawVA, RCSetFaceCulling* &rcSetFaceCulling)
    {
        auto model = modelComponent.GetModel();
        assert(model != nullptr);

        auto mesh = model->meshes[meshIndex];
        assert(mesh != nullptr);


        rcDrawVA         = &this->RenderCommands.rcDrawVA[Renderer::BackIndex].Acquire();
        rcSetFaceCulling = &this->RenderCommands.rcSetFaceCulling[Renderer::BackIndex].Acquire();

        rcDrawVA->SetVertexArray(this->GetMeshGeometry(*mesh, model->IsAnimating()));
        rcDrawVA->SetParameter("MVPMatrix",    mvpMatrix);
        rcDrawVA->SetParameter("NormalMatrix", normalMatrix);

        rcSetFaceCulling->SetCullingMode(modelComponent.CullFrontFaces(), modelComponent.CullBackFaces());


        return true;
    }

    bool DefaultViewportRenderer::AcquireMeshRCs(ModelComponent &modelComponent, size_t meshIndex, const glm::mat4 &mvpMatrix, const glm::mat3 &normalMatrix, const glm::mat4 &modelViewMatrix, RCDrawVA* &rcDrawVA, RCSetFaceCulling* &rcSetFaceCulling)
    {
        if (this->AcquireMeshRCs(modelComponent, meshIndex, mvpMatrix, normalMatrix, rcDrawVA, rcSetFaceCulling))
        {
            assert(rcDrawVA != nullptr);

            rcDrawVA->SetParameter("ModelViewMatrix", modelViewMatrix);

            return true;
        }
        
        return false;
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
