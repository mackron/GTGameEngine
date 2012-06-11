
#include <GTEngine/Animation/Animation.hpp>

namespace GTEngine
{
    Animation::Animation()
        : timeline(), channels()
    {
    }

    Animation::~Animation()
    {
    }

    AnimationKeyFrame & Animation::AppendKeyFrame(double time)
    {
        auto item = this->keyFrames.Add(time, AnimationKeyFrame(time));
        assert(item != nullptr);

        return item->value;
    }

    AnimationKeyFrame* Animation::GetKeyFrame(double time)
    {
        auto item = this->keyFrames.Find(time);
        if (item != nullptr)
        {
            return &item->value;
        }

        return nullptr;
    }

    AnimationChannel & Animation::CreateChannel()
    {
        auto newChannel = new AnimationChannel;
        this->channels.PushBack(newChannel);

        return *newChannel;
    }

    void Animation::DeleteChannel(AnimationChannel &channel)
    {
        this->channels.RemoveFirst(&channel);
        delete &channel;
    }
}
