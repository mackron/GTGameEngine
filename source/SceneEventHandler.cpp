// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneEventHandler.hpp>

namespace GT
{
    SceneEventHandler::SceneEventHandler()
    {
    }

    SceneEventHandler::~SceneEventHandler()
    {
    }

    void SceneEventHandler::OnSceneNodeAdded(SceneNode &)
    {
    }

    void SceneEventHandler::OnSceneNodeRemoved(SceneNode &)
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

    void SceneEventHandler::OnSceneNodeComponentChanged(SceneNode &, Component &, uint32_t)
    {
    }

    void SceneEventHandler::OnStateStackFrameCommitted()
    {
    }
}