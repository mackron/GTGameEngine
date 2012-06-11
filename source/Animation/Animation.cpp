
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
