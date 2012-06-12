
#include <GTEngine/Animation/Animation.hpp>

namespace GTEngine
{
    Animation::Animation()
        : keyFrames(), channels(),
          segments(), keyFrameQueue(),
          isPlaying(false), playbackTime(0.0),
          loopStartQueueIndex(static_cast<size_t>(-1))
    {
    }

    Animation::~Animation()
    {
    }


    size_t Animation::AppendKeyFrame(double time)
    {
        auto item = this->keyFrames.Add(time, AnimationKeyFrame(time));
        assert(item != nullptr);

        for (size_t i = 0; i < this->keyFrames.count; ++i)
        {
            if (this->keyFrames.buffer[i]->key == time)
            {
                return i;
            }
        }

        return 0;
    }

    double Animation::GetKeyFrameTimeByIndex(size_t index)
    {
        assert(index < this->keyFrames.count);

        return this->keyFrames.buffer[index]->value.GetTime();
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

    void Animation::AddNamedSegment(const char* name, size_t startKeyFrame, size_t endKeyFrame)
    {
        this->segments.Add(name, AnimationSegment(startKeyFrame, endKeyFrame));
    }

    AnimationSegment* Animation::GetNamedSegment(const char* name)
    {
        auto iSegment = this->segments.Find(name);
        if (iSegment != nullptr)
        {
            return &iSegment->value;
        }

        return nullptr;
    }
    const AnimationSegment* Animation::GetNamedSegment(const char* name) const
    {
        auto iSegment = this->segments.Find(name);
        if (iSegment != nullptr)
        {
            return &iSegment->value;
        }

        return nullptr;
    }




    ////////////////////////////////////////////////////////////////////////
    // Playback.

    void Animation::Play(const AnimationSequence &sequence)
    {
        // First we need to make sure our queue is reset.
        this->keyFrameQueue.RemoveAll();
     
        // If we don't have any key frames, we won't bother doing anything.
        if (this->keyFrames.count > 0)
        {
            // We loop through each frame in the sequence an then add them to the queue.
            for (size_t i = 0U; i < sequence.GetFrameCount(); ++i)
            {
                auto frame = sequence.GetFrame(i);
                if (frame != nullptr)
                {
                    size_t keyFrameStart = frame->startKeyFrame;
                    size_t keyFrameEnd   = frame->endKeyFrame;
                    if (frame->segmentName.GetLength() > 0)
                    {
                        // We're looking at a named segment.
                        auto iSegment = this->segments.Find(frame->segmentName.c_str());
                        if (iSegment != nullptr)
                        {
                            keyFrameStart = iSegment->value.startKeyFrame;
                            keyFrameEnd   = iSegment->value.endKeyFrame;
                        }
                    }

                    // We will clamp the start and end frames against the timeline so we don't crash.
                    keyFrameStart = GTCore::Clamp(keyFrameStart, 0U, this->keyFrames.count - 1);
                    keyFrameEnd   = GTCore::Clamp(keyFrameEnd,   0U, this->keyFrames.count - 1);


                    // Here we check if this frame is looping. If so, we need to set the index of the item in the key frame queue. The value will be the
                    // current count.
                    if (frame->loop)
                    {
                        this->loopStartQueueIndex = this->keyFrameQueue.GetCount();
                    }


                    // We calculate transition times for frames by looking at the times of each and getting the difference. This variable keeps track of
                    // the previous frame's time.
                    double prevFrameTime = 0.0f;

                    // Now we can loop through each remaining key frame.
                    for (size_t j = keyFrameStart; j <= keyFrameEnd; ++j)
                    {
                        auto jTime = this->keyFrames.buffer[j]->value.GetTime();

                        this->keyFrameQueue.Append(j, jTime - prevFrameTime);
                        prevFrameTime = jTime;
                    }
                }

                this->playbackTime = this->keyFrameQueue.GetKeyFramePlaybackTime(0);
            }

            this->isPlaying = true;
        }
    }

    void Animation::Stop()
    {
        this->isPlaying = false;
    }

    void Animation::Pause()
    {
        this->isPlaying = false;
    }

    void Animation::Resume()
    {
        this->isPlaying = true;
    }


    void Animation::Step(double step)
    {
        this->SetPlaybackTime(this->GetPlaybackTime() + step);
    }

    void Animation::SetPlaybackTime(double time)
    {
        this->playbackTime = time;
    }


    float Animation::GetKeyFramesAtTime(double time, size_t &startKeyFrame, size_t &endKeyFrame)
    {
        double totalDuration = this->keyFrameQueue.GetTotalDuration();
        if (time > totalDuration)
        {
            // If we're looping, we just mod the value. Otherwise we will clamp to the end.
            if (this->loopStartQueueIndex != static_cast<size_t>(-1))
            {
                double loopStartTime = this->keyFrameQueue.GetKeyFramePlaybackTime(this->loopStartQueueIndex);
                double loopDuration  = totalDuration - loopStartTime;

                double devnull;
                time = loopStartTime + std::modf(this->playbackTime / loopDuration, &devnull) * loopDuration;
            }
            else
            {
                // Not looping.
                time = totalDuration;
            }
        }

        return this->keyFrameQueue.GetKeyFramesAtTime(time, this->loopStartQueueIndex, startKeyFrame, endKeyFrame);
    }

    float Animation::GetKeyFramesAtCurrentPlayback(size_t &startKeyFrame, size_t &endKeyFrame)
    {
        return this->GetKeyFramesAtTime(this->GetPlaybackTime(), startKeyFrame, endKeyFrame);
    }
}
