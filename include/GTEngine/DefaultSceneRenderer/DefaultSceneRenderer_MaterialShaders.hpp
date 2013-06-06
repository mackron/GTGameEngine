// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_MaterialShaders_hpp_
#define __GTEngine_DefaultSceneRenderer_MaterialShaders_hpp_

#include "../Rendering/Shader.hpp"
#include "DefaultSceneRenderer_MaterialShaderID.hpp"

namespace GTEngine
{
    /// Structure containing the shaders associated with a material.
    struct DefaultSceneRenderer_MaterialShaders
    {
        /// Constructor.
        DefaultSceneRenderer_MaterialShaders();

        /// Destructor.
        ~DefaultSceneRenderer_MaterialShaders();



        /// The map of shaders, keyed by a material shader ID.
        GTCore::Map<DefaultSceneRenderer_MaterialShaderID, Shader*> shaders;


    private:    // No copying.
        DefaultSceneRenderer_MaterialShaders(const DefaultSceneRenderer_MaterialShaders &);
        DefaultSceneRenderer_MaterialShaders & operator=(const DefaultSceneRenderer_MaterialShaders &);
    };
}

#endif