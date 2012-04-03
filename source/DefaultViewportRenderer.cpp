
#include <GTEngine/DefaultViewportRenderer.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    DVR_RCBegin::DVR_RCBegin()
        : framebuffer(nullptr), depthStencil(nullptr), finalOutput(nullptr), materialBuffer0(nullptr), materialBuffer1(nullptr), materialBuffer2(nullptr),
          viewportWidth(0), viewportHeight(0)
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
            int drawBuffers[] = {0, 1, 2};

            // We will prepare the framebuffer for the first pass, which is the material pass.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencil);
            this->framebuffer->AttachColourBuffer(this->materialBuffer0, 0);
            this->framebuffer->AttachColourBuffer(this->materialBuffer1, 1);
            this->framebuffer->AttachColourBuffer(this->materialBuffer2, 2);

            Renderer::SetFramebuffer(this->framebuffer);
            Renderer::SetDrawBuffers(3, drawBuffers);

            Renderer::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);

            Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer::ClearDepth(1.0f);
            Renderer::Clear(GTEngine::ColourBuffer | GTEngine::DepthBuffer);

            Renderer::EnableDepthTest();
            Renderer::SetDepthFunc(DepthFunc_LEqual);
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
        int drawBuffers[] = {0, 1};

        // The lighting buffers need to be cleared to black at the start of the frame.
        this->framebuffer->AttachColourBuffer(this->lightingBuffer0, 0);
        this->framebuffer->AttachColourBuffer(this->lightingBuffer1, 1);

        Renderer::SetFramebuffer(this->framebuffer);
        Renderer::SetDrawBuffers(2, drawBuffers);

        // Clearing to black is important here.
        Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::Clear(GTEngine::ColourBuffer);

        Renderer::SetDepthFunc(DepthFunc_Equal);
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
            int drawBuffers[] = {0};

            this->framebuffer->AttachColourBuffer(this->finalOutputBuffer, 0);
            Renderer::SetFramebuffer(this->framebuffer);
            Renderer::SetDrawBuffers(1, drawBuffers);

            Renderer::SetShader(this->combinerShader);
            Renderer::SetShaderParameter("LightingBuffer0", this->lightingBuffer0);
            Renderer::SetShaderParameter("LightingBuffer1", this->lightingBuffer1);
            Renderer::SetShaderParameter("MaterialBuffer0", this->materialBuffer0);
            Renderer::SetShaderParameter("MaterialBuffer1", this->materialBuffer1);

            Renderer::DisableDepthTest();
            Renderer::DisableBlending();

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
        }
    }
}

namespace GTEngine
{
    DefaultViewportRenderer::DefaultViewportRenderer()
        : owner(nullptr), framebuffer(), Shaders(), RenderCommands(), backRCIndex(0),
          materialMetadata(),
          projection(), view(),
          screenSize(),
          framebufferNeedsResize(false)
    {
        RenderCommands.rcBegin[0].framebuffer = &this->framebuffer;
        RenderCommands.rcBegin[1].framebuffer = &this->framebuffer;

        Shaders.lightingD1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_D1");
        Shaders.lightingA1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1");
        Shaders.lightingP1   = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_P1");
        Shaders.lightingA1D1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1D1");
        Shaders.lightingA1P1 = ShaderLibrary::Acquire("Engine_DefaultVS",         "Engine_LightingPass_A1P1");
        Shaders.combiner     = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS", "Engine_LightingMaterialCombiner");

        RenderCommands.rcEnd[0].combinerShader = Shaders.combiner;
        RenderCommands.rcEnd[1].combinerShader = Shaders.combiner;
    }

    DefaultViewportRenderer::~DefaultViewportRenderer()
    {
        ShaderLibrary::Unacquire(Shaders.lightingD1);
        ShaderLibrary::Unacquire(Shaders.lightingA1);
        ShaderLibrary::Unacquire(Shaders.lightingP1);
        ShaderLibrary::Unacquire(Shaders.lightingA1D1);
        ShaderLibrary::Unacquire(Shaders.lightingA1P1);
        ShaderLibrary::Unacquire(Shaders.combiner);

        /// The material shaders need to be deleted.
        auto &shaders = this->Shaders.materialPassShaders.GetShaders();
        for (size_t i = 0; i < shaders.count; ++i)
        {
            delete shaders.buffer[i]->value;
        }

        /// Each material's metadata needs to be deleted.
        for (size_t i = 0; i < this->materialMetadata.count; ++i)
        {
            delete this->materialMetadata.buffer[i]->value;
        }
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
            auto &modelNodes            = this->owner->GetModelNodes();
            auto &ambientLightNodes     = this->owner->GetAmbientLightNodes();
            auto &directionalLightNodes = this->owner->GetDirectionalLightNodes();
            auto &pointLightNodes       = this->owner->GetPointLightNodes();


            // First we'll grab the render command objects we'll be adding to the back buffer.
            auto &rcBegin = this->RenderCommands.rcBegin[this->backRCIndex];
            auto &rcEnd   = this->RenderCommands.rcEnd[this->backRCIndex];

            
            

            // Step 1: Begin the frame.
            rcBegin.Init(this->framebuffer);
            Renderer::BackRCQueue->Append(rcBegin);

            // Step 2: Material pass. This will also fill the depth buffer. No lighting is done here.
            this->DoMaterialPass(modelNodes);

            // Step 3: The lighting pass.
            this->DoLightingPass(modelNodes, ambientLightNodes, directionalLightNodes, pointLightNodes);

            // Step 4: End the frame.
            rcEnd.Init(this->framebuffer);
            Renderer::BackRCQueue->Append(rcEnd);
        }
    }

    void DefaultViewportRenderer::OnSwapRCQueues()
    {
        this->backRCIndex = !this->backRCIndex;

        // The new back RC caches need to be reset in preparation for the next frame.
        this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Reset();
        this->RenderCommands.rcDrawVA[this->backRCIndex].Reset();

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




    DefaultViewportRenderer::MaterialMetadata & DefaultViewportRenderer::GetMaterialMetadata(Material &material)
    {
        auto iMetadata = this->materialMetadata.Find(&material);
        if (iMetadata != nullptr)
        {
            // If we get here it means the metadata has already been created. We return that same object.
            assert(iMetadata->value != nullptr);
            return *iMetadata->value;
        }
        else
        {
            // If we get here it means the metadata has not yet been created. We need to create it.
            auto metadata = new MaterialMetadata;
            this->materialMetadata.Add(&material, metadata);

            return *metadata;
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

    void DefaultViewportRenderer::DoMaterialPass(const GTCore::Vector<SceneNode*> &modelNodes)
    {
        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelNode->GetWorldTransformMatrix();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh     = model->meshes[iMesh];
                    auto material = model->materials[iMesh];
                    assert(mesh != nullptr);

                    if (material != nullptr)
                    {
                        // We need to grab a render command from the cache...
                        auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                        rc.SetVertexArray(mesh->va);

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
                        rc.SetShader(materialMetadata.materialPassShader);

                        rc.SetParameter("MVPMatrix",    MVPMatrix);
                        rc.SetParameter("NormalMatrix", NormalMatrix);
                            
                        // The material may have pending properties. These need to be set on the shader also.
                        auto &materialParams = material->GetPendingParameters();
                        for (size_t iProperty = 0; iProperty < materialParams.count; ++iProperty)
                        {
                            auto iParam = materialParams.buffer[iProperty];
                            assert(iParam        != nullptr);
                            assert(iParam->value != nullptr);

                            rc.SetParameter(iParam->key, iParam->value);
                        }


                        // Now we simply append the render command...
                        Renderer::BackRCQueue->Append(rc);
                    }
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass(const GTCore::Vector<SceneNode*> &modelNodes,
                                                 const GTCore::Vector<SceneNode*> &ambientLightNodes,
                                                 const GTCore::Vector<SceneNode*> &directionalLightNodes,
                                                 const GTCore::Vector<SceneNode*> &pointLightNodes)
    {
        auto &rc = this->RenderCommands.rcBeginLighting[this->backRCIndex];
        rc.SetFramebuffer(this->framebuffer);

        Renderer::BackRCQueue->Append(rc);


        size_t iALight = 0;
        size_t iDLight = 0;
        size_t iPLight = 0;

        
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
    }

    void DefaultViewportRenderer::DoLightingPass_A1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(lightNode != nullptr);

        auto lightComponent = lightNode->GetComponent<AmbientLightComponent>();
        assert(lightComponent != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 MVPMatrix    = this->projection * (this->view * modelNode->GetWorldTransformMatrix());
                glm::mat3 NormalMatrix = glm::inverse(glm::transpose(glm::mat3(MVPMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    // We need to grab a render command from the cache...
                    auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                    rc.SetVertexArray(mesh->va);

                    rc.SetParameter("ALights[0].Colour", lightComponent->GetColour());

                    rc.SetParameter("MVPMatrix",    MVPMatrix);
                    rc.SetParameter("NormalMatrix", NormalMatrix);

                    Renderer::BackRCQueue->Append(rc);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_D1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(lightNode != nullptr);

        auto lightComponent = lightNode->GetComponent<DirectionalLightComponent>();
        assert(lightComponent != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingD1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelNode->GetWorldTransformMatrix();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    // We need to grab a render command from the cache...
                    auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                    rc.SetVertexArray(mesh->va);

                    rc.SetParameter("DLights[0].Colour",    lightComponent->GetColour());
                    rc.SetParameter("DLights[0].Direction", glm::normalize(glm::mat3(this->view) * lightNode->GetForwardVector()));

                    rc.SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc.SetParameter("MVPMatrix",       MVPMatrix);
                    rc.SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackRCQueue->Append(rc);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_P1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(lightNode != nullptr);

        auto lightComponent = lightNode->GetComponent<PointLightComponent>();
        assert(lightComponent != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingP1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelNode->GetWorldTransformMatrix();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    // We need to grab a render command from the cache...
                    auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                    rc.SetVertexArray(mesh->va);

                    rc.SetParameter("PLights[0].Position",             glm::vec3(this->view * glm::vec4(lightNode->GetWorldPosition(), 1.0f)));
                    rc.SetParameter("PLights[0].Colour",               lightComponent->GetColour());
                    rc.SetParameter("PLights[0].ConstantAttenuation",  lightComponent->GetConstantAttenuation());
                    rc.SetParameter("PLights[0].LinearAttenuation",    lightComponent->GetLinearAttenuation());
                    rc.SetParameter("PLights[0].QuadraticAttenuation", lightComponent->GetQuadraticAttenuation());
                    
                    rc.SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc.SetParameter("MVPMatrix",       MVPMatrix);
                    rc.SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackRCQueue->Append(rc);
                }
            }
        }
    }


    void DefaultViewportRenderer::DoLightingPass_A1D1(SceneNode* A0, SceneNode* D0, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(A0 != nullptr);
        assert(D0 != nullptr);

        auto ambientLightComponent     = A0->GetComponent<AmbientLightComponent>();
        auto directionalLightComponent = D0->GetComponent<DirectionalLightComponent>();

        assert(ambientLightComponent     != nullptr);
        assert(directionalLightComponent != nullptr);


        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1D1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelNode->GetWorldTransformMatrix();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    // We need to grab a render command from the cache...
                    auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                    rc.SetVertexArray(mesh->va);

                    rc.SetParameter("ALights[0].Colour",    ambientLightComponent->GetColour());

                    rc.SetParameter("DLights[0].Colour",    directionalLightComponent->GetColour());
                    rc.SetParameter("DLights[0].Direction", glm::normalize(glm::mat3(this->view) * D0->GetForwardVector()));

                    rc.SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc.SetParameter("MVPMatrix",       MVPMatrix);
                    rc.SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackRCQueue->Append(rc);
                }
            }
        }
    }

    void DefaultViewportRenderer::DoLightingPass_A1P1(SceneNode* A0, SceneNode* P0, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(A0 != nullptr);
        assert(P0 != nullptr);

        auto ambientLightComponent = A0->GetComponent<AmbientLightComponent>();
        auto pointLightComponent   = P0->GetComponent<PointLightComponent>();

        assert(ambientLightComponent != nullptr);
        assert(pointLightComponent   != nullptr);


        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        auto &rc = this->RenderCommands.rcBeginLightingPass[this->backRCIndex].Acquire();
        rc.Init(this->framebuffer, this->Shaders.lightingA1P1, this->screenSize);

        Renderer::BackRCQueue->Append(rc);

        for (size_t iNode = 0; iNode < modelNodes.count; ++iNode)
        {
            auto modelNode = modelNodes[iNode];
            assert(modelNode != nullptr);
            
            auto modelComponent = modelNode->GetComponent<ModelComponent>();
            assert(modelComponent != nullptr);

            auto model = modelComponent->GetModel();
            if (model != nullptr)
            {
                glm::mat4 ModelViewMatrix = this->view * modelNode->GetWorldTransformMatrix();
                glm::mat4 MVPMatrix       = this->projection * ModelViewMatrix;
                glm::mat3 NormalMatrix    = glm::inverse(glm::transpose(glm::mat3(ModelViewMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh = model->meshes[iMesh];
                    assert(mesh != nullptr);

                    // We need to grab a render command from the cache...
                    auto &rc = this->RenderCommands.rcDrawVA[this->backRCIndex].Acquire();
                    rc.SetVertexArray(mesh->va);

                    rc.SetParameter("ALights[0].Colour",    ambientLightComponent->GetColour());

                    rc.SetParameter("PLights[0].Position",             glm::vec3(this->view * glm::vec4(P0->GetWorldPosition(), 1.0f)));
                    rc.SetParameter("PLights[0].Colour",               pointLightComponent->GetColour());
                    rc.SetParameter("PLights[0].ConstantAttenuation",  pointLightComponent->GetConstantAttenuation());
                    rc.SetParameter("PLights[0].LinearAttenuation",    pointLightComponent->GetLinearAttenuation());
                    rc.SetParameter("PLights[0].QuadraticAttenuation", pointLightComponent->GetQuadraticAttenuation());

                    rc.SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc.SetParameter("MVPMatrix",       MVPMatrix);
                    rc.SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackRCQueue->Append(rc);
                }
            }
        }
    }
}
