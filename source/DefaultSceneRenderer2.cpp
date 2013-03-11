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
    // DefaultSceneRendererVisibleObjects

    DefaultSceneRendererVisibleObjects::DefaultSceneRendererVisibleObjects(SceneViewport &viewport)
        : opaqueObjects(),     alphaTransparentObjects(),     refractiveTransparentObjects(),
          opaqueObjectsLast(), alphaTransparentObjectsLast(), refractiveTransparentObjectsLast(),
          ambientLights(), directionalLights(), pointLights(), spotLights(),
          meshesToAnimate(),
          projectionMatrix(), viewMatrix(), projectionViewMatrix()
    {
        auto cameraNode = viewport.GetCameraNode();
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
                    DefaultSceneRendererPointLightObject object;
                    object.colour               = lightComponent->GetColour();
                    object.position             = sceneNode.GetWorldPosition();
                    object.constantAttenuation  = lightComponent->GetConstantAttenuation();
                    object.linearAttenuation    = lightComponent->GetLinearAttenuation();
                    object.quadraticAttenuation = lightComponent->GetQuadraticAttenuation();

                    this->pointLights.PushBack(object);
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
                    DefaultSceneRendererSpotLightObject object;
                    object.colour               = lightComponent->GetColour();
                    object.position             = sceneNode.GetWorldPosition();
                    object.direction            = sceneNode.GetWorldForwardVector();
                    object.constantAttenuation  = lightComponent->GetConstantAttenuation();
                    object.linearAttenuation    = lightComponent->GetLinearAttenuation();
                    object.quadraticAttenuation = lightComponent->GetQuadraticAttenuation();
                    object.innerAngle           = lightComponent->GetInnerAngle();
                    object.outerAngle           = lightComponent->GetOuterAngle();

                    this->spotLights.PushBack(object);
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
                    DefaultSceneRendererAmbientLightObject object;
                    object.colour = lightComponent->GetColour();

                    this->ambientLights.PushBack(object);
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
                    DefaultSceneRendererDirectionalLightObject object;
                    object.colour    = lightComponent->GetColour();
                    object.direction = sceneNode.GetWorldForwardVector();

                    this->directionalLights.PushBack(object);
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
          materialLibraryEventHandler(*this)
    {
        this->depthPassShader = Renderer2::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassFS"));


        // The event handler needs to be attached to the material library so we can catch when a material definition is deleted an
        // in turn delete the shaders we've associated with that material.
        MaterialLibrary::AttachEventHandler(this->materialLibraryEventHandler);
    }

    DefaultSceneRenderer2::~DefaultSceneRenderer2()
    {
        Renderer2::DeleteShader(this->depthPassShader);

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
        DefaultSceneRendererVisibleObjects visibleObjects(viewport);
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
    }

    void DefaultSceneRenderer2::AddViewport(SceneViewport &viewport)
    {
        // We create a framebuffer for every viewport. We map these to viewports with a simple GTCore::Map container.
        auto framebuffer = this->CreateFramebuffer(viewport.GetWidth(), viewport.GetHeight());
        assert(framebuffer != nullptr);
        {
            viewport.SetColourBuffer(framebuffer->colourOutputBuffer);
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
        newFramebuffer->colourOutputBuffer = Renderer2::CreateTexture2D();
        newFramebuffer->lightingBuffer0    = Renderer2::CreateTexture2D();
        newFramebuffer->lightingBuffer1    = Renderer2::CreateTexture2D();

        // Sizes and formats need to be set. All we need to do is call the Resize() method.
        newFramebuffer->Resize(width, height);


        // Filters.
        Renderer2::SetTexture2DFilter(*newFramebuffer->colourOutputBuffer, TextureFilter_Nearest, TextureFilter_Nearest);
        Renderer2::SetTexture2DFilter(*newFramebuffer->lightingBuffer0,    TextureFilter_Nearest, TextureFilter_Nearest);
        Renderer2::SetTexture2DFilter(*newFramebuffer->lightingBuffer1,    TextureFilter_Nearest, TextureFilter_Nearest);
        
        // Wrap Modes.
        Renderer2::SetTexture2DWrapMode(*newFramebuffer->colourOutputBuffer, TextureWrapMode_ClampToEdge);


        // Attach to the main framebuffer.
        newFramebuffer->framebuffer->AttachDepthStencilBuffer(newFramebuffer->depthStencilBuffer);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->colourOutputBuffer, 0);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->lightingBuffer0,    1);
        newFramebuffer->framebuffer->AttachColourBuffer(newFramebuffer->lightingBuffer1,    2);

        Renderer2::PushAttachments(*newFramebuffer->framebuffer);


        return newFramebuffer;
    }

    void DefaultSceneRenderer2::DeleteFramebuffer(DefaultSceneRendererFramebuffer* framebufferToDelete)
    {
        Renderer2::DeleteTexture2D(framebufferToDelete->depthStencilBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->colourOutputBuffer);
        Renderer2::DeleteTexture2D(framebufferToDelete->lightingBuffer0);
        Renderer2::DeleteTexture2D(framebufferToDelete->lightingBuffer1);
        
        Renderer2::DeleteFramebuffer(framebufferToDelete->framebuffer);

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
        (void)framebuffer;


        // The lighting buffers must be cleared to black. Also need to clear the depth/stencil buffer.
        int lightingBuffers[] = {1, 2};
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

        // Spot Lights.
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
    }

    void DefaultSceneRenderer2::RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        // This pass draws the objects like normal and grab the lighting information from the lighting buffers.
        Renderer2::DisableBlending();
        Renderer2::SetDepthFunction(RendererFunction_Equal);

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
        auto &light = visibleObjects.ambientLights[lightIndex];

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
                        shader->SetParameter("Colour",          light.colour);
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
        auto &light = visibleObjects.directionalLights[lightIndex];

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
                        shader->SetParameter("Colour",          light.colour);
                        shader->SetParameter("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light.direction));
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
        auto &light = visibleObjects.pointLights[lightIndex];

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
                        shader->SetParameter("Colour",               light.colour);
                        shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light.position, 1.0f)));
                        shader->SetParameter("ConstantAttenuation",  light.constantAttenuation);
                        shader->SetParameter("LinearAttenuation",    light.linearAttenuation);
                        shader->SetParameter("QuadraticAttenuation", light.quadraticAttenuation);
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
        auto &light = visibleObjects.spotLights[lightIndex];

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
                        shader->SetParameter("Colour",               light.colour);
                        shader->SetParameter("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light.position, 1.0f)));
                        shader->SetParameter("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light.direction));
                        shader->SetParameter("ConstantAttenuation",  light.constantAttenuation);
                        shader->SetParameter("LinearAttenuation",    light.linearAttenuation);
                        shader->SetParameter("QuadraticAttenuation", light.quadraticAttenuation);
                        shader->SetParameter("CosAngleInner",        glm::cos(glm::radians(light.innerAngle)));
                        shader->SetParameter("CosAngleOuter",        glm::cos(glm::radians(light.outerAngle)));
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
