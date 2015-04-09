// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneEventHandler_hpp_
#define __GT_SceneEventHandler_hpp_

namespace GT
{
    class Scene;
    class SceneNode;
    class SceneNodeComponent;


    /// Base class for handling events from a scene.
    class SceneEventHandler
    {
    public:

        /// Called after a scene node is added to the scene.
        ///
        /// @param pSceneNode [in] A pointer to the new scene node.
        virtual void OnSceneNodeAdded(SceneNode* pSceneNode) = 0;

        /// Called after a scene node has been removed from the scene, but before it is deleted.
        ///
        /// @param pSceneNode [in] A pointer to the scene node.
        virtual void OnSceneNodeRemoved(SceneNode* pSceneNode) = 0;
    };
}

#endif
