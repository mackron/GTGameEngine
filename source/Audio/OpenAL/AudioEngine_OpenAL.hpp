// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTGameEngine_AudioEngine_OpenAL_hpp_
#define __GTGameEngine_AudioEngine_OpenAL_hpp_

#include <GTEngine/Audio/AudioEngine.hpp>
#include <GTLib/Vector.hpp>

#define AL_NO_PROTOTYPES
#include <AL/al.h>
#include <AL/alc.h>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

namespace GTEngine
{
    typedef void(*OpenALProc)();

    /// The OpenAL implementation of the audio engine.
    class AudioEngine_OpenAL : public AudioEngine
    {
    public:

        /// Constructor.
        AudioEngine_OpenAL();

        /// Destructor.
        ~AudioEngine_OpenAL();


        ///////////////////////////////////////////////////////
        // Virtual Methods

        /// AudioEngine::Startup().
        bool Startup();

        /// AudioEngine::Shutdown().
        void Shutdown();



        /// AudioEngine::GetPlaybackDeviceCount().
        size_t GetPlaybackDeviceCount() const;

        /// AudioEngine::GetPlaybackDeviceInfo().
        PlaybackDeviceInfo GetPlaybackDeviceInfo(size_t deviceIndex) const;

        /// AudioEngine::OpenPlaybackDevice().
        PlaybackDeviceHandle OpenPlaybackDevice(size_t deviceIndex);

        /// AudioEngine::ClosePlaybackDevice().
        void ClosePlaybackDevice(PlaybackDeviceHandle device);



        /// AudioEngine::CreateListener().
        ListenerHandle CreateListener(PlaybackDeviceHandle device);

        /// AudioEngine::DeleteListener().
        void DeleteListener(ListenerHandle listener);

        /// AudioEngine::SetListenerPosition().
        void SetListenerPosition(ListenerHandle listener, glm::vec3 position);
        
        /// AudioEngine::GetListenerPosition().
        glm::vec3 GetListenerPosition(ListenerHandle listener) const;

        /// AudioEngine::SetListenerOrientation().
        void SetListenerOrientation(ListenerHandle listener, glm::quat orientation);

        /// AudioEngine::GetListenerOrientation().
        glm::quat GetListenerOrientation(ListenerHandle listener) const;



        /// AudioEngine::CreateSound().
        SoundHandle CreateSound(PlaybackDeviceHandle device);

        /// AudioEngine::DeleteSound().
        void DeleteSound(SoundHandle sound);

        /// AudioEngine::SetSoundPosition().
        void SetSoundPosition(SoundHandle sound, glm::vec3 position);

        /// AudioEngine::GetSoundPosition().
        glm::vec3 GetSoundPosition(SoundHandle sound) const;

        /// AudioEngine::SetSoundPositionRelative().
        void SetIsSoundPositionRelative(SoundHandle sound, bool isRelative);

        /// AudioEngine::QueueAudioBuffer().
        void QueueAudioBuffer(SoundHandle sound, AudioBufferHandle buffer);

        /// AudioEngine::UnqueueAudioBuffer().
        void UnqueueAudioBuffer(SoundHandle sound);

        /// AudioEngine::GetQueuedAudioBufferCount().
        size_t GetQueuedAudioBufferCount(SoundHandle sound);

        /// AudioEngine::GetProcessedQueuedAudioBufferCount().
        size_t GetProcessedQueuedAudioBufferCount(SoundHandle sound);

        /// AudioEngine::PlaySound().
        void PlaySound(SoundHandle sound);

        /// AudioEngine::StopSound().
        void StopSound(SoundHandle sound);

        /// AudioEngine::PauseSound().
        void PauseSound(SoundHandle sound);

        /// AudioEngine::RewindSound().
        void RewindSound(SoundHandle sound);

        /// AudioEngine::IsSoundPlaying().
        bool IsSoundPlaying(SoundHandle sound);

        /// AudioEngine::IsSoundPaused().
        bool IsSoundPaused(SoundHandle sound);

        /// AudioEngine::IsSoundStopped().
        bool IsSoundStopped(SoundHandle sound);



        /// AudioEngine::CreateAudioBuffer().
        AudioBufferHandle CreateAudioBuffer(PlaybackDeviceHandle device);

        /// AudioEngine::DeleteAudioBuffer().
        void DeleteAudioBuffer(AudioBufferHandle buffer);

        /// AudioEngine::SetAudioBufferData().
        void SetAudioBufferData(AudioBufferHandle buffer, const void *data, size_t dataSizeInBytes, AudioDataFormat format, unsigned int frequency);



    private:

        /// Loads the OpenAL dynamic library.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This should only be called one, in Startup(). If this fails, which probably means the OpenAL library isn't installed, Startup() will fail.
        ///     @par
        ///     Shutdown() will unload the library.
        bool LoadOpenALLibrary();

        /// Extracts the playback devices.
        ///
        /// @remarks
        ///     This should only be called once, in Startup().
        void ExtractPlaybackDevices();

        /// Extracts the capture devices.
        ///
        /// @remarks
        ///     This should only be called once, in Startup().
        void ExtractCaptureDevices();


        /// A static helper method for retrieving the address of the given OpenAL function.
        ///
        /// @param library  [in] The handle to the library to get the function pointer from.
        /// @param procName [in] The name of the function to retrieve.
        ///
        /// @remarks
        ///     The returned value can be cast to any other of the OpenAL function pointers.
#if defined(GT_PLATFORM_WINDOWS)
        static OpenALProc GetOpenALProc(HMODULE library, const char* procName);
#endif
#if defined(GT_PLATFORM_LINUX)
        static OpenALProc GetOpenALProc(void* library, const char* procName);
#endif

    private:

#if defined(GT_PLATFORM_WINDOWS)
        /// A handle to the OpenAL32.dll library to load the OpenAL functions from. This is created with LoadLibrary().
        HMODULE m_library;
#endif
#if defined(GT_PLATFORM_LINUX)
        /// A pointer to the OpenAL shared object to load the OpenAL functions from. This is creatd with dlopen().
        void* m_library;
#endif

        /// The list of playback device info structures.
        GTLib::Vector<PlaybackDeviceInfo> m_playbackDevices;

        /// The list of capture device info structures.
        GTLib::Vector<CaptureDeviceInfo> m_captureDevices;


        /// The list of instantiated playback devices.
        GTLib::Vector<PlaybackDeviceHandle> m_instantiatedPlaybackDevices;

        /// The list of instantiated capture devices.
        GTLib::Vector<CaptureDeviceHandle> m_instantiatedCaptureDevices;


        // ALC functions.
        LPALCCREATECONTEXT      m_alcCreateContext;
        LPALCDESTROYCONTEXT     m_alcDestroyContext;
        LPALCMAKECONTEXTCURRENT m_alcMakeContextCurrent;
        LPALCOPENDEVICE         m_alcOpenDevice;
        LPALCCLOSEDEVICE        m_alcCloseDevice;
        LPALCISEXTENSIONPRESENT m_alcIsExtensionPresent;
        LPALCGETSTRING          m_alcGetString;

        // AL functions.
        LPALGETSTRING            m_alGetString;
        LPALISEXTENSIONPRESENT   m_alIsExtensionPresent;
        LPALLISTENER3F           m_alListener3f;
        LPALLISTENERFV           m_alListenerfv;
        LPALGENSOURCES           m_alGenSources;
        LPALDELETESOURCES        m_alDeleteSources;
        LPALSOURCE3F             m_alSource3f;
        LPALSOURCEI              m_alSourcei;
        LPALGETSOURCEFV          m_alGetSourcefv;
        LPALGETSOURCEI           m_alGetSourcei;
        LPALSOURCEQUEUEBUFFERS   m_alSourceQueueBuffers;
        LPALSOURCEUNQUEUEBUFFERS m_alSourceUnqueueBuffers;
        LPALSOURCEPLAY           m_alSourcePlay;
        LPALSOURCESTOP           m_alSourceStop;
        LPALSOURCEREWIND         m_alSourceRewind;
        LPALSOURCEPAUSE          m_alSourcePause;
        LPALGENBUFFERS           m_alGenBuffers;
        LPALDELETEBUFFERS        m_alDeleteBuffers;
        LPALBUFFERDATA           m_alBufferData;
        
        
    private:    // No copying.
        AudioEngine_OpenAL(const AudioEngine_OpenAL &);
        AudioEngine_OpenAL & operator=(const AudioEngine_OpenAL &);
    };
}

#endif
