// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneRendererMesh_hpp_
#define __GTEngine_DefaultSceneRendererMesh_hpp_

#include "../SceneRenderer.hpp"
#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GT
{
    /// Structure representing a mesh in the default renderer.
    struct DefaultSceneRendererMesh : public SceneRendererMesh
    {
        /// The indices of the lights that are touching this mesh.
        const DefaultSceneRenderer_LightGroup* touchingLights;

        /// The highlight colour.
        glm::vec3 highlightColour;



        /// Constructor.
        DefaultSceneRendererMesh()
            : touchingLights(nullptr), highlightColour(1.0f, 1.0f, 1.0f)
        {
        }

        /// Copy constructor.
        DefaultSceneRendererMesh(const DefaultSceneRendererMesh &other)
            : SceneRendererMesh(other), touchingLights(other.touchingLights), highlightColour(other.highlightColour)
        {
        }

        /// Destructor.
        ~DefaultSceneRendererMesh()
        {
        }



    private:    // No copying.
        DefaultSceneRendererMesh & operator=(const DefaultSceneRendererMesh &);
    };
}

#endif