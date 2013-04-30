// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_DirectionalLight_hpp_
#define __GTEngine_DefaultSceneRenderer_DirectionalLight_hpp_

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GTEngine
{
    /// Structure representing a directional light in the default scene renderer.
    struct DefaultSceneRendererDirectionalLight : public SceneRendererDirectionalLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;

        /// Keeps track of whether or not the light casts shadows.
        bool shadowCasting;



        /// Constructor.
        DefaultSceneRendererDirectionalLight()
            : containedMeshes(), projection(), view(), shadowCasting(false)
        {
        }

        /// Determines whether or not the light casts shadows.
        bool IsShadowCasting() const
        {
            return this->shadowCasting;
        }
    };
}

#endif