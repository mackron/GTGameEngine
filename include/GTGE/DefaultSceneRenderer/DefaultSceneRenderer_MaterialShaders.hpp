// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_DefaultSceneRenderer_MaterialShaders
#define GT_DefaultSceneRenderer_MaterialShaders

#include "../Rendering/Shader.hpp"
#include "DefaultSceneRenderer_MaterialShaderID.hpp"

namespace GT
{
    /// Structure containing the shaders associated with a material.
    struct DefaultSceneRenderer_MaterialShaders
    {
        /// Constructor.
        DefaultSceneRenderer_MaterialShaders();

        /// Destructor.
        ~DefaultSceneRenderer_MaterialShaders();



        /// The map of shaders, keyed by a material shader ID.
        Map<DefaultSceneRenderer_MaterialShaderID, Shader*> shaders;


    private:    // No copying.
        DefaultSceneRenderer_MaterialShaders(const DefaultSceneRenderer_MaterialShaders &);
        DefaultSceneRenderer_MaterialShaders & operator=(const DefaultSceneRenderer_MaterialShaders &);
    };
}

#endif