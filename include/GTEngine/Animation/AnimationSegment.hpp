
#ifndef __GTEngine_AnimationSegment_hpp_
#define __GTEngine_AnimationSegment_hpp_

#include <cstdint>

namespace GTEngine
{
    /// Simple struct representing a segment in an animation. It contains a start and end key frame index.
    struct AnimationSegment
    {
        AnimationSegment()
            : startKeyFrame(0), endKeyFrame(0)
        {
        }

        AnimationSegment(size_t startKeyFrame, size_t endKeyFrame)
            : startKeyFrame(startKeyFrame), endKeyFrame(endKeyFrame)
        {
        }

        size_t startKeyFrame;
        size_t endKeyFrame;
    };
}

#endif