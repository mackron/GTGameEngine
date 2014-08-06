// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Scripting.hpp>

namespace GTEngine
{
    DefaultSceneUpdateManager::DefaultSceneUpdateManager()
        : sceneNodes()
    {
    }

    DefaultSceneUpdateManager::~DefaultSceneUpdateManager()
    {
    }


    bool DefaultSceneUpdateManager::NeedsUpdate(SceneNode &sceneNode) const
    {
        auto scriptComponent = sceneNode.GetComponent<ScriptComponent>();
        if (scriptComponent != nullptr && scriptComponent->HasOnUpdate())
        {
            return true;
        }

        if (sceneNode.HasComponent<ParticleSystemComponent>())
        {
            return true;
        }


        return !(sceneNode.GetFlags() & SceneNode::NoUpdate);
    }
    
    void DefaultSceneUpdateManager::AddSceneNode(SceneNode &sceneNode)
    {
        this->sceneNodes.PushBack(&sceneNode);
    }

    void DefaultSceneUpdateManager::RemoveSceneNode(SceneNode &sceneNode)
    {
        this->sceneNodes.RemoveFirstOccuranceOf(&sceneNode);
    }

    void DefaultSceneUpdateManager::Step(double deltaTimeInSeconds, SceneCullingManager &cullingManager)
    {
        for (size_t i = 0; i < this->sceneNodes.count; ++i)
        {
            auto sceneNode = this->sceneNodes[i];
            assert(sceneNode != nullptr);
            {
                this->StepSceneNode(*sceneNode, deltaTimeInSeconds, cullingManager);
            }
        }
    }



    /////////////////////////////////////////////////////////
    // Protected Methods.

    void DefaultSceneUpdateManager::StepSceneNode(SceneNode &node, double deltaTimeInSeconds, SceneCullingManager &cullingManager)
    {
        // If we have an animated model component, that needs to have it's animation stepped.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr && model->IsAnimating() && !model->IsAnimationPaused())
            {
                model->StepAnimation(deltaTimeInSeconds);
                cullingManager.UpdateModelAABB(node);
            }
        }

        // If we have a particle system component that will need to be stepped also.
        auto particleSystemComponent = node.GetComponent<ParticleSystemComponent>();
        if (particleSystemComponent != nullptr)
        {
            if (particleSystemComponent->IsPlaying())
            {
                auto particleSystem = particleSystemComponent->GetParticleSystem();
                if (particleSystem != nullptr)
                {
                    particleSystem->Update(deltaTimeInSeconds);
                    cullingManager.UpdateParticleSystemAABB(node);
                }
            }
        }


        // Now we just let the scene node itself know about it.
        node.OnUpdate(deltaTimeInSeconds);


        // Now we want to update on the scripting environment.
        auto registeredScript = node.GetScene()->GetRegisteredScript();
        if (registeredScript != nullptr)
        {
            auto scriptComponent = node.GetComponent<ScriptComponent>();
            if (scriptComponent != nullptr && scriptComponent->HasOnUpdate())
            {
                Scripting::PostSceneNodeEvent_OnUpdate(*registeredScript, node, deltaTimeInSeconds);
            }
        }
    }
}
