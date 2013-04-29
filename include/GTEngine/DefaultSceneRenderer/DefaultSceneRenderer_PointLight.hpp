// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_ShadowPointLight_hpp_
#define __GTEngine_DefaultSceneRenderer_ShadowPointLight_hpp_

#include "DefaultSceneRenderer_ShadowVisibilityProcessor.hpp"

namespace GTEngine
{
    /// Structure representing a shadow-casting point light.
    struct DefaultSceneRendererPointLight : public SceneRendererPointLight
    {
        /// The list of meshes to draw when doing the positive X face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesPositiveX;

        /// The list of meshes to draw when doing the negative X face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesNegativeX;

        /// The list of meshes to draw when doing the positive Y face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesPositiveY;

        /// The list of meshes to draw when doing the negative Y face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesNegativeY;

        /// The list of meshes to draw when doing the positive Z face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesPositiveZ;

        /// The list of meshes to draw when doing the negative Z face.
        DefaultSceneRenderer_ShadowVisibilityProcessor containedMeshesNegativeZ;


        /// The positive X view matrix.
        glm::mat4 positiveXView;

        /// The negative X view matrix.
        glm::mat4 negativeXView;

        /// The positive Y view matrix.
        glm::mat4 positiveYView;

        /// The negative Y view matrix.
        glm::mat4 negativeYView;

        /// The positive Z view matrix.
        glm::mat4 positiveZView;

        /// The negative Z view matrix.
        glm::mat4 negativeZView;


        /// The projection matrix.
        glm::mat4 projection;


        /// Constructor.
        DefaultSceneRendererPointLight()
            : containedMeshesPositiveX(), containedMeshesNegativeX(),
              containedMeshesPositiveY(), containedMeshesNegativeY(),
              containedMeshesPositiveZ(), containedMeshesNegativeZ(),
              positiveXView(), negativeXView(),
              positiveYView(), negativeYView(),
              positiveZView(), negativeZView(),
              projection()
        {
        }

        /// Determines whether or not the light casts shadows.
        bool IsShadowCasting() const
        {
            return this->containedMeshesPositiveX.meshes.count > 0 ||
                   this->containedMeshesNegativeX.meshes.count > 0 ||
                   this->containedMeshesPositiveY.meshes.count > 0 ||
                   this->containedMeshesNegativeY.meshes.count > 0 ||
                   this->containedMeshesPositiveZ.meshes.count > 0 ||
                   this->containedMeshesNegativeZ.meshes.count > 0;
        }
    };
}

#endif