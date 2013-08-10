// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorSceneUpdateManager.hpp>

namespace GTEngine
{
    SceneEditorSceneUpdateManager::SceneEditorSceneUpdateManager(const SceneNode &cameraNodeIn)
        : DefaultSceneUpdateManager(), cameraNode(cameraNodeIn), isEnabled(false), isParticlesEnabled(true)
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


    void SceneEditorSceneUpdateManager::EnableParticles()
    {
        this->isParticlesEnabled = true;
    }

    void SceneEditorSceneUpdateManager::DisableParticles()
    {
        this->isParticlesEnabled = false;
    }


    void SceneEditorSceneUpdateManager::StepSceneNode(SceneNode &node, double deltaTimeInSeconds, SceneCullingManager &cullingManager)
    {
        // We need to orientate any sprites to face the camera.
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            if (metadata->IsUsingSprite())
            {
                metadata->UpdateSpriteTransform(this->cameraNode);
            }

            // If we have dynamics or proximity component, we should update the wireframe meshes.
            if (metadata->IsSelected())
            {
                if (node.HasComponent<DynamicsComponent>())
                {
                    metadata->UpdateCollisionShapeMeshGeometry(this->cameraNode);
                }

                if (node.HasComponent<ProximityComponent>())
                {
                    metadata->UpdateProximityShapeMeshGeometry(this->cameraNode);
                }
            }
        }


        if (this->isEnabled)
        {
            
            DefaultSceneUpdateManager::StepSceneNode(node, deltaTimeInSeconds, cullingManager);
        }
        else
        {
            // If the particles are being updated, we should do that now.
            if (this->isParticlesEnabled)
            {
                auto particleSystemComponent = node.GetComponent<ParticleSystemComponent>();
                if (particleSystemComponent != nullptr)
                {
                    if (particleSystemComponent->IsPlayingOnStartup())      // Prevents particles from playing in-editor when not playing on startup. Unlikely deseriable.
                    {
                        auto particleSystem = particleSystemComponent->GetParticleSystem();
                        if (particleSystem != nullptr)
                        {
                            particleSystem->Update(deltaTimeInSeconds);
                            cullingManager.UpdateParticleSystemAABB(node);
                        }
                    }
                }
            }
        }
    }
}
