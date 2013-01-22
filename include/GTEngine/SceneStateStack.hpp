
#ifndef __GTEngine_SceneStateStack_hpp_
#define __GTEngine_SceneStateStack_hpp_

#include "SceneStateStackBranch.hpp"
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Class representing the state stack of a scene.
    ///
    /// The state stack is used to efficiently keep track of the state of a scene at different points in time. It is used for doing
    /// things like undo/redo, replays, saving, etc.
    ///
    /// The state stack is made up of branches. Each branch has a parent, except for the master branch.
    class SceneStateStack
    {
    public:

        /// Constructor.
        SceneStateStack();

        /// Destructor.
        ~SceneStateStack();


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



    private:

        /// Retrieves a unique ID for a new branch.
        uint32_t GetNewBranchID() const;



    private:

        /// The list of branches. We use a map here because we want consistent branch IDs due to serialization.
        GTCore::Map<uint32_t, SceneStateStackBranch*> branches;

        /// A pointer to the current branch.
        SceneStateStackBranch* currentBranch;
    };
}

#endif