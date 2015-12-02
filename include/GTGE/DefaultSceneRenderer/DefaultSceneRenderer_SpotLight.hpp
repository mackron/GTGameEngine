// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_DefaultSceneRenderer_SpotLight
#define GT_DefaultSceneRenderer_SpotLight

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GT
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

        /// Keeps track of whether or not the light casts shadows.
        bool shadowCasting;


        /// Constructor.
        DefaultSceneRendererSpotLight()
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