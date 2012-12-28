
#include <GTEngine/Editor/SceneEditorSceneEventHandler.hpp>
#include <GTEngine/Editor/Editor_SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorSceneEventHandler::SceneEditorSceneEventHandler(Editor_SceneEditor &sceneEditorIn, Scene &sceneIn)
        : sceneEditor(sceneEditorIn), scene(sceneIn)
    {
    }

    SceneEditorSceneEventHandler::~SceneEditorSceneEventHandler()
    {
    }


    void SceneEditorSceneEventHandler::OnObjectAdded(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);
            
            if (!node.HasComponent<EditorMetadataComponent>())
            {
                node.AddComponent<EditorMetadataComponent>();
            }
        }
    }

    void SceneEditorSceneEventHandler::OnObjectRemoved(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            // We need to make sure scene nodes are deseleted when they are removed from the scene.
            this->sceneEditor.DeselectSceneNode(static_cast<SceneNode &>(object));
        }
    }
}