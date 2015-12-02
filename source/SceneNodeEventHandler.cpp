// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneNodeEventHandler.hpp>

namespace GT
{
    void SceneNodeEventHandler::OnAttach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDetach(SceneNode &, SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnDestroy(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnTransform(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnScale(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnLayerChanged(SceneNode &, unsigned int)
    {
    }

    void SceneNodeEventHandler::OnSceneChanged(SceneNode &, Scene *)
    {
    }

    void SceneNodeEventHandler::OnStaticChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnVisibleChanged(SceneNode &)
    {
    }

    void SceneNodeEventHandler::OnUpdate(SceneNode &, double)
    {
    }

    void SceneNodeEventHandler::OnContact(SceneNode &, SceneNode &, const btManifoldPoint &)
    {
    }
}
