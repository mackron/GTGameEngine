// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_AnimationTrack_hpp_
#define __GTEngine_AnimationTrack_hpp_

#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Serialization.hpp>

namespace GT
{
    /// Class representing a set of animation key frames that will be used to play a model's animation.
    ///
    /// The animation track is essentially just a list of animation segments that should be played in sequence. Each segment
    /// is given a start time which defines when it should be played.
    class AnimationTrack
    {
    public:

        /// Constructor.
        AnimationTrack();

        /// Destructor.
        ~AnimationTrack();


        /// Appends a key frame to the end.
        ///
        /// @param index          [in] The index of the global key frame this new local key frame represents.
        /// @param transitionTime [in] The amount of time it takes to transition from the previous key frame.
        ///
        /// @remarks
        ///     If there are no key frames already in the track, 'transitionTime' will be ignored.
        void AppendKeyFrame(size_t index, float transitionTime);

        /// Retrieves the number of keyframes currently making up the animation track.
        size_t GetKeyFrameCount() const;

        /// Clears the animation track.
        void Clear();

        /// Retrieves the total duration of the animation track.
        float GetTotalDuration() const;

        /// Retrieves the playback time of the item in the queue.
        ///
        /// @param keyFrameIndex [in] The index of the item in the queue.
        float GetKeyFramePlaybackTime(size_t keyFrameIndex);

        /// Retrieves key frame information for the given time.
        ///
        /// @param time                [in ] The time to use for retrieving the information.
        /// @param loopStartIndex      [in ] The index of the queue item to interpolate the last frame towards. Set this to -1 to not interpolate the last frame.
        /// @param startKeyFrame       [out] A reference to the variable that will receive the index of the start key frame for the given time.
        /// @param nextKeyFrame        [out] A reference to the variable that will receive the index of the next key frame for the given time.
        ///
        /// @return The amount of interpolation to apply between startKeyFrame and endKeyFrame.
        ///
        /// @remarks
        ///     Since the transition times are not constant, this method must do a linear iteration over each key frame.
        float GetKeyFramesAtTime(float time, size_t loopStartIndex, size_t &startKeyFrameOut, size_t &endKeyFrameOut);



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the animation track.
        ///
        /// @param serializer [in] A reference ot the serializer to write to.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the animation track.
        ///
        /// @param deserializer [in] A reference to the deserializer for reading the data from.
        void Deserialize(Deserializer &deserializer);



    private:

        /// Structure representing a key frame in the animation track.
        struct LocalKeyFrame
        {
            /// Constructor.
            LocalKeyFrame(size_t indexIn, float timeIn)
                : index(indexIn), time(timeIn)
            {
            }


            /// The index of the global key frame this local key frame represents.
            size_t index;

            /// The time the key frame will be hit.
            float time;
        };

        /// The list of local key frames, sorted by time.
        Vector<LocalKeyFrame> localKeyFrames;



    private:    // No copying.
        AnimationTrack(const AnimationTrack &);
        AnimationTrack & operator=(const AnimationTrack &);
    };
}

#endif