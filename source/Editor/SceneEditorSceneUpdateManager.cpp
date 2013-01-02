
#include <GTEngine/Editor/SceneEditorSceneUpdateManager.hpp>

namespace GTEngine
{
    SceneEditorSceneUpdateManager::SceneEditorSceneUpdateManager(const SceneNode &cameraNodeIn)
        : DefaultSceneUpdateManager(), cameraNode(cameraNodeIn)
    {
    }

    SceneEditorSceneUpdateManager::~SceneEditorSceneUpdateManager()
    {
    }


    void SceneEditorSceneUpdateManager::StepSceneNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // We need to orientate any sprites to face the camera.
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            if (metadata->GetModelTransformMode() == EditorMetadataComponent::ModelTransformMode_FaceCamera)
            {
                glm::mat4 transform(Math::CalculateLookAtMatrix(node.GetWorldPosition(), this->cameraNode.GetWorldPosition()));
                transform[3] = glm::vec4(node.GetWorldPosition(), 1.0f);

                transform *= glm::scale(node.GetWorldScale());

                metadata->SetCustomModelTransform(transform);
            }
        }

        DefaultSceneUpdateManager::StepSceneNode(node, deltaTimeInSeconds);
    }
}
