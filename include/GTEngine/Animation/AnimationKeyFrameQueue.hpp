// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_AnimationKeyFrameQueue_hpp_
#define __GTEngine_AnimationKeyFrameQueue_hpp_

#include "../Serialization.hpp"
#include <GTCore/Vector.hpp>


namespace GTEngine
{
    /// Class for storing a list of key frames used for playback.
    ///
    /// The key frame queue can be used to retrieve the information about key frames at a given time in the animation.
    class AnimationKeyFrameQueue
    {
    public:

        /// Constructor.
        AnimationKeyFrameQueue();

        /// Destructor.
        ~AnimationKeyFrameQueue();


        /// Appends a new item to the end of the queue.
        ///
        /// @param keyFrameIndex  [in] The index of the key frame.
        /// @param transitionTime [in] The amount of time it takes to transition from the previous key frame.
        void Append(size_t keyFrameIndex, double transitionTime);

        /// Removes every item from the queue.
        void RemoveAll();

        /// Removes the item at the given index.
        ///
        /// @param index [in] The index of the item to remove.
        void RemoveAt(size_t index);

        /// Retrieves the number of items in the queue.
        size_t GetCount() const { return this->keyFrames.count; }


        /// Retrieves key frame information for the given time.
        ///
        /// @param time                [in ] The time to use for retrieving the information.
        /// @param startLoopQueueIndex [in ] The index of the queue item to interpolate the last frame towards. Set this to -1 to not interpolate the last frame.
        /// @param startKeyFrame       [out] A reference to the variable that will receive the index of the start key frame for the given time.
        /// @param nextKeyFrame        [out] A reference to the variable that will receive the index of the next key frame for the given time.
        ///
        /// @return The amount of interpolation to apply between startKeyFrame and endKeyFrame.
        ///
        /// @remarks
        ///     Since the transition times are not constant, this method must do a linear iteration over each key frame.
        float GetKeyFramesAtTime(double time, size_t startLoopQueueIndex, size_t &startKeyFrameOut, size_t &endKeyFrameOut);


        /// Retrieves the total duration of the animatino defined by this queue.
        double GetTotalDuration() const { return this->totalDuration; }


        /// Retrieves the playback time of the item in the queue.
        ///
        /// @param itemIndex [in] The index of the item in the queue.
        double GetKeyFramePlaybackTime(size_t itemIndex);


        /// Sets the transition time of the key frame at the given index.
        ///
        /// @param keyFrameIndex  [in] The index of the key frame whose transition time is being modified.
        /// @param transitionTime [in] The new transition time.
        void SetKeyFrameTransitionTime(size_t keyFrameIndex, double transitionTime);

        /// Retrieves the transition time of the key frame at the given index.
        ///
        /// @param keyFrameIndex [in] The index of the key frame whose transition time is being retrieved.
        double GetKeyFrameTransitionTime(size_t keyFrameIndex) const;



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the key frame queue.
        ///
        /// @param serializer [in] A reference ot the serializer to write to.
        void Serialize(GTCore::Serializer &serializer) const;

        /// Deserializes the key frame queue.
        ///
        /// @param deserializer [in] A reference to the deserializer for reading the data from.
        void Deserialize(GTCore::Deserializer &deserializer);


    private:

        /// Structure representing an item in the queue.
        struct Item
        {
            Item(size_t keyFrameIndex, double transitionTime)
                : keyFrameIndex(keyFrameIndex), transitionTime(transitionTime)
            {
            }

            /// The key frame index.
            size_t keyFrameIndex;

            /// The transition time from the previous key frame.
            double transitionTime;
        };

        /// The list of key frame indices.
        GTCore::Vector<Item> keyFrames;

        /// The total duration of the animation defined by this queue.
        double totalDuration;
    };
}

#endif
