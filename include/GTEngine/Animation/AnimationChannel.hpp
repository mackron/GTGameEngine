
#ifndef __GTEngine_AnimationChannel_hpp_
#define __GTEngine_AnimationChannel_hpp_

#include "AnimationKeyFrame.hpp"
#include "AnimationKey.hpp"
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Class representing a channel of an animation.
    ///
    /// A channel usually represents the state of a bone or object on a timeline. Each different state is represented
    /// with a key. The key is linked to a timeline via a keyframe.
    class AnimationChannel
    {
    public:

        /// Constructor.
        AnimationChannel();

        /// Destructor.
        ~AnimationChannel();


        /// Retrieves the key for the given key frame.
        AnimationKey* GetKey(AnimationKeyFrame &keyframe);

        /// Sets the animation key for the given key frame.
        ///
        /// @param keyframe [in] A reference to the key frame that the key will be associated with.
        /// @param key      [in] A reference to the key.
        void SetKey(AnimationKeyFrame& keyframe, AnimationKey &key);


    private:

        /// The map of keys, each of which are linked to a key frame.
        GTCore::Map<AnimationKeyFrame*, AnimationKey*> keys;
    };
}

#endif