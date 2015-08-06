// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/Scene.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneState.hpp>

namespace GT
{
    Scene::Scene(SceneState &sceneState)
        : m_idAllocator(1),
          m_pState(&sceneState)
    {
    }


    SceneNode* Scene::CreateSceneNode()
    {
        return m_pState->CreateSceneNode(m_idAllocator.AllocateID());
    }

    void Scene::DeleteSceneNode(SceneNode* pSceneNode)
    {
        m_pState->DeleteSceneNode(pSceneNode);
    }


    bool Scene::InsertSceneNode(SceneNode* pSceneNode)
    {
        if (pSceneNode != nullptr)
        {
            return m_pState->InsertSceneNode(pSceneNode);
        }

        return false;
    }

    void Scene::RemoveSceneNode(SceneNode* pSceneNode)
    {
        if (pSceneNode != nullptr)
        {
            m_pState->RemoveSceneNode(pSceneNode);
        }
    }


    SceneNode* Scene::GetSceneNodeByID(uint64_t sceneNodeID)
    {
        return m_pState->GetSceneNodeByID(sceneNodeID);
    }


    void Scene::SetEventHandler(SceneEventHandler* pEventHandler)
    {
        m_pState->SetEventHandler(pEventHandler);
    }

    SceneEventHandler* Scene::GetEventHandler()
    {
        return m_pState->GetEventHandler();
    }
}
