// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorPrefabLinker::SceneEditorPrefabLinker(SceneEditor &sceneEditorIn)
        : PrefabLinker(), sceneEditor(sceneEditorIn)
    {
    }

    SceneEditorPrefabLinker::~SceneEditorPrefabLinker()
    {
    }


    SceneNode* SceneEditorPrefabLinker::CreateSceneNode() const
    {
        return this->sceneEditor.GetScene().CreateNewSceneNode();
    }

    void SceneEditorPrefabLinker::DeleteSceneNode(SceneNode* sceneNode) const
    {
        assert(sceneNode != nullptr);

        this->sceneEditor.GetScene().RemoveSceneNode(*sceneNode);
    }
}