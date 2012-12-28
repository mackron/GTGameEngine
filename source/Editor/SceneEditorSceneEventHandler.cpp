
#include <GTEngine/Editor/SceneEditorSceneEventHandler.hpp>
#include <GTEngine/Scene.hpp>

namespace GTEngine
{
    SceneEditorSceneEventHandler::SceneEditorSceneEventHandler(Scene &sceneIn)
        : scene(sceneIn)
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
        (void)object;
    }
}