// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Editor_SceneEditorPrefabLinker_hpp_
#define __GTEngine_Editor_SceneEditorPrefabLinker_hpp_

#include "../../PrefabLinker.hpp"

namespace GTEngine
{
    class SceneEditor;

    class SceneEditorPrefabLinker : public PrefabLinker
    {
    public:

        /// Constructor.
        SceneEditorPrefabLinker(SceneEditor &sceneEditor);

        /// Destructor.
        ~SceneEditorPrefabLinker();


    private:

        /// PrefabLinker::CreateSceneNode()
        SceneNode* CreateSceneNode();

        /// PrefabLinker::DeleteSceneNode()
        void DeleteSceneNode(SceneNode* sceneNode);

        /// PrefabLinker::OnSceneNodeDeserializeStart()
        void OnSceneNodeDeserializeStart(SceneNode &sceneNode);

        /// PrefabLinker::OnSceneNodeDeserializeEnd()
        void OnSceneNodeDeserializeEnd(SceneNode &sceneNode);


    private:

        /// A reference to the scene editor that owns this linker.
        SceneEditor &sceneEditor;


        /// Structure containing information about the scene node that is currently being deserialized.
        struct DeserializingSceneNodeData
        {
            DeserializingSceneNodeData()
                : sceneNode(nullptr), wasSelected(false), name(),
                  worldPosition(), worldOrientation(), worldScale()
            {
            }

            DeserializingSceneNodeData(const DeserializingSceneNodeData &other)
                : sceneNode(other.sceneNode), wasSelected(other.wasSelected), name(other.name),
                  worldPosition(other.worldPosition), worldOrientation(other.worldOrientation), worldScale(other.worldScale)
            {
            }

            DeserializingSceneNodeData & operator=(const DeserializingSceneNodeData &other)
            {
                this->sceneNode        = other.sceneNode;
                this->wasSelected      = other.wasSelected;
                this->name             = other.name;
                this->worldPosition    = other.worldPosition;
                this->worldOrientation = other.worldOrientation;
                this->worldScale       = other.worldScale;

                return *this;
            }


            /// A pointer to the scene node currently being deserialized.
            SceneNode* sceneNode;

            /// Keeps track of whether or not the scene was selected before deserializing.
            bool wasSelected;

            /// The name of the scene node before deserialization.
            GTCore::String name;

            /// The world position of the scene node before deserialization.
            glm::vec3 worldPosition;

            /// The world orientation of the scene node before deserialization.
            glm::quat worldOrientation;

            /// The worls scale of the scene node before deserialization.
            glm::vec3 worldScale;

        }deserializingSceneNodeData;

        /// A pointer to the last scene node that was created. This is used by the deserialization event handlers
        /// so that they can determine whether or not the deserialization is happening for a newly created scene
        /// node or not.
        SceneNode* lastCreatedSceneNode;


    private:    // No copying.
        SceneEditorPrefabLinker(const SceneEditorPrefabLinker &);
        SceneEditorPrefabLinker & operator=(const SceneEditorPrefabLinker &);
    };
}

#endif