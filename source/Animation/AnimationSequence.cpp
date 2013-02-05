// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Animation/AnimationSequence.hpp>

namespace GTEngine
{
    AnimationSequence::AnimationSequence()
        : frames()
    {
    }

    AnimationSequence::~AnimationSequence()
    {
        for (size_t i = 0; i < this->frames.count; ++i)
        {
            delete this->frames.buffer[i];
        }
    }

    void AnimationSequence::AddFrame(const char* name, double transitionTime, bool loop)
    {
        auto newFrame = new AnimationSequenceFrame;
        newFrame->segmentName    = name;
        newFrame->transitionTime = transitionTime;
        newFrame->loop           = loop;

        this->frames.PushBack(newFrame);
    }

    void AnimationSequence::AddFrame(size_t startKeyFrame, size_t endKeyFrame, double transitionTime, bool loop)
    {
        auto newFrame = new AnimationSequenceFrame;
        newFrame->startKeyFrame  = startKeyFrame;
        newFrame->endKeyFrame    = endKeyFrame;
        newFrame->transitionTime = transitionTime;
        newFrame->loop           = loop;

        this->frames.PushBack(newFrame);
    }
}