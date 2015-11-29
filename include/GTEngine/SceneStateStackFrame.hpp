// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneStateStackFrame_hpp_
#define __GTEngine_SceneStateStackFrame_hpp_

#include "SceneStateStackStagingArea.hpp"

namespace GT
{
    class Scene;
    class SceneStateStackBranch;

    class SceneStateStackFrame
    {
    public:

        /// Constructor.
        SceneStateStackFrame(SceneStateStackBranch &branch, const SceneStateStackStagingArea &stagingArea);
        SceneStateStackFrame(SceneStateStackBranch &branch, Deserializer &deserializer);

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
        BasicSerializer* GetSerializer(uint64_t sceneNodeID) const;

        /// Retrieves the ID of the parent scene node.
        bool GetParentSceneNodeID(uint64_t sceneNodeID, uint64_t &parentSceneNodeIDOut) const;


        /// Retrieves a reference to the internal list of insert commands.
              Map<uint64_t, BasicSerializer*> & GetInserts()       { return this->serializedInserts; }
        const Map<uint64_t, BasicSerializer*> & GetInserts() const { return this->serializedInserts; }

        /// Retrieves a reference to the internal list of delete commands.
              Map<uint64_t, BasicSerializer*> & GetDeletes()       { return this->serializedDeletes; }
        const Map<uint64_t, BasicSerializer*> & GetDeletes() const { return this->serializedDeletes; }

        /// Retrieves a reference to the internal list of update commands.
              Map<uint64_t, BasicSerializer*> & GetUpdates()       { return this->serializedUpdates; }
        const Map<uint64_t, BasicSerializer*> & GetUpdates() const { return this->serializedUpdates; }

        /// Retrieves a reference to the hierarchy map.
              Map<uint64_t, uint64_t> & GetHierarchy()       { return this->hierarchy; }
        const Map<uint64_t, uint64_t> & GetHierarchy() const { return this->hierarchy; }



        /////////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the state stack frame.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the state stack frame.
        void Deserialize(Deserializer &deserializer);



    private:

        /// Serializes the given scene node.
        bool SerializeSceneNode(uint64_t sceneNodeID, Serializer &serializer) const;

        /// Clears the frame.
        void Clear();


    private:

        /// A reference to the branch that owns this frame.
        SceneStateStackBranch &branch;

        /// The map containing the serialized data of inserted scene nodes. Indexed by the scene node ID.
        Map<uint64_t, BasicSerializer*> serializedInserts;

        /// The map containing the serialized data of deleted scene nodes. Indexed by the scene node ID.
        Map<uint64_t, BasicSerializer*> serializedDeletes;

        /// The map containing the serialized data of updated scene nodes. Indexed by the scene node ID.
        Map<uint64_t, BasicSerializer*> serializedUpdates;

        /// The hierarchy. The key is the child ID and the value is the parent ID. If the node does not have a parent, the value will be 0.
        Map<uint64_t, uint64_t> hierarchy;



    private:    // No copying.
        SceneStateStackFrame(const SceneStateStackFrame &);
        SceneStateStackFrame & operator=(const SceneStateStackFrame &);
    };
}

#endif
