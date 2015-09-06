// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultSceneCallback
#define GT_DefaultSceneCallback

#include "SceneCallback.hpp"

namespace GT
{
    class GraphicsWorld;

    /// Helper class for scene callbacks.
    ///
    /// This can be used by most games and only references built-in scene node components. Every method is virtual so that
    /// sub-classes can customize them if required.
    ///
    /// This class requires a graphics world in order to manage graphics updates.
    class DefaultSceneCallback : public SceneCallback
    {
    public:

        /// Destructor.
        virtual ~DefaultSceneCallback();


        ////////////////////////////////////////////////////////////
        // Notifications

        /// @copydoc SceneCallback::OnSceneNodeInserted()
        virtual void OnSceneNodeInserted(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale);

        /// @copydoc SceneCallback::OnSceneNodeRemoved()
        virtual void OnSceneNodeRemoved(SceneNode* pSceneNode);

        /// @copydoc SceneCallback::OnSceneNodeTransformed()
        virtual void OnSceneNodeTransformed(SceneNode* pSceneNode, const vec4 &worldPosition, const quat &worldRotation, const vec4 &worldScale);



        ////////////////////////////////////////////////////////////
        // Operations

        /// @copydoc SceneCallback::UpdateSceneNode()
        virtual void UpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds);

        /// @copydoc SceneCallback::PostUpdateSceneNode()
        virtual void PostUpdateSceneNode(SceneNode* pSceneNode, double deltaTimeInSeconds);

        /// @copydoc SceneCallback::UpdateSceneNodePhysicsTransform()
        virtual void UpdateSceneNodePhysicsTransform(SceneNode* pSceneNode);

        /// @copydoc SceneCallback::StepPhysics()
        virtual void StepPhysics(double deltaTimeInSeconds);



        ////////////////////////////////////////////////////////////
        // Queries and Optimizations

        /// @copydoc SceneCallback::DoesSceneNodeRequireUpdate()
        virtual bool DoesSceneNodeRequireUpdate(SceneNode* pSceneNode);

        /// @copydoc SceneCallback::DoesSceneNodeRequirePostUpdate()
        virtual bool DoesSceneNodeRequirePostUpdate(SceneNode* pSceneNode);

        /// @copydoc SceneCallback::IsSceneNodeStatic()
        virtual bool IsSceneNodeStatic(SceneNode* pSceneNode);

        /// @copydoc SceneCallback::IsPhysicsObject()
        virtual bool IsPhysicsObject(SceneNode* pSceneNode);
    };
}

#endif
