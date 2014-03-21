// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_LightManager.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_LightManager::DefaultSceneRenderer_LightManager()
        : ambientLights(), directionalLights(), pointLights(), spotLights(),
          shadowDirectionalLights(), shadowPointLights(), shadowSpotLights()
    {
    }

    DefaultSceneRenderer_LightManager::~DefaultSceneRenderer_LightManager()
    {
        for (size_t i = 0; i < this->ambientLights.count; ++i)
        {
            delete this->ambientLights.buffer[i]->value;
        }

        for (size_t i = 0; i < this->directionalLights.count; ++i)
        {
            delete this->directionalLights.buffer[i]->value;
        }

        for (size_t i = 0; i < this->pointLights.count; ++i)
        {
            delete this->pointLights.buffer[i]->value;
        }

        for (size_t i = 0; i < this->spotLights.count; ++i)
        {
            delete this->spotLights.buffer[i]->value;
        }
    }


    void DefaultSceneRenderer_LightManager::AddAmbientLight(const AmbientLightComponent* lightComponent)
    {
        assert(lightComponent != nullptr);
        {
            auto light = new SceneRendererAmbientLight;
            light->colour = lightComponent->GetColour();


            this->ambientLights.Add(lightComponent, light);
        }
    }

    void DefaultSceneRenderer_LightManager::AddDirectionalLight(const DirectionalLightComponent* lightComponent)
    {
        assert(lightComponent != nullptr);
        {
            auto light = new DefaultSceneRendererDirectionalLight;
            light->position      = lightComponent->GetNode().GetWorldPosition();
            light->orientation   = lightComponent->GetNode().GetWorldOrientation();
            light->colour        = lightComponent->GetColour();
            light->shadowCasting = lightComponent->IsShadowCastingEnabled();

            if (lightComponent->IsShadowCastingEnabled())
            {
                light->projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 500.0f);
                light->view       = glm::mat4_cast(glm::inverse(light->orientation)) * glm::translate(-light->position);
            }



            this->directionalLights.Add(lightComponent, light);

            if (light->shadowCasting)
            {
                this->shadowDirectionalLights.PushBack(lightComponent);
            }
        }
    }

    void DefaultSceneRenderer_LightManager::AddPointLight(const PointLightComponent* lightComponent)
    {
        assert(lightComponent != nullptr);
        {
            auto light = new DefaultSceneRendererPointLight;
            light->position      = lightComponent->GetNode().GetWorldPosition();
            light->colour        = lightComponent->GetColour();
            light->radius        = lightComponent->GetRadius();
            light->falloff       = lightComponent->GetFalloff();
            light->shadowCasting = lightComponent->IsShadowCastingEnabled();
            
            if (lightComponent->IsShadowCastingEnabled())
            {
                light->projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light->radius);

                glm::vec3 origin(light->position);
                glm::vec3 posx(  1.0f, 0.0f, 0.0f);
                glm::vec3 posy(  0.0f, 1.0f, 0.0f);
                glm::vec3 posz(  0.0f, 0.0f, 1.0f);

                light->positiveXView = glm::lookAt(origin, origin + posx, -posy);
                light->negativeXView = glm::lookAt(origin, origin - posx, -posy);
                light->positiveYView = glm::lookAt(origin, origin + posy,  posz);
                light->negativeYView = glm::lookAt(origin, origin - posy, -posz);
                light->positiveZView = glm::lookAt(origin, origin + posz, -posy);
                light->negativeZView = glm::lookAt(origin, origin - posz, -posy);
            }



            this->pointLights.Add(lightComponent, light);

            if (light->shadowCasting)
            {
                this->shadowPointLights.PushBack(lightComponent);
            }
        }
    }

    void DefaultSceneRenderer_LightManager::AddSpotLight(const SpotLightComponent* lightComponent)
    {
        assert(lightComponent != nullptr);
        {
            auto light = new DefaultSceneRendererSpotLight;
            light->position      = lightComponent->GetNode().GetWorldPosition();
            light->orientation   = lightComponent->GetNode().GetWorldOrientation();
            light->colour        = lightComponent->GetColour();
            light->length        = lightComponent->GetLength();
            light->falloff       = lightComponent->GetFalloff();
            light->innerAngle    = lightComponent->GetInnerAngle();
            light->outerAngle    = lightComponent->GetOuterAngle();
            light->shadowCasting = lightComponent->IsShadowCastingEnabled();

            if (lightComponent->IsShadowCastingEnabled())
            {
                light->projection = glm::perspective(glm::radians(light->outerAngle * 2.0f), 1.0f, 0.1f, light->length);
                light->view       = glm::mat4_cast(glm::inverse(light->orientation)) * glm::translate(-light->position);
            }



            this->spotLights.Add(lightComponent, light);

            if (light->shadowCasting)
            {
                this->shadowSpotLights.PushBack(lightComponent);
            }
        }
    }


    unsigned int DefaultSceneRenderer_LightManager::GetTotalLightCount() const
    {
        return this->ambientLights.count + this->directionalLights.count + this->pointLights.count + this->spotLights.count;
    }


    void DefaultSceneRenderer_LightManager::SubdivideLightGroup(const DefaultSceneRenderer_LightGroup &source, GTCore::Vector<DefaultSceneRenderer_LightGroup> &output, unsigned int options)
    {
        output.Clear();     // TODO: Check that we actually need this clear.


        // For now we will make it so every non-shadow-casting light is in a single group. This will need to be split later on.
        DefaultSceneRenderer_LightGroup lightGroup;

        for (uint16_t i = 0; i < source.GetAmbientLightCount(); ++i)
        {
            lightGroup.AddAmbientLight(source.lightIDs[i + source.GetAmbientLightStartIndex()]);
        }

        for (uint16_t i = 0; i < source.GetDirectionalLightCount(); ++i)
        {
            lightGroup.AddDirectionalLight(source.lightIDs[i + source.GetDirectionalLightStartIndex()]);
        }

        for (uint16_t i = 0; i < source.GetPointLightCount(); ++i)
        {
            lightGroup.AddPointLight(source.lightIDs[i + source.GetPointLightStartIndex()]);
        }

        for (uint16_t i = 0; i < source.GetSpotLightCount(); ++i)
        {
            lightGroup.AddSpotLight(source.lightIDs[i + source.GetSpotLightStartIndex()]);
        }


        if ((options & SubdivideOption_ConvertShadowLights))
        {
            for (uint16_t i = 0; i < source.GetShadowDirectionalLightCount(); ++i)
            {
                lightGroup.AddDirectionalLight(source.lightIDs[i + source.GetShadowDirectionalLightStartIndex()]);
            }

            for (uint16_t i = 0; i < source.GetShadowPointLightCount(); ++i)
            {
                lightGroup.AddPointLight(source.lightIDs[i + source.GetShadowPointLightStartIndex()]);
            }

            for (uint16_t i = 0; i < source.GetShadowSpotLightCount(); ++i)
            {
                lightGroup.AddSpotLight(source.lightIDs[i + source.GetShadowSpotLightStartIndex()]);
            }
        }

        if (lightGroup.GetLightCount() > 0)
        {
            output.PushBack(lightGroup);
        }



        // Each shadow-casting light should have it's own group.

        if (!(options & SubdivideOption_ConvertShadowLights))
        {
            for (uint16_t i = 0; i < source.GetShadowDirectionalLightCount(); ++i)
            {
                output.PushBack(DefaultSceneRenderer_LightGroup());
                auto &lightGroup = output.GetBack();
                {
                    lightGroup.AddShadowDirectionalLight(source.lightIDs[i + source.GetShadowDirectionalLightStartIndex()]);
                }
            }

            for (uint16_t i = 0; i < source.GetShadowPointLightCount(); ++i)
            {
                output.PushBack(DefaultSceneRenderer_LightGroup());
                auto &lightGroup = output.GetBack();
                {
                    lightGroup.AddShadowPointLight(source.lightIDs[i + source.GetShadowPointLightStartIndex()]);
                }
            }

            for (uint16_t i = 0; i < source.GetShadowSpotLightCount(); ++i)
            {
                output.PushBack(DefaultSceneRenderer_LightGroup());
                auto &lightGroup = output.GetBack();
                {
                    lightGroup.AddShadowSpotLight(source.lightIDs[i + source.GetShadowSpotLightStartIndex()]);
                }
            }
        }
    }
}