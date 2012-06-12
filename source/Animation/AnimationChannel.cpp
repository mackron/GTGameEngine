
#include <GTEngine/Animation/AnimationChannel.hpp>

namespace GTEngine
{
    AnimationChannel::AnimationChannel()
        : keys()
    {
    }

    AnimationChannel::~AnimationChannel()
    {
    }

    AnimationKey* AnimationChannel::GetKey(size_t keyframe)
    {
        auto item = this->keys.Find(keyframe);
        if (item != nullptr)
        {
            return item->value;
        }

        return nullptr;
    }

    void AnimationChannel::SetKey(size_t keyframe, AnimationKey* key)
    {
        this->keys.Add(keyframe, key);
    }
}
