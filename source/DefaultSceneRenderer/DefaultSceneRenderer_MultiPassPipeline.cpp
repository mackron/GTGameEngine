// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer.hpp>
#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_MultiPassPipeline.hpp>
#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_VisibilityProcessor.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_MultiPassPipeline::DefaultSceneRenderer_MultiPassPipeline(DefaultSceneRenderer &rendererIn, DefaultSceneRendererFramebuffer &viewportFramebufferIn, const DefaultSceneRenderer_VisibilityProcessor &visibleObjectsIn, bool splitShadowLightsIn)
        : renderer(rendererIn), viewportFramebuffer(viewportFramebufferIn), visibleObjects(visibleObjectsIn), splitShadowLights(splitShadowLightsIn),
          opaqueObjects(nullptr), blendedTransparentObjects(nullptr), refractiveTransparentObjects(nullptr),
          hasBackgroundBeenCleared(false),
          hasGeneratedMainLightGroup(false), mainLightGroup()
    {
    }

    DefaultSceneRenderer_MultiPassPipeline::~DefaultSceneRenderer_MultiPassPipeline()
    {
    }


    void DefaultSceneRenderer_MultiPassPipeline::Execute()
    {
        // Clear.
        int lightingBuffers[] = {1, 2};
        Renderer::SetDrawBuffers(2, lightingBuffers);

        Renderer::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::SetClearDepth(1.0f);
        Renderer::SetClearStencil(0);
        Renderer::Clear(BufferType_Colour | BufferType_Depth | BufferType_Stencil);



        // There is two groups of meshes. The regular group and the "draw last" group.
        this->BindObjects(&this->visibleObjects.opaqueObjects, &this->visibleObjects.blendedTransparentObjects, &this->visibleObjects.refractiveTransparentObjects);
        {
            this->OpaquePass();
            this->TransparentPass();
        }

        this->BindObjects(&this->visibleObjects.opaqueObjectsLast, &this->visibleObjects.blendedTransparentObjectsLast, nullptr);
        {
            this->OpaquePass();
            this->TransparentPass();
        }
    }


    void DefaultSceneRenderer_MultiPassPipeline::BindObjects(
            const GTCore::Map<const MaterialDefinition*, GTCore::Vector<DefaultSceneRendererMesh>*>* opaqueObjectsIn,
            const GTCore::Vector<DefaultSceneRendererMesh>* blendedTransparentObjectsIn,
            const GTCore::Vector<DefaultSceneRendererMesh>* refractiveTransparentObjectsIn)
    {
        this->opaqueObjects                = opaqueObjectsIn;
        this->blendedTransparentObjects    = blendedTransparentObjectsIn;
        this->refractiveTransparentObjects = refractiveTransparentObjectsIn;
    }


    void DefaultSceneRenderer_MultiPassPipeline::OpaquePass()
    {
        if (this->opaqueObjects != nullptr && this->opaqueObjects->count > 0)
        {
            // We'll do a depth pre-pass because since the scene is complex enough to use the multi-pass pipeline, it's probably complex
            // enough to do a depth-only pass. Might still need to experiment. Perhaps combine it with the ambient pass?
            this->DepthPass();

            // Lighting.
            this->OpaqueLightingPass();

            // Material.
            this->OpaqueMaterialPass();
        }
    }

    void DefaultSceneRenderer_MultiPassPipeline::TransparentPass()
    {
        if (this->blendedTransparentObjects != nullptr && this->blendedTransparentObjects->count > 0)
        {
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


            // Setup initial rendering state. We assert that the main framebuffer is current and the correct draw buffer is in place.
            Renderer::EnableBlending();

            GTCore::Vector<DefaultSceneRenderer_LightGroup> lightGroups;

            for (size_t iMesh = 0; iMesh < sortedMeshes.count; ++iMesh)
            {
                auto &mesh = *sortedMeshes[iMesh].mesh;
                {
                    lightGroups.Clear();
                    this->SubdivideLightGroup(mesh.touchingLights, lightGroups, ConvertShadowLights);


                    // Blending setup.
                    Renderer::SetBlendEquation(mesh.material->GetBlendEquation());
                    Renderer::SetBlendFunction(mesh.material->GetBlendSourceFactor(), mesh.material->GetBlendDestinationFactor());

                    if (DoesBlendFunctionUseConstantColour(mesh.material->GetBlendSourceFactor()) || DoesBlendFunctionUseConstantColour(mesh.material->GetBlendDestinationFactor()))
                    {
                        Renderer::SetBlendColour(mesh.material->GetBlendColour());
                    }


                    // Draw.
                    this->RenderMesh(mesh, lightGroups[0], DefaultSceneRenderer_MaterialShaderID::IncludeMaterialPass | DefaultSceneRenderer_MaterialShaderID::NoNormalMapping); // <-- take away NoNormalMapping here and have it use the material in determining whether or not to use normal mapping.
                    
                    // Highlight.
                    if ((mesh.flags & SceneRendererMesh::DrawHighlight))
                    {
                        this->RenderMeshHighlight(mesh);
                    }
                }
            }

            Renderer::DisableBlending();
        }




        if (this->refractiveTransparentObjects != nullptr && this->refractiveTransparentObjects->count > 0)
        {

        }
    }


    void DefaultSceneRenderer_MultiPassPipeline::DepthPass()
    {
        assert(this->opaqueObjects != nullptr);


        Renderer::DisableColourWrites();
        {
            auto shader = this->renderer.GetDepthPassShader();
            assert(shader != nullptr);
            {
                for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
                {
                    auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
                    assert(meshList != nullptr);
                    {
                        for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                        {
                            auto &mesh = meshList->buffer[iMesh];

                            if (!(mesh.flags & SceneRendererMesh::NoDepthWrite))
                            {
                                // Shader setup.
                                Renderer::SetCurrentShader(shader);
                                shader->SetUniform("PVMMatrix", this->visibleObjects.projectionViewMatrix * mesh.transform);
                                Renderer::PushPendingUniforms(*shader);


                                // Draw.
                                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer::DisableDepthTest();
                                {
                                    Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
                                }
                                if ((mesh.flags & SceneRendererMesh::NoDepthTest))  Renderer::EnableDepthTest();
                            }
                        }
                    }
                }
            }
        }
        Renderer::EnableColourWrites();
    }

    void DefaultSceneRenderer_MultiPassPipeline::OpaqueLightingPass()
    {
        if (this->visibleObjects.lightManager.GetTotalLightCount() > 0)
        {
            // If we're not splitting the shadow-casting lights (i.e. grouping them all together with non-shadow-casting lights) we will
            // render their shadow-maps immediately.
            if (!this->splitShadowLights)
            {
                bool framebufferChanged = false;

                // Shadow-casting directional lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowDirectionalLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.directionalLights.Find(this->visibleObjects.lightManager.shadowDirectionalLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            this->renderer.RenderDirectionalLightShadowMap(*light, i);
                            framebufferChanged = true;
                        }
                    }
                }

                // Shadow-casting point lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowPointLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.pointLights.Find(this->visibleObjects.lightManager.shadowPointLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            this->renderer.RenderPointLightShadowMap(*light, i);
                            framebufferChanged = true;
                        }
                    }
                }

                // Shadow-casting spot lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowSpotLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.spotLights.Find(this->visibleObjects.lightManager.shadowSpotLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            this->renderer.RenderSpotLightShadowMap(*light, i);
                            framebufferChanged = true;
                        }
                    }
                }


                // The framebuffer needs to be changed back if required.
                if (framebufferChanged)
                {
                    this->SwitchToLightingFramebuffer();
                }
            }
            else
            {
                // The shadow-casting lights are split (can not be grouped with non-shadow-casting lights) which means we need to use
                // a slightly different, less efficient technique.
                //
                // Here we are just doing the shadow-casting lighting. We iterate over each light, and for each of those first render
                // the shadow map and then do the actual lighting for each contained mesh.

                // Directional lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowDirectionalLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.directionalLights.Find(this->visibleObjects.lightManager.shadowDirectionalLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            auto shadowMap = this->renderer.RenderDirectionalLightShadowMap(*light, i);
                            assert(shadowMap != nullptr);
                            {
                                // Rendering the shadow map will have changed some state, so it needs to be re-enabled here.
                                this->EnableLightingState();

                                // Now we just iterate over and draw every mesh. We just use a light group with a single light in it.
                                DefaultSceneRenderer_LightGroup lightGroup;
                                lightGroup.AddShadowDirectionalLight(static_cast<uint32_t>(i));

                                for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
                                {
                                    auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
                                    assert(meshList != nullptr);
                                    {
                                        for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                                        {
                                            this->RenderMesh(meshList->buffer[iMesh], lightGroup, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Point lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowPointLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.pointLights.Find(this->visibleObjects.lightManager.shadowPointLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            auto shadowMap = this->renderer.RenderPointLightShadowMap(*light, i);
                            assert(shadowMap != nullptr);
                            {
                                // Rendering the shadow map will have changed some state, so it needs to be re-enabled here.
                                this->EnableLightingState();

                                // Now we just iterate over and draw every mesh. We just use a light group with a single light in it.
                                DefaultSceneRenderer_LightGroup lightGroup;
                                lightGroup.AddShadowPointLight(static_cast<uint32_t>(i));

                                for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
                                {
                                    auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
                                    assert(meshList != nullptr);
                                    {
                                        for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                                        {
                                            this->RenderMesh(meshList->buffer[iMesh], lightGroup, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Spot lights.
                for (size_t i = 0; i < this->visibleObjects.lightManager.shadowSpotLights.count; ++i)
                {
                    auto iLight = this->visibleObjects.lightManager.spotLights.Find(this->visibleObjects.lightManager.shadowSpotLights[i]);
                    assert(iLight != nullptr);
                    {
                        auto light = iLight->value;
                        assert(light != nullptr);
                        {
                            auto shadowMap = this->renderer.RenderSpotLightShadowMap(*light, i);
                            assert(shadowMap != nullptr);
                            {
                                // Rendering the shadow map will have changed some state, so it needs to be re-enabled here.
                                this->EnableLightingState();

                                // Now we just iterate over and draw every mesh. We just use a light group with a single light in it.
                                DefaultSceneRenderer_LightGroup lightGroup;
                                lightGroup.AddShadowSpotLight(static_cast<uint32_t>(i));

                                for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
                                {
                                    auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
                                    assert(meshList != nullptr);
                                    {
                                        for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                                        {
                                            this->RenderMesh(meshList->buffer[iMesh], lightGroup, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }




            // Blending.
            this->EnableLightingBlending();

            // Depth testing.
            this->EnableLightingDepthTesting();
        


            // Determine whether or not to include shadow-casting lights in the next pass.
            uint32_t lightSubdivisionOptions = 0;
            
            if (this->splitShadowLights)
            {
                lightSubdivisionOptions |= NoShadowLights;
            }



            // Now we just draw the remaining lights.
            GTCore::Vector<DefaultSceneRenderer_LightGroup> lightGroups;

            for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
            {
                auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
                assert(meshList != nullptr);
                {
                    for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                    {
                        auto &mesh = meshList->buffer[iMesh];

                        lightGroups.Clear();
                        this->SubdivideLightGroup(mesh.touchingLights, lightGroups, lightSubdivisionOptions);

                        for (size_t iLightGroup = 0; iLightGroup < lightGroups.count; ++iLightGroup)
                        {
                            this->RenderMesh(mesh, lightGroups[iLightGroup], 0);
                        }
                    }
                }
            }

            // We want to disable blending at the end of the lighting pass.
            Renderer::DisableBlending();
        }
    }

    void DefaultSceneRenderer_MultiPassPipeline::OpaqueMaterialPass()
    {
        // We can assert that the current framebuffer will be the main one. All we need to do is change the draw buffer.
        int outputBuffer[] = {0};
        Renderer::SetDrawBuffers(1, outputBuffer);

        // Clear the background, if applicable.
        if (!this->hasBackgroundBeenCleared && this->renderer.IsBackgroundColourClearingEnabled())
        {
            auto &colour = this->renderer.GetBackgroundClearColour();
            Renderer::SetClearColour(colour.x, colour.y, colour.z, 1.0f);
            Renderer::Clear(BufferType_Colour);

            this->hasBackgroundBeenCleared = true;
        }


        DefaultSceneRenderer_LightGroup emptyLightGroup;

        for (size_t iMeshList = 0; iMeshList < this->opaqueObjects->count; ++iMeshList)
        {
            auto meshList = this->opaqueObjects->buffer[iMeshList]->value;
            assert(meshList != nullptr);
            {
                for (size_t iMesh = 0; iMesh < meshList->count; ++iMesh)
                {
                    auto &mesh = meshList->buffer[iMesh];
                    {
                        this->RenderMesh(meshList->buffer[iMesh], emptyLightGroup, DefaultSceneRenderer_MaterialShaderID::IncludeMaterialPass | DefaultSceneRenderer_MaterialShaderID::GetLightingFromTextures);

                        // Highlight.
                        if ((mesh.flags & SceneRendererMesh::DrawHighlight))
                        {
                            Renderer::EnableBlending();
                            {
                                this->RenderMeshHighlight(mesh);
                            }
                            Renderer::DisableBlending();
                        }
                    }
                }
            }
        }
    }


    void DefaultSceneRenderer_MultiPassPipeline::SubdivideLightGroup(const DefaultSceneRenderer_LightGroup* sourceLightGroup, GTCore::Vector<DefaultSceneRenderer_LightGroup> &lightGroupsOut, uint32_t options)
    {
        const uint32_t maxComplexity               = 1000;     // <-- Increase this to generate bigger groups. Better GPU's can use bigger values.
        const uint32_t ambientComplexity           = 10;
        const uint32_t directionalComplexity       = 50;
        const uint32_t pointComplexity             = 100;
        const uint32_t spotComplexity              = 150;
        const uint32_t shadowDirectionalComplexity = 150;
        const uint32_t shadowPointComplexity       = 200;
        const uint32_t shadowSpotComplexity        = 250;

        uint32_t currentComplexity = 0;
        
        bool includeShadowLights = !(options & NoShadowLights);
        bool convertShadowLights =  (options & ConvertShadowLights) != 0;


        // If the source light group is null we will use the main light group.
        if (sourceLightGroup == nullptr)
        {
            sourceLightGroup = &this->GetMainLightGroup();
        }


        DefaultSceneRenderer_LightGroup currentLightGroup;

        // Ambient lights.
        if (!(options & NoAmbientLights))
        {
            for (size_t i = 0; i < sourceLightGroup->GetAmbientLightCount(); ++i)
            {
                if (currentComplexity + ambientComplexity > maxComplexity)
                {
                    lightGroupsOut.PushBack(currentLightGroup);
                    currentLightGroup.Clear();
                }

                currentLightGroup.AddAmbientLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetAmbientLightStartIndex()]);
                currentComplexity += ambientComplexity;
            }
        }


        // Directional lights.
        for (size_t i = 0; i < sourceLightGroup->GetDirectionalLightCount(); ++i)
        {
            if (currentComplexity + directionalComplexity > maxComplexity)
            {
                lightGroupsOut.PushBack(currentLightGroup);
                currentLightGroup.Clear();
            }

            currentLightGroup.AddDirectionalLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetDirectionalLightStartIndex()]);
            currentComplexity += directionalComplexity;
        }

        // Shadow Directional lights.
        if (includeShadowLights)
        {
            for (size_t i = 0; i < sourceLightGroup->GetShadowDirectionalLightCount(); ++i)
            {
                if (currentComplexity + shadowDirectionalComplexity > maxComplexity)
                {
                    lightGroupsOut.PushBack(currentLightGroup);
                    currentLightGroup.Clear();
                }

                if (!convertShadowLights)
                {
                    currentLightGroup.AddShadowDirectionalLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowDirectionalLightStartIndex()]);
                    currentComplexity += shadowDirectionalComplexity;
                }
                else
                {
                    currentLightGroup.AddDirectionalLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowDirectionalLightStartIndex()]);
                    currentComplexity += directionalComplexity;
                }
            }
        }


        // Point lights.
        for (size_t i = 0; i < sourceLightGroup->GetPointLightCount(); ++i)
        {
            if (currentComplexity + pointComplexity > maxComplexity)
            {
                lightGroupsOut.PushBack(currentLightGroup);
                currentLightGroup.Clear();
            }

            currentLightGroup.AddPointLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetPointLightStartIndex()]);
            currentComplexity += pointComplexity;
        }

        // Shadow Point lights.
        if (includeShadowLights)
        {
            for (size_t i = 0; i < sourceLightGroup->GetShadowPointLightCount(); ++i)
            {
                if (currentComplexity + shadowPointComplexity > maxComplexity)
                {
                    lightGroupsOut.PushBack(currentLightGroup);
                    currentLightGroup.Clear();
                }

                if (!convertShadowLights)
                {
                    currentLightGroup.AddShadowPointLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowPointLightStartIndex()]);
                    currentComplexity += shadowPointComplexity;
                }
                else
                {
                    currentLightGroup.AddPointLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowPointLightStartIndex()]);
                    currentComplexity += pointComplexity;
                }
            }
        }


        // Spot lights.
        for (size_t i = 0; i < sourceLightGroup->GetSpotLightCount(); ++i)
        {
            if (currentComplexity + spotComplexity > maxComplexity)
            {
                lightGroupsOut.PushBack(currentLightGroup);
                currentLightGroup.Clear();
            }

            currentLightGroup.AddSpotLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetSpotLightStartIndex()]);
            currentComplexity += spotComplexity;
        }

        // Shadow Spot lights.
        if (includeShadowLights)
        {
            for (size_t i = 0; i < sourceLightGroup->GetShadowSpotLightCount(); ++i)
            {
                if (currentComplexity + shadowSpotComplexity > maxComplexity)
                {
                    lightGroupsOut.PushBack(currentLightGroup);
                    currentLightGroup.Clear();
                }

                if (!convertShadowLights)
                {
                    currentLightGroup.AddShadowSpotLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowSpotLightStartIndex()]);
                    currentComplexity += shadowSpotComplexity;
                }
                else
                {
                    currentLightGroup.AddSpotLight(sourceLightGroup->lightIDs[i + sourceLightGroup->GetShadowSpotLightStartIndex()]);
                    currentComplexity += spotComplexity;
                }
            }
        }


        // We might have one last group to add.
        if (currentLightGroup.GetLightCount() > 0)
        {
            lightGroupsOut.PushBack(currentLightGroup);
        }


        // We want at least a single light group, even if it's empty.
        if (lightGroupsOut.count == 0)
        {
            lightGroupsOut.PushBack(DefaultSceneRenderer_LightGroup());
        }
    }



    ///////////////////////////////////////////
    // Rendering Helpers.

    void DefaultSceneRenderer_MultiPassPipeline::SwitchToLightingFramebuffer()
    {
        Renderer::SetCurrentFramebuffer(this->viewportFramebuffer.framebuffer);

        int lightingBuffers[] = {1, 2};
        Renderer::SetDrawBuffers(2, lightingBuffers);
        Renderer::SetViewport(0, 0, this->viewportFramebuffer.width, this->viewportFramebuffer.height);
    }

    void DefaultSceneRenderer_MultiPassPipeline::EnableLightingBlending()
    {
        Renderer::EnableBlending();
        Renderer::SetBlendEquation(BlendEquation_Add);
        Renderer::SetBlendFunction(BlendFunc_One, BlendFunc_One);
    }

    void DefaultSceneRenderer_MultiPassPipeline::EnableLightingDepthTesting()
    {
        Renderer::DisableDepthWrites();
    }

    void DefaultSceneRenderer_MultiPassPipeline::EnableLightingState()
    {
        this->SwitchToLightingFramebuffer();
        this->EnableLightingBlending();
        this->EnableLightingDepthTesting();
    }


    void DefaultSceneRenderer_MultiPassPipeline::RenderMesh(const DefaultSceneRendererMesh &mesh, const DefaultSceneRenderer_LightGroup &lightGroup, uint32_t shaderFlags)
    {
        auto shader = this->renderer.GetMaterialShader(*mesh.material, lightGroup.id, shaderFlags);
        if (shader != nullptr)
        {
            glm::mat4 viewModelMatrix = visibleObjects.viewMatrix * mesh.transform;
            glm::mat3 normalMatrix    = glm::inverse(glm::transpose(glm::mat3(viewModelMatrix)));


            // Shader setup.
            Renderer::SetCurrentShader(shader);
            this->renderer.SetMaterialShaderUniforms(*shader, *mesh.material, lightGroup, shaderFlags, this->visibleObjects);

            // For now, we will just set the shadow maps manually.
            if (lightGroup.GetShadowDirectionalLightCount() > 0)
            {
                shader->SetUniform(GTCore::String::CreateFormatted("ShadowDirectionalLightFS%d_ShadowMap", 0).c_str(), this->renderer.GetDirectionalShadowMapByIndex(0));
            }

            if (lightGroup.GetShadowPointLightCount() > 0)
            {
                shader->SetUniform(GTCore::String::CreateFormatted("ShadowPointLightFS%d_ShadowMap", 0).c_str(), this->renderer.GetPointShadowMapByIndex(0));
            }

            if (lightGroup.GetShadowSpotLightCount() > 0)
            {
                shader->SetUniform(GTCore::String::CreateFormatted("ShadowSpotLightFS%d_ShadowMap", 0).c_str(), this->renderer.GetSpotShadowMapByIndex(0));
            }

            shader->SetUniform("ModelMatrix",       mesh.transform);
            shader->SetUniform("ViewModelMatrix",   viewModelMatrix);
            shader->SetUniform("NormalMatrix",      normalMatrix);
            shader->SetUniform("PVMMatrix",         visibleObjects.projectionViewMatrix * mesh.transform);

            if ((shaderFlags & DefaultSceneRenderer_MaterialShaderID::GetLightingFromTextures))
            {
                shader->SetUniform("DiffuseLighting",  this->viewportFramebuffer.lightingBuffer0);
                shader->SetUniform("SpecularLighting", this->viewportFramebuffer.lightingBuffer1);
            }

            Renderer::PushPendingUniforms(*shader);



            // Draw.
            if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::DisableDepthTest();
            {
                Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
            }
            if ((mesh.flags & SceneRendererMesh::NoDepthTest)) Renderer::EnableDepthTest();
        }
    }

    void DefaultSceneRenderer_MultiPassPipeline::RenderMeshHighlight(const DefaultSceneRendererMesh &mesh)
    {
        Renderer::SetBlendColour(mesh.highlightColour.x, mesh.highlightColour.y, mesh.highlightColour.z, 1.0f);
        Renderer::SetBlendFunction(BlendFunc_Zero, BlendFunc_ConstantColour);


        // Shader.
        auto highlightShader = this->renderer.GetHighlightShader();
        assert(highlightShader != nullptr);
        {
            Renderer::SetCurrentShader(highlightShader);
            highlightShader->SetUniform("PVMMatrix", visibleObjects.projectionViewMatrix * mesh.transform);
            Renderer::PushPendingUniforms(*highlightShader);


            // Draw.
            Renderer::Draw(*mesh.vertexArray, mesh.drawMode);
        }
    }



    const DefaultSceneRenderer_LightGroup & DefaultSceneRenderer_MultiPassPipeline::GetMainLightGroup() const
    {
        if (!this->hasGeneratedMainLightGroup)
        {
            for (size_t i = 0; i < this->visibleObjects.lightManager.ambientLights.count; ++i)
            {
                this->mainLightGroup.AddAmbientLight(i);
            }

            for (size_t i = 0; i < this->visibleObjects.lightManager.directionalLights.count; ++i)
            {
                auto iLight = this->visibleObjects.lightManager.directionalLights.buffer[i]->value;
                assert(iLight != nullptr);
                {
                    if (!iLight->IsShadowCasting())
                    {
                        this->mainLightGroup.AddDirectionalLight(i);
                    }
                    else
                    {
                        this->mainLightGroup.AddShadowDirectionalLight(i);
                    }
                }
            }

            for (size_t i = 0; i < this->visibleObjects.lightManager.pointLights.count; ++i)
            {
                auto iLight = this->visibleObjects.lightManager.pointLights.buffer[i]->value;
                assert(iLight != nullptr);
                {
                    if (!iLight->IsShadowCasting())
                    {
                        this->mainLightGroup.AddPointLight(i);
                    }
                    else
                    {
                        this->mainLightGroup.AddShadowPointLight(i);
                    }
                }
            }

            for (size_t i = 0; i < this->visibleObjects.lightManager.spotLights.count; ++i)
            {
                auto iLight = this->visibleObjects.lightManager.spotLights.buffer[i]->value;
                assert(iLight != nullptr);
                {
                    if (!iLight->IsShadowCasting())
                    {
                        this->mainLightGroup.AddSpotLight(i);
                    }
                    else
                    {
                        this->mainLightGroup.AddShadowSpotLight(i);
                    }
                }
            }

            this->hasGeneratedMainLightGroup = true;
        }


        return this->mainLightGroup;
    }
}