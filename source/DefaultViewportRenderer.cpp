
#include <GTEngine/DefaultViewportRenderer.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    DVR_RCBegin::DVR_RCBegin()
        : framebuffer(nullptr), depthStencil(nullptr), finalOutput(nullptr), materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr),
          viewportWidth(0), viewportHeight(0),
          clearColour(0.5f, 0.5f, 0.5f), colourClearingEnabled(false)
    {
    }

    void DVR_RCBegin::Init(DVRFramebuffer &framebuffer)
    {
        this->framebuffer     = &framebuffer;
        this->depthStencil    = framebuffer.depthStencil;
        this->finalOutput     = framebuffer.finalOutput;
        this->materialBuffer0 = framebuffer.materialBuffer0;
        this->materialBuffer1 = framebuffer.materialBuffer1;
        this->materialBuffer2 = framebuffer.materialBuffer2;

        this->viewportWidth  = framebuffer.width;
        this->viewportHeight = framebuffer.height;
    }

    void DVR_RCBegin::Execute()
    {
        if (this->framebuffer != nullptr)
        {
            Renderer::SetFramebuffer(this->framebuffer);

            int drawBuffers[] = {1, 2, 3};      // Material Buffers 0/1/2
            Renderer::SetDrawBuffers(3, drawBuffers);

            Renderer::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);

            unsigned int clearbuffers = GTEngine::DepthBuffer;
            if (this->colourClearingEnabled)
            {
                clearbuffers |= GTEngine::ColourBuffer;
                Renderer::ClearColour(this->clearColour.x, this->clearColour.y, this->clearColour.z, 0.0f);     // Important that alpha colour is 0.0f.
            }

            Renderer::ClearDepth(1.0f);
            Renderer::Clear(clearbuffers);

            Renderer::EnableDepthTest();
            Renderer::SetDepthFunc(RendererFunction_LEqual);
            Renderer::EnableDepthWrites();
        }
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
        Renderer::ClearColour(0.0f, 0.0f, 0.0f, 0.0f);      // Important that we set the alpha to 0.0f. This will be used for removing discarding pixels for layer support.
        Renderer::Clear(GTEngine::ColourBuffer);

        Renderer::SetDepthFunc(RendererFunction_Equal);
        Renderer::DisableDepthWrites();

        // We combine lighting passes using standard blending.
        Renderer::EnableBlending();
        Renderer::SetBlendEquation(BlendEquation_Add);
        Renderer::SetBlendFunc(BlendFunc_One, BlendFunc_One);
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
    


    DVR_RCEnd::DVR_RCEnd()
        : framebuffer(nullptr), combinerShader(nullptr)
    {
    }

    void DVR_RCEnd::Init(DVRFramebuffer &framebuffer)
    {
        this->framebuffer       = &framebuffer;
        this->finalOutputBuffer = framebuffer.finalOutput;
        this->lightingBuffer0   = framebuffer.lightingBuffer0;
        this->lightingBuffer1   = framebuffer.lightingBuffer1;
        this->materialBuffer0   = framebuffer.materialBuffer0;
        this->materialBuffer1   = framebuffer.materialBuffer1;
        this->materialBuffer2   = framebuffer.materialBuffer2;
    }

    void DVR_RCEnd::Execute()
    {
        if (this->framebuffer != nullptr)
        {
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

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
        }
    }
}

namespace GTEngine
{
    DefaultViewportRenderer::DefaultViewportRenderer()
        : owner(nullptr), framebuffer(), Shaders(), RenderCommands(),
          materialLibraryEventHandler(*this),
          materialMetadatas(),
          projection(), view(),
          screenSize(),
          framebufferNeedsResize(false)
    {
        RenderCommands.rcBegin[0].framebuffer = &this->framebuffer;
        RenderCommands.rcBegin[1].framebuffer = &this->framebuffer;

        Shaders.lightingD1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_D1");
        Shaders.lightingA1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1");
        Shaders.lightingP1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_P1");
        Shaders.lightingS1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_S1");
        Shaders.lightingA1D1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1D1");
        Shaders.lightingA1P1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1P1");
        Shaders.combiner     = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_LightingMaterialCombiner");

        RenderCommands.rcEnd[0].combinerShader = Shaders.combiner;
        RenderCommands.rcEnd[1].combinerShader = Shaders.combiner;


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
        // We don't do the actual resize yet. Instead we delay it for OnSwapRCQueues().
        this->framebufferNeedsResize = true;

        this->screenSize.x = static_cast<float>(newWidth);
        this->screenSize.y = static_cast<float>(newHeight);
    }

    void DefaultViewportRenderer::Render()
    {
        if (this->owner != nullptr)
        {
            // We need to retrieve a few properties to begin with. These will be used later on in the pipeline.
            auto cameraNode = this->owner->GetCameraNode();
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


            // We need to iterate over the renderable scene nodes and draw them.
            auto &modelComponents            = this->owner->GetModelComponents();
            auto &ambientLightComponents     = this->owner->GetAmbientLightComponents();
            auto &directionalLightComponents = this->owner->GetDirectionalLightComponents();
            auto &pointLightComponents       = this->owner->GetPointLightComponents();
            auto &spotLightComponents        = this->owner->GetSpotLightComponents();


            // First we'll grab the render command objects we'll be adding to the back buffer.
            auto &rcBegin = this->RenderCommands.rcBegin[Renderer::BackIndex];
            auto &rcEnd   = this->RenderCommands.rcEnd[Renderer::BackIndex];
            
            

            // Step 1: Begin the frame.
            rcBegin.Init(this->framebuffer);
            Renderer::BackRCQueue->Append(rcBegin);

            // Step 2: Material pass. This will also fill the depth buffer. No lighting is done here.
            this->DoMaterialPass(modelComponents);

            // Step 3: The lighting pass.
            this->DoLightingPass(modelComponents, ambientLightComponents, directionalLightComponents, pointLightComponents, spotLightComponents);

            // Step 4: End the frame.
            rcEnd.Init(this->framebuffer);
            Renderer::BackRCQueue->Append(rcEnd);
        }
    }

    void DefaultViewportRenderer::OnSwapRCQueues()
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
        // Probably should set this like this, but it won't crash.
        this->RenderCommands.rcBegin[0].clearColour = glm::vec3(r, g, b);
        this->RenderCommands.rcBegin[1].clearColour = glm::vec3(r, g, b);
    }

    void DefaultViewportRenderer::DisableColourClears()
    {
        this->RenderCommands.rcBegin[0].colourClearingEnabled = false;
        this->RenderCommands.rcBegin[1].colourClearingEnabled = false;
    }

    void DefaultViewportRenderer::EnableColourClears()
    {
        this->RenderCommands.rcBegin[0].colourClearingEnabled = true;
        this->RenderCommands.rcBegin[1].colourClearingEnabled = true;
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


                        // Now we simply append the render command...
                        Renderer::BackRCQueue->Append(*rcSetFaceCulling);
                        Renderer::BackRCQueue->Append(*rcDrawVA);
                    }
                }
            }
        }
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
