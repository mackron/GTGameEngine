// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor/SceneEditor/SceneEditorPrefabLinker.hpp>
#include <GTGE/Editor/SceneEditor/SceneEditor.hpp>

namespace GT
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
            this->sceneEditor.OnPrefabDeserializeStart(sceneNode);
            {
                this->wasSelected = this->sceneEditor.IsSceneNodeSelected(sceneNode);
            }
        }
    }

    void SceneEditorPrefabLinker::OnSceneNodeDeserializeEnd(SceneNode &sceneNode)
    {
        DefaultPrefabLinker::OnSceneNodeDeserializeEnd(sceneNode);
        {
            this->sceneEditor.OnPrefabDeserializeEnd(sceneNode);
            {
                // We need to make sure the selection flag in the editor metadata is unset.
                auto editorMetadata = sceneNode.GetComponent<EditorMetadataComponent>();
                assert(editorMetadata != nullptr);
                {
                    editorMetadata->Deselect();
                }

                if (this->wasSelected)
                {
                    // The selection hasn't actually changed, we just simulate a re-selection so that things like the outline meshes are updated correctly.
                    this->sceneEditor.SelectSceneNode(sceneNode, SceneEditor::SelectionOption_Force | SceneEditor::SelectionOption_NoScriptNotify);
                }
            }
        }
    }
}
