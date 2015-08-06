// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneEventHandler_hpp_
#define __GT_SceneEventHandler_hpp_

#include "../Math.hpp"

namespace GT
{
    class SceneNode;
    class SceneNodeComponent;


    /// Base class for handling events from a scene.
    class SceneEventHandler
    {
    public:

        /// Called after a scene node is added to the scene.
        ///
        /// @param pSceneNode [in] A pointer to the new scene node.
        virtual void OnSceneNodeInserted(SceneNode* pSceneNode) = 0;

        /// Called after a scene node has been removed from the scene, but before it is deleted.
        ///
        /// @param pSceneNode [in] A pointer to the scene node.
        virtual void OnSceneNodeRemoved(SceneNode* pSceneNode) = 0;


        /// Called when the transformation of a scene node has changed.
        ///
        /// @param pSceneNode    [in] A pointer to the scene node.
        /// @param worldPosition [in] The absolute position of the scene node.
        /// @param worldRotation [in] The absolute rotation of the scene node.
        /// @param worldScale    [in] The absolute scale of the scene node.
        ///
        /// @remarks
        ///     To retrieve the local position (relative to the parent), use pSceneNode->GetPosition(), etc.
        ///     @par
        ///     This is where you will want to update the graphical representation of the scene node.
        virtual void OnSceneNodeTransformed(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale) = 0;
    };
}

#endif
