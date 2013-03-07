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
        : opaqueObjects(), alphaTransparentObjects(), refractiveTransparentObjects(),
          ambientLights(), directionalLights(),
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
                        glm::mat4 transform = sceneNode.GetWorldTransform();

                        for (size_t i = 0; i < model->meshes.count; ++i)
                        {
                            auto mesh = model->meshes[i];
                            assert(mesh != nullptr);
                            {
                                // The mesh will not be drawn at all if it has not material. This material is allowed to be null.
                                auto material = mesh->GetMaterial();
                                if (material != nullptr)
                                {
                                    GTCore::Vector<DefaultSceneRendererMeshObject>* objectList = nullptr;

                                    auto &materialDefinition = material->GetDefinition();
                                    if (material->IsRefractive())
                                    {
                                        auto iObjectList = this->refractiveTransparentObjects.Find(&materialDefinition);
                                        if (iObjectList != nullptr)
                                        {
                                            objectList = iObjectList->value;
                                        }
                                        else
                                        {
                                            objectList = new GTCore::Vector<DefaultSceneRendererMeshObject>;
                                            this->refractiveTransparentObjects.Add(&materialDefinition, objectList);
                                        }
                                    }
                                    //else if (material->IsAlphaTransparent())
                                    //{
                                    //}
                                    else    // Opaque
                                    {
                                        auto iObjectList = this->opaqueObjects.Find(&materialDefinition);
                                        if (iObjectList != nullptr)
                                        {
                                            objectList = iObjectList->value;
                                        }
                                        else
                                        {
                                            objectList = new GTCore::Vector<DefaultSceneRendererMeshObject>(100);
                                            this->opaqueObjects.Add(&materialDefinition, objectList);
                                        }
                                    }

                                    assert(objectList != nullptr);
                                    {
                                        DefaultSceneRendererMeshObject object;
                                        object.mesh      = mesh;
                                        object.transform = transform;

                                        objectList->PushBack(object);
                                    }



                                    // If the mesh needs to be animated, mark it as such.
                                    if (model->IsAnimating() && mesh->IsAnimated())
                                    {
                                        if (!this->meshesToAnimate.Exists(mesh))
                                        {
                                            this->meshesToAnimate.PushBack(mesh);
                                        }
                                    }
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
        (void)object;
    }

    void DefaultSceneRendererVisibleObjects::ProcessObjectSpotLight(SceneObject &object)
    {
        (void)object;
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


    void DefaultSceneRendererVisibleObjects::PostProcess()
    {
        // Apply skinning.
        for (size_t i = 0; i < this->meshesToAnimate.count; ++i)
        {
            auto mesh = this->meshesToAnimate[i];
            assert(mesh != nullptr);
            {
                mesh->ApplySkinning();
            }
        }
    }




    ///////////////////////////////////////////////////////////
    // DefaultSceneRenderer

    DefaultSceneRenderer2::DefaultSceneRenderer2()
        : viewportFramebuffers(), materialShadersToDelete(), depthPassShader(nullptr), materialLibraryEventHandler(*this)
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

        visibleObjects.PostProcess();



        // 1) Make everything current.
        auto framebuffer = this->GetViewportFramebuffer(viewport);
        assert(framebuffer != nullptr);
        {
            //Renderer2::SetCurrentFramebuffer(framebuffer->framebuffer);
            //Renderer2::SetViewport(0, 0, framebuffer->width, framebuffer->height);

            //int drawBuffers[] = {0};
            //Renderer2::SetDrawBuffers(1, drawBuffers);


            // Clear.
            //Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            //Renderer2::SetClearDepth(1.0f);
            //Renderer2::SetClearStencil(0);
            //Renderer2::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);
        }





        // 2) The opaque pass.
        //this->RenderOpaquePass(scene, framebuffer, visibleObjects);

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

    void DefaultSceneRenderer2::RenderOpaquePass(Scene &scene, DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRendererVisibleObjects &visibleObjects)
    {
        (void)scene;

        /*
        // Lighting buffers need to be cleared to black. This will be the first clear, so we'll also clear the depth/stencil buffer here.
        int lightingBuffers[] = {1, 2};
        Renderer2::SetDrawBuffers(2, lightingBuffers);

        Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::SetClearStencil(0);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);
        */
        /*
        // First Pass: Depth Pre-Pass.
        //
        // This is needed so that blending works correctly. An optimization could be that when there is a full-scene light, skip this
        // pass and instead just disable blending for the first full-scene light. Will experiment with this later.
        Renderer2::DisableColourWrites();
        Renderer2::DisableBlending();
        Renderer2::EnableDepthWrites();
        Renderer2::EnableDepthTest();
        Renderer2::SetDepthFunction(RendererFunction_LEqual);
        Renderer2::SetCurrentShader(this->depthPassShader);
        
        for (size_t iObjectList = 0; iObjectList < visibleObjects.opaqueObjects.count; ++iObjectList)
        {
            auto objectList = visibleObjects.opaqueObjects.buffer[iObjectList]->value;
            assert(objectList != nullptr);
            {
                for (size_t iObject = 0; iObject < objectList->count; ++iObject)
                {
                    auto &object = objectList->Get(iObject);
                    {
                        // Shader Properties.
                        this->depthPassShader->SetParameter("PVMMatrix", object.transform);
                        {
                            Renderer2::PushShaderPendingProperties(*this->depthPassShader);
                        }
                        this->depthPassShader->ClearPendingParameters();


                        // Draw.
                        Renderer2::Draw(*object.mesh->GetGeometry());
                    }
                }
            }
        }

        Renderer2::EnableColourWrites();
        Renderer2::DisableDepthWrites();        // <-- Depth writing is not needed now that the depth pre-pass is done.
        */
        
        /*
        // Second Pass: Lighting.
        //
        // We use additive blending here.
        Renderer2::EnableBlending();
        Renderer2::SetBlendEquation(BlendEquation_Add);
        Renderer2::SetBlendFunction(BlendFunc_One, BlendFunc_One);

        size_t iAmbientLight     = 0;
        size_t iDirectionalLight = 0;

        while (iAmbientLight < visibleObjects.ambientLights.count)
        {
            auto &light = visibleObjects.ambientLights[iAmbientLight];

            for (size_t iObjectList = 0; iObjectList < visibleObjects.opaqueObjects.count; ++iObjectList)
            {
                auto objectList = visibleObjects.opaqueObjects.buffer[iObjectList]->value;
                assert(objectList != nullptr);
                {
                    for (size_t iObject = 0; iObject < objectList->count; ++iObject)
                    {
                        auto &object = objectList->Get(iObject);
                        {
                            auto material = object.mesh->GetMaterial();
                            assert(material != nullptr);                    // <-- This is asserted because it should never be placed into this list in the first place if it is null.
                            {
                                // Setup Shader.
                                auto shader = this->GetMaterialAmbientLightShader(*material);
                                assert(shader != nullptr);
                                {
                                    Renderer2::SetCurrentShader(shader);

                                    shader->SetParametersFromMaterial(*material);
                                    shader->SetParameter("NormalMatrix", glm::inverse(glm::transpose(glm::mat3(visibleObjects.viewMatrix * object.transform))));
                                    shader->SetParameter("PVMMatrix",    visibleObjects.projectionViewMatrix * object.transform);
                                    shader->SetParameter("Colour",       light.colour);
                                    {
                                        Renderer2::PushShaderPendingProperties(*shader);
                                    }
                                    shader->ClearPendingParameters();
                                }


                                // Draw.
                                Renderer2::Draw(*object.mesh->GetGeometry());
                            }
                        }
                    }
                }
            }


            ++iAmbientLight;
        }

        while (iDirectionalLight < visibleObjects.directionalLights.count)
        {
            ++iDirectionalLight;
        }
        


        // Third Pass: Materials.
        //
        // This pass draws the objects like normal and grab the lighting information from the lighting buffers.
        Renderer2::DisableBlending();
        Renderer2::SetDepthFunction(RendererFunction_Equal);
        Renderer2::DisableDepthTest();

        int outputBuffer[] = {0};
        Renderer2::SetDrawBuffers(1, outputBuffer);

        for (size_t iObjectList = 0; iObjectList < visibleObjects.opaqueObjects.count; ++iObjectList)
        {
            auto objectList = visibleObjects.opaqueObjects.buffer[iObjectList]->value;
            assert(objectList != nullptr);
            {
                for (size_t iObject = 0; iObject < objectList->count; ++iObject)
                {
                    auto &object = objectList->Get(iObject);
                    {
                        auto material = object.mesh->GetMaterial();
                        assert(material != nullptr);
                        {
                            // Shader Setup.
                            auto shader = this->GetMaterialMaterialShader(*material);
                            assert(shader != nullptr);
                            {
                                glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * object.transform;
                                glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                                Renderer2::SetCurrentShader(shader);

                                shader->SetParametersFromMaterial(*material);
                                shader->SetParameter("ViewModelMatrix",  viewModelMatrix);
                                shader->SetParameter("NormalMatrix",     normalMatrix);
                                shader->SetParameter("PVMMatrix",        visibleObjects.projectionViewMatrix * object.transform);
                                shader->SetParameter("DiffuseLighting",  framebuffer->lightingBuffer0);
                                shader->SetParameter("SpecularLighting", framebuffer->lightingBuffer1);
                                {
                                    Renderer2::PushShaderPendingProperties(*shader);
                                }
                                shader->ClearPendingParameters();
                            }

                            // Draw.
                            Renderer2::Draw(*object.mesh->GetGeometry());
                        }
                    }
                }
            }
        }
        */
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
