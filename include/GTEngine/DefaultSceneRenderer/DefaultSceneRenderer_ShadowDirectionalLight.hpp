// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_ShadowDirectionalLight_hpp_
#define __GTEngine_DefaultSceneRenderer_ShadowDirectionalLight_hpp_

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GTEngine
{
    /// Structure representing a shadow-casting directional light.
    struct DefaultSceneRendererShadowDirectionalLight : public SceneRendererDirectionalLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;



        /// Constructor.
        DefaultSceneRendererShadowDirectionalLight()
            : containedMeshes(), projection(), view()
        {
        }
    };
}

#endif