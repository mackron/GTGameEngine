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
                metadata->UpdateSpriteTransform(this->cameraNode);
            }
        }


        if (this->isEnabled)
        {
            DefaultSceneUpdateManager::StepSceneNode(node, deltaTimeInSeconds);
        }
    }
}
