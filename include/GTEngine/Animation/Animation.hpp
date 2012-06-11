
#ifndef __GTEngine_Animation_hpp_
#define __GTEngine_Animation_hpp_

#include "AnimationTimeline.hpp"
#include "AnimationChannel.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing an animation.
    ///
    /// An animation is made up of a timeline and a bunch of channels that are linked to the timeline.
    class Animation
    {
    public:

        /// Constructor.
        Animation();

        /// Destructor.
        ~Animation();


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


        /// Adds a channel to the animation.
        ///
        /// @return The new channel.
        ///
        /// @remarks
        ///     Delete the channel with DeleteChannel().
        AnimationChannel & CreateChannel();

        /// Removes a channel from the animation.
        ///
        /// @param channel [in] A reference to the channel to delete.
        void DeleteChannel(AnimationChannel &channel);



    private:

        /// The main timeline of the animation.
        AnimationTimeline timeline;

        /// The map of key frames, sorted by time.
        GTCore::Map<double, AnimationKeyFrame> keyFrames;

        /// The list of channels attached to the animation.
        GTCore::Vector<AnimationChannel*> channels;
    };
}

#endif
