
#ifndef __GTEngine_ArmatureAnimation_hpp_
#define __GTEngine_ArmatureAnimation_hpp_

#include <GTEngine/Bone.hpp>
#include <GTCore/String.hpp>
#include <GTCore/Map.hpp>


namespace GTEngine
{
    /// A transformation key for armature animations.
    template <typename T>
    class ArmatureAnimationKey
    {
    public:

        /// Constructor.
        ArmatureAnimationKey(double timeIn, const T &valueIn)
            : time(timeIn), value(valueIn)
        {
        }


    public:

        /// The time of the key.
        double time;

        /// The transformation matrix attached to this key.
        T value;
    };


    /// Class representing a channel in an armature animation.
    class ArmatureAnimationChannel
    {
    public:

        /// Constructor.
        ArmatureAnimationChannel(Bone &bone)
            : bone(bone), positionKeys(), rotationKeys(), scaleKeys()
        {
        }

        /// Destructor.
        ~ArmatureAnimationChannel()
        {
        }


        /// Retrieves the bone that is affected by this channel.
        Bone & GetBone() { return this->bone; }


        /// Adds a position key.
        void AddPositionKey(double time, const glm::vec3 &position)
        {
            this->positionKeys.Add(time, ArmatureAnimationKey<glm::vec3>(time, position));
        }

        /// Adds a rotation key.
        void AddRotationKey(double time, const glm::quat &rotation)
        {
            this->rotationKeys.Add(time, ArmatureAnimationKey<glm::quat>(time, rotation));
        }

        /// Adds a scale key.
        void AddScaleKey(double time, const glm::vec3 &scale)
        {
            this->scaleKeys.Add(time, ArmatureAnimationKey<glm::vec3>(time, scale));
        }


        /// Retrieves the number of position keys.
        size_t GetPositionKeyCount() const { return this->positionKeys.count; }

        /// Retrieves the number of rotation keys.
        size_t GetRotationKeyCount() const { return this->rotationKeys.count; }

        /// Retrieves the number of scale keys.
        size_t GetScaleKeyCount() const { return this->scaleKeys.count; }


        /// Retrieves the position key at the given index.
        ArmatureAnimationKey<glm::vec3> & GetPositionKey(size_t index) { return this->positionKeys.buffer[index]->value; }

        /// Retrieves the rotation key at the given index.
        ArmatureAnimationKey<glm::quat> & GetRotationKey(size_t index) { return this->rotationKeys.buffer[index]->value; }

        /// Retrieves the scale key at the given index.
        ArmatureAnimationKey<glm::vec3> & GetScaleKey(size_t index) { return this->scaleKeys.buffer[index]->value; }



    private:

        /// The bone this channel affects.
        Bone &bone;

        /// The list of position keys for this channel. This should be sorted by the time of the channel. Thus, we're going to use a map.
        GTCore::Map<double, ArmatureAnimationKey<glm::vec3>> positionKeys;

        /// The list of rotation keys for this channel.
        GTCore::Map<double, ArmatureAnimationKey<glm::quat>> rotationKeys;

        /// The list of scale keys for this channel.
        GTCore::Map<double, ArmatureAnimationKey<glm::vec3>> scaleKeys;
    };
}


namespace GTEngine
{
    class ArmatureAnimation
    {
    public:

        /// Constructor.
        ArmatureAnimation(const char* name);

        /// Destructor.
        ~ArmatureAnimation();


        /// Retrieves the name of the animation.
        const char* GetName() const { return this->name.c_str(); }

        /// Sets the name of the animation.
        void SetName(const char* newName) { this->name = newName; }


        /// Sets the duration of the animation.
        void SetDurationInSeconds(double seconds) { this->durationSeconds = seconds; }

        /// Retrieves the duration of the animation in seconds.
        double GetDurationInSeconds() const { return this->durationSeconds; }


        /// Adds a new animation channel.
        ///
        /// @param bone [in] A reference to the bone that is affected by the channel.
        ///
        /// @return A pointer to the new animation channel.
        ArmatureAnimationChannel* AddChannel(Bone &bone);


        /// Retrieves the number of channels in the animation.
        size_t GetChannelCount() const { return this->channels.count; }

        /// Retrieves a reference to a channel based on an index.
        ///
        /// @remarks
        ///     Use GetChannelCount() for determining the number of channels.
        ArmatureAnimationChannel & GetChannel(size_t index);


    private:

        /// The name of the animation.
        GTCore::String name;

        /// The duration of the animations in seconds.
        double durationSeconds;

        /// The list of channels affected by this animation.
        GTCore::Vector<ArmatureAnimationChannel*> channels;
    };
}

#endif
