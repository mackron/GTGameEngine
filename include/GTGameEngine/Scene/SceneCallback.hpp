// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneCallback_hpp_
#define __GT_SceneCallback_hpp_

#include "../Math.hpp"

namespace GT
{
    class Scene;
    class SceneNode;

    /// Class containing callback routines used by the scenes to both notify the host application
    /// of changes to the scene and to request higher level information about scene nodes.
    class SceneCallback
    {
    public:

        ////////////////////////////////////////////////////////////
        // Notifications

        /// Called after a scene node has been added to the scene.
        ///
        /// @param pSceneNode [in] A pointer to the scene node that was just added.
        ///
        /// @remarks
        ///     If the scene is being stepped at the time the scene node is inserted, this event will be
        ///     deferred until the end of the step.
        virtual void OnSceneNodeInserted(SceneNode* pSceneNode);

        /// Called when a scene node is about to be removed from the scene.
        ///
        /// @param pSceneNode [in] A pointer to the scene node that is about to be removed.
        ///
        /// @remarks
        ///     If the scene is being stepped at the time the scene node is inserted, this event will be
        ///     deferred until the end of the step.
        virtual void OnSceneNodeRemoved(SceneNode* pSceneNode);

        /// Called after a scene node has had it's transformation changed.
        ///
        /// @param pSceneNode    [in] A pointer to the scene node that has been transformed.
        /// @param worldPosition [in] The absolute position of the scene node.
        /// @param worldRotation [in] The absolute rotation of the scene node.
        /// @param worldScale    [in] The absolute scale of the scene node.
        ///
        /// @remarks
        ///     Use pSceneNode->GetPosition(), etc. to retrieve the local transformation (relative to the parent node).
        ///     @par
        ///     The default implementation does nothing, but this is where the graphical representation of the node will
        ///     typically want to be updated.
        virtual void OnSceneNodeTransformed(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale);



        ////////////////////////////////////////////////////////////
        // Operations

        /// Called when a scene node needs to be updated.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds.
        ///
        /// @remarks
        ///     The default implementation does nothing. Typically you will want to call an update function on
        ///     the scene node or whatnot.
        virtual void UpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds);

        /// Called when a scene node needs to be updated in the post-update stage.
        ///
        /// @param deltaTimeInSeconds [in] The delta time in seconds.
        ///
        /// @remarks
        ///     The default implementation does nothing. Typically you will want to call an update function on
        ///     the scene node or whatnot.
        virtual void PostUpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds);

        /// Called when transformation of the corresponding physics object for the given scene node needs to be updated.
        ///
        /// @param pSceneNode [in] A pointer to the relevant scene node.
        ///
        /// @remarks
        ///     The default implementation does nothing.
        virtual void UpdateSceneNodePhysicsTransform(SceneNode* pSceneNode);

        /// Steps the physics simulation.
        ///
        /// @param deltaTimeInSeconds [in] The time between frames, in seconds.
        ///
        /// @remarks
        ///     The default implementation does nothing.
        virtual void StepPhysics(double deltaTimeInSeconds);



        ////////////////////////////////////////////////////////////
        // Queries and Optimizations

        /// Optimization: Determines whether or not the given scene node requires an update on each frame.
        ///
        /// @param pSceneNode [in] A pointer to the scene node in question.
        ///
        /// @remarks
        ///     This is used to optimize the update process. Many scene nodes such as decorations and collision
        ///     volumes don't need to have UpdateSceneNode() called on them. This allows the stepping pipeline
        ///     to optimize this.
        ///     @par
        ///     This will be called once whenever the scene node is added to the scene.
        ///     @par
        ///     The default implementation returns true for all scene nodes.
        virtual bool DoesSceneNodeRequireUpdate(SceneNode* pSceneNode);

        /// Optimization: The same as DoesSceneNodeRequireUpdate(), except for the post-update stage.
        ///
        /// @param pSceneNode [in] A pointer to the scene node in question.
        ///
        /// @remarks
        ///     The post-update stage is done after the main update and physics simulation has been done.
        ///     @par
        ///     For the sake of technical correctness and consistency, the default default implementation will
        ///     return true for every scene node, however most scene nodes will not need a post-update stage
        ///     so it is recommended to return false for all scene nodes except those that strictly require it.
        virtual bool DoesSceneNodeRequirePostUpdate(SceneNode* pSceneNode);

        /// Optimization: Determines whether or not the given scene node is static.
        ///
        /// @param pSceneNode [in] A pointer to the scene node in question.
        ///
        /// @remarks
        ///     The stepping pipeline can optimize static objects because it is able to run with the assumption
        ///     that it's transformation will never change. By default this will return false (non-static) for
        ///     every scene node, but implementations should return true whenever possible.
        virtual bool IsSceneNodeStatic(SceneNode* pSceneNode);

        /// Optimization: Determines whether or not the given scene node should be treated as a physics object.
        ///
        /// @remarks
        ///     This is used for optimizing the default stepping pipeline. Returning false here means that the
        ///     stepping pipeline can assume the scene node will not be used in physics simulations and allows
        ///     it to place the node in an optimized list.
        ///     @par
        ///     It is safe to return true for every scene node, but you will not receive the benefit of this
        ///     particular optimization. This is the default behaviour.
        virtual bool IsPhysicsObject(SceneNode* pSceneNode);
    };
}

#endif
