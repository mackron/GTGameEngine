// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_AnimationKeyFrame_hpp_
#define __GTEngine_AnimationKeyFrame_hpp_

namespace GTEngine
{
    class AnimationKeyFrame
    {
    public:

        /// Constructor.
        AnimationKeyFrame(double time);

        /// Destructor.
        ~AnimationKeyFrame();


        /// Retrieves the time of the key frame.
        double GetTime() const { return this->time; }

        /// Sets the time of the key frame.
        void SetTime(double newTime) { this->time = newTime; }


    private:

        /// The time of the key frame.
        double time;
    };
}

#endif