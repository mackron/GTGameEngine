// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    DefaultSceneRenderer::DefaultSceneRenderer()
        : viewportFramebuffers(), materialShaders(), depthPassShader(nullptr), externalMeshes(),
          shadowMapFramebuffer(512, 512), shadowMapShader(nullptr), pointShadowMapFramebuffer(256, 256), pointShadowMapShader(nullptr),
          fullscreenTriangleVA(nullptr), finalCompositionShaderHDR(nullptr), finalCompositionShaderHDRNoBloom(nullptr), finalCompositionShaderLDR(nullptr),
          bloomShader(nullptr), highlightShader(nullptr),
          blurShaderX(nullptr), blurShaderY(nullptr), blurShaderX7x7(nullptr), blurShaderY7x7(nullptr), blurShaderX11x11(nullptr), blurShaderY11x11(nullptr), blurShaderX15x15(nullptr), blurShaderY15x15(nullptr),
          isHDREnabled(true), isBloomEnabled(true), hdrExposure(1.0f), bloomFactor(1.0f),
          materialLibraryEventHandler(*this)
    {
        this->depthPassShader                  = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassVS"),        ShaderLibrary::GetShaderString("DefaultSceneRenderer_DepthPassFS"));
        this->shadowMapShader                  = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowMapVS"),        ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowMapFS"));
        this->pointShadowMapShader             = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointShadowMapVS"),   ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointShadowMapFS"));
        this->finalCompositionShaderHDR        = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionHDRFS"));
        this->finalCompositionShaderHDRNoBloom = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionHDRNoBloomFS"));
        this->finalCompositionShaderLDR        = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionLDRFS"));
        this->bloomShader                      = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_FinalCompositionVS"), ShaderLibrary::GetShaderString("DefaultSceneRenderer_BloomFS"));
        this->highlightShader                  = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_HighlightVS"),        ShaderLibrary::GetShaderString("DefaultSceneRenderer_HighlightFS"));
        this->blurShaderX                      = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurXFS"));
        this->blurShaderY                      = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurYFS"));
        this->blurShaderX7x7                   = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurX7x7FS"));
        this->blurShaderY7x7                   = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurY7x7FS"));
        this->blurShaderX11x11                 = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurX11x11FS"));
        this->blurShaderY11x11                 = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurY11x11FS"));
        this->blurShaderX15x15                 = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurX15x15FS"));
        this->blurShaderY15x15                 = Renderer::CreateShader(ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurVS"),             ShaderLibrary::GetShaderString("DefaultSceneRenderer_BlurY15x15FS"));



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
        Renderer::DeleteShader(this->blurShaderX);
        Renderer::DeleteShader(this->blurShaderY);
        Renderer::DeleteShader(this->blurShaderX7x7);
        Renderer::DeleteShader(this->blurShaderY7x7);
        Renderer::DeleteShader(this->blurShaderX11x11);
        Renderer::DeleteShader(this->blurShaderY11x11);
        Renderer::DeleteShader(this->blurShaderX15x15);
        Renderer::DeleteShader(this->blurShaderY15x15);

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



        // 1) Set default state.
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




        // 2) The opaque pass.
        this->RenderOpaquePass(framebuffer, visibleObjects);

        // 3) The alpha transparent pass (for things like sprites and particle effects).
        this->RenderBlendedTransparentPass(framebuffer, visibleObjects);

        // 4) The refractive transparent pass (for things with refractive properties).
        if (visibleObjects.refractiveTransparentObjects.count > 0)
        {
            this->RenderRefractiveTransparentPass(framebuffer, visibleObjects);
        }


        // 5) The final composition. The source colour buffer will depend on whether or not transparency was used.
        auto sourceColourBuffer = framebuffer->opaqueColourBuffer;
        if (visibleObjects.refractiveTransparentObjects.count > 0)
        {
            sourceColourBuffer = framebuffer->finalColourBufferHDR;
        }

        this->RenderFinalComposition(framebuffer, sourceColourBuffer);
    }

    void DefaultSceneRenderer::AddViewport(SceneViewport &viewport)
    {
        // We create a framebuffer for every viewport. We map these to viewports with a simple GTCore::Map container.
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

    void DefaultSceneRenderer::RenderOpaquePass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        this->RenderOpaqueLightingPass(framebuffer, visibleObjects);
        this->RenderOpaqueMaterialPass(framebuffer, visibleObjects);
    }

    void DefaultSceneRenderer::RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        // We need to do a depth pre-pass.
        Renderer::DisableColourWrites();

        // Shader needs to be made current, once.
        Renderer::SetCurrentShader(this->depthPassShader);


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


        Renderer::EnableColourWrites();
    }

    void DefaultSceneRenderer::RenderDepthPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        (void)framebuffer;

        for (size_t iMesh = 0; iMesh < meshes.count; ++iMesh)
        {
            auto &mesh = meshes.Get(iMesh);
            {
                // Shader Properties.
                this->depthPassShader->SetUniform("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
                Renderer::PushPendingUniforms(*this->depthPassShader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer::DisableDepthTest();
                if ((mesh.flags & SceneRendererMesh::NoDepthWrite)) Renderer::DisableDepthWrites();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer::EnableDepthTest();
                if ((mesh.flags & SceneRendererMesh::NoDepthWrite)) Renderer::EnableDepthWrites();
            }
        }
    }


    void DefaultSceneRenderer::RenderOpaqueLightingPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        // TODO: This needs a big improvement here. Need to combine lights into a single pass for objects.


        if (visibleObjects.opaqueObjects.count > 0 || visibleObjects.opaqueObjectsLast.count > 0)
        {
            // The lighting buffers must be cleared to black. Also need to clear the depth/stencil buffer.
            int lightingBuffers[] = {1, 2};
            Renderer::SetDrawBuffers(2, lightingBuffers);

            Renderer::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer::SetClearDepth(1.0f);
            Renderer::SetClearStencil(0);
            Renderer::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);


            // Depth Pass first.
            this->RenderDepthPass(framebuffer, visibleObjects);


            // All lighting will use additive blending so we'll set it once here.
            Renderer::SetBlendEquation(BlendEquation_Add);
            Renderer::SetBlendFunction(BlendFunc_One, BlendFunc_One);


            // The first set of lights to render are the non-shadow-casting lights. We can set some rendering state at the start so that we don't
            // have to keep changing it for each individual light. This will be different for shadow-casting lights, though, because they will
            // need to have their shadow maps rendered also.
            size_t ambientLightsRemaining     = visibleObjects.ambientLights.count;
            size_t directionalLightsRemaining = visibleObjects.directionalLights.count;
            size_t pointLightsRemaining       = visibleObjects.pointLights.count;
            size_t spotLightsRemaining        = visibleObjects.spotLights.count;

            if (ambientLightsRemaining > 0 || directionalLightsRemaining > 0 || pointLightsRemaining > 0 || spotLightsRemaining > 0)
            {
                // Depth is laid down. No need to write.
                Renderer::DisableDepthWrites();
                Renderer::SetDepthFunction(RendererFunction_Equal);
                Renderer::EnableBlending();


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

                // We have finished with the non-shadow casting lights, so now some state needs to be restored. The next lights will have shadow
                // maps generated which means they will need to handle state changes themselves.
                Renderer::EnableDepthWrites();
                Renderer::SetDepthFunction(RendererFunction_LEqual);
                Renderer::DisableBlending();
            }



            // Shadow-Casting Lights.
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
    }

    void DefaultSceneRenderer::RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        int outputBuffer[] = {0};
        Renderer::SetDrawBuffers(1, outputBuffer);

        if (this->IsBackgroundColourClearingEnabled())
        {
            auto &colour = this->GetBackgroundClearColour();
            Renderer::SetClearColour(colour.x, colour.y, colour.z, 1.0f);
            Renderer::Clear(BufferType_Colour);
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

    void DefaultSceneRenderer::RenderOpaqueMaterialPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
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


                        Renderer::SetCurrentShader(shader);
                        shader->SetUniformsFromMaterial(*material);
                        shader->SetUniform("ViewModelMatrix",  viewModelMatrix);
                        shader->SetUniform("NormalMatrix",     normalMatrix);
                        shader->SetUniform("PVMMatrix",        visibleObjects.projectionViewMatrix * mesh.transform);
                        shader->SetUniform("DiffuseLighting",  framebuffer->lightingBuffer0);
                        shader->SetUniform("SpecularLighting", framebuffer->lightingBuffer1);
                        Renderer::PushPendingUniforms(*shader);
                    }

                    // Draw.
                    if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                    {
                        Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                    }
                    if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();



                    // If we're drawing a highlight, we'll need to draw a solid colour transparent mesh over the top using alpha blending.
                    if ((mesh.flags & SceneRendererMesh::DrawHighlight))
                    {
                        Renderer::EnableBlending();
                        Renderer::SetBlendColour(1.0f, 0.66f, 0.33f, 1.0f);
                        Renderer::SetBlendFunction(BlendFunc_Zero, BlendFunc_ConstantColour);


                        // Shader.
                        Renderer::SetCurrentShader(this->highlightShader);
                        this->highlightShader->SetUniform("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
                        Renderer::PushPendingUniforms(*this->highlightShader);


                        // Draw.
                        Renderer::Draw(*mesh.vertexArray, mesh.drawMode);


                        Renderer::DisableBlending();
                    }
                }
            }
        }
    }




    void DefaultSceneRenderer::RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
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

    void DefaultSceneRenderer::RenderOpaqueAmbientLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.ambientLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("Colour",          light->colour);
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer::RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
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

    void DefaultSceneRenderer::RenderOpaqueDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.directionalLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("Colour",          light->colour);
                            shader->SetUniform("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }


    void DefaultSceneRenderer::RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer::SetCurrentFramebuffer(this->shadowMapFramebuffer.framebuffer);
        Renderer::SetCurrentShader(this->shadowMapShader);
        Renderer::SetViewport(0, 0, this->shadowMapFramebuffer.width, this->shadowMapFramebuffer.height);

        Renderer::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::Clear(BufferType_Colour | BufferType_Depth);

        int colourBufferIndex = 0;
        int blurBufferIndex   = 1;
        Renderer::SetDrawBuffers(1, &colourBufferIndex);


        auto light = visibleObjects.shadowDirectionalLights.buffer[lightIndex]->value;
        assert(light != nullptr);
        {
            glm::mat4 projectionView = light->projection * light->view;

            for (size_t i = 0; i < light->containedMeshes.meshes.count; ++i)
            {
                auto &mesh = light->containedMeshes.meshes[i];
                assert(mesh.vertexArray != nullptr);
                {
                    // Shader setup.
                    shadowMapShader->SetUniform("PVMMatrix", projectionView * mesh.transform);
                    Renderer::PushPendingUniforms(*this->shadowMapShader);


                    // Draw.
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
            }
        }


        // Here is where we perform the blurring of the shadow map. This renders a fullscreen quad, so we don't want depth testing here.
        Renderer::DisableDepthWrites();
        Renderer::DisableDepthTest();

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderX);
            this->blurShaderX->SetUniform("Texture", this->shadowMapFramebuffer.colourBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &colourBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderY);
            this->blurShaderY->SetUniform("Texture", this->shadowMapFramebuffer.blurBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // The blurring is now done, but we want to restore depth testing.
        Renderer::EnableDepthTest();


        // At this point the shad map has been generated and we need to set the state as if we were doing normal lighting. Note how the blur pass has already
        // disabled depth writing, so no need to disable it again.
        Renderer::SetDepthFunction(RendererFunction_Equal);
        Renderer::EnableBlending();     // <-- Blending functions were set higher up.



        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {1, 2};
        Renderer::SetDrawBuffers(2, lightingBuffers);
        Renderer::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);


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


        // Restore state.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_LEqual);
    }

    void DefaultSceneRenderer::RenderOpaqueShadowDirectionalLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowDirectionalLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("LightPVMMatrix",  light->projection * light->view * mesh.transform);
                            shader->SetUniform("Colour",          light->colour);
                            shader->SetUniform("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetUniform("ShadowMap",       this->shadowMapFramebuffer.colourBuffer);
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer::RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
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

    void DefaultSceneRenderer::RenderOpaquePointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.pointLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);

                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("Colour",          light->colour);
                            shader->SetUniform("Position",        glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetUniform("Radius",          light->radius);
                            shader->SetUniform("Falloff",         light->falloff);
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer::RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer::SetCurrentFramebuffer(this->pointShadowMapFramebuffer.framebuffer);
        Renderer::SetViewport(0, 0, this->pointShadowMapFramebuffer.width, this->pointShadowMapFramebuffer.height);


        auto light = visibleObjects.shadowPointLights.buffer[lightIndex]->value;
        assert(light != nullptr);
        {
            this->RenderPointShapowMapFace(*light, light->positiveXView, 0, light->containedMeshesPositiveX.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeXView, 1, light->containedMeshesNegativeX.meshes);
            this->RenderPointShapowMapFace(*light, light->positiveYView, 2, light->containedMeshesPositiveY.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeYView, 3, light->containedMeshesNegativeY.meshes);
            this->RenderPointShapowMapFace(*light, light->positiveZView, 4, light->containedMeshesPositiveZ.meshes);
            this->RenderPointShapowMapFace(*light, light->negativeZView, 5, light->containedMeshesNegativeZ.meshes);
        }


        // At this point the shadow map is done and we can now set the state appropriately and render.
        Renderer::DisableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_Equal);
        Renderer::EnableBlending();     // <-- Blending functions were set higher up.




        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {1, 2};
        Renderer::SetDrawBuffers(2, lightingBuffers);
        Renderer::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);


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


        // Restore state.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_LEqual);
    }

    void DefaultSceneRenderer::RenderOpaqueShadowPointLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowPointLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("ModelMatrix",     mesh.transform);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("Colour",          light->colour);
                            shader->SetUniform("Position",        glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetUniform("Radius",          light->radius);
                            shader->SetUniform("Falloff",         light->falloff);
                            shader->SetUniform("ShadowMap",       this->pointShadowMapFramebuffer.colourBuffer);
                            shader->SetUniform("LightPositionWS", light->position);
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }

    void DefaultSceneRenderer::RenderPointShapowMapFace(const DefaultSceneRendererShadowPointLight &light, const glm::mat4 &faceViewMatrix, int faceIndex, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
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
            Renderer::SetCurrentShader(this->blurShaderX11x11);
            this->blurShaderX11x11->SetUniform("Texture", this->pointShadowMapFramebuffer.blurBuffer0);
            Renderer::PushPendingUniforms(*this->blurShaderX11x11);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y
        {
            Renderer::SetDrawBuffers(1, &faceIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderY11x11);
            this->blurShaderY11x11->SetUniform("Texture", this->pointShadowMapFramebuffer.blurBuffer1);
            Renderer::PushPendingUniforms(*this->blurShaderY11x11);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
    }



    void DefaultSceneRenderer::RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
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

    void DefaultSceneRenderer::RenderOpaqueSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.spotLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                            shader->SetUniform("NormalMatrix",    normalMatrix);
                            shader->SetUniform("Colour",          light->colour);
                            shader->SetUniform("Position",        glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetUniform("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetUniform("Length",          light->length);
                            shader->SetUniform("Falloff",         light->falloff);
                            shader->SetUniform("CosAngleInner",   glm::cos(glm::radians(light->innerAngle)));
                            shader->SetUniform("CosAngleOuter",   glm::cos(glm::radians(light->outerAngle)));
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer::RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, DefaultSceneRendererFramebuffer* mainFramebuffer)
    {
        // We first need to build the shadow map.
        Renderer::SetCurrentFramebuffer(this->shadowMapFramebuffer.framebuffer);
        Renderer::SetCurrentShader(this->shadowMapShader);
        Renderer::SetViewport(0, 0, this->shadowMapFramebuffer.width, this->shadowMapFramebuffer.height);

        Renderer::SetClearColour(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::Clear(BufferType_Colour | BufferType_Depth);

        int colourBufferIndex = 0;
        int blurBufferIndex   = 1;
        Renderer::SetDrawBuffers(1, &colourBufferIndex);



        auto light = visibleObjects.shadowSpotLights.buffer[lightIndex]->value;
        assert(light != nullptr);
        {
            glm::mat4 projectionView = light->projection * light->view;

            for (size_t i = 0; i < light->containedMeshes.meshes.count; ++i)
            {
                auto &mesh = light->containedMeshes.meshes[i];
                assert(mesh.vertexArray != nullptr);
                {
                    // Shader setup.
                    shadowMapShader->SetUniform("PVMMatrix", projectionView * mesh.transform);
                    Renderer::PushPendingUniforms(*this->shadowMapShader);


                    // Draw.
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
            }
        }


        // Here is where we perform the blurring of the shadow map. This renders a fullscreen quad, so we don't want depth testing here.
        Renderer::DisableDepthWrites();
        Renderer::DisableDepthTest();

        // Blur X.
        {
            Renderer::SetDrawBuffers(1, &blurBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderX11x11);
            this->blurShaderX11x11->SetUniform("Texture", this->shadowMapFramebuffer.colourBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderX11x11);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &colourBufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderY11x11);
            this->blurShaderY11x11->SetUniform("Texture", this->shadowMapFramebuffer.blurBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderY11x11);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // The blurring is now done, but we want to restore depth testing.
        Renderer::EnableDepthTest();


        // At this point the shad map has been generated and we need to set the state as if we were doing normal lighting. Note how the blur pass has already
        // disabled depth writing, so no need to disable it again.
        Renderer::SetDepthFunction(RendererFunction_Equal);
        Renderer::EnableBlending();     // <-- Blending functions were set higher up.



        // With the shadow map done, we now need to go back to the main framebuffer.
        Renderer::SetCurrentFramebuffer(mainFramebuffer->framebuffer);

        int lightingBuffers[] = {1, 2};
        Renderer::SetDrawBuffers(2, lightingBuffers);
        Renderer::SetViewport(0, 0, mainFramebuffer->width, mainFramebuffer->height);


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


        // Restore state.
        Renderer::DisableBlending();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_LEqual);
    }

    void DefaultSceneRenderer::RenderOpaqueShadowSpotLightingPass(size_t lightIndex, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects, const GTCore::Vector<DefaultSceneRendererMesh> &meshes)
    {
        auto light = visibleObjects.shadowSpotLights.buffer[lightIndex]->value;
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


                            Renderer::SetCurrentShader(shader);
                            shader->SetUniformsFromMaterial(*material);
                            shader->SetUniform("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                            shader->SetUniform("ViewModelMatrix",      viewModelMatrix);
                            shader->SetUniform("NormalMatrix",         normalMatrix);
                            shader->SetUniform("LightPVMMatrix",       light->projection * light->view * mesh.transform);
                            shader->SetUniform("Colour",               light->colour);
                            shader->SetUniform("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                            shader->SetUniform("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                            shader->SetUniform("Length",               light->length);
                            shader->SetUniform("Falloff",              light->falloff);
                            shader->SetUniform("CosAngleInner",        glm::cos(glm::radians(light->innerAngle)));
                            shader->SetUniform("CosAngleOuter",        glm::cos(glm::radians(light->outerAngle)));
                            shader->SetUniform("ShadowMap",            this->shadowMapFramebuffer.colourBuffer);
                            Renderer::PushPendingUniforms(*shader);
                        }


                        // Draw.
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                        {
                            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                        }
                        if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
                    }
                }
            }
        }
    }



    void DefaultSceneRenderer::RenderBlendedTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        // We loop over every refractive object and draw them one-by-one. For each object, we draw the lighting constribution to the lighting buffers and then
        // do the material pass.

        // We need to render these back to front. We'll just build a new list. Shouldn't be too many refractive objects on screen at a time.
        struct SortedMesh
        {
            float distanceToCamera;
            const DefaultSceneRendererMesh* mesh;

            SortedMesh(float distanceToCameraIn, const DefaultSceneRendererMesh* meshIn)
                : distanceToCamera(distanceToCameraIn), mesh(meshIn)
            {
            }

            bool operator<(const SortedMesh &other) const
            {
                return this->distanceToCamera > other.distanceToCamera;     // <-- Intentionally opposite.
            }
            bool operator>(const SortedMesh &other) const
            {
                return this->distanceToCamera < other.distanceToCamera;     // <-- Intentionally opposite.
            }

            bool operator==(const SortedMesh &other) const
            {
                return this->distanceToCamera == other.distanceToCamera;
            }
            bool operator!=(const SortedMesh &other) const
            {
                return this->distanceToCamera != other.distanceToCamera;
            }
        };

        GTCore::SortedVector<SortedMesh> sortedMeshes;
        for (size_t iMesh = 0; iMesh < visibleObjects.blendedTransparentObjects.count; ++iMesh)
        {
            auto &mesh = visibleObjects.blendedTransparentObjects[iMesh];
            {
                float distanceToCamera = glm::distance(glm::inverse(visibleObjects.viewMatrix)[3], mesh.transform[3]);
                sortedMeshes.Insert(SortedMesh(distanceToCamera, &mesh));
            }
        }


        for (size_t iMesh = 0; iMesh < sortedMeshes.count; ++iMesh)
        {
            auto &mesh = *sortedMeshes[iMesh].mesh;
            {
                // First step is to draw the lighting.
                this->RenderMeshLighting(mesh, visibleObjects);


                int colourBuffer[] = {0};
                Renderer::SetDrawBuffers(1, colourBuffer);

                // Now we do the material pass. We need to enable blending and set the equation and factors.
                Renderer::EnableBlending();
                Renderer::SetBlendEquation(mesh.material->GetBlendEquation());
                Renderer::SetBlendFunction(mesh.material->GetBlendSourceFactor(), mesh.material->GetBlendDestinationFactor());

                if (DoesBlendFunctionUseConstantColour(mesh.material->GetBlendSourceFactor()) || DoesBlendFunctionUseConstantColour(mesh.material->GetBlendDestinationFactor()))
                {
                    Renderer::SetBlendColour(mesh.material->GetBlendColour());
                }


                // Shader Setup.
                auto shader = this->GetMaterialMaterialShader(*mesh.material);
                assert(shader != nullptr);
                {
                    glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                    glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                    Renderer::SetCurrentShader(shader);
                    shader->SetUniformsFromMaterial(*mesh.material);
                    shader->SetUniform("ViewModelMatrix",   viewModelMatrix);
                    shader->SetUniform("NormalMatrix",      normalMatrix);
                    shader->SetUniform("PVMMatrix",         visibleObjects.projectionViewMatrix * mesh.transform);
                    shader->SetUniform("DiffuseLighting",   framebuffer->lightingBuffer0);
                    shader->SetUniform("SpecularLighting",  framebuffer->lightingBuffer1);
                    Renderer::PushPendingUniforms(*shader);
                }

                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                // If we're drawing a highlight, we'll need to draw a solid colour transparent mesh over the top using alpha blending.
                if ((mesh.flags & SceneRendererMesh::DrawHighlight))
                {
                    Renderer::SetBlendColour(1.0f, 0.66f, 0.33f, 1.0f);
                    Renderer::SetBlendFunction(BlendFunc_Zero, BlendFunc_ConstantColour);


                    // Shader.
                    Renderer::SetCurrentShader(this->highlightShader);
                    this->highlightShader->SetUniform("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
                    Renderer::PushPendingUniforms(*this->highlightShader);


                    // Draw.
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }

                Renderer::DisableBlending();
            }
        }
    }

    void DefaultSceneRenderer::RenderRefractiveTransparentPass(DefaultSceneRendererFramebuffer* framebuffer, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        // We need to copy the content from the opaque buffer over the to buffer that will output the results of the transparent pass.
        int colourBuffer[] = {3};
        Renderer::SetDrawBuffers(1, colourBuffer);

        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();


        // The opaque buffer must have a Nearest/Nearest filter.
        Renderer::SetTexture2DFilter(*framebuffer->opaqueColourBuffer, TextureFilter_Nearest, TextureFilter_Nearest);


        // Shader.
        Renderer::SetCurrentShader(this->finalCompositionShaderLDR);
        this->finalCompositionShaderLDR->SetUniform("ColourBuffer", framebuffer->opaqueColourBuffer);
        Renderer::PushPendingUniforms(*this->finalCompositionShaderLDR);


        // Draw.
        Renderer::Draw(*this->fullscreenTriangleVA);


        // Depth testing needs to be re-enabled now.
        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
        Renderer::SetDepthFunction(RendererFunction_LEqual);



        // We loop over every refractive object and draw them one-by-one. For each object, we draw the lighting constribution to the lighting buffers and then
        // do the material pass.

        // We need to render these back to front. We'll just build a new list. Shouldn't be too many refractive objects on screen at a time.
        struct SortedMesh
        {
            float distanceToCamera;
            const DefaultSceneRendererMesh* mesh;

            SortedMesh(float distanceToCameraIn, const DefaultSceneRendererMesh* meshIn)
                : distanceToCamera(distanceToCameraIn), mesh(meshIn)
            {
            }

            bool operator<(const SortedMesh &other) const
            {
                return this->distanceToCamera > other.distanceToCamera;     // <-- Intentionally opposite.
            }
            bool operator>(const SortedMesh &other) const
            {
                return this->distanceToCamera < other.distanceToCamera;     // <-- Intentionally opposite.
            }

            bool operator==(const SortedMesh &other) const
            {
                return this->distanceToCamera == other.distanceToCamera;
            }
            bool operator!=(const SortedMesh &other) const
            {
                return this->distanceToCamera != other.distanceToCamera;
            }
        };

        GTCore::SortedVector<SortedMesh> sortedMeshes;
        for (size_t iMesh = 0; iMesh < visibleObjects.refractiveTransparentObjects.count; ++iMesh)
        {
            auto &mesh = visibleObjects.refractiveTransparentObjects[iMesh];
            {
                float distanceToCamera = glm::distance(glm::inverse(visibleObjects.viewMatrix)[3], mesh.transform[3]);
                sortedMeshes.Insert(SortedMesh(distanceToCamera, &mesh));
            }
        }

        for (size_t iMesh = 0; iMesh < sortedMeshes.count; ++iMesh)
        {
            auto &mesh = *sortedMeshes[iMesh].mesh;
            {
                // First step is to draw the lighting.
                this->RenderMeshLighting(mesh, visibleObjects);


                // Now we do the actual material pass.
                int colourBuffer[] = {3};
                Renderer::SetDrawBuffers(1, colourBuffer);



                // Shader Setup.
                auto shader = this->GetMaterialMaterialShader(*mesh.material);
                assert(shader != nullptr);
                {
                    glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
                    glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


                    Renderer::SetCurrentShader(shader);
                    shader->SetUniformsFromMaterial(*mesh.material);
                    shader->SetUniform("ViewModelMatrix",   viewModelMatrix);
                    shader->SetUniform("NormalMatrix",      normalMatrix);
                    shader->SetUniform("PVMMatrix",         visibleObjects.projectionViewMatrix * mesh.transform);
                    shader->SetUniform("DiffuseLighting",   framebuffer->lightingBuffer0);
                    shader->SetUniform("SpecularLighting",  framebuffer->lightingBuffer1);
                    shader->SetUniform("BackgroundTexture", framebuffer->opaqueColourBuffer);
                    Renderer::PushPendingUniforms(*shader);
                }

                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                // If we're drawing a highlight, we'll need to draw a solid colour transparent mesh over the top using alpha blending.
                if ((mesh.flags & SceneRendererMesh::DrawHighlight))
                {
                    Renderer::EnableBlending();
                    Renderer::SetBlendColour(1.0f, 0.66f, 0.33f, 1.0f);
                    Renderer::SetBlendFunction(BlendFunc_Zero, BlendFunc_ConstantColour);


                    // Shader.
                    Renderer::SetCurrentShader(this->highlightShader);
                    this->highlightShader->SetUniform("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
                    Renderer::PushPendingUniforms(*this->highlightShader);


                    // Draw.
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);


                    Renderer::DisableBlending();
                }


                // At this point the mesh will have been drawn. We now want to copy the destination buffer over to to opaque buffer so that the next objects
                // have an up-to-date background texture.
                if (iMesh < sortedMeshes.count - 1)
                {
                    int opaqueColourBuffer[] = {0};
                    Renderer::SetDrawBuffers(1, opaqueColourBuffer);

                    Renderer::SetCurrentShader(this->finalCompositionShaderLDR);
                    this->finalCompositionShaderLDR->SetUniform("ColourBuffer", framebuffer->finalColourBufferHDR);
                    Renderer::PushPendingUniforms(*this->finalCompositionShaderLDR);


                    Renderer::DisableDepthTest();
                    Renderer::DisableDepthWrites();
                    {
                        Renderer::Draw(*this->fullscreenTriangleVA);
                    }
                    Renderer::EnableDepthTest();
                    Renderer::EnableDepthWrites();
                }
            }
        }
    }

    void DefaultSceneRenderer::RenderMeshLighting(const DefaultSceneRendererMesh &mesh, const DefaultSceneRenderer_VisibilityProcessor &visibleObjects)
    {
        auto lights = mesh.touchingLights;
        if (lights != nullptr)
        {
            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


            // Keeps track of whether or not the first light has been done.
            bool doneFirstLight = false;

            int lightingBuffers[] = {1, 2};
            Renderer::SetDrawBuffers(2, lightingBuffers);

            // We want to explicitly disable blending here. It will be reenabled with additive blending after we've done the first light.
            Renderer::SetBlendEquation(BlendEquation_Add);
            Renderer::SetBlendFunction(BlendFunc_One, BlendFunc_One);


            auto ambientLightStartIndex           = lights->GetAmbientLightStartIndex();
            auto directionalLightStartIndex       = lights->GetDirectionalLightStartIndex();
            auto pointLightStartIndex             = lights->GetPointLightStartIndex();
            auto spotLightStartIndex              = lights->GetSpotLightStartIndex();
            auto shadowDirectionalLightStartIndex = lights->GetShadowDirectionalLightStartIndex();
            auto shadowPointLightStartIndex       = lights->GetShadowPointLightStartIndex();
            auto shadowSpotLightStartIndex        = lights->GetSpotLightStartIndex();


            // Ambient Lights.
            auto ambientLightShader = this->GetMaterialAmbientLightShader(*mesh.material);
            for (size_t i = 0; i < lights->GetAmbientLightCount(); ++i)
            {
                auto light  = visibleObjects.ambientLights.buffer[lights->lightIDs[i + ambientLightStartIndex]]->value;
                auto shader = ambientLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                shader->SetUniform("NormalMatrix",    normalMatrix);
                shader->SetUniform("Colour",          light->colour);
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }


            // Directional Lights.
            auto directionalLightShader = this->GetMaterialDirectionalLightShader(*mesh.material);
            for (size_t i = 0; i < lights->GetDirectionalLightCount(); ++i)
            {
                auto light  = visibleObjects.directionalLights.buffer[lights->lightIDs[i + directionalLightStartIndex]]->value;
                auto shader = directionalLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                shader->SetUniform("NormalMatrix",    normalMatrix);
                shader->SetUniform("Colour",          light->colour);
                shader->SetUniform("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }

            for (size_t i = 0; i < lights->GetShadowDirectionalLightCount(); ++i)
            {
                auto light  = visibleObjects.shadowDirectionalLights.buffer[lights->lightIDs[i + shadowDirectionalLightStartIndex]]->value;
                auto shader = directionalLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",       visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix", viewModelMatrix);
                shader->SetUniform("NormalMatrix",    normalMatrix);
                shader->SetUniform("Colour",          light->colour);
                shader->SetUniform("Direction",       glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();



                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }


            // Point Lights.
            auto pointLightShader = this->GetMaterialPointLightShader(*mesh.material);
            for (size_t i = 0; i < lights->GetPointLightCount(); ++i)
            {
                auto light  = visibleObjects.pointLights.buffer[lights->lightIDs[i + pointLightStartIndex]]->value;
                auto shader = pointLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix",      viewModelMatrix);
                shader->SetUniform("NormalMatrix",         normalMatrix);
                shader->SetUniform("Colour",               light->colour);
                shader->SetUniform("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader->SetUniform("Radius",               light->radius);
                shader->SetUniform("Falloff",              light->falloff);
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();



                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }

            for (size_t i = 0; i < lights->GetShadowPointLightCount(); ++i)
            {
                auto light  = visibleObjects.shadowPointLights.buffer[lights->lightIDs[i + shadowPointLightStartIndex]]->value;
                auto shader = pointLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix",      viewModelMatrix);
                shader->SetUniform("NormalMatrix",         normalMatrix);
                shader->SetUniform("Colour",               light->colour);
                shader->SetUniform("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader->SetUniform("Radius",               light->radius);
                shader->SetUniform("Falloff",              light->falloff);
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }


            // Spot Lights.
            auto spotLightShader = this->GetMaterialSpotLightShader(*mesh.material);
            for (size_t i = 0; i < lights->GetSpotLightCount(); ++i)
            {
                auto light  = visibleObjects.spotLights.buffer[lights->lightIDs[i + spotLightStartIndex]]->value;
                auto shader = spotLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix",      viewModelMatrix);
                shader->SetUniform("NormalMatrix",         normalMatrix);
                shader->SetUniform("Colour",               light->colour);
                shader->SetUniform("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader->SetUniform("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                shader->SetUniform("Length",               light->length);
                shader->SetUniform("Falloff",              light->falloff);
                shader->SetUniform("CosAngleInner",        glm::cos(glm::radians(light->innerAngle)));
                shader->SetUniform("CosAngleOuter",        glm::cos(glm::radians(light->outerAngle)));
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }

            for (size_t i = 0; i < lights->GetShadowSpotLightCount(); ++i)
            {
                auto light  = visibleObjects.shadowSpotLights.buffer[lights->lightIDs[i + shadowSpotLightStartIndex]]->value;
                auto shader = spotLightShader;


                // Shader.
                Renderer::SetCurrentShader(shader);
                shader->SetUniformsFromMaterial(*mesh.material);
                shader->SetUniform("PVMMatrix",            visibleObjects.projectionViewMatrix * mesh.transform);
                shader->SetUniform("ViewModelMatrix",      viewModelMatrix);
                shader->SetUniform("NormalMatrix",         normalMatrix);
                shader->SetUniform("Colour",               light->colour);
                shader->SetUniform("Position",             glm::vec3(visibleObjects.viewMatrix * glm::vec4(light->position, 1.0f)));
                shader->SetUniform("Direction",            glm::normalize(glm::mat3(visibleObjects.viewMatrix) * light->GetForwardVector()));
                shader->SetUniform("Length",               light->length);
                shader->SetUniform("Falloff",              light->falloff);
                shader->SetUniform("CosAngleInner",        glm::cos(glm::radians(light->innerAngle)));
                shader->SetUniform("CosAngleOuter",        glm::cos(glm::radians(light->outerAngle)));
                Renderer::PushPendingUniforms(*shader);


                // Draw.
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
                {
                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                }
                if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();


                if (!doneFirstLight)
                {
                    Renderer::EnableBlending();
                    doneFirstLight = true;
                }
            }


            // Blending might need to be disabled.
            if (doneFirstLight)
            {
                Renderer::DisableBlending();
            }
        }
    }


    void DefaultSceneRenderer::RenderFinalComposition(DefaultSceneRendererFramebuffer* framebuffer, Texture2D* sourceColourBuffer)
    {
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();
        Renderer::DisableStencilTest();


        if (this->IsHDREnabled())
        {
            // HDR requires luminosity. We downsample the colour buffer to 1x1 to get this. Mipmap generation will do it for us.
            Renderer::SetTexture2DFilter(*sourceColourBuffer, TextureFilter_NearestNearest, TextureFilter_Nearest);
            Renderer::GenerateTexture2DMipmaps(*sourceColourBuffer);


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
                this->finalCompositionShaderHDR->SetUniform("ColourBuffer", sourceColourBuffer);
                this->finalCompositionShaderHDR->SetUniform("BloomBuffer",  framebuffer->bloomBuffer);
                this->finalCompositionShaderHDR->SetUniform("Exposure",     this->hdrExposure);
                this->finalCompositionShaderHDR->SetUniform("BloomFactor",  this->bloomFactor);
                Renderer::PushPendingUniforms(*this->finalCompositionShaderHDR);
            }
            else
            {
                Renderer::SetCurrentShader(this->finalCompositionShaderHDRNoBloom);
                this->finalCompositionShaderHDRNoBloom->SetUniform("ColourBuffer", sourceColourBuffer);
                this->finalCompositionShaderHDRNoBloom->SetUniform("Exposure",     this->hdrExposure);
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
            Renderer::SetCurrentShader(this->blurShaderX);
            this->blurShaderX->SetUniform("Texture", framebuffer->bloomBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderX);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }

        // Blur Y.
        {
            Renderer::SetDrawBuffers(1, &bufferIndex);

            // Shader.
            Renderer::SetCurrentShader(this->blurShaderY);
            this->blurShaderY->SetUniform("Texture", framebuffer->bloomBlurBuffer);
            Renderer::PushPendingUniforms(*this->blurShaderY);

            // Draw.
            Renderer::Draw(*this->fullscreenTriangleVA);
        }
    }



    ///////////////////////
    // Materials.

    DefaultSceneRendererMaterialShaders* DefaultSceneRenderer::GetMaterialShaders(Material &material)
    {
        // A single set of shaders is created for each definition. We map the shaders to the definition, with the definition acting as the key.
        auto iMaterialShaders = this->materialShaders.Find(&material.GetDefinition());
        if (iMaterialShaders == nullptr)
        {
            // The shaders structure has not yet been created, so it needs to be created now.
            auto shaders = new DefaultSceneRendererMaterialShaders;
            this->materialShaders.Add(&material.GetDefinition(), shaders);

            return shaders;
        }
        else
        {
            // The shaders structure has already been created, so we just return that.
            return iMaterialShaders->value;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialAmbientLightShader(Material &material)
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

                shaders->ambientLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->ambientLightShader;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialDirectionalLightShader(Material &material)
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

                shaders->directionalLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->directionalLightShader;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialPointLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->pointLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointLightVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_PointLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->pointLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->pointLightShader;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialSpotLightShader(Material &material)
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

                shaders->spotLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->spotLightShader;
        }
    }


    Shader* DefaultSceneRenderer::GetMaterialShadowDirectionalLightShader(Material &material)
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

                shaders->shadowDirectionalLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowDirectionalLightShader;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialShadowPointLightShader(Material &material)
    {
        auto shaders = this->GetMaterialShaders(material);
        assert(shaders != nullptr);
        {
            if (shaders->shadowPointLightShader == nullptr)
            {
                /// The shader has not yet been created, so it needs to be.
                GTCore::Strings::List<char> vertexSource;
                vertexSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowPointLightVS"));

                GTCore::Strings::List<char> fragmentSource;
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_LightingFS"));
                fragmentSource.Append(ShaderLibrary::GetShaderString("DefaultSceneRenderer_ShadowPointLight"));
                fragmentSource.Append(ShaderLibrary::GetShaderString(material.GetNormalShaderID()));

                shaders->shadowPointLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowPointLightShader;
        }
    }

    Shader* DefaultSceneRenderer::GetMaterialShadowSpotLightShader(Material &material)
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

                shaders->shadowSpotLightShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->shadowSpotLightShader;
        }
    }



    Shader* DefaultSceneRenderer::GetMaterialMaterialShader(Material &material)
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

                shaders->materialShader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str(), nullptr);
            }

            return shaders->materialShader;
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
