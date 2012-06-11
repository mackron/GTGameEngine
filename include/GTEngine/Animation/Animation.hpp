
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

        /// The list of channels attached to the animation.
        GTCore::Vector<AnimationChannel*> channels;
    };
}

#endif
