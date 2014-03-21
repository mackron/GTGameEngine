// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTGameEngine_AudioEngine_hpp_
#define __GTGameEngine_AudioEngine_hpp_

#include <GTCore/String.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace GTEngine
{
    typedef size_t PlaybackDeviceHandle;
    typedef size_t CaptureDeviceHandle;
    typedef size_t ListenerHandle;
    typedef size_t SoundHandle;


    /// Base class for handling audio playback.
    ///
    /// The audio engine is essentially the audio equivalent of Renderer. This class is just a base class for other audio engine implementations. At the moment
    /// there is only a single implementation (OpenAL), but more will be added in the future, such as DirectSound and maybe even PulseAudio.
    class AudioEngine
    {
    public:

        /// Structure containing information about a playback device.
        struct PlaybackDeviceInfo
        {
            PlaybackDeviceInfo()
                : name()
            {
            }


            /// The name of the device.
            GTCore::String name;
        };


        /// Structure containing information about a capture device.
        struct CaptureDeviceInfo
        {
            CaptureDeviceInfo()
                : name()
            {
            }


            /// The name of the device.
            GTCore::String name;
        };



    public:

        /// Constructor.
        ///
        /// @remarks
        ///     This does NOT do initialization.
        AudioEngine();

        /// Destructor.
        virtual ~AudioEngine();



        ///////////////////////////////////////////////////////
        // Virtual Methods

        /// Initializes the audio engine.
        ///
        /// @return True if the audio engine is initialized successfully; false otherwise.
        virtual bool Startup() = 0;

        /// Shuts down the audio engine.
        ///
        /// @remarks
        ///     After calling this function, the audio engine can not be used until Startup() is called again.
        virtual void Shutdown() = 0;


        /// Retrieves the number of playback devices.
        ///
        /// @remarks
        ///     Use this for iterating over each device.
        virtual size_t GetPlaybackDeviceCount() const = 0;

        /// Retrieves the name of the playback device at the given index.
        ///
        /// @param deviceIndex [in] The index of the device.
        ///
        /// @return A structure containing information about the device.
        virtual PlaybackDeviceInfo GetPlaybackDeviceInfo(size_t deviceIndex) const = 0;

        /// Opens a playback device.
        ///
        /// @param deviceIndex [in] The index of the device. Index 0 is always the default device.
        ///
        /// @return A handle to the device, or 0 if the device could not be opened.
        ///
        /// @remarks
        ///     If deviceIndex is 0, the default device will be opened.
        ///     @par
        ///     The device will need to be passed to every creation function. This is because each listener, sound, etc, must be associated with a device so the
        ///     audio engine knows which device to playback the audio from.
        virtual PlaybackDeviceHandle OpenPlaybackDevice(size_t deviceIndex) = 0;

        /// Closes a playback device.
        ///
        /// @param device [in] A handle to the device to close.
        ///
        /// @remarks
        ///     When a playback device is closed, every resource (listeners, sounds, etc) that's associated with that device will be deleted.
        virtual void ClosePlaybackDevice(PlaybackDeviceHandle device) = 0;



        /// Creates a listener.
        ///
        /// @param device [in] The device the listener should be associated with.
        virtual ListenerHandle CreateListener(PlaybackDeviceHandle device) = 0;

        /// Deletes a listener.
        ///
        /// @param listener [in] A handle to the listener to delete.
        virtual void DeleteListener(ListenerHandle listener) = 0;

        /// Sets the position of the given listener.
        ///
        /// @param listener [in] The listener whose position is being set.
        /// @param position [in] The new position of the listener.
        virtual void SetListenerPosition(ListenerHandle listener, glm::vec3 position) = 0;
        
        /// Retrieves the position of the listener.
        ///
        /// @param listener [in]  The listener whose position is being retrieved.
        ///
        /// @return A vec3 containing the position of the sound.
        virtual glm::vec3 GetListenerPosition(ListenerHandle listener) const = 0;



        /// Creates a sound.
        ///
        /// @param device [in] The device the sound should be played from.
        ///
        /// @return A handle to the new sound object.
        virtual SoundHandle CreateSound(PlaybackDeviceHandle device) = 0;

        /// Deletes a sound that was previously created with CreateSound().
        ///
        /// @param sound [in] A handle to the sound to delete.
        virtual void DeleteSound(SoundHandle sound) = 0;

        /// Sets the position of the given sound.
        ///
        /// @param sound    [in] The sound whose position is being set.
        /// @param position [in] The new position of the sound.
        virtual void SetSoundPosition(SoundHandle sound, glm::vec3 position) = 0;

        /// Retrieves the position of the given sound.
        ///
        /// @param sound [in] The sound whose position is being retrieved.
        ///
        /// @return A vec3 containing the position of the sound.
        virtual glm::vec3 GetSoundPosition(SoundHandle sound) const = 0;






        ///////////////////////////////////////////////////////
        // Non-Virtual Helpers.

        /// Sets the position of the given listener.
        ///
        /// @param listener [in] The listener whose position is being set.
        /// @param xPos     [in] The new x position of the listener.
        /// @param yPos     [in] The new y position of the listener.
        /// @param zPos     [in] The new z position of the listener.
        void SetListenerPosition(ListenerHandle listener, float xPos, float yPos, float zPos) { this->SetListenerPosition(listener, glm::vec3(xPos, yPos, zPos)); }

        /// Sets the position of the given sound.
        ///
        /// @param listener [in] The sound whose position is being set.
        /// @param xPos     [in] The new x position of the sound.
        /// @param yPos     [in] The new y position of the sound.
        /// @param zPos     [in] The new z position of the sound.
        void SetSoundPosition(ListenerHandle sound, float xPos, float yPos, float zPos) { this->SetSoundPosition(sound, glm::vec3(xPos, yPos, zPos)); }
    };
}

#endif
