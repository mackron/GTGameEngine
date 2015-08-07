// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneCallback.hpp>

namespace GT
{
    ////////////////////////////////////////////////////////////
    // Notifications

    void SceneCallback::OnSceneNodeInserted(SceneNode*)
    {
    }

    void SceneCallback::OnSceneNodeRemoved(SceneNode*)
    {
    }



    ////////////////////////////////////////////////////////////
    // Operations

    void SceneCallback::UpdateSceneNode(SceneNode*, double)
    {
    }

    void SceneCallback::PostUpdateSceneNode(SceneNode*, double)
    {
    }

    void SceneCallback::UpdateSceneNodePhysicsTransform(SceneNode*)
    {
    }

    void SceneCallback::StepPhysics(double)
    {
    }



    ////////////////////////////////////////////////////////////
    // Queries and Optimizations

    bool SceneCallback::DoesSceneNodeRequireUpdate(SceneNode*)
    {
        return true;
    }

    bool SceneCallback::DoesSceneNodeRequirePostUpdate(SceneNode*)
    {
        return true;
    }

    bool SceneCallback::IsSceneNodeStatic(SceneNode*)
    {
        return false;
    }

    bool SceneCallback::IsPhysicsObject(SceneNode*)
    {
        return true;
    }
}
