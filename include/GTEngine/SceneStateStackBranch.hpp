
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
    /// A branch is created at a frame of another branch. 
    class SceneStateStackBranch
    {
    public:

        /// Constructor.
        SceneStateStackBranch(SceneStateStackBranch* parent, uint32_t parentFrameIndex);

        /// Destructor.
        ~SceneStateStackBranch();


        /// Retrieves a pointer to the parent branch.
              SceneStateStackBranch* GetParent()       { return this->parent; };
        const SceneStateStackBranch* GetParent() const { return this->parent; };


        /// Retrieves the index of the frame of the parent branch that this branch begins from.
        uint32_t GetParentFrameIndex() const { return this->parentFrameIndex; }


        /// Retrieves the index of the current frame.
        uint32_t GetCurrentFrameIndex() const { return this->currentFrameIndex; }


    private:

        /// The parent branch. This will be null if the branch does not have a parent (the master branch).
        SceneStateStackBranch* parent;

        /// The index of the frame of the parent branch that this branch begins from.
        uint32_t parentFrameIndex;


        /// The flat list of frames belonging to this branch.
        GTCore::Vector<SceneStateStackFrame*> frames;

        /// The index of the current frame.
        uint32_t currentFrameIndex;
    };
}

#endif
