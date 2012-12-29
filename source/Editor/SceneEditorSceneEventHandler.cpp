
#include <GTEngine/Editor/SceneEditorSceneEventHandler.hpp>
#include <GTEngine/Editor/Editor_SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorSceneEventHandler::SceneEditorSceneEventHandler(Editor_SceneEditor &sceneEditorIn)
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

    void SceneEditorSceneEventHandler::OnObjectRefreshed(SceneObject &object)
    {
        this->sceneEditor.OnObjectRefreshed(object);
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
}