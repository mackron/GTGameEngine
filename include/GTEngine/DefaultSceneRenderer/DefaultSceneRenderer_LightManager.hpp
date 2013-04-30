// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_LightManager_hpp_
#define __GTEngine_DefaultSceneRenderer_LightManager_hpp_

#include "DefaultSceneRenderer_DirectionalLight.hpp"
#include "DefaultSceneRenderer_PointLight.hpp"
#include "DefaultSceneRenderer_SpotLight.hpp"

namespace GTEngine
{
    class DefaultSceneRenderer_LightManager
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_LightManager();

        /// Destructor.
        ~DefaultSceneRenderer_LightManager();


        /// Adds an ambient light to the manager.
        ///
        /// @param lightComponent [in] A pointer to the ambient light component.
        void AddAmbientLight(const AmbientLightComponent* lightComponent);

        /// Adds a directional light to the manager.
        ///
        /// @param lightComponent [in] A pointer to the directional light component.
        void AddDirectionalLight(const DirectionalLightComponent* lightComponent);

        /// Adds a point light to the manager.
        ///
        /// @param lightComponent [in] A pointer to the point light component.
        void AddPointLight(const PointLightComponent* lightComponent);

        /// Adds a spot light to the manager.
        ///
        /// @param lightComponent [in] A pointer to the spot light component.
        void AddSpotLight(const SpotLightComponent* lightComponent);





        /// The list of ambient lights.
        GTCore::Map<const AmbientLightComponent*, SceneRendererAmbientLight*> ambientLights;

        /// The list of directional lights.
        GTCore::Map<const DirectionalLightComponent*, DefaultSceneRendererDirectionalLight*> directionalLights;

        /// The list of point lights.
        GTCore::Map<const PointLightComponent*, DefaultSceneRendererPointLight*> pointLights;

        /// The list of spot lights.
        GTCore::Map<const SpotLightComponent*, DefaultSceneRendererSpotLight*> spotLights;


        /// The list of light indices of shadow-casting directional lights.
        GTCore::Vector<const DirectionalLightComponent*> shadowDirectionalLights;

        /// The list of light indices of shadow-casting point lights.
        GTCore::Vector<const PointLightComponent*> shadowPointLights;

        /// The list of light indices of shadow-casting spot lights.
        GTCore::Vector<const SpotLightComponent*> shadowSpotLights;
    };
}

#endif
