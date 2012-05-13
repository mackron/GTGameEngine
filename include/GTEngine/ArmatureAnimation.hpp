
#ifndef __GTEngine_ArmatureAnimation_hpp_
#define __GTEngine_ArmatureAnimation_hpp_

#include <GTEngine/Bone.hpp>
#include <GTCore/String.hpp>
#include <GTCore/Map.hpp>


namespace GTEngine
{
    /// A transformation key for armature animations.
    class ArmatureAnimationKey
    {
    public:

        /// Constructor.
        ArmatureAnimationKey(double timeIn, const glm::vec3 &positionIn, const glm::quat &rotationIn, const glm::vec3 &scaleIn)
            : time(timeIn), position(positionIn), rotation(rotationIn), scale(scaleIn)
        {
        }


    public:

        /// The time of the key.
        double time;

        /// The position.
        glm::vec3 position;

        /// The rotation.
        glm::quat rotation;

        /// The scale.
        glm::vec3 scale;
    };



    /// Class representing a channel in an armature animation.
    class ArmatureAnimationChannel
    {
    public:

        /// Constructor.
        ArmatureAnimationChannel(Bone &bone)
            : bone(bone), keys()
        {
        }

        /// Destructor.
        ~ArmatureAnimationChannel()
        {
        }


        /// Retrieves the bone that is affected by this channel.
        Bone & GetBone() { return this->bone; }


        /// Adds a key.
        void AddKey(double time, const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
        {
            this->keys.Add(time, ArmatureAnimationKey(time, position, rotation, scale));
        }

        /// Retrieves the number of keys in the animation.
        size_t GetKeyCount() const { return this->keys.count; }

        /// Retrieves a reference to the key at the given index.
        ArmatureAnimationKey & GetKey(size_t index) { return this->keys.buffer[index]->value; }


        /// Retrieves the time of the first key.
        double GetFirstKeyTime() const
        {
            if (this->keys.count > 0)
            {
                return this->keys.buffer[0]->value.time;
            }
            
            return 0.0;
        }

        /// Retrieves the time of the last key.
        double GetLastKeyTime() const
        {
            if (this->keys.count > 0)
            {
                return this->keys.buffer[this->keys.count - 1]->value.time;
            }

            return 0.0;
        }


    // Playback.
    public:

        /// Updates the channel based on the given time.
        ///
        /// @param time [in] The animation time.
        void Update(double time)
        {
            ArmatureAnimationKey* currentKey;
            ArmatureAnimationKey* nextKey;
            float ratio;
            if (this->FindKeys(time, currentKey, nextKey, ratio))
            {
                glm::vec3 position = glm::mix(currentKey->position, nextKey->position, ratio);
                glm::quat rotation = glm::mix(currentKey->rotation, nextKey->rotation, ratio);
                glm::vec3 scale    = glm::mix(currentKey->scale,    nextKey->scale,    ratio);

                bone.SetTransform(glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale));
            }
        }



    private:

        /// Helper method for retrieving the keys for the current playback time.
        bool FindKeys(double time, ArmatureAnimationKey* &currentKey, ArmatureAnimationKey* &nextKey, float &ratio)
        {
            assert(time >= 0.0);

            if (this->keys.count > 0)
            {
                if (time < this->GetFirstKeyTime())
                {
                    // We will get here if the time is lower than the time of the first key. In this case we should interpolate between the last and the first keys.
                    currentKey = &this->keys.buffer[this->keys.count - 1]->value;
                    nextKey    = &this->keys.buffer[0]->value;
                    ratio      = static_cast<float>(time / nextKey->time);
                }
                else if (time >= this->GetLastKeyTime())
                {
                    // We will get here if the time is higher than the time of the last key. In this case we will just clamp the model to the last key.
                    currentKey = nextKey = &this->keys.buffer[this->keys.count - 1]->value;
                    ratio      = 1.0f;
                }
                else
                {
                    // We will get here if the time is somewhere between the first and last keys.
                    size_t iCurrent = 0;
                    for ( ; iCurrent < (this->keys.count); ++iCurrent)
                    {
                        auto &key = this->keys.buffer[iCurrent]->value;

                        if (key.time <= time)
                        {
                            currentKey = &key;
                        }
                        else
                        {
                            break;
                        }
                    }

                    // Based on previous checks, we should be able to assert that the current index will in bounds.
                    assert(iCurrent < this->keys.count);
                    nextKey = &this->keys.buffer[iCurrent]->value;


                    double frameTime           = nextKey->time - currentKey->time;
                    double timeSinceCurrentKey = time - currentKey->time;
                    ratio = static_cast<float>(timeSinceCurrentKey / frameTime);
                }

                return true;
            }

            return false;
        }



    private:

        /// The bone this channel affects.
        Bone &bone;

        /// The list of keys for this channel. This should be sorted by the time of the channel. Thus, we're going to use a map.
        GTCore::Map<double, ArmatureAnimationKey> keys;
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


    // Playback.
    public:

        /// Plays the animation.
        void Play();
        void Play(bool loop);

        /// Stops the animation.
        ///
        /// @remarks
        ///     This will bring the animation back to it's first frame. Use Pause() to pause the animation.
        void Stop();

        /// Pauses the animation.
        void Pause();

        /// Steps the animation by the given time.
        ///
        /// @param deltaTimeInSeconds [in] The amount of time to step the animation.
        void Step(double deltaTimeInSeconds);



    private:

        /// Helper method for finding the time of the first key.
        void MoveToFirstKey();



    private:

        /// The name of the animation.
        GTCore::String name;

        /// The duration of the animation in seconds.
        double durationSeconds;

        /// The list of channels affected by this animation.
        GTCore::Vector<ArmatureAnimationChannel*> channels;


        /// Keeps track of whether or not the animation is currently playing.
        bool isPlaying;

        /// Keeps track of whether or not the animation should loop.
        bool isLooping;

        /// Keeps track of whether or not the animation has been played yet. We need to use this for checking where the playback time should
        /// start in Play().
        bool hasPlayed;

        /// Keeps track of the current animation time.
        double playbackTime;
    };
}

#endif
