
#include <GTEngine/ArmatureAnimation.hpp>

namespace GTEngine
{
    ArmatureAnimation::ArmatureAnimation(const char *nameIn)
        : name(nameIn), durationSeconds(0.0), channels()
    {
    }

    ArmatureAnimation::~ArmatureAnimation()
    {
        for (size_t i = 0; i < this->channels.count; ++i)
        {
            delete this->channels[i];
        }
    }

    ArmatureAnimationChannel* ArmatureAnimation::AddChannel(Bone &bone)
    {
        auto newChannel = new ArmatureAnimationChannel(bone);
        
        this->channels.PushBack(newChannel);

        return newChannel;
    }

    ArmatureAnimationChannel & ArmatureAnimation::GetChannel(size_t index)
    {
        return *this->channels[index];
    }
}