// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultPrefabLinker_hpp_
#define __GTEngine_DefaultPrefabLinker_hpp_

#include "PrefabLinker.hpp"

namespace GTEngine
{
    class Scene;

    /// The default prefab linker.
    ///
    /// @remarks
    ///     This linker needs a reference to the owner scene, which is set in the constructor.
    class DefaultPrefabLinker : public PrefabLinker
    {
    public:

        /// Constructor.
        DefaultPrefabLinker(Scene &scene);

        /// Destructor.
        virtual ~DefaultPrefabLinker();



    protected:

        /// PrefabLinker::CreateSceneNode()
        virtual SceneNode* CreateSceneNode();

        /// PrefabLinker::DeleteSceneNode()
        virtual void DeleteSceneNode(SceneNode &sceneNode);

        /// PrefabLinker::OnSceneNodeDeserializeStart()
        virtual void OnSceneNodeDeserializeStart(SceneNode &sceneNode);

        /// PrefabLinker::OnSceneNodeDeserializeEnd()
        virtual void OnSceneNodeDeserializeEnd(SceneNode &sceneNode);



    private:

        /// A reference to the owner scene.
        Scene &scene;

        /// Structure containing information about the scene node that is currently being deserialized.
        struct DeserializingSceneNodeData
        {
            DeserializingSceneNodeData()
                : sceneNode(nullptr), name(),
                  worldPosition(), worldOrientation(), worldScale()
            {
            }

            DeserializingSceneNodeData(const DeserializingSceneNodeData &other)
                : sceneNode(other.sceneNode), name(other.name),
                  worldPosition(other.worldPosition), worldOrientation(other.worldOrientation), worldScale(other.worldScale)
            {
            }

            DeserializingSceneNodeData & operator=(const DeserializingSceneNodeData &other)
            {
                this->sceneNode        = other.sceneNode;
                this->name             = other.name;
                this->worldPosition    = other.worldPosition;
                this->worldOrientation = other.worldOrientation;
                this->worldScale       = other.worldScale;

                return *this;
            }


            /// A pointer to the scene node currently being deserialized.
            SceneNode* sceneNode;

            /// The name of the scene node before deserialization.
            GTLib::String name;

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
        DefaultPrefabLinker(const DefaultPrefabLinker &);
        DefaultPrefabLinker & operator=(const DefaultPrefabLinker &);
    };
}

#endif