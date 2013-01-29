
#ifndef __GTEngine_SceneStateStackBranch_hpp_
#define __GTEngine_SceneStateStackBranch_hpp_

#include "SceneStateStackFrame.hpp"
#include "SceneStateStackStagingArea.hpp"
#include "SceneStateStackRestoreCommands.hpp"

namespace GTEngine
{
    class Scene;
    class SceneStateStack;

    /// Class representing a branch on the scene stack.
    ///
    /// A branch is usually a child to another branch, except for the master branch. A branch can also, of course, have multiple children.
    ///
    /// A branch is created at a frame of another branch. Each branch has it's own set of frames, so a frame index on one brach will point
    /// to a different frame for another branch.
    class SceneStateStackBranch
    {
    public:

        /// Constructor.
        SceneStateStackBranch(SceneStateStack &stateStack, SceneStateStackBranch* parent, uint32_t rootFrameIndex);

        /// Destructor.
        ~SceneStateStackBranch();


        /// Retrieves a reference to the state stack that owns this branch.
              SceneStateStack & GetStateStack()       { return this->stateStack; }
        const SceneStateStack & GetStateStack() const { return this->stateStack; }

        /// Retrieves a reference to the scene that owns the state stack that subsequently owns this branch.
              Scene & GetScene();
        const Scene & GetScene() const;



        /// Retrieves a pointer to the parent branch.
              SceneStateStackBranch* GetParent()       { return this->parent; };
        const SceneStateStackBranch* GetParent() const { return this->parent; };

        /// Re-parents the branch.
        ///
        /// @param newParent         [in] A pointer to the new parent branch.
        /// @param newRootFrameIndex [in] The new root frame index.
        ///
        /// @remarks
        ///     Re-parenting causes the local frames and every child branch to be deleted. The reason is that with a change in parent, the data
        ///     contained in any existing frames will have become invalid.
        void SetParent(SceneStateStackBranch* newParent, uint32_t newRootFrameIndex);


        /// Retrieves the index of the frame this branch is rooted at.
        uint32_t GetRootFrameIndex() const { return this->rootFrameIndex; }

        /// Sets the root frame index.
        void SetRootFrameIndex(uint32_t newRootFrameIndex) { this->rootFrameIndex = newRootFrameIndex; }


        /// Retrieves the index of the current frame.
        uint32_t GetCurrentFrameIndex() const { return this->currentFrameIndex; }

        /// Retrieves the maximum frame index.
        uint32_t GetMaxFrameIndex() const;

        /// Retrieves the number of local frames.
        size_t GetLocalFrameCount() const { return this->frames.count; }

        /// Retrieves the total number of frames on the branch.
        size_t GetTotalFrameCount() const;

        /// Retrieves the frame at the given index.
        ///
        /// @param index [in] The index of the frame to retrieve.
        SceneStateStackFrame* GetFrameAtIndex(uint32_t index) const;



        /// Creates and appends a child branch at the current frame index.
        ///
        /// @remarks
        ///     This method asserts that the current frame index is pointing to a local frame.
        SceneStateStackBranch* CreateBranch();

        /// Deletes a child branch.
        ///
        /// @param childBranch [in] The child branch to delete.
        void DeleteBranch(SceneStateStackBranch* childBranch);

        /// Removes all child branches.
        void DeleteAllBranches();


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
        void ClearStagingArea();


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


        /// Seeks the current frame by the given amount.
        ///
        /// @param step [in] The seek distance. Can be positive or negative.
        void Seek(int step);

        /// Reverts the staging area.
        void RevertStagingArea();

        /// Applies the state defined at the current frame to the scene.
        void ApplyToScene();

        /// Retrieves the commands that should be used to restore the scene to the given frame index.
        ///
        /// @remarks
        ///     This ignores the staging area.
        void GetRestoreCommands(uint32_t newFrameIndex, SceneStateStackRestoreCommands &commands);


        /// Finds the most recent serializer for the given scene node starting from the current frame.
        ///
        /// @param sceneNodeID     [in] The ID of the scene node whose most recent serializer is being retrieved.
        /// @param startFrameIndex [in] The index of the frame to start at.
        GTCore::BasicSerializer* FindMostRecentSerializer(uint64_t sceneNodeID, uint32_t startFrameIndex) const;


        /////////////////////////////////////////////
        // Internal Use Only

        /// Deletes a child branch by it's index.
        ///
        /// @param childIndex [in] The index of the child to remove.
        void _DeleteBranchByIndex(size_t childIndex);


    private:

        /// Recursively finds the branch where the given frame index is local.
        SceneStateStackBranch* FindBranch(uint32_t frameIndex);


    private:

        /// The state stack that owns this branch.
        SceneStateStack &stateStack;


        /// The parent branch. This will be null if the branch does not have a parent (the master branch).
        SceneStateStackBranch* parent;

        /// The list of children.
        GTCore::Vector<SceneStateStackBranch*> children;

        /// The index of the frame of the parent branch that this branch begins from.
        uint32_t rootFrameIndex;


        /// The flat list of frames belonging to this branch.
        GTCore::Vector<SceneStateStackFrame*> frames;

        /// The index of the current frame.
        uint32_t currentFrameIndex;


        /// The staging area.
        SceneStateStackStagingArea stagingArea;



    private:    // No copying.
        SceneStateStackBranch(const SceneStateStackBranch &);
        SceneStateStackBranch & operator=(const SceneStateStackBranch &);
    };
}

#endif
