
#include <GTEngine/Animation/AnimationTimeline.hpp>

namespace GTEngine
{
    AnimationTimeline::AnimationTimeline()
        : keyFrames()
    {
    }

    AnimationTimeline::~AnimationTimeline()
    {
    }

    AnimationKeyFrame & AnimationTimeline::AppendKeyFrame(double time)
    {
        auto item = this->keyFrames.Add(time, AnimationKeyFrame(time));
        assert(item != nullptr);

        return item->value;
    }

    AnimationKeyFrame* AnimationTimeline::GetKeyFrame(double time)
    {
        auto item = this->keyFrames.Find(time);
        if (item != nullptr)
        {
            return &item->value;
        }

        return nullptr;
    }


}