
#ifndef __GTEngine_AnimationTimeline_hpp_
#define __GTEngine_AnimationTimeline_hpp_

#include "AnimationKeyFrame.hpp"
#include "AnimationChannel.hpp"
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Class representing the timeline of an animation.
    ///
    /// A timeline is made up of key frames. Each channel in the animation has a key for each of those key frames.
    class AnimationTimeline
    {
    public:

        /// Constructor.
        AnimationTimeline();

        /// Destructor.
        ~AnimationTimeline();


        /// Appends a new key frame.
        ///
        /// @param time [in] The time in the timeline to append the keyframe.
        ///
        /// @return A reference to the new key frame.
        ///
        /// @remarks
        ///     If a key frame at the same time already exists, this function will return the existing frame.
        AnimationKeyFrame & AppendKeyFrame(double time);

        /// Retrieves the key frame at the given time.
        ///
        /// @param time [in] The time of the key frame.
        ///
        /// @return A pointer to the key frame at the given time.
        AnimationKeyFrame* GetKeyFrame(double time);


        /// Retrieves the number of key frames in the timeline.
        size_t GetKeyFrameCount() const { return this->keyFrames.count; }

        

    private:

        /// The map of key frames, sorted by time.
        GTCore::Map<double, AnimationKeyFrame> keyFrames;
    };
}

#endif