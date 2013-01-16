
#include <GTEngine/Editor/SceneEditor/SceneEditorPhysicsManager.hpp>

namespace GTEngine
{
    SceneEditorPhysicsManager::SceneEditorPhysicsManager()
        : DefaultScenePhysicsManager(), isEnabled(false)
    {
    }

    SceneEditorPhysicsManager::~SceneEditorPhysicsManager()
    {
    }

    void SceneEditorPhysicsManager::EnableSimulation()
    {
        this->isEnabled = true;
    }

    void SceneEditorPhysicsManager::DisableSimulation()
    {
        this->isEnabled = false;
    }


    void SceneEditorPhysicsManager::Step(double deltaTimeInSeconds)
    {
        if (this->isEnabled)
        {
            DefaultScenePhysicsManager::Step(deltaTimeInSeconds);
        }
    }
}