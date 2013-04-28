// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_ShadowSpotLight_hpp_
#define __GTEngine_DefaultSceneRenderer_ShadowSpotLight_hpp_

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GTEngine
{
    /// Structure representing a shadow-casting spot light for the default scene renderer.
    struct DefaultSceneRendererShadowSpotLight : public SceneRendererSpotLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;


        /// Constructor.
        DefaultSceneRendererShadowSpotLight()
            : containedMeshes(), projection(), view()
        {
        }
    };
}

#endif