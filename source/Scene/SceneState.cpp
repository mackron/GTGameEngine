// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneState.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>

namespace GT
{
    SceneState::SceneState()
    {
    }

    SceneState::~SceneState()
    {
    }


    void SceneState::DeleteSceneNode(SceneNode* pSceneNode)
    {
        if (pSceneNode != nullptr)
        {
            this->DeleteSceneNode(pSceneNode->GetID());
        }
    }
}
