
#ifndef __GTEngine_SceneStateStackStagingArea_hpp_
#define __GTEngine_SceneStateStackStagingArea_hpp_

#include "SceneStateStackRestoreCommands.hpp"

namespace GTEngine
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
              GTCore::Vector<uint64_t> & GetInserts()       { return this->inserts; }
        const GTCore::Vector<uint64_t> & GetInserts() const { return this->inserts; }

        /// Retrieves a reference to the delete commands.
              GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetDeletes()       { return this->deletes; }
        const GTCore::Map<uint64_t, GTCore::BasicSerializer*> & GetDeletes() const { return this->deletes; }

        /// Retrieves a reference to the update commands.
              GTCore::Vector<uint64_t> & GetUpdates()       { return this->updates; }
        const GTCore::Vector<uint64_t> & GetUpdates() const { return this->updates; }



        /// Retrieves the set of commands to use to restore the scene from the changes in the staging area.
        ///
        /// @param commands [out] A reference to the object that will receive the restore commands.
        void GetRestoreCommands(SceneStateStackRestoreCommands &commands);


    private:

        /// The branch that owns this staging area.
        SceneStateStackBranch &branch;


        /// The list of scene node ID's of newly inserted scene nodes in the staging area.
        GTCore::Vector<uint64_t> inserts;

        /// The list of scene node ID's of newly deleted scene nodes in the staging area. We need to keep track of the serialized data
        /// because the scene will want to delete the node, after which point we won't be able to retrieve the data.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> deletes;

        /// The list of scene node ID's of newly updated scene nodes in the staging area.
        GTCore::Vector<uint64_t> updates;
    };
}

#endif
