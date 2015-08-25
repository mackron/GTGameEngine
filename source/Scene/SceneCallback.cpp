// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneCallback.hpp>

namespace GT
{
    SceneCallback::~SceneCallback()
    {
    }



    ////////////////////////////////////////////////////////////
    // Notifications

    void SceneCallback::OnSceneNodeInserted(SceneNode*, const vec4 &, const quat &, const vec4 &)
    {
    }

    void SceneCallback::OnSceneNodeRemoved(SceneNode*)
    {
    }

    void SceneCallback::OnSceneNodeTransformed(SceneNode*, const vec4 &, const quat &, const vec4 &)
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
