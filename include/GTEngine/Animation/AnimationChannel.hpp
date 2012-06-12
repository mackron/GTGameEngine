
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
        ///
        /// @param keyFrameIndex [in] The index of the key frame whose animation key is being retrieved.
        ///
        /// @return A pointer to the animation key at the given time.
        AnimationKey* GetKey(size_t keyFrameIndex);

        /// Sets the animation key for the given key frame.
        ///
        /// @param keyFrameIndex [in] A reference to the key frame that the key will be associated with.
        /// @param key           [in] A reference to the key.
        void SetKey(size_t keyFrameIndex, AnimationKey* key);

        
        /// Retrieves a constant reference to the internal key map.
        const GTCore::Map<size_t, AnimationKey*> & GetKeys() const { return this->keys; }


    private:

        /// The map of keys, each of which are linked to a key frame by the key frame's index.
        GTCore::Map<size_t, AnimationKey*> keys;
    };
}

#endif