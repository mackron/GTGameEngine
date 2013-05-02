// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_MaterialShaders_hpp_
#define __GTEngine_DefaultSceneRenderer_MaterialShaders_hpp_

#include "../Rendering/Shader.hpp"
#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GTEngine
{
    /// Structure representing an ID of a material's shader.
    ///
    /// A material shader ID is created from a light group ID and material-specific flags.
    struct DefaultSceneRenderer_MaterialShaderID
    {
        /// Constructor.
        DefaultSceneRenderer_MaterialShaderID(const DefaultSceneRenderer_LightGroupID &lightGroupIDIn, uint32_t flagsIn)
            : lightGroupID(lightGroupIDIn), flags(flagsIn)
        {
        }



        /// Enumerator containing the different flags that can be used in a material shader ID.
        enum Flags
        {
            IncludeMaterialPass = (1 << 1),
            NoNormalMapping     = (1 << 2)
        };


        /// The light group ID.
        DefaultSceneRenderer_LightGroupID lightGroupID;

        /// The material-specific flags. Defaults to zero.
        uint32_t flags;
    };




    /// Structure containing the shaders associated with a material.
    struct DefaultSceneRenderer_MaterialShaders
    {
        /// Constructor.
        DefaultSceneRenderer_MaterialShaders();

        /// Destructor.
        ~DefaultSceneRenderer_MaterialShaders();


        /// The shader to use when doing an ambient light pass.
        Shader* ambientLightShader;

        /// The shader to use when doing a directional light pass.
        Shader* directionalLightShader;

        /// The shader to use when doing a point light pass.
        Shader* pointLightShader;

        /// The shader to use when doing a spot light pass.
        Shader* spotLightShader;


        /// The shader to use when doing a shadow directional light pass.
        Shader* shadowDirectionalLightShader;

        /// The shader to use when doing a shadow point light pass.
        Shader* shadowPointLightShader;

        /// The shader to use when doing a shadow spot light pass.
        Shader* shadowSpotLightShader;


        /// The shader to use when doing the material pass.
        Shader* materialShader;



        /// The map of shaders, keyed by a material shader ID.
        GTCore::Map<DefaultSceneRenderer_MaterialShaderID, Shader*> shaders;


    private:    // No copying.
        DefaultSceneRenderer_MaterialShaders(const DefaultSceneRenderer_MaterialShaders &);
        DefaultSceneRenderer_MaterialShaders & operator=(const DefaultSceneRenderer_MaterialShaders &);
    };
}

#endif