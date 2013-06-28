// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditorPrefabLinker::SceneEditorPrefabLinker(Scene &scene, SceneEditor &sceneEditorIn)
        : DefaultPrefabLinker(scene), sceneEditor(sceneEditorIn), wasSelected(false)
    {
    }

    SceneEditorPrefabLinker::~SceneEditorPrefabLinker()
    {
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeStart(SceneNode &sceneNode)
    {
        DefaultPrefabLinker::OnSceneNodeDeserializeStart(sceneNode);
        {
            this->wasSelected = this->sceneEditor.IsSceneNodeSelected(sceneNode);
        }
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeEnd(SceneNode &sceneNode)
    {
        DefaultPrefabLinker::OnSceneNodeDeserializeEnd(sceneNode);
        {
            // We need to make sure the selection flag in the editor metadata is unset.
            auto editorMetadata = sceneNode.GetComponent<EditorMetadataComponent>();
            assert(editorMetadata != nullptr);
            {
                editorMetadata->Deselect();
            }

            if (this->wasSelected)
            {
                this->sceneEditor.SelectSceneNode(sceneNode, SceneEditor::SelectionOption_Force);
            }
        }
    }
}