// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_SpotLight_hpp_
#define __GTEngine_DefaultSceneRenderer_SpotLight_hpp_

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GTEngine
{
    /// Structure representing a shadow-casting spot light for the default scene renderer.
    struct DefaultSceneRendererSpotLight : public SceneRendererSpotLight
    {
        /// A list of meshes to draw when building the shadow map.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshes;

        /// The projection matrix to use when building the shadow map.
        glm::mat4 projection;

        /// The view matrix to use when building the shadow map.
        glm::mat4 view;


        /// Constructor.
        DefaultSceneRendererSpotLight()
            : containedMeshes(), projection(), view()
        {
        }

        /// Determines whether or not the light casts shadows.
        bool IsShadowCasting() const
        {
            return this->containedMeshes.meshes.count > 0;
        }
    };
}

#endif