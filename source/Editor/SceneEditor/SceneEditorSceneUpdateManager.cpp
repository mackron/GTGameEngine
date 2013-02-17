// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorSceneUpdateManager.hpp>

namespace GTEngine
{
    SceneEditorSceneUpdateManager::SceneEditorSceneUpdateManager(const SceneNode &cameraNodeIn)
        : DefaultSceneUpdateManager(), cameraNode(cameraNodeIn), isEnabled(false)
    {
    }

    SceneEditorSceneUpdateManager::~SceneEditorSceneUpdateManager()
    {
    }


    void SceneEditorSceneUpdateManager::Enable()
    {
        this->isEnabled = true;
    }

    void SceneEditorSceneUpdateManager::Disable()
    {
        this->isEnabled = false;
    }


    void SceneEditorSceneUpdateManager::StepSceneNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // We need to orientate any sprites to face the camera.
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            if (metadata->IsUsingSprite())
            {
                // After transforming with the camera, we want the orientation to be such that it's facing +1.0 on the z axis. If we make it's orientation
                // that of the camera orientation and then rotate around Y by 180 degrees, we should have it.
                glm::quat orientation = this->cameraNode.GetWorldOrientation() * glm::angleAxis(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

                glm::mat4 transform = glm::mat4_cast(orientation);
                transform[3] = glm::vec4(node.GetWorldPosition(), 1.0f);
                transform   *= glm::scale(node.GetWorldScale());

                metadata->SetSpriteTransform(transform);
                metadata->ApplyTransformToSprite();
            }
        }

        if (this->isEnabled)
        {
            DefaultSceneUpdateManager::StepSceneNode(node, deltaTimeInSeconds);
        }
    }
}
