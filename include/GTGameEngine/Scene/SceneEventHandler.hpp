// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneEventHandler_hpp_
#define __GT_SceneEventHandler_hpp_

#include "../Math.hpp"

namespace GT
{
    class SceneNode;
    class SceneNodeComponent;


    /// Callback class for use by scenes. One of these should be attached to each scene so that it can provide information
    /// to it as required.
    class SceneEventHandler
    {
    public:

        /// Determines whether or not the given scene node is a physics object.
        ///
        /// @param sceneNode [in] A pointer to the scene node being queried.
        ///
        /// @remarks
        ///     Typically true will be returned when a Dynamics component is attached to the scene node.
        virtual bool IsPhysicsObject(SceneNode* pSceneNode) = 0;



        /// Called after a scene node is added to the scene.
        ///
        /// @param pSceneNode [in] A pointer to the new scene node.
        virtual void OnSceneNodeInserted(SceneNode* pSceneNode) = 0;

        /// Called after a scene node has been removed from the scene, but before it is deleted.
        ///
        /// @param pSceneNode [in] A pointer to the scene node.
        virtual void OnSceneNodeRemoved(SceneNode* pSceneNode) = 0;

        /// Called when a scene node needs to be updated.
        ///
        /// @remarks
        ///     This event is unique in that it can be called simultaneously from any thread and is not deferred until the end of the update pipeline.
        ///     @par
        ///     This is where the 
        virtual void OnSceneNodeUpdate(SceneNode* pSceneNode, double deltaTimeInSeconds) = 0;

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


        /// Called when the position of a scene node needs to be updated on the physics system.
        ///
        /// @remarks
        ///     This will be called for scene nodes whose transform is explicitly changed by the previous update.
        virtual void UpdateSceneNodePhysicsTransform(SceneNode* pSceneNode) = 0;

        /// Called when the physics simulation needs to be performed.
        virtual void DoPhysics(double deltaTimeInSeconds) = 0;
    };
}

#endif
