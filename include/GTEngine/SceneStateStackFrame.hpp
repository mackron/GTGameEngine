
#ifndef __GTEngine_SceneStateStackFrame_hpp_
#define __GTEngine_SceneStateStackFrame_hpp_

#include "SceneStateStackStagingArea.hpp"

namespace GTEngine
{
    class Scene;
    class SceneStateStackBranch;

    class SceneStateStackFrame
    {
    public:

        /// Constructor.
        SceneStateStackFrame(SceneStateStackBranch &branch, const SceneStateStackStagingArea &stagingArea);
        SceneStateStackFrame(SceneStateStackBranch &branch, GTCore::Deserializer &deserializer);

        /// Destructor.
        ~SceneStateStackFrame();


        /// Retrieves a reference to the scene that owns the state stack that subsequently owns this frame.
              Scene & GetScene();
        const Scene & GetScene() const;


        /// Retrieves the serializer for the given scene node, if any.
        ///
        /// @param sceneNodeID [in] The ID of the scene node whose serializer is being retrieved.
        ///
        /// @remarks
        ///     If the scene node is not featured in this frame null will be returned.
        GTCore::BasicSerializer* GetSerializer(uint64_t sceneNodeID) const;

        /// Retrieves the ID of the parent scene node.
        bool GetParentSceneNodeID(uint64_t sceneNodeID, uint64_t &parentSceneNodeIDOut) const;


        /// Retrieves a reference to the internal list of insert commands.
              GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetInserts()       { return this->serializedInserts; }
        const GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetInserts() const { return this->serializedInserts; }

        /// Retrieves a reference to the internal list of delete commands.
              GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetDeletes()       { return this->serializedDeletes; }
        const GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetDeletes() const { return this->serializedDeletes; }

        /// Retrieves a reference to the internal list of update commands.
              GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetUpdates()       { return this->serializedUpdates; }
        const GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetUpdates() const { return this->serializedUpdates; }

        /// Retrieves a reference to the hierarchy map.
              GTCore::Map<uint64_t, uint64_t> & GetHierarchy()       { return this->hierarchy; }
        const GTCore::Map<uint64_t, uint64_t> & GetHierarchy() const { return this->hierarchy; }



        /////////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the state stack frame.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the state stack frame.
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// Serializes the given scene node.
        bool SerializeSceneNode(uint64_t sceneNodeID, GTCore::Serializer &serializer) const;

        /// Clears the frame.
        void Clear();


    private:

        /// A reference to the branch that owns this frame.
        SceneStateStackBranch &branch;

        /// The map containing the serialized data of inserted scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedInserts;

        /// The map containing the serialized data of deleted scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedDeletes;

        /// The map containing the serialized data of updated scene nodes. Indexed by the scene node ID.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> serializedUpdates;

        /// The hierarchy. The key is the child ID and the value is the parent ID. If the node does not have a parent, the value will be 0.
        GTCore::Map<uint64_t, uint64_t> hierarchy;



    private:    // No copying.
        SceneStateStackFrame(const SceneStateStackFrame &);
        SceneStateStackFrame & operator=(const SceneStateStackFrame &);
    };
}

#endif
