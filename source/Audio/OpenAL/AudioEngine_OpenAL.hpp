// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTGameEngine_AudioEngine_OpenAL_hpp_
#define __GTGameEngine_AudioEngine_OpenAL_hpp_

#include <GTEngine/Audio/AudioEngine.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/HandleManager.hpp>

#define AL_NO_PROTOTYPES
#include <AL/al.h>
#include <AL/alc.h>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

namespace GTEngine
{
    typedef void(*OpenALProc)();


    struct OpenALDevice;
    struct OpenALListener;
    struct OpenALSound;
    struct OpenALBuffer;

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
        HPlaybackDevice OpenPlaybackDevice(size_t deviceIndex);

        /// AudioEngine::ClosePlaybackDevice().
        void ClosePlaybackDevice(HPlaybackDevice device);



        /// AudioEngine::CreateListener().
        HListener CreateListener(HPlaybackDevice device);

        /// AudioEngine::DeleteListener().
        void DeleteListener(HListener listener);

        /// AudioEngine::SetListenerPosition().
        void SetListenerPosition(HListener listener, glm::vec3 position);

        /// AudioEngine::GetListenerPosition().
        glm::vec3 GetListenerPosition(HListener listener) const;

        /// AudioEngine::SetListenerOrientation().
        void SetListenerOrientation(HListener listener, glm::quat orientation);

        /// AudioEngine::GetListenerOrientation().
        glm::quat GetListenerOrientation(HListener listener) const;



        /// AudioEngine::CreateSound().
        HSound CreateSound(HPlaybackDevice device);

        /// AudioEngine::DeleteSound().
        void DeleteSound(HSound sound);

        /// AudioEngine::IsValidSound()
        bool IsValidSound(HSound hSound) const;

        /// AudioEngine::SetSoundPosition().
        void SetSoundPosition(HSound sound, glm::vec3 position);

        /// AudioEngine::GetSoundPosition().
        glm::vec3 GetSoundPosition(HSound sound) const;

        /// AudioEngine::SetIsSoundPositionRelative().
        void SetIsSoundPositionRelative(HSound sound, bool isRelative);

        /// AudioEngine::IsSoundPositionRelative().
        bool IsSoundPositionRelative(HSound sound) const;

        /// AudioEngine::QueueAudioBuffer().
        void QueueAudioBuffer(HSound sound, HAudioBuffer buffer);

        /// AudioEngine::UnqueueAudioBuffer().
        void UnqueueAudioBuffer(HSound sound);

        /// AudioEngine::GetQueuedAudioBufferCount().
        unsigned int GetQueuedAudioBufferCount(HSound sound);

        /// AudioEngine::GetProcessedQueuedAudioBufferCount().
        unsigned int GetProcessedQueuedAudioBufferCount(HSound sound);

        /// AudioEngine::PlaySound().
        void PlaySound(HSound sound);

        /// AudioEngine::StopSound().
        void StopSound(HSound sound);

        /// AudioEngine::PauseSound().
        void PauseSound(HSound sound);

        /// AudioEngine::RewindSound().
        void RewindSound(HSound sound);

        /// AudioEngine::IsSoundPlaying().
        bool IsSoundPlaying(HSound sound);

        /// AudioEngine::IsSoundPaused().
        bool IsSoundPaused(HSound sound);

        /// AudioEngine::IsSoundStopped().
        bool IsSoundStopped(HSound sound);



        /// AudioEngine::CreateAudioBuffer().
        HAudioBuffer CreateAudioBuffer(HPlaybackDevice device);

        /// AudioEngine::DeleteAudioBuffer().
        void DeleteAudioBuffer(HAudioBuffer buffer);

        /// AudioEngine::SetAudioBufferData().
        void SetAudioBufferData(HAudioBuffer buffer, const void *data, size_t dataSizeInBytes, AudioDataFormat format, unsigned int frequency);



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


        /// Retrieves a pointer to the object associated with the given playback device handle.
        OpenALDevice* GetPlaybackDevicePtr(HPlaybackDevice hPlaybackDevice) const;

        /// Retrieves a pointer to the object associated with the given listener handle.
        OpenALListener* GetListenerPtr(HListener hListener) const;

        /// Retrieves a pointer to the object associated with the given sound handle.
        OpenALSound* GetSoundPtr(HSound hSound) const;

        /// Retrieves a pointer to the object associated with the given audio buffer.
        OpenALBuffer* GetAudioBufferPtr(HAudioBuffer hAudioBuffer) const;


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
        GTLib::Vector<PlaybackDeviceInfo> m_playbackDeviceInfos;

        /// The list of capture device info structures.
        GTLib::Vector<CaptureDeviceInfo> m_captureDeviceInfos;


        /// The list of instantiated playback devices.
        //GTLib::Vector<HPlaybackDevice> m_instantiatedPlaybackDevices;

        /// The list of instantiated capture devices.
        //GTLib::Vector<HCaptureDevice> m_instantiatedCaptureDevices;


        /// The handle manager for instantiated playback devices.
        GT::HandleManager<HPlaybackDevice, OpenALDevice> m_playbackDevices;

        /// The handle manager for instantiated listenders.
        GT::HandleManager<HListener, OpenALListener> m_listeners;

        /// The handle manager for instantiated sounds.
        GT::HandleManager<HSound, OpenALSound> m_sounds;

        /// The handle manager for instantiated audio buffers.
        GT::HandleManager<HAudioBuffer, OpenALBuffer> m_audioBuffers;


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
