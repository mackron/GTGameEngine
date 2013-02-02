
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


    void SceneEditorSceneEventHandler::OnObjectAdded(SceneObject &object)
    {
        this->sceneEditor.OnObjectAdded(object);
    }

    void SceneEditorSceneEventHandler::OnObjectRemoved(SceneObject &object)
    {
        this->sceneEditor.OnObjectRemoved(object);
    }


    void SceneEditorSceneEventHandler::OnSceneNodeNameChanged(SceneNode &node)
    {
        this->sceneEditor.OnSceneNodeNameChanged(node);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeTransform(SceneNode &node)
    {
        this->sceneEditor.OnSceneNodeTransform(node);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeScale(SceneNode &node)
    {
        this->sceneEditor.OnSceneNodeScale(node);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeHide(SceneNode &node)
    {
        this->sceneEditor.OnSceneNodeHide(node);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeShow(SceneNode &node)
    {
        this->sceneEditor.OnSceneNodeShow(node);
    }


    void SceneEditorSceneEventHandler::OnSceneNodeComponentAdded(SceneNode &node, Component &component)
    {
        this->sceneEditor.OnSceneNodeComponentAdded(node, component);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeComponentRemoved(SceneNode &node, Component &component)
    {
        this->sceneEditor.OnSceneNodeComponentRemoved(node, component);
    }

    void SceneEditorSceneEventHandler::OnSceneNodeComponentChanged(SceneNode &node, Component &component)
    {
        this->sceneEditor.OnSceneNodeComponentChanged(node, component);
    }

    void SceneEditorSceneEventHandler::OnStateStackFrameCommitted()
    {
        this->sceneEditor.OnStateStackFrameCommitted();
    }
}