// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneRenderer_LightManager_hpp_
#define __GTEngine_DefaultSceneRenderer_LightManager_hpp_

#include "DefaultSceneRenderer_DirectionalLight.hpp"
#include "DefaultSceneRenderer_PointLight.hpp"
#include "DefaultSceneRenderer_SpotLight.hpp"
#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GT
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


        /// Retrieves the total number of lights.
        unsigned int GetTotalLightCount() const;


        /// Takes a light group and sub-divides it into smaller chunks for use by the renderer.
        ///
        /// @remarks
        ///     The renderer will use the smaller chunks when doing rendering.
        void SubdivideLightGroup(const DefaultSceneRenderer_LightGroup &source, Vector<DefaultSceneRenderer_LightGroup> &output, unsigned int options = 0);



        /// Enumerator for sub-division options.
        enum SubdivideOptions
        {
            SubdivideOption_ConvertShadowLights = (1 << 1)      // <-- Shadow-casting lights should be converted to non-shadow-casting lights.
        };


        /// The list of ambient lights.
        Map<const AmbientLightComponent*, SceneRendererAmbientLight*> ambientLights;

        /// The list of directional lights.
        Map<const DirectionalLightComponent*, DefaultSceneRendererDirectionalLight*> directionalLights;

        /// The list of point lights.
        Map<const PointLightComponent*, DefaultSceneRendererPointLight*> pointLights;

        /// The list of spot lights.
        Map<const SpotLightComponent*, DefaultSceneRendererSpotLight*> spotLights;


        /// The list of light indices of shadow-casting directional lights.
        Vector<const DirectionalLightComponent*> shadowDirectionalLights;

        /// The list of light indices of shadow-casting point lights.
        Vector<const PointLightComponent*> shadowPointLights;

        /// The list of light indices of shadow-casting spot lights.
        Vector<const SpotLightComponent*> shadowSpotLights;
    };
}

#endif
