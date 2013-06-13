// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>

namespace GTEngine
{
    SceneEditorPrefabLinker::SceneEditorPrefabLinker()
        : PrefabLinker()
    {
    }

    SceneEditorPrefabLinker::~SceneEditorPrefabLinker()
    {
    }


    SceneNode* SceneEditorPrefabLinker::CreateSceneNode() const
    {
        return nullptr;
    }

    void SceneEditorPrefabLinker::DeleteSceneNode(SceneNode* sceneNode) const
    {
        (void)sceneNode;
    }
}