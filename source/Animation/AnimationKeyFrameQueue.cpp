
#include <GTEngine/Animation/AnimationKeyFrameQueue.hpp>
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    AnimationKeyFrameQueue::AnimationKeyFrameQueue()
        : keyFrames(), totalDuration(0.0)
    {
    }

    AnimationKeyFrameQueue::~AnimationKeyFrameQueue()
    {
    }

    void AnimationKeyFrameQueue::Append(size_t keyFrameIndex, double transitionTime)
    {
        this->keyFrames.PushBack(Item(keyFrameIndex, transitionTime));
        this->totalDuration += transitionTime;
    }

    void AnimationKeyFrameQueue::RemoveAll()
    {
        this->keyFrames.count = 0;
        this->totalDuration   = 0.0;
    }

    void AnimationKeyFrameQueue::RemoveAt(size_t index)
    {
        this->totalDuration -= this->keyFrames.buffer[index].transitionTime;
        this->keyFrames.Remove(index);
    }


    float AnimationKeyFrameQueue::GetKeyFramesAtTime(double time, size_t startLoopQueueIndex, size_t &startKeyFrameOut, size_t &endKeyFrameOut)
    {
        size_t iStart = 0;
        size_t iEnd   = 0;

        // This will find the first key.
        size_t i = 0;
        double startKeyTime = 0.0f;
        
        while (i < this->keyFrames.count)
        {
            double nextKeyTime = startKeyTime;
            if (i > 0)
            {
                nextKeyTime += this->keyFrames[i].transitionTime;
            }

            if (nextKeyTime > time)
            {
                break;
            }
            else
            {
                iStart = i;
                startKeyTime = nextKeyTime;
            }

            ++i;
        }
        

        // Here is where we find the end key.
        if (iStart == this->keyFrames.count - 1)
        {
            if (startLoopQueueIndex != static_cast<size_t>(-1))
            {
                iEnd = startLoopQueueIndex;
            }
            else
            {
                iEnd = iStart;
            }
        }
        else
        {
            iEnd = iStart + 1;
        }


        // Here we translate our start and end key frame indices.
        startKeyFrameOut = this->keyFrames[iStart].keyFrameIndex;
        endKeyFrameOut   = this->keyFrames[iEnd].keyFrameIndex;


        // At this point we will have the start and end frames. Now we need to get an interpolation factor between the two. This is the return value.
        return static_cast<float>((time - startKeyTime) / this->keyFrames[iEnd].transitionTime);
    }

    double AnimationKeyFrameQueue::GetKeyFramePlaybackTime(size_t itemIndex)
    {
        double startTime = 0.0;
        for (size_t i = 0; i <= itemIndex; ++i)
        {
            startTime += this->keyFrames[i].transitionTime;
        }

        return startTime;
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void AnimationKeyFrameQueue::Serialize(GTCore::Serializer &serializer) const
    {
        Serialization::ChunkHeader header;
        header.id = Serialization::ChunkID_AnimationKeyFrameQueue;
        header.version = 1;
        header.sizeInBytes = 
            sizeof(uint32_t) +                          // <-- Key frame count.
            sizeof(Item) * this->keyFrames.count +      // <-- Key frame data.
            sizeof(double);                             // <-- Total duration.


        serializer.Write(header);
        {
            serializer.Write(static_cast<uint32_t>(this->keyFrames.count));
            serializer.Write(this->keyFrames.buffer, sizeof(Item) * this->keyFrames.count);
        
            serializer.Write(this->totalDuration);
        }
    }

    void AnimationKeyFrameQueue::Deserialize(GTCore::Deserializer &deserializer)
    {
        this->keyFrames.Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);

        assert(header.id == Serialization::ChunkID_AnimationKeyFrameQueue);
        {
            switch (header.version)
            {
            case 1:
                {
                    uint32_t keyFramesCount;
                    deserializer.Read(keyFramesCount);

                    this->keyFrames.Reserve(keyFramesCount);
                    this->keyFrames.count = keyFramesCount;
                    deserializer.Read(this->keyFrames.buffer, sizeof(Item) * keyFramesCount);

                    deserializer.Read(this->totalDuration);

                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing AnimationKeyFrameQueue. Chunk version is unsupported (%d).", header.version);
                    break;
                }
            }
        }
    }
}