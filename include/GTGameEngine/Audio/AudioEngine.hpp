// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AudioEngine
#define GT_AudioEngine

#include "AudioDataFormats.hpp"
#include "../Math.hpp"
#include <GTLib/String.hpp>


namespace GT
{
    typedef uint32_t HPlaybackDevice;
    typedef uint32_t HCaptureDevice;
    typedef uint32_t HListener;
    typedef uint32_t HSound;
    typedef uint32_t HAudioBuffer;


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
            GTLib::String name;
        };


        /// Structure containing information about a capture device.
        struct CaptureDeviceInfo
        {
            CaptureDeviceInfo()
                : name()
            {
            }


            /// The name of the device.
            GTLib::String name;
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
        virtual HPlaybackDevice OpenPlaybackDevice(size_t deviceIndex) = 0;

        /// Closes a playback device.
        ///
        /// @param device [in] A handle to the device to close.
        ///
        /// @remarks
        ///     When a playback device is closed, every resource (listeners, sounds, etc) that's associated with that device will be deleted.
        virtual void ClosePlaybackDevice(HPlaybackDevice device) = 0;



        /// Creates a listener.
        ///
        /// @param device [in] The device the listener should be associated with.
        virtual HListener CreateListener(HPlaybackDevice device) = 0;

        /// Deletes a listener.
        ///
        /// @param listener [in] A handle to the listener to delete.
        virtual void DeleteListener(HListener listener) = 0;

        /// Sets the position of the given listener.
        ///
        /// @param listener [in] The listener whose position is being set.
        /// @param position [in] The new position of the listener.
        virtual void SetListenerPosition(HListener listener, vec3 position) = 0;

        /// Retrieves the position of the listener.
        ///
        /// @param listener [in]  The listener whose position is being retrieved.
        ///
        /// @return A vec3 containing the position of the sound.
        virtual vec3 GetListenerPosition(HListener listener) const = 0;

        /// Sets the orientation of the given listener.
        ///
        /// @param listener    [in] The listener whose orientation is being set.
        /// @param orientation [in] The quaternion representing the target orientation.
        virtual void SetListenerOrientation(HListener listener, const quat &orientation) = 0;

        /// Retrieves the orientation of the given listener.
        ///
        /// @param listener [in] The listener whose orientation is being retrieved.
        virtual quat GetListenerOrientation(HListener listener) const = 0;



        /// Creates a sound.
        ///
        /// @param device [in] The device the sound should be played from.
        ///
        /// @return A handle to the new sound object.
        virtual HSound CreateSound(HPlaybackDevice device) = 0;

        /// Deletes a sound that was previously created with CreateSound().
        ///
        /// @param sound [in] A handle to the sound to delete.
        virtual void DeleteSound(HSound sound) = 0;

        /// Determines whether or not the given handle is a valid sound object.
        ///
        /// @param hSound [in] A handle to the sound object.
        virtual bool IsValidSound(HSound hSound) const = 0;

        /// Sets the position of the given sound.
        ///
        /// @param sound    [in] The sound whose position is being set.
        /// @param position [in] The new position of the sound.
        virtual void SetSoundPosition(HSound sound, vec3 position) = 0;

        /// Retrieves the position of the given sound.
        ///
        /// @param sound [in] The sound whose position is being retrieved.
        ///
        /// @return A vec3 containing the position of the sound.
        virtual vec3 GetSoundPosition(HSound sound) const = 0;

        /// Sets whether or not the position of the sound should be relative to the listener.
        ///
        /// @param sound      [in] The sound handle.
        /// @param isRelative [in] True is the sound's position should be relative to the player; false otherwise.
        virtual void SetIsSoundPositionRelative(HSound sound, bool isRelative) = 0;

        /// Determines whether or not the position of the given sound is relative to the listener.
        ///
        /// @param sound [in] The sound handle.
        ///
        /// @return True if the sound is relative to the listener; false otherwise.
        virtual bool IsSoundPositionRelative(HSound sound) const = 0;

        /// Adds the given buffer to the end of the given sound's buffer queue.
        ///
        /// @param sound  [in] The sound whose having an audio buffer queued.
        /// @param buffer [in] The buffer to add to the queue.
        virtual void QueueAudioBuffer(HSound sound, HAudioBuffer buffer) = 0;

        /// Removes the oldest processed audio buffer from the given sound's queue.
        ///
        /// @param sound  [in] The sound whose having the buffer removed.
        ///
        /// @remarks
        ///     Use GetProcessedAudioBufferCount() to retrieve the number of buffers that have been processed.
        virtual void UnqueueAudioBuffer(HSound sound) = 0;

        /// Retrieves the number of buffers that are queud on the given sound.
        ///
        /// @param sound [in] The sound whose buffer queue is being counted.
        virtual unsigned int GetQueuedAudioBufferCount(HSound sound) = 0;

        /// Retrieves the number of buffers that are queue on the given sound, but have also been processed (played).
        ///
        /// @param sound [in] The sound handle.
        virtual unsigned int GetProcessedQueuedAudioBufferCount(HSound sound) = 0;

        /// Starts, replay or resume the given sound.
        ///
        /// @param sound [in] The sound to play.
        virtual void PlaySound(HSound sound) = 0;

        /// Stops playing the sound.
        ///
        /// @param sound [in] The sound to stop.
        ///
        /// @remarks
        ///     Calling Play() after this will cause the sound to restart from the start. Use Pause() if you want pause functionality.
        virtual void StopSound(HSound sound) = 0;

        /// Pauses playback of the given sound.
        ///
        /// @param sound [in] The sound to pause.
        ///
        /// @remarks
        ///     Calling Play() after this will cause the sound to resume from where it was left off. Use Stop() or Rewind() if you want to go back to the beginning.
        virtual void PauseSound(HSound sound) = 0;

        /// Restarts the given sound from the beginning.
        ///
        /// @param sound [in] The sound to restart.
        ///
        /// @remarks
        ///     This does not stop playback.
        virtual void RewindSound(HSound sound) = 0;

        /// Determines whether or not the sound is currently being played.
        ///
        /// @param sound [in] The sound handle.
        ///
        /// @return True if the sound is currently in a playing state; false otherwise.
        virtual bool IsSoundPlaying(HSound sound) = 0;

        /// Determines whether or not the sound is currently paused.
        ///
        /// @param sound [in] The sound handle.
        ///
        /// @return True if the sound is currently paused; false otherwise.
        virtual bool IsSoundPaused(HSound sound) = 0;

        /// Determines whether or not the sound is currently stopped.
        ///
        /// @param sound [in] The sound handle.
        ///
        /// @return True if the sound is currently stopped; false otherwise.
        virtual bool IsSoundStopped(HSound sound) = 0;



        /// Creates an audio buffer.
        ///
        /// @param device [in] the device the buffer should be created for.
        ///
        /// @return A handle to the new buffer object.
        virtual HAudioBuffer CreateAudioBuffer(HPlaybackDevice device) = 0;

        /// Deletes an audio buffer that was previously created with CreateAudioBuffer().
        ///
        /// @param buffer [in] The audio buffer to delete.
        virtual void DeleteAudioBuffer(HAudioBuffer buffer) = 0;

        /// Sets the data of the given audio buffer.
        ///
        /// @param buffer          [in] A handle to the buffer whose data is being set.
        /// @param data            [in] A pointer to the buffer containing the audio data.
        /// @param dataSizeInBytes [in] The size of the audio data, in bytes.
        /// @param format          [in] The format of the audio (8-bit mono, etc.)
        /// @param frequency       [in] The sample rate of the audio - 22050, 44100, etc.
        ///
        /// @remarks
        ///     This will make a copy of the data. The data can be deleted once this returns.
        virtual void SetAudioBufferData(HAudioBuffer buffer, const void *data, size_t dataSizeInBytes, AudioDataFormat format, unsigned int frequency) = 0;





        ///////////////////////////////////////////////////////
        // Non-Virtual Helpers.

        /// Sets the position of the given listener.
        ///
        /// @param listener [in] The listener whose position is being set.
        /// @param xPos     [in] The new x position of the listener.
        /// @param yPos     [in] The new y position of the listener.
        /// @param zPos     [in] The new z position of the listener.
        void SetListenerPosition(HListener listener, float xPos, float yPos, float zPos) { this->SetListenerPosition(listener, vec3(xPos, yPos, zPos)); }

        /// Sets the position of the given sound.
        ///
        /// @param sound [in] The sound whose position is being set.
        /// @param xPos  [in] The new x position of the sound.
        /// @param yPos  [in] The new y position of the sound.
        /// @param zPos  [in] The new z position of the sound.
        void SetSoundPosition(HSound sound, float xPos, float yPos, float zPos) { this->SetSoundPosition(sound, vec3(xPos, yPos, zPos)); }
    };
}

#endif
