
#include <GTEngine/SkeletalAnimation.hpp>

namespace GTEngine
{
    SkeletalAnimation::SkeletalAnimation(const char *nameIn)
        : name(nameIn), durationSeconds(0.0), channels(),
          isPlaying(false), isLooping(true), playbackTime(0.0)
    {
    }

    SkeletalAnimation::~SkeletalAnimation()
    {
        for (size_t i = 0; i < this->channels.count; ++i)
        {
            delete this->channels[i];
        }
    }

    SkeletalAnimationChannel* SkeletalAnimation::AddChannel(Bone &bone)
    {
        auto newChannel = new SkeletalAnimationChannel(bone);
        
        this->channels.PushBack(newChannel);

        return newChannel;
    }

    SkeletalAnimationChannel & SkeletalAnimation::GetChannel(size_t index)
    {
        return *this->channels[index];
    }


    // !!! Playback !!!
    void SkeletalAnimation::Play()
    {
        this->Play(this->isLooping);
    }

    void SkeletalAnimation::Play(bool loop)
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

    void SkeletalAnimation::Stop()
    {
        this->isPlaying = false;
        this->MoveToFirstKey();
    }

    void SkeletalAnimation::Pause()
    {
        this->isPlaying = false;
    }

    void SkeletalAnimation::Step(double deltaSeconds)
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

        // Now we need to loop over each channel again and update the skinning transformations. It's important that this is done separately from the
        // loop above to ensure all dependants have been updated beforehand.
        for (size_t i = 0; i < this->channels.count; ++i)
        {
            auto channel = this->channels[i];
            assert(channel != nullptr);

            channel->GetBone().UpdateSkinningTransform();
        }
    }



    void SkeletalAnimation::MoveToFirstKey()
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