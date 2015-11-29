// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/SceneEditor/SceneEditorPhysicsManager.hpp>

namespace GT
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