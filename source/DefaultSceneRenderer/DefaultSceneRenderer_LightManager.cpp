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
                light->projection = glm::perspective(90.0f, 1.0f, 0.1f, light->radius);

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
                light->projection = glm::perspective(light->outerAngle * 2.0f, 1.0f, 0.1f, light->length);
                light->view       = glm::mat4_cast(glm::inverse(light->orientation)) * glm::translate(-light->position);
            }



            this->spotLights.Add(lightComponent, light);

            if (light->shadowCasting)
            {
                this->shadowSpotLights.PushBack(lightComponent);
            }
        }
    }
}