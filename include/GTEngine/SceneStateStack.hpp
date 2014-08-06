// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneStateStack_hpp_
#define __GTEngine_SceneStateStack_hpp_

#include "SceneStateStackBranch.hpp"
#include <GTLib/Map.hpp>

namespace GTEngine
{
    class Scene;

    /// Class representing the state stack of a scene.
    ///
    /// The state stack is used to efficiently keep track of the state of a scene at different points in time. It is used for doing
    /// things like undo/redo, replays, saving, etc.
    ///
    /// The state stack works in a very similar way to the Git version constrol system. In fact, many of the same concepts are in
    /// place here.
    ///
    /// The state stack is made up of branches. Each branch has a parent, except for the master branch. Each branch has a staging area
    /// which defines the things that will be added to the next "commit" or "frame", as we call it. When the changes are compiled and
    /// ready to be commited, you just call Commit(). All operations operate on the current branch.
    ///
    /// With the staging area, there are three kinds of operations: insert, delete and update. No scene node pointers are actually
    /// used here. Instead, the ID of the scene node is used. An important detail is that when a scene node is staged, it's state is
    /// not saved until the commit is actually performed.
    class SceneStateStack
    {
    public:

        /// Constructor.
        SceneStateStack(Scene &scene);

        /// Destructor.
        ~SceneStateStack();


        /// Retrieves a reference to the scene this state stack belongs to.
              Scene & GetScene()       { return this->scene; }
        const Scene & GetScene() const { return this->scene; }


        /// Creates a new branch at the current branch and frame but does not switch to it.
        ///
        /// @return The ID of the new branch.
        ///
        /// @remarks
        ///     To switch to the new branch, pass the return value to SwitchBranch().
        uint32_t CreateBranch();

        /// Switches to the given branch.
        ///
        /// @param branchID [in] The ID of the branch to switch to.
        ///
        /// @return True if the branch was switched; false otherwise. This will only fail if the branch does not exist.
        bool SwitchBranch(uint32_t branchID);

        /// Retrieves a pointer to the current branch.
              SceneStateStackBranch* GetCurrentBranch()       { return this->currentBranch; }
        const SceneStateStackBranch* GetCurrentBranch() const { return this->currentBranch; }


        /// Stages an insert command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was inserted.
        void StageInsert(uint64_t sceneNodeID);

        /// Stages a delete command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was deleted.
        ///
        /// @remarks
        ///     This should be called before the scene node is actually removed from the scene.
        void StageDelete(uint64_t sceneNodeID);

        /// Stages an update command of the given scene node.
        ///
        /// @param sceneNodeID [in] The ID of the scene node that was updated.
        void StageUpdate(uint64_t sceneNodeID);


        /// Commits the changes by creating a new frame on the current branch.
        ///
        /// @remarks
        ///     This will remove all frames after the current frame before appending the new frame, including child branches that are rooted after it.
        ///     @par
        ///     The new frame will become the current one.
        ///     @par
        ///     If the current frame is not local to the current branch, it will be re-parentend such that the parent frame will be changed the current
        ///     frame. No branches will be deleted.
        void Commit();


        /// Determines whether or not an initial commit has been performed.
        ///
        /// @return True if the initial commit has been performed; false otherwise.
        bool HasInitialFrame() const;


        /// Seeks the current branch by the given amount.
        ///
        /// @param step [in] The distance to seek. Can be positive or negative.
        void Seek(int step);


        /// Reverts the staging area of the current branch.
        void RevertStagingArea();

        /// Clears the staging area (does not revert anything).
        void ClearStagingArea();


        /// Applies the current state of the stack to the scene.
        void ApplyToScene();


        /// Clears the stack.
        void Clear();



        /// Retrieves the scene node serialization flags.
        ///
        /// @return The flags to use when serializing scene nodes.
        unsigned int GetSceneNodeSerializationFlags() const;

        /// Sets the scene node serialization flags.
        ///
        /// @param flags [in] The new serialization flags to use when serializing scene nodes.
        void SetSceneNodeSerializationFlags(unsigned int flags);

        /// Retrieves the scene node deserialization flags.
        ///
        /// @return The flags to use when deserializing scene nodes.
        unsigned int GetSceneNodeDeserializationFlags() const;

        /// Sets the scene node deserialization flags.
        ///
        /// @param flags [in] The new deserialization flags to use when deserializing scene nodes.
        void SetSceneNodeDeserializationFlags(unsigned int flags);



        /////////////////////////////////////////////////
        // Serialization/Deserialization

        /// Serializes the state stack.
        void Serialize(GTLib::Serializer &serializer) const;

        /// Deserializes the state stack.
        void Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// Retrieves a unique ID for a new branch.
        uint32_t GetNewBranchID() const;

        /// Retrieves the ID of the given branch.
        ///
        /// @remarks
        ///     This runs in linear time.
        uint32_t GetBranchID(const SceneStateStackBranch &branch) const;

        /// Retrieves a pointer to the branch by the ID.
        SceneStateStackBranch* GetBranchByID(uint32_t branchID) const;



    private:

        /// A reference to the scene this state stack belongs to.
        Scene &scene;

        /// The list of branches. We use a map here because we want consistent branch IDs due to serialization.
        GTLib::Map<uint32_t, SceneStateStackBranch*> branches;

        /// A pointer to the master branch.
        SceneStateStackBranch masterBranch;

        /// A pointer to the current branch.
        SceneStateStackBranch* currentBranch;


        /// Serialization flags for scene nodes. Note, this is not used for the serialization of the stack itself, but for scene node serialization.
        unsigned int sceneNodeSerializationFlags;

        /// Deserialization flags for scene nodes. Note, this is not used for the deserialization of the stack itself, but for scene node deserialization.
        unsigned int sceneNodeDeserializationFlags;


    private:    // No copying.
        SceneStateStack(const SceneStateStack &);
        SceneStateStack & operator=(const SceneStateStack &);
    };
}

#endif