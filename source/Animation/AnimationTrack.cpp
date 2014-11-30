// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Animation/AnimationTrack.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    AnimationTrack::AnimationTrack()
        : localKeyFrames()
    {
    }

    AnimationTrack::~AnimationTrack()
    {
    }


    void AnimationTrack::AppendKeyFrame(size_t index, float transitionTime)
    {
        if (this->localKeyFrames.count == 0)
        {
            this->localKeyFrames.PushBack(LocalKeyFrame(index, 0.0f));
        }
        else
        {
            this->localKeyFrames.PushBack(LocalKeyFrame(index, this->GetTotalDuration() + transitionTime));
        }
    }

    size_t AnimationTrack::GetKeyFrameCount() const
    {
        return this->localKeyFrames.count;
    }

    void AnimationTrack::Clear()
    {
        this->localKeyFrames.Clear();
    }

    float AnimationTrack::GetTotalDuration() const
    {
        if (this->localKeyFrames.count == 0)
        {
            return 0.0f;
        }
        else
        {
            return this->localKeyFrames.GetBack().time;
        }
    }

    float AnimationTrack::GetKeyFramePlaybackTime(size_t keyFrameIndex)
    {
        return this->localKeyFrames[keyFrameIndex].time;
    }

    float AnimationTrack::GetKeyFramesAtTime(float time, size_t loopStartIndex, size_t &startKeyFrameOut, size_t &endKeyFrameOut)
    {
        size_t iStartKeyFrame = 0;
        size_t iEndKeyFrame   = 0;


        // Start key.
        size_t iKeyFrame = 0;
        while (iKeyFrame < this->localKeyFrames.count)
        {
            if (this->localKeyFrames[iKeyFrame].time >= time)
            {
                break;
            }
            else
            {
                iStartKeyFrame = iKeyFrame;
            }

            ++iKeyFrame;
        }

        // End key.
        if (iStartKeyFrame == this->localKeyFrames.count - 1)       // <-- Are we at the end of the track?
        {
            if (loopStartIndex != static_cast<size_t>(-1))
            {
                iEndKeyFrame = loopStartIndex;
            }
            else
            {
                iEndKeyFrame = iStartKeyFrame;
            }
        }
        else
        {
            iEndKeyFrame = iStartKeyFrame + 1;
        }



        // Output and return.
        if (this->localKeyFrames.count >= 2)
        {
            startKeyFrameOut = this->localKeyFrames[iStartKeyFrame].index;
            endKeyFrameOut   = this->localKeyFrames[iEndKeyFrame].index;

            float startTime = this->localKeyFrames[iStartKeyFrame].time;
            float endTime   = this->localKeyFrames[iEndKeyFrame].time;

            // At this point we will have the start and end frames. Now we need to get an interpolation factor between the two. This is the return value.
            return static_cast<float>((time - startTime) / (endTime - startTime));
        }
        else
        {
            startKeyFrameOut = 0;
            endKeyFrameOut   = 0;

            return 0.0f;
        }
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void AnimationTrack::Serialize(GTLib::Serializer &serializer) const
    {
        Serialization::ChunkHeader header;
        header.id = Serialization::ChunkID_AnimationKeyFrameQueue;
        header.version = 1;
        header.sizeInBytes = 
            sizeof(uint32_t) +                                                              // <-- Key frame count.
            sizeof(LocalKeyFrame) * static_cast<uint32_t>(this->localKeyFrames.count) +     // <-- Key frame data.
            sizeof(double);                                                                 // <-- Total duration.


        serializer.Write(header);
        {
            serializer.Write(static_cast<uint32_t>(this->localKeyFrames.count));
            serializer.Write(this->localKeyFrames.buffer, sizeof(LocalKeyFrame) * this->localKeyFrames.count);
        }
    }

    void AnimationTrack::Deserialize(GTLib::Deserializer &deserializer)
    {
        this->localKeyFrames.Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_AnimationKeyFrameQueue);
        {
            if (header.version == 1)
            {
                uint32_t keyFramesCount;
                deserializer.Read(keyFramesCount);

                this->localKeyFrames.Reserve(keyFramesCount);
                this->localKeyFrames.count = keyFramesCount;
                deserializer.Read(this->localKeyFrames.buffer, sizeof(LocalKeyFrame) * keyFramesCount);
            }
            else
            {
                GTEngine::Log("Error deserializing AnimationTrack. Chunk version is unsupported (%d).", header.version);
            }
        }
    }
}