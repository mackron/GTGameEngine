// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneStateStackStagingArea_hpp_
#define __GTEngine_SceneStateStackStagingArea_hpp_

#include "SceneStateStackRestoreCommands.hpp"
#include "Serialization.hpp"

namespace GT
{
    class Scene;
    class SceneStateStackBranch;

    /// Class representing the staging area.
    class SceneStateStackStagingArea
    {
    public:

        /// Constructor.
        SceneStateStackStagingArea(SceneStateStackBranch &branch);

        /// Destructor.
        ~SceneStateStackStagingArea();


        /// Retrieves a reference to the branch that owns this staging area.
              SceneStateStackBranch & GetBranch()       { return this->branch; }
        const SceneStateStackBranch & GetBranch() const { return this->branch; }

        /// Retrieves a reference to the relevant scene.
              Scene & GetScene();
        const Scene & GetScene() const;



        /// Stages an insert command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was inserted.
        void StageInsert(uint64_t sceneNodeID);

        /// Stages a delete command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was deleted.
        void StageDelete(uint64_t sceneNodeID);

        /// Stages an update command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was updated.
        void StageUpdate(uint64_t sceneNodeID);


        /// Clears the staging area.
        void Clear();


        /// Retrieves a reference to the insert commands.
              Vector<uint64_t> & GetInserts()       { return this->inserts; }
        const Vector<uint64_t> & GetInserts() const { return this->inserts; }

        /// Retrieves a reference to the delete commands.
              Map<uint64_t, BasicSerializer*> & GetDeletes()       { return this->deletes; }
        const Map<uint64_t, BasicSerializer*> & GetDeletes() const { return this->deletes; }

        /// Retrieves a reference to the update commands.
              Vector<uint64_t> & GetUpdates()       { return this->updates; }
        const Vector<uint64_t> & GetUpdates() const { return this->updates; }

        /// Retrieves a reference to the hierarchy.
              Map<uint64_t, uint64_t> & GetHierarchy()       { return this->hierarchy; }
        const Map<uint64_t, uint64_t> & GetHierarchy() const { return this->hierarchy; }



        /// Retrieves the set of commands to use to revert the scene from the changes in the staging area.
        ///
        /// @param commands [out] A reference to the object that will receive the restore commands.
        void GetRevertCommands(SceneStateStackRestoreCommands &commands);

        /// Retrieves a set of commands that can be used to put the scene into a state as currently defined by the staging area.
        ///
        /// @param commands [out] A reference to the object that will receive the restore commands.
        void GetRestoreCommands(SceneStateStackRestoreCommands &commands);



        /////////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the state stack staging area.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the state stack staging area.
        void Deserialize(Deserializer &deserializer);



    private:

        /// Adds the given scene node to the hierarchy.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that is being added to the hierarchy.
        ///
        /// @remarks
        ///     This does nothing if the scene node does not have a parent.
        void AddToHierarchy(uint64_t sceneNodeID);

        /// Removes the given scene node from the hierarchy.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that is being removed from the hierarchy.
        void RemoveFromHierarchy(uint64_t sceneNodeID);

        /// Retrieves the ID of the parent node from the hierarchy, or 0 if it does not have a parent.
        ///
        /// @param childSceneNodeID [in] The ID of the scene node whose parent ID is being retrieved.
        uint64_t GetParentSceneNodeIDFromHierarchy(uint64_t childSceneNodeID) const;



    private:

        /// The branch that owns this staging area.
        SceneStateStackBranch &branch;


        /// The list of scene node ID's of newly inserted scene nodes in the staging area.
        Vector<uint64_t> inserts;

        /// The list of scene node ID's of newly deleted scene nodes in the staging area. We need to keep track of the serialized data
        /// because the scene will want to delete the node, after which point we won't be able to retrieve the data.
        Map<uint64_t, BasicSerializer*> deletes;

        /// The list of scene node ID's of newly updated scene nodes in the staging area.
        Vector<uint64_t> updates;

        /// The hierarchy of the nodes containined in the staging area. The key is the child node ID and the value is the parent node ID.
        Map<uint64_t, uint64_t> hierarchy;
    };
}

#endif
