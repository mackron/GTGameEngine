// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorPrefabLinker::SceneEditorPrefabLinker(SceneEditor &sceneEditorIn)
        : PrefabLinker(), sceneEditor(sceneEditorIn), deserializingSceneNodeData(), lastCreatedSceneNode(nullptr)
    {
    }

    SceneEditorPrefabLinker::~SceneEditorPrefabLinker()
    {
    }


    SceneNode* SceneEditorPrefabLinker::CreateSceneNode()
    {
        this->lastCreatedSceneNode = this->sceneEditor.GetScene().CreateNewSceneNode();

        return this->lastCreatedSceneNode;
    }

    void SceneEditorPrefabLinker::DeleteSceneNode(SceneNode* sceneNode)
    {
        assert(sceneNode != nullptr);

        this->sceneEditor.GetScene().RemoveSceneNode(*sceneNode);
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeStart(SceneNode &sceneNode)
    {
        this->deserializingSceneNodeData.sceneNode   = &sceneNode;
        this->deserializingSceneNodeData.name        = sceneNode.GetName();
        this->deserializingSceneNodeData.wasSelected = this->sceneEditor.IsSceneNodeSelected(sceneNode);

        sceneNode.GetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeEnd(SceneNode &sceneNode)
    {
        assert(this->deserializingSceneNodeData.sceneNode != nullptr);
        {
            // The name needs to be restored, but only if there actually was a name. If the name was empty, we'll just leave it be, which
            // will cause it to be set as defined by the prefab.
            if (!this->deserializingSceneNodeData.name.IsEmpty())
            {
                sceneNode.SetName(this->deserializingSceneNodeData.name.c_str());
            }

            // The world transform needs to be restored, but only if the scene node is the root and not a newly created one.
            bool isNewlyCreatedSceneNode = this->lastCreatedSceneNode == &sceneNode;
            if (!isNewlyCreatedSceneNode)
            {
                if (this->IsRootSceneNode(sceneNode))
                {
                    sceneNode.SetWorldTransformComponents(this->deserializingSceneNodeData.worldPosition, this->deserializingSceneNodeData.worldOrientation, this->deserializingSceneNodeData.worldScale);
                }
            }

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