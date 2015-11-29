// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AnimationKeyFrame
#define GT_AnimationKeyFrame

namespace GT
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
