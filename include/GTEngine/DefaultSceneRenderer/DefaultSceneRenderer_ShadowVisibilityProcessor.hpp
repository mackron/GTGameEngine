// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_DefaultSceneRenderer_ShadowVisibilityProcessor_hpp_
#define __GTEngine_DefaultSceneRenderer_ShadowVisibilityProcessor_hpp_

#include "../SceneCullingManager.hpp"
#include "DefaultSceneRenderer_Mesh.hpp"

namespace GT
{
    /// Callback class that will be used when querying the visible objects in a lights view frustum.
    class DefaultSceneRenderer_ShadowVisibilityProcessor : public SceneCullingManager::VisibilityCallback
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_ShadowVisibilityProcessor();

        /// Destructor.
        ~DefaultSceneRenderer_ShadowVisibilityProcessor();


        ////////////////////////////////////////////
        // Virtual Implementations.

        /// SceneCullingManager::VisibilityCallback::ProcessModel().
        void ProcessModel(const SceneNode &object);



        //////////////////////////////////////
        // Member Variables.

        /// The list of visible meshes.
        Vector<DefaultSceneRendererMesh> meshes;


    private:
        DefaultSceneRenderer_ShadowVisibilityProcessor(const DefaultSceneRenderer_ShadowVisibilityProcessor &);
        DefaultSceneRenderer_ShadowVisibilityProcessor & operator=(const DefaultSceneRenderer_ShadowVisibilityProcessor &);
    };
}

#endif