
#include <GTEngine/SceneStateStackBranch.hpp>

namespace GTEngine
{
    SceneStateStackBranch::SceneStateStackBranch(SceneStateStackBranch* parentIn, uint32_t parentFrameIndexIn)
        : parent(parentIn), parentFrameIndex(parentFrameIndexIn),
          frames(), currentFrameIndex(0)
    {
    }

    SceneStateStackBranch::~SceneStateStackBranch()
    {
        for (size_t i = 0; i < this->frames.count; ++i)
        {
            delete this->frames[i];
        }
    }
}
