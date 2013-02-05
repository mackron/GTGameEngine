// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneEventHandler.hpp>

namespace GTEngine
{
    SceneEventHandler::SceneEventHandler()
    {
    }

    SceneEventHandler::~SceneEventHandler()
    {
    }

    void SceneEventHandler::OnObjectAdded(SceneObject &)
    {
    }

    void SceneEventHandler::OnObjectRemoved(SceneObject &)
    {
    }


    void SceneEventHandler::OnSceneNodeNameChanged(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeParentChanged(SceneNode &, SceneNode*)
    {
    }


    void SceneEventHandler::OnSceneNodeTransform(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeScale(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeHide(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeShow(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeComponentAdded(SceneNode &, Component &)
    {
    }

    void SceneEventHandler::OnSceneNodeComponentRemoved(SceneNode &, Component &)
    {
    }

    void SceneEventHandler::OnSceneNodeComponentChanged(SceneNode &, Component &)
    {
    }

    void SceneEventHandler::OnStateStackFrameCommitted()
    {
    }
}