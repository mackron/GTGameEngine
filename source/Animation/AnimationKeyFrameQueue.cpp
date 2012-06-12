
#include <GTEngine/Animation/AnimationKeyFrameQueue.hpp>

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
}