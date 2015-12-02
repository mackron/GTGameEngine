// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Animation/AnimationSequence.hpp>

namespace GT
{
    AnimationSequence::AnimationSequence()
        : name(), frames()
    {
    }

    AnimationSequence::~AnimationSequence()
    {
    }

    void AnimationSequence::SetName(const char* newName)
    {
        this->name = newName;
    }

    const char* AnimationSequence::GetName() const
    {
        return this->name.c_str();
    }

    void AnimationSequence::AddFrame(const char* segmentName, double transitionTime, bool loop)
    {
        AnimationSequenceFrame newFrame;
        newFrame.segmentName    = segmentName;
        newFrame.transitionTime = transitionTime;
        newFrame.loop           = loop;

        this->frames.PushBack(newFrame);
    }

    void AnimationSequence::AddFrame(size_t startKeyFrame, size_t endKeyFrame, double transitionTime, bool loop)
    {
        AnimationSequenceFrame newFrame;
        newFrame.startKeyFrame  = startKeyFrame;
        newFrame.endKeyFrame    = endKeyFrame;
        newFrame.transitionTime = transitionTime;
        newFrame.loop           = loop;

        this->frames.PushBack(newFrame);
    }
}