// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/SceneEditor/SceneEditorSceneEventHandler.hpp>
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorSceneEventHandler::SceneEditorSceneEventHandler(SceneEditor &sceneEditorIn)
        : sceneEditor(sceneEditorIn)
    {
    }

    SceneEditorSceneEventHandler::~SceneEditorSceneEventHandler()
    {
    }


    void SceneEditorSceneEventHandler::OnSceneNodeAdded(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeAdded(sceneNode);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeRemoved(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeRemoved(sceneNode);
    }


    void SceneEditorSceneEventHandler::OnSceneNodeNameChanged(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeNameChanged(sceneNode);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeParentChanged(SceneNode &sceneNode, SceneNode* previousParent)
    {
        this->sceneEditor.OnSceneNodeParentChanged(sceneNode, previousParent);
    }


    void SceneEditorSceneEventHandler::OnSceneNodeTransform(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeTransform(sceneNode);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeScale(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeScale(sceneNode);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeHide(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeHide(sceneNode);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeShow(SceneNode &sceneNode)
    {
        this->sceneEditor.OnSceneNodeShow(sceneNode);
    }


    void SceneEditorSceneEventHandler::OnSceneNodeComponentAdded(SceneNode &sceneNode, Component &component)
    {
        this->sceneEditor.OnSceneNodeComponentAdded(sceneNode, component);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeComponentRemoved(SceneNode &sceneNode, Component &component)
    {
        this->sceneEditor.OnSceneNodeComponentRemoved(sceneNode, component);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeComponentChanged(SceneNode &sceneNode, Component &component, uint32_t whatChangedFlags)
    {
        this->sceneEditor.OnSceneNodeComponentChanged(sceneNode, component, whatChangedFlags);
    }

    void SceneEditorSceneEventHandler::OnStateStackFrameCommitted()
    {
        this->sceneEditor.OnStateStackFrameCommitted();
    }
}
