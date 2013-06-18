// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorPrefabLinker::SceneEditorPrefabLinker(SceneEditor &sceneEditorIn)
        : PrefabLinker(), sceneEditor(sceneEditorIn), deserializingSceneNodeData()
    {
    }

    SceneEditorPrefabLinker::~SceneEditorPrefabLinker()
    {
    }


    SceneNode* SceneEditorPrefabLinker::CreateSceneNode()
    {
        return this->sceneEditor.GetScene().CreateNewSceneNode();
    }

    void SceneEditorPrefabLinker::DeleteSceneNode(SceneNode* sceneNode)
    {
        assert(sceneNode != nullptr);

        this->sceneEditor.GetScene().RemoveSceneNode(*sceneNode);
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeStart(SceneNode &sceneNode)
    {
        this->deserializingSceneNodeData.sceneNode   = &sceneNode;
        this->deserializingSceneNodeData.wasSelected = this->sceneEditor.IsSceneNodeSelected(sceneNode);

        sceneNode.GetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeEnd(SceneNode &sceneNode)
    {
        assert(this->deserializingSceneNodeData.sceneNode != nullptr);
        {
            // The world transform needs to be restored.
            sceneNode.SetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);

            // We need to make sure the selection flag in the editor metadata is unset.
            auto editorMetadata = sceneNode.GetComponent<EditorMetadataComponent>();
            assert(editorMetadata != nullptr);
            {
                editorMetadata->Deselect();
            }

            if (this->deserializingSceneNodeData.wasSelected)
            {
                this->sceneEditor.SelectSceneNode(sceneNode, 0);
            }
        }

        this->deserializingSceneNodeData.sceneNode = nullptr;
    }
}