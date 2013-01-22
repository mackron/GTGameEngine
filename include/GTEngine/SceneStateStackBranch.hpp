
#ifndef __GTEngine_SceneStateStackBranch_hpp_
#define __GTEngine_SceneStateStackBranch_hpp_

#include "SceneStateStackFrame.hpp"
#include <GTCore/Vector.hpp>
//#include <cstdint>

namespace GTEngine
{
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
        SceneStateStackBranch(SceneStateStackBranch* parent, uint32_t rootFrameIndex);

        /// Destructor.
        ~SceneStateStackBranch();


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


        /// Appends a new frame to the branch at the position just after the current frame index.
        ///
        /// @remarks
        ///     The new frame will become the current one.
        ///     @par
        ///     Everything coming after the current frame will be deleted, including child branches.
        void AppendFrame();



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
    };
}

#endif
