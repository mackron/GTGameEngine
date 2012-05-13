
#include <GTEngine/ArmatureAnimation.hpp>

namespace GTEngine
{
    ArmatureAnimation::ArmatureAnimation(const char *nameIn)
        : name(nameIn), durationSeconds(0.0), channels(),
          isPlaying(false), isLooping(true), playbackTime(0.0)
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


    // !!! Playback !!!
    void ArmatureAnimation::Play()
    {
        this->Play(this->isLooping);
    }

    void ArmatureAnimation::Play(bool loop)
    {
        this->isPlaying = true;
        this->isLooping = loop;

        // If this is the first playback, we need to start the playback time from the first key.
        if (!this->hasPlayed)
        {
            this->MoveToFirstKey();
            this->hasPlayed = true;
        }
    }

    void ArmatureAnimation::Stop()
    {
        this->isPlaying = false;
        this->MoveToFirstKey();
    }

    void ArmatureAnimation::Pause()
    {
        this->isPlaying = false;
    }

    void ArmatureAnimation::Step(double deltaSeconds)
    {
        this->playbackTime += deltaSeconds;

        // Here we check our playback time and reset if required.
        if (this->playbackTime > this->durationSeconds)
        {
            if (this->isLooping)
            {
                this->playbackTime -= this->durationSeconds;
            }
            else
            {
                this->playbackTime = this->durationSeconds;
            }
        }

        // We need to update every channel.
        for (size_t i = 0; i < this->channels.count; ++i)
        {
            auto channel = this->channels[i];
            assert(channel != nullptr);

            channel->Update(this->playbackTime);
        }
    }



    void ArmatureAnimation::MoveToFirstKey()
    {
        if (this->channels.count > 0)
        {
            this->playbackTime = this->channels[0]->GetFirstKeyTime();
        }
        else
        {
            this->playbackTime = 0.0;
        }
    }
}