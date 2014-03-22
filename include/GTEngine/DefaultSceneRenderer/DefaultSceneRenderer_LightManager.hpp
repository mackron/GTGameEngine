// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_LightManager_hpp_
#define __GTEngine_DefaultSceneRenderer_LightManager_hpp_

#include "DefaultSceneRenderer_DirectionalLight.hpp"
#include "DefaultSceneRenderer_PointLight.hpp"
#include "DefaultSceneRenderer_SpotLight.hpp"
#include "DefaultSceneRenderer_LightGroup.hpp"

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


        /// Retrieves the total number of lights.
        unsigned int GetTotalLightCount() const;


        /// Takes a light group and sub-divides it into smaller chunks for use by the renderer.
        ///
        /// @remarks
        ///     The renderer will use the smaller chunks when doing rendering.
        void SubdivideLightGroup(const DefaultSceneRenderer_LightGroup &source, GTLib::Vector<DefaultSceneRenderer_LightGroup> &output, unsigned int options = 0);



        /// Enumerator for sub-division options.
        enum SubdivideOptions
        {
            SubdivideOption_ConvertShadowLights = (1 << 1)      // <-- Shadow-casting lights should be converted to non-shadow-casting lights.
        };


        /// The list of ambient lights.
        GTLib::Map<const AmbientLightComponent*, SceneRendererAmbientLight*> ambientLights;

        /// The list of directional lights.
        GTLib::Map<const DirectionalLightComponent*, DefaultSceneRendererDirectionalLight*> directionalLights;

        /// The list of point lights.
        GTLib::Map<const PointLightComponent*, DefaultSceneRendererPointLight*> pointLights;

        /// The list of spot lights.
        GTLib::Map<const SpotLightComponent*, DefaultSceneRendererSpotLight*> spotLights;


        /// The list of light indices of shadow-casting directional lights.
        GTLib::Vector<const DirectionalLightComponent*> shadowDirectionalLights;

        /// The list of light indices of shadow-casting point lights.
        GTLib::Vector<const PointLightComponent*> shadowPointLights;

        /// The list of light indices of shadow-casting spot lights.
        GTLib::Vector<const SpotLightComponent*> shadowSpotLights;
    };
}

#endif
