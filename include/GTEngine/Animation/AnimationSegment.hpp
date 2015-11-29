// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AnimationSegment
#define GT_AnimationSegment

#include <cstdint>
#include <GTEngine/Core/String.hpp>

namespace GT
{
    /// Simple struct representing a segment in an animation. It contains a start and end key frame index.
    struct AnimationSegment
    {
        AnimationSegment()
            : name(), startKeyFrame(0), endKeyFrame(0)
        {
        }

        AnimationSegment(size_t startKeyFrame, size_t endKeyFrame)
            : name(), startKeyFrame(startKeyFrame), endKeyFrame(endKeyFrame)
        {
        }

        AnimationSegment(const char* nameIn)
            : name(nameIn), startKeyFrame(0), endKeyFrame(0)
        {
        }

        AnimationSegment(const char* nameIn, size_t startKeyFrameIn, size_t endKeyFrameIn)
            : name(nameIn), startKeyFrame(startKeyFrameIn), endKeyFrame(endKeyFrameIn)
        {
        }




        /// The name of the animation segment.
        String name;

        /// The first key frame of the segment.
        size_t startKeyFrame;

        /// The end last frame of the segment.
        size_t endKeyFrame;
    };
}

#endif