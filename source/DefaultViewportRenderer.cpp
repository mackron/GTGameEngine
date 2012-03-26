
#include <GTEngine/DefaultViewportRenderer.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTCore/Strings/List.hpp>


namespace GTEngine
{
    DefaultViewportRenderer_RCBegin::DefaultViewportRenderer_RCBegin()
        : framebuffer(nullptr), depthStencil(nullptr), materialDiffuse(nullptr), materialEmissive(nullptr),
          viewportWidth(0), viewportHeight(0)
    {
    }

    void DefaultViewportRenderer_RCBegin::Init(DefaultViewportRendererFramebuffer &framebuffer)
    {
        this->framebuffer      = &framebuffer;
        this->depthStencil     = framebuffer.depthStencil;
        this->materialDiffuse  = framebuffer.materialDiffuse;
        this->materialEmissive = framebuffer.materialEmissive;

        this->viewportWidth  = framebuffer.width;
        this->viewportHeight = framebuffer.height;
    }

    void DefaultViewportRenderer_RCBegin::Execute()
    {
        if (this->framebuffer != nullptr)
        {
            int drawBuffers[] = {0, 1};

            // We will prepare the framebuffer for the first pass, which is the material pass.
            this->framebuffer->AttachDepthStencilBuffer(this->depthStencil);
            this->framebuffer->AttachColourBuffer(this->materialDiffuse,  0);
            this->framebuffer->AttachColourBuffer(this->materialEmissive, 1);

            Renderer::SetFramebuffer(this->framebuffer);
            Renderer::SetDrawBuffers(2, drawBuffers);

            Renderer::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);

            Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer::ClearDepth(1.0f);
            Renderer::Clear(GTEngine::ColourBuffer | GTEngine::DepthBuffer);

            Renderer::EnableDepthTest();
            Renderer::SetDepthFunc(DepthFunc_LEqual);
            Renderer::EnableDepthWrites();
        }
    }



    DefaultViewportRenderer_RCBeginLighting::DefaultViewportRenderer_RCBeginLighting(DefaultViewportRendererFramebuffer &framebuffer)
        : framebuffer(framebuffer),
          lightingDiffuseInput(framebuffer.lightingDiffuseInput),   lightingDiffuseOutput(framebuffer.lightingDiffuseOutput),
          lightingSpecularInput(framebuffer.lightingSpecularInput), lightingSpecularOutput(framebuffer.lightingSpecularOutput)
    {
    }

    void DefaultViewportRenderer_RCBeginLighting::Execute()
    {
        int drawBuffers[] = {0, 1};

        // The first pass will be the lighting pass. We need to setup the framebuffer for this. The input lighting buffers
        // need to be cleared to 0.0. We also clear the depth/stencil buffer at the same time.
        this->framebuffer.AttachColourBuffer(this->lightingDiffuseInput,  0);
        this->framebuffer.AttachColourBuffer(this->lightingSpecularInput, 1);

        Renderer::SetFramebuffer(&this->framebuffer);
        Renderer::SetDrawBuffers(2, drawBuffers);

        // Now we clear everything. We also clear
        Renderer::ClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::Clear(GTEngine::ColourBuffer);

        // With everything clear, we now need to change the colour attachments.
        this->framebuffer.AttachColourBuffer(this->lightingDiffuseOutput,  0);
        this->framebuffer.AttachColourBuffer(this->lightingSpecularOutput, 1);

        Renderer::SetFramebuffer(&this->framebuffer);    // <-- This is required, otherwise the framebuffer won't have a chance to sync for the new colour attachments.
        Renderer::SetDrawBuffers(2, drawBuffers);

        Renderer::SetDepthFunc(DepthFunc_Equal);
        Renderer::DisableDepthWrites();
    }



    DefaultViewportRenderer_RCBeginLightingPass::DefaultViewportRenderer_RCBeginLightingPass(DefaultViewportRendererFramebuffer &framebuffer, Shader* shader, const glm::vec2 &screenSize, const glm::vec3 &cameraPosition)
        : framebuffer(framebuffer), shader(shader), screenSize(screenSize), cameraPosition(cameraPosition),
          lightingDiffuseInput(framebuffer.lightingDiffuseInput), lightingSpecularInput(framebuffer.lightingSpecularInput)
    {
    }

    void DefaultViewportRenderer_RCBeginLightingPass::Execute()
    {
        Renderer::SetShader(this->shader);

        Renderer::SetShaderParameter("Lighting_Diffuse",  this->lightingDiffuseInput);
        Renderer::SetShaderParameter("Lighting_Specular", this->lightingSpecularInput);
        Renderer::SetShaderParameter("ScreenSize",        this->screenSize);
        Renderer::SetShaderParameter("CameraPosition",    this->cameraPosition);
    }
    


    DefaultViewportRenderer_RCEnd::DefaultViewportRenderer_RCEnd()
        : framebuffer(nullptr), combinerShader(nullptr)
    {
    }

    void DefaultViewportRenderer_RCEnd::Init(DefaultViewportRendererFramebuffer &framebuffer)
    {
        this->framebuffer       = &framebuffer;
        this->finalOutputBuffer = framebuffer.finalOutput;
        this->lightingDiffuse   = framebuffer.lightingDiffuseOutput;
        this->lightingSpecular  = framebuffer.lightingSpecularOutput;
        this->materialDiffuse   = framebuffer.materialDiffuse;
        this->materialEmissive  = framebuffer.materialEmissive;
    }

    void DefaultViewportRenderer_RCEnd::Execute()
    {
        if (this->framebuffer != nullptr)
        {
            int drawBuffers[] = {0};

            this->framebuffer->AttachColourBuffer(this->finalOutputBuffer, 0);
            Renderer::SetFramebuffer(this->framebuffer);
            Renderer::SetDrawBuffers(1, drawBuffers);

            Renderer::SetShader(this->combinerShader);
            Renderer::SetShaderParameter("Lighting_Diffuse",  this->lightingDiffuse);
            Renderer::SetShaderParameter("Lighting_Specular", this->lightingSpecular);
            Renderer::SetShaderParameter("MaterialBuffer0",   this->materialDiffuse);
            Renderer::SetShaderParameter("MaterialBuffer1",   this->materialEmissive);

            Renderer::DisableDepthTest();

            Renderer::Draw(VertexArrayLibrary::GetFullscreenQuadVA());
        }
    }




    DefaultViewportRenderer_RCDrawMesh::DefaultViewportRenderer_RCDrawMesh(VertexArray* mesh)
        : mesh(mesh), shader(nullptr), parameters()
    {
    }

    DefaultViewportRenderer_RCDrawMesh::~DefaultViewportRenderer_RCDrawMesh()
    {
        // The list of properties need to be deleted.
        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            delete this->parameters.buffer[i]->value;
        }
    }

    void DefaultViewportRenderer_RCDrawMesh::Execute()
    {
        // The first thing to do is activate the shader, if we have one. It's possible that the shader has already been set
        // by a previous command. If this->shader is null, it means that is the case and we don't want to change it.
        if (this->shader != nullptr)
        {
            Renderer::SetShader(this->shader);
        }

        // Now properties need to be set on the shader.
        for (size_t i = 0; i < this->parameters.count; ++i)
        {
            auto iParam = this->parameters.buffer[i];
            assert(iParam->value != nullptr);

            iParam->value->SetOnCurrentShader(iParam->key);
        }

        // Now we draw the mesh.
        Renderer::Draw(this->mesh);
    }


    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, float x)
    {
        this->SetParameter(name, new ShaderParameter_Float(x));
    }
    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, float x, float y)
    {
        this->SetParameter(name, new ShaderParameter_Float2(x, y));
    }
    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, float x, float y, float z)
    {
        this->SetParameter(name, new ShaderParameter_Float3(x, y, z));
    }
    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, float x, float y, float z, float w)
    {
        this->SetParameter(name, new ShaderParameter_Float4(x, y, z, w));
    }

    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, const glm::mat2 &v)
    {
        this->SetParameter(name, new ShaderParameter_Float2x2(v));
    }
    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, const glm::mat3 &v)
    {
        this->SetParameter(name, new ShaderParameter_Float3x3(v));
    }
    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, const glm::mat4 &v)
    {
        this->SetParameter(name, new ShaderParameter_Float4x4(v));
    }

    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, Texture2D* texture)
    {
        this->SetParameter(name, new ShaderParameter_Texture2D(texture));
    }

    void DefaultViewportRenderer_RCDrawMesh::SetParameter(const char* name, ShaderParameter* parameter)
    {
        // If a property of the same name already exists, it means it is being overwritten. We need to ensure the old
        // ShaderParameter object is deleted.
        auto iProperty = this->parameters.Find(name);
        if (iProperty != nullptr)
        {
            delete iProperty->value;
        }

        this->parameters.Add(name, parameter);
    }



    // RCSetLightingBuffers
    DefaultViewportRenderer_RCSetLightingBuffers::DefaultViewportRenderer_RCSetLightingBuffers(DefaultViewportRendererFramebuffer &framebuffer)
        : framebuffer(framebuffer), lightingDiffuse(framebuffer.lightingDiffuseOutput), lightingSpecular(framebuffer.lightingSpecularOutput)
    {
    }

    void DefaultViewportRenderer_RCSetLightingBuffers::Execute()
    {
        int drawBuffers[] = {0, 1};

        this->framebuffer.AttachColourBuffer(this->lightingDiffuse,  0);
        this->framebuffer.AttachColourBuffer(this->lightingSpecular, 1);

        Renderer::SetFramebuffer(&this->framebuffer);
        Renderer::SetDrawBuffers(2, drawBuffers);
    }

    void DefaultViewportRenderer_RCSetLightingBuffers::OnExecuted()
    {
        delete this;
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

        Shaders.lightingD1 = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_D1");
        Shaders.lightingA1 = ShaderLibrary::Acquire("Engine_DefaultVS",          "Engine_LightingPass_A1");
        Shaders.lightingP1 = ShaderLibrary::Acquire("Engine_LightingPass_P1_VS", "Engine_LightingPass_P1");
        Shaders.combiner   = ShaderLibrary::Acquire("Engine_FullscreenQuad_VS",  "Engine_LightingMaterialCombiner");

        RenderCommands.rcEnd[0].combinerShader = Shaders.combiner;
        RenderCommands.rcEnd[1].combinerShader = Shaders.combiner;
    }

    DefaultViewportRenderer::~DefaultViewportRenderer()
    {
        ShaderLibrary::Unacquire(Shaders.lightingD1);
        ShaderLibrary::Unacquire(Shaders.lightingA1);
        ShaderLibrary::Unacquire(Shaders.combiner);
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
        // We don't do the actual resize yet. Instead we delay it for OnSwapRCBuffers().
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
            Renderer::BackBuffer->Append(&rcBegin);

            // Step 2: Material pass. This will also fill the depth buffer. No lighting is done here.
            this->DoMaterialPass(modelNodes);

            // Step 3: The lighting pass.
            this->DoLightingPass(modelNodes, ambientLightNodes, directionalLightNodes, pointLightNodes);

            // Step 4: End the frame.
            rcEnd.Init(this->framebuffer);
            Renderer::BackBuffer->Append(&rcEnd);
        }
    }

    void DefaultViewportRenderer::OnSwapRCBuffers()
    {
        this->backRCIndex = !this->backRCIndex;

        // When a framebuffer is resized, it marks the previous buffers for deletion, but does not delete them straight away. It is
        // done this way because the rendering thread may not have finished with those buffers by the time the resize occurs. The
        // call below will clean the dead buffers.
        this->framebuffer.DoCleanup();

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
        // TODO (PRIORITY): At the moment there will be a shader created for each material instantiation. This is bad. Instead
        //                  we want to reuse shaders where possible.

        // There are two parts to the shader. There is the base shader and then the material components.
        const char* baseShaderVS      = ShaderLibrary::GetShaderString("Engine_MaterialPass_VS");
        const char* baseShaderFS      = ShaderLibrary::GetShaderString("Engine_MaterialPass_FS");
        const char* materialDiffuse   = ShaderLibrary::GetShaderString(material.GetDiffuseShaderID());
        const char* materialEmissive  = ShaderLibrary::GetShaderString(material.GetEmissiveShaderID());
        const char* materialShininess = ShaderLibrary::GetShaderString(material.GetShininessShaderID());

        // With the shader strings retrieved, we need to concatinate the shaders and create the shader object.
        GTCore::Strings::List<char> fragmentShaderString;
        fragmentShaderString.Append(baseShaderFS);
        fragmentShaderString.Append("\n");
        fragmentShaderString.Append(materialDiffuse);
        fragmentShaderString.Append("\n");
        fragmentShaderString.Append(materialEmissive);
        fragmentShaderString.Append("\n");
        fragmentShaderString.Append(materialShininess);

        return new Shader(baseShaderVS, fragmentShaderString.c_str());
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
                glm::mat4 MVPMatrix    = this->projection * (this->view * modelNode->GetWorldTransformMatrix());
                glm::mat3 NormalMatrix = glm::inverse(glm::transpose(glm::mat3(MVPMatrix)));

                for (size_t iMesh = 0; iMesh < model->meshes.count; ++iMesh)
                {
                    auto mesh     = model->meshes[iMesh];
                    auto material = model->materials[iMesh];
                    assert(mesh != nullptr);

                    if (material != nullptr)
                    {
                        // We'll be drawing something. Here is a good time to create the render command. The way we do this will
                        // probably change later on to use a cache.
                        auto rc = new DefaultViewportRenderer_RCDrawMesh(mesh->va);

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
                        rc->shader = materialMetadata.materialPassShader;
                            
                        rc->SetParameter("MVPMatrix",    MVPMatrix);
                        rc->SetParameter("NormalMatrix", NormalMatrix);
                            
                        // The material may have pending properties. These need to be set on the shader also.
                        auto &materialParams = material->GetPendingParameters();
                        for (size_t iProperty = 0; iProperty < materialParams.count; ++iProperty)
                        {
                            auto iParam = materialParams.buffer[iProperty];
                            assert(iParam        != nullptr);
                            assert(iParam->value != nullptr);

                            rc->SetParameter(iParam->key, CopyShaderParameter(iParam->value));
                        }


                        // Now we simply append the render command...
                        Renderer::BackBuffer->Append(rc);
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
        (void)ambientLightNodes;
        (void)pointLightNodes;

        auto rc = new DefaultViewportRenderer_RCBeginLighting(this->framebuffer);
        Renderer::BackBuffer->Append(rc);

        bool doneFirstLightingPass = false;

        for (size_t iALight = 0; iALight < ambientLightNodes.count; ++iALight)
        {
            if (doneFirstLightingPass)
            {
                this->SwapLightingBuffers();
            }

            this->DoLightingPass_A1(ambientLightNodes[iALight], modelNodes);
            doneFirstLightingPass = true;
        }
        
        for (size_t iDLight = 0; iDLight < directionalLightNodes.count; ++iDLight)
        {
            if (doneFirstLightingPass)
            {
                this->SwapLightingBuffers();
            }

            this->DoLightingPass_D1(directionalLightNodes[iDLight], modelNodes);
            doneFirstLightingPass = true;
        }

        for (size_t iPLight = 0; iPLight < pointLightNodes.count; ++iPLight)
        {
            if (doneFirstLightingPass)
            {
                this->SwapLightingBuffers();
            }

            this->DoLightingPass_P1(pointLightNodes[iPLight], modelNodes);
            doneFirstLightingPass = true;
        }
    }

    void DefaultViewportRenderer::DoLightingPass_A1(SceneNode* lightNode, const GTCore::Vector<SceneNode*> &modelNodes)
    {
        assert(lightNode != nullptr);

        auto lightComponent = lightNode->GetComponent<AmbientLightComponent>();
        assert(lightComponent != nullptr);

        // Right from the start we can set some shader parameters. These will remain constant for every model in this pass.
        Renderer::BackBuffer->Append(new DefaultViewportRenderer_RCBeginLightingPass(this->framebuffer, this->Shaders.lightingA1, this->screenSize, glm::vec3()));  // Cam pos is not used...

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

                    // We'll be drawing something. Here is a good time to create the render command. The way we do this will
                    // probably change later on to use a cache.
                    auto rc = new DefaultViewportRenderer_RCDrawMesh(mesh->va);

                    rc->SetParameter("ALights[0].Colour", lightComponent->GetColour());

                    rc->SetParameter("MVPMatrix",    MVPMatrix);
                    rc->SetParameter("NormalMatrix", NormalMatrix);

                    Renderer::BackBuffer->Append(rc);
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
        Renderer::BackBuffer->Append(new DefaultViewportRenderer_RCBeginLightingPass(this->framebuffer, this->Shaders.lightingD1, this->screenSize, this->owner->GetCameraNode()->GetWorldPosition()));

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

                    // We'll be drawing something. Here is a good time to create the render command. The way we do this will
                    // probably change later on to use a cache.
                    auto rc = new DefaultViewportRenderer_RCDrawMesh(mesh->va);

                    rc->SetParameter("DLights[0].Colour",    lightComponent->GetColour());
                    rc->SetParameter("DLights[0].Direction", glm::normalize(glm::mat3(this->view) * lightNode->Forward())); // TODO: Test that we actually need this normalize.

                    rc->SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc->SetParameter("MVPMatrix",       MVPMatrix);
                    rc->SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackBuffer->Append(rc);
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
        Renderer::BackBuffer->Append(new DefaultViewportRenderer_RCBeginLightingPass(this->framebuffer, this->Shaders.lightingP1, this->screenSize, this->owner->GetCameraNode()->GetWorldPosition()));

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

                    // We'll be drawing something. Here is a good time to create the render command. The way we do this will
                    // probably change later on to use a cache.
                    auto rc = new DefaultViewportRenderer_RCDrawMesh(mesh->va);

                    rc->SetParameter("PLights[0].Position",             glm::vec3(this->view * glm::vec4(lightNode->GetWorldPosition(), 1.0f)));
                    rc->SetParameter("PLights[0].Colour",               lightComponent->GetColour());
                    rc->SetParameter("PLights[0].ConstantAttenuation",  lightComponent->GetConstantAttenuation());
                    rc->SetParameter("PLights[0].LinearAttenuation",    lightComponent->GetLinearAttenuation());
                    rc->SetParameter("PLights[0].QuadraticAttenuation", lightComponent->GetQuadraticAttenuation());
                    
                    rc->SetParameter("ModelViewMatrix", ModelViewMatrix);
                    rc->SetParameter("MVPMatrix",       MVPMatrix);
                    rc->SetParameter("NormalMatrix",    NormalMatrix);

                    Renderer::BackBuffer->Append(rc);
                }
            }
        }
    }




    void DefaultViewportRenderer::SwapLightingBuffers()
    {
        // With the sub-pass done, we need to swap the input and output lighting buffers.
        this->framebuffer.SwapLightingBuffers();

        auto rcSetLightingBuffers = new DefaultViewportRenderer_RCSetLightingBuffers(this->framebuffer);
        Renderer::BackBuffer->Append(rcSetLightingBuffers);
    }
}
