// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Animation/Animation.hpp>

namespace GTEngine
{
    Animation::Animation()
        : keyFrames(), channels(),
          segments(),
          currentAnimationTrack(),
          isPlaying(false), isPaused(false), playbackTime(0.0),
          loopStartIndex(static_cast<size_t>(-1))
    {
    }

    Animation::~Animation()
    {
        for (size_t i = 0; i < this->channels.count; ++i)
        {
            delete this->channels[i];
        }
    }


    size_t Animation::AppendKeyFrame(double time)
    {
        this->keyFrames.Add(time, AnimationKeyFrame(time));

        for (size_t i = 0; i < this->keyFrames.count; ++i)
        {
            if (this->keyFrames.buffer[i]->key == time)
            {
                return i;
            }
        }

        return 0;
    }

    double Animation::GetKeyFrameTimeByIndex(size_t index) const
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
        this->channels.RemoveFirstOccuranceOf(&channel);
        delete &channel;
    }

    void Animation::AddNamedSegment(const char* name, size_t startKeyFrame, size_t endKeyFrame)
    {
        this->segments.PushBack(AnimationSegment(name, startKeyFrame, endKeyFrame));
    }

    
    void Animation::RemoveFirstSegmentByName(const char* name)
    {
        for (size_t iSegment = 0; iSegment < this->segments.count; ++iSegment)
        {
            auto &segment = this->segments[iSegment];
            {
                if (segment.name == name)
                {
                    this->segments.Remove(iSegment);
                    return;
                }
            }
        }
    }

    void Animation::RemoveSegmentsByName(const char* name)
    {
        size_t iSegment = 0;
        while (iSegment < this->segments.count)
        {
            auto &segment = this->segments[iSegment];

            if (segment.name == name)
            {
                this->segments.Remove(iSegment);
            }
            else
            {
                ++iSegment;
            }
        }
    }

    void Animation::RemoveSegmentByIndex(size_t index)
    {
        this->segments.Remove(index);
    }


    AnimationSegment* Animation::GetNamedSegment(const char* name)
    {
        return const_cast<AnimationSegment*>(const_cast<const Animation*>(this)->GetNamedSegment(name));
    }
    const AnimationSegment* Animation::GetNamedSegment(const char* name) const
    {
        for (size_t iSegment = 0; iSegment < this->segments.count; ++iSegment)
        {
            auto &segment = this->segments[iSegment];
            {
                if (segment.name == name)
                {
                    return &segment;
                }
            }
        }

        return nullptr;
    }

    AnimationSegment* Animation::GetNamedSegmentByIndex(size_t index)
    {
        return &this->segments[index];
    }
    const AnimationSegment* Animation::GetNamedSegmentByIndex(size_t index) const
    {
        return &this->segments[index];
    }

    const char* Animation::GetNamedSegmentNameByIndex(size_t index) const
    {
        return this->segments[index].name.c_str();
    }



    ////////////////////////////////////////////////////////////////////////
    // Playback.

    void Animation::Play(const AnimationSequence &sequence, double transitionTimeIn)
    {
        // The animation track needs to be reset.
        this->currentAnimationTrack.Clear();

        /// The loop index needs to reset.
        this->loopStartIndex = static_cast<size_t>(-1);


        // If we don't have any key frames, we won't bother doing anything.
        if (this->keyFrames.count > 0)
        {
            // We need to loop over each fraame in the input animation segments and add them to the track.
            for (size_t iSequenceFrame = 0U; iSequenceFrame < sequence.GetFrameCount(); ++iSequenceFrame)
            {
                auto &sequenceFrame = sequence.GetFrame(iSequenceFrame);

                size_t keyFrameStart = sequenceFrame.startKeyFrame;
                size_t keyFrameEnd   = sequenceFrame.endKeyFrame;
                if (sequenceFrame.segmentName.GetLength() > 0)
                {
                    auto segment = this->GetNamedSegment(sequenceFrame.segmentName.c_str());
                    if (segment != nullptr)
                    {
                        keyFrameStart = segment->startKeyFrame;
                        keyFrameEnd   = segment->endKeyFrame;
                    }
                }

                // We will clamp the start and end frames against the timeline so we don't crash.
                keyFrameStart = GTLib::Clamp(keyFrameStart, static_cast<size_t>(0U), this->keyFrames.count - 1);
                keyFrameEnd   = GTLib::Clamp(keyFrameEnd,   static_cast<size_t>(0U), this->keyFrames.count - 1);


                // Here we check if this frame is looping. If so, we need to set the index of the item in the key frame queue. The value will be the
                // current count.
                if (sequenceFrame.loop)
                {
                    this->loopStartIndex = this->currentAnimationTrack.GetKeyFrameCount();
                }


                // Now we need to loop over and add the key frames to the queue.
                for (size_t jKeyFrame = keyFrameStart; jKeyFrame <= keyFrameEnd; ++jKeyFrame)
                {
                    double transitionTime = 0.0;

                    if (jKeyFrame == keyFrameStart)         // <-- The first key frame in the current segment?
                    {
                        if (iSequenceFrame == 0)            // <-- The first segment in the animation sequence?
                        {
                            transitionTime = transitionTimeIn;
                        }
                        else
                        {
                            transitionTime = sequenceFrame.transitionTime;
                        }
                    }
                    else
                    {
                        transitionTime = this->keyFrames.buffer[jKeyFrame]->value.GetTime() - this->keyFrames.buffer[jKeyFrame - 1]->value.GetTime();
                    }

                        
                    this->currentAnimationTrack.AppendKeyFrame(jKeyFrame, static_cast<float>(transitionTime));
                }
            }

            this->playbackTime = 0.0;
            this->isPlaying    = true;
            this->isPaused     = false;
        }
    }

    void Animation::Stop()
    {
        this->isPlaying = false;
        this->isPaused  = false;
    }

    void Animation::Pause()
    {
        if (this->isPlaying)
        {
            this->isPaused = true;
        }
    }

    void Animation::Resume()
    {
        if (this->isPlaying && this->isPaused)
        {
            this->isPaused = false;
        }
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
        double totalDuration = this->currentAnimationTrack.GetTotalDuration();
        if (time > totalDuration)
        {
            // If we're looping, we just mod the value. Otherwise we will clamp to the end.
            if (this->loopStartIndex != static_cast<size_t>(-1))
            {
                double loopStartTime = this->currentAnimationTrack.GetKeyFramePlaybackTime(this->loopStartIndex);
                double loopDuration  = totalDuration - loopStartTime;

                double devnull;
                time = loopStartTime + std::modf((this->playbackTime - loopStartTime) / loopDuration, &devnull) * loopDuration;
            }
            else
            {
                // Not looping.
                time = totalDuration;
            }
        }

        return this->currentAnimationTrack.GetKeyFramesAtTime(static_cast<float>(time), this->loopStartIndex, startKeyFrame, endKeyFrame);
    }

    float Animation::GetKeyFramesAtCurrentPlayback(size_t &startKeyFrame, size_t &endKeyFrame)
    {
        return this->GetKeyFramesAtTime(this->GetPlaybackTime(), startKeyFrame, endKeyFrame);
    }


    void Animation::Clear(bool clearNamedSegments)
    {
        this->keyFrames.Clear();


        for (size_t i = 0; i < this->channels.count; ++i)
        {
            delete this->channels[i];
        }
        this->channels.Clear();


        if (clearNamedSegments)
        {
            this->segments.Clear();
        }
    }

    void Animation::ClearNamedSegments()
    {
        this->segments.Clear();
    }




    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void Animation::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer playbackSerializer;

        this->currentAnimationTrack.Serialize(playbackSerializer);

        playbackSerializer.Write(this->isPlaying);
        playbackSerializer.Write(this->isPaused);
        playbackSerializer.Write(this->playbackTime);
        playbackSerializer.Write(static_cast<uint32_t>(this->loopStartIndex));


        // The current playback state chunk.
        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_Animation_PlaybackState;
        header.version     = 1;
        header.sizeInBytes = playbackSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(playbackSerializer.GetBuffer(), header.sizeInBytes);



        // Null terminator.
        header.id          = Serialization::ChunkID_Null;
        header.version     = 1;
        header.sizeInBytes = 0;
        serializer.Write(header);
    }

    void Animation::Deserialize(GTLib::Deserializer &deserializer)
    {
        // We are going to use an iteration based deserializer here.
        Serialization::ChunkHeader header;

        do
        {
            deserializer.Read(header);

            switch (header.id)
            {
            case Serialization::ChunkID_Animation_PlaybackState:
                {
                    this->currentAnimationTrack.Deserialize(deserializer);

                    deserializer.Read(this->isPlaying);
                    deserializer.Read(this->isPaused);
                    deserializer.Read(this->playbackTime);
                    
                    uint32_t newLoopStartQueueIndex;
                    deserializer.Read(newLoopStartQueueIndex);

                    this->loopStartIndex = static_cast<size_t>(newLoopStartQueueIndex);

                    break;
                }

            default:
                {
                    // We don't know the chunk, so we'll skip over it.
                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }

        } while (header.id != Serialization::ChunkID_Null);
    }
}
