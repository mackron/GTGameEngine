// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "AudioEngine_OpenAL.hpp"
#include <GTEngine/Errors.hpp>

#include <cstdio>
#include <cstring>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array will be default initialized
#endif

#if defined(GT_PLATFORM_LINUX)
#include <dlfcn.h>
#endif // defined

namespace GTEngine
{
    struct OpenALDevice
    {
        OpenALDevice()
            : device(nullptr), context(nullptr),
              listeners(), sounds(),
              currentListener(0)
        {
        }


        /// The OpenAL device.
        ALCdevice* device;

        /// The OpenAL context.
        ALCcontext* context;


        /// The list of instantiated listeners that belong to this device.
        GTLib::Vector<ListenerHandle> listeners;

        /// The list of instantiated sounds that belong to this device.
        GTLib::Vector<SoundHandle> sounds;


        /// The current listener. We need to keep track of this so that when the listener is changed while current, it will be updated on the OpenAL side immediately.
        ListenerHandle currentListener;
        
        
    private:    // No copying.
        OpenALDevice(const OpenALDevice &);
        OpenALDevice & operator=(const OpenALDevice &);
    };

    struct OpenALListener
    {
        OpenALListener()
            : device(0), position()
        {
        }


        /// The device that owns this listener.
        PlaybackDeviceHandle device;

        /// The position of the listener.
        glm::vec3 position;
    };

    struct OpenALSound
    {
        OpenALSound()
            : device(0), source(0), buffers(), backBufferIndex(0)
        {
            buffers[0] = 0;
            buffers[1] = 0;
        }


        /// The device that owns this sound.
        PlaybackDeviceHandle device;

        /// The OpenAL source object.
        ALuint source;

        /// The two buffers that will be used for managing data flow for streaming.
        ALuint buffers[2];

        /// The index of the back buffer in 'buffers'. This will be zero or 1, and is swapped when the buffers swap.
        unsigned int backBufferIndex;
    };



    AudioEngine_OpenAL::AudioEngine_OpenAL()
        : m_library(nullptr),
          m_playbackDevices(), m_captureDevices(),
          m_instantiatedPlaybackDevices(), m_instantiatedCaptureDevices(),
          m_alcCreateContext(nullptr),
          m_alcDestroyContext(nullptr),
          m_alcMakeContextCurrent(nullptr),
          m_alcOpenDevice(nullptr),
          m_alcCloseDevice(nullptr),
          m_alcIsExtensionPresent(nullptr),
          m_alcGetString(nullptr),
          m_alListener3f(nullptr),
          m_alListenerfv(nullptr),
          m_alGenSources(nullptr),
          m_alDeleteSources(nullptr),
          m_alSource3f(nullptr),
          m_alSourcei(nullptr),
          m_alGetSourcefv(nullptr),
          m_alSourceQueueBuffers(nullptr),
          m_alSourceUnqueueBuffers(nullptr),
          m_alGenBuffers(nullptr),
          m_alDeleteBuffers(nullptr)
    {
    }

    AudioEngine_OpenAL::~AudioEngine_OpenAL()
    {
    }


    ///////////////////////////////////////////////////////
    // Virtual Methods

    bool AudioEngine_OpenAL::Startup()
    {
        if (this->LoadOpenALLibrary())
        {
            this->ExtractPlaybackDevices();
            this->ExtractCaptureDevices();

            return true;
        }

        return false;
    }

    void AudioEngine_OpenAL::Shutdown()
    {
        // Every playback device needs to be closed. This will in turn delete every listener and sound.
        while (m_instantiatedPlaybackDevices.GetCount() > 0)
        {
            this->ClosePlaybackDevice(m_instantiatedPlaybackDevices[0]);
        }


#if defined(GT_PLATFORM_WINDOWS)
        FreeLibrary(m_library);
        m_library = nullptr;
#endif

#if defined(GT_PLATFORM_LINUX)
        dlclose(m_library);
        m_library = nullptr;
#endif

        m_playbackDevices.Clear();
        m_captureDevices.Clear();

        m_alcMakeContextCurrent(nullptr);
    }



    size_t AudioEngine_OpenAL::GetPlaybackDeviceCount() const
    {
        return m_playbackDevices.GetCount();
    }

    AudioEngine::PlaybackDeviceInfo AudioEngine_OpenAL::GetPlaybackDeviceInfo(size_t deviceIndex) const
    {
        return m_playbackDevices[deviceIndex];
    }

    PlaybackDeviceHandle AudioEngine_OpenAL::OpenPlaybackDevice(size_t deviceIndex)
    {
        auto deviceAL = m_alcOpenDevice(m_playbackDevices[deviceIndex].name.c_str());
        if (deviceAL == nullptr)
        {
            GTEngine::PostError("AudioEngine_OpenAL - Failed to open device.");

            return 0;
        }

        auto contextAL = m_alcCreateContext(deviceAL, nullptr);
        if (contextAL == nullptr)
        {
            GTEngine::PostError("AudioEngine_OpenAL - Failed to create context.");

            m_alcCloseDevice(deviceAL);
            return 0;
        }

        auto deviceHandle = new OpenALDevice;
        deviceHandle->device  = deviceAL;
        deviceHandle->context = contextAL;

        m_instantiatedPlaybackDevices.PushBack(reinterpret_cast<size_t>(deviceHandle));

        return reinterpret_cast<size_t>(deviceHandle);
    }

    void AudioEngine_OpenAL::ClosePlaybackDevice(PlaybackDeviceHandle device)
    {
        auto deviceAL = reinterpret_cast<OpenALDevice*>(device);
        if (deviceAL != nullptr)
        {
            // Every associated listener needs to be deleted.
            while (deviceAL->listeners.GetCount() > 0)
            {
                this->DeleteListener(deviceAL->listeners[0]);
            }

            // Everyt associated sound needs to be deleted.
            while (deviceAL->sounds.GetCount() > 0)
            {
                this->DeleteSound(deviceAL->sounds[0]);
            }



            m_alcDestroyContext(deviceAL->context);
            m_alcCloseDevice(deviceAL->device);

            m_instantiatedPlaybackDevices.RemoveFirstOccuranceOf(device);
            delete deviceAL;
        }
    }



    ListenerHandle AudioEngine_OpenAL::CreateListener(PlaybackDeviceHandle device)
    {
        auto deviceAL = reinterpret_cast<OpenALDevice*>(device);
        if (deviceAL != nullptr)
        {
            auto listenerAL = new OpenALListener;
            listenerAL->device = device;

            deviceAL->listeners.PushBack(reinterpret_cast<size_t>(listenerAL));

            return reinterpret_cast<size_t>(listenerAL);
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteListener(ListenerHandle listener)
    {
        auto listenerAL = reinterpret_cast<OpenALListener*>(listener);
        if (listenerAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(listenerAL->device);
            assert(deviceAL != nullptr);
            {
                deviceAL->listeners.RemoveFirstOccuranceOf(listener);
            }

            delete listenerAL;
        }
    }

    void AudioEngine_OpenAL::SetListenerPosition(ListenerHandle listener, glm::vec3 position)
    {
        auto listenerAL = reinterpret_cast<OpenALListener*>(listener);
        if (listenerAL != nullptr)
        {
            listenerAL->position = position;

            auto deviceAL = reinterpret_cast<OpenALDevice*>(listenerAL->device);
            assert(deviceAL != nullptr);
            {
                if (deviceAL->currentListener == listener)
                {
                    m_alcMakeContextCurrent(deviceAL->context);
                    m_alListener3f(AL_POSITION, position.x, position.y, position.z);
                }
            }
        }
    }

    glm::vec3 AudioEngine_OpenAL::GetListenerPosition(ListenerHandle listener) const
    {
        auto listenerAL = reinterpret_cast<OpenALListener*>(listener);
        if (listenerAL != nullptr)
        {
            return listenerAL->position;
        }

        return glm::vec3(0, 0, 0);
    }

    

    SoundHandle AudioEngine_OpenAL::CreateSound(PlaybackDeviceHandle device)
    {
        auto deviceAL = reinterpret_cast<OpenALDevice*>(device);
        if (deviceAL != nullptr)
        {
            auto soundAL = new OpenALSound;
            soundAL->device = device;

            m_alcMakeContextCurrent(deviceAL->context);
            m_alGenSources(1, &soundAL->source);
            m_alGenBuffers(2, soundAL->buffers);


            deviceAL->sounds.PushBack(reinterpret_cast<SoundHandle>(soundAL));

            return reinterpret_cast<SoundHandle>(soundAL);
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteSound(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourceUnqueueBuffers(soundAL->source, 2, soundAL->buffers);
                m_alDeleteBuffers(2, soundAL->buffers);
                m_alDeleteSources(1, &soundAL->source);
            }

            delete soundAL;
        }
    }

    void AudioEngine_OpenAL::SetSoundPosition(SoundHandle sound, glm::vec3 position)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSource3f(soundAL->source, AL_POSITION, position.x, position.y, position.z);
            }
        }
    }

    glm::vec3 AudioEngine_OpenAL::GetSoundPosition(SoundHandle sound) const
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                
                ALfloat position[3];
                m_alGetSourcefv(soundAL->source, AL_POSITION, position);

                return glm::vec3(position[0], position[1], position[2]);
            }
        }

        return glm::vec3(0, 0, 0);
    }




    //////////////////////////////////////////////////////////
    // Private

    bool AudioEngine_OpenAL::LoadOpenALLibrary()
    {
#if defined(GT_PLATFORM_WINDOWS)
        m_library = LoadLibraryA("OpenAL32.dll");
        if (m_library == nullptr)
        {
            return false;
        }
#endif

#if defined(GT_PLATFORM_LINUX)
        m_library = dlopen("openal.so", RTLD_NOW | RTLD_GLOBAL);
        if (m_library == nullptr)
        {
            return false;
        }
#endif

        // ALC
        m_alcCreateContext      = reinterpret_cast<LPALCCREATECONTEXT     >(GetOpenALProc(m_library, "alcCreateContext"));
        m_alcDestroyContext     = reinterpret_cast<LPALCDESTROYCONTEXT    >(GetOpenALProc(m_library, "alcDestroyContext"));
        m_alcMakeContextCurrent = reinterpret_cast<LPALCMAKECONTEXTCURRENT>(GetOpenALProc(m_library, "alcMakeContextCurrent"));
        m_alcOpenDevice         = reinterpret_cast<LPALCOPENDEVICE        >(GetOpenALProc(m_library, "alcOpenDevice"));
        m_alcCloseDevice        = reinterpret_cast<LPALCCLOSEDEVICE       >(GetOpenALProc(m_library, "alcCloseDevice"));
        m_alcIsExtensionPresent = reinterpret_cast<LPALCISEXTENSIONPRESENT>(GetOpenALProc(m_library, "alcIsExtensionPresent"));
        m_alcGetString          = reinterpret_cast<LPALCGETSTRING         >(GetOpenALProc(m_library, "alcGetString"));

        // AL
        m_alListener3f           = reinterpret_cast<LPALLISTENER3F          >(GetOpenALProc(m_library, "alListener3f"));
        m_alListenerfv           = reinterpret_cast<LPALLISTENERFV          >(GetOpenALProc(m_library, "alListenerfv"));
        m_alGenSources           = reinterpret_cast<LPALGENSOURCES          >(GetOpenALProc(m_library, "alGenSources"));
        m_alDeleteSources        = reinterpret_cast<LPALDELETESOURCES       >(GetOpenALProc(m_library, "alDeleteSources"));
        m_alSource3f             = reinterpret_cast<LPALSOURCE3F            >(GetOpenALProc(m_library, "alSource3f"));
        m_alSourcei              = reinterpret_cast<LPALSOURCEI             >(GetOpenALProc(m_library, "alSourcei"));
        m_alGetSourcefv          = reinterpret_cast<LPALGETSOURCEFV         >(GetOpenALProc(m_library, "alGetSourcefv"));
        m_alSourceQueueBuffers   = reinterpret_cast<LPALSOURCEQUEUEBUFFERS  >(GetOpenALProc(m_library, "alSourceQueueBuffers"));
        m_alSourceUnqueueBuffers = reinterpret_cast<LPALSOURCEUNQUEUEBUFFERS>(GetOpenALProc(m_library, "alSourceUnqueueBuffers"));
        m_alGenBuffers           = reinterpret_cast<LPALGENBUFFERS          >(GetOpenALProc(m_library, "alGenBuffers"));
        m_alDeleteBuffers        = reinterpret_cast<LPALDELETEBUFFERS       >(GetOpenALProc(m_library, "alDeleteBuffers"));

        return true;
    }

    void AudioEngine_OpenAL::ExtractPlaybackDevices()
    {
        const char* playbackDeviceList = nullptr;
        if (m_alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
        {
            playbackDeviceList = m_alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
        }
        else
        {
            playbackDeviceList = m_alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        }


        size_t deviceNameLength;
        while ((deviceNameLength = strlen(playbackDeviceList)) > 0)
        {
            PlaybackDeviceInfo deviceInfo;
            deviceInfo.name = playbackDeviceList;

            m_playbackDevices.PushBack(deviceInfo);

            playbackDeviceList += deviceNameLength + 1;
        }
    }

    void AudioEngine_OpenAL::ExtractCaptureDevices()
    {
        const char* captureDeviceList = m_alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
        if (captureDeviceList != nullptr)
        {
            size_t deviceNameLength;
            while ((deviceNameLength = strlen(captureDeviceList)) > 0)
            {
                CaptureDeviceInfo deviceInfo;
                deviceInfo.name = captureDeviceList;

                m_captureDevices.PushBack(deviceInfo);

                captureDeviceList += deviceNameLength + 1;
            }
        }
    }



    ///////////////////////////////////////////
    // Private, Static

#if defined(GT_PLATFORM_WINDOWS)
    OpenALProc AudioEngine_OpenAL::GetOpenALProc(HMODULE library, const char* procName)
    {
        return reinterpret_cast<OpenALProc>(GetProcAddress(library, procName));
    }
#endif
#if defined(GT_PLATFORM_LINUX)
    OpenALProc AudioEngine_OpenAL::GetOpenALProc(void* library, const char* procName)
    {
        return reinterpret_cast<OpenALProc>(dlsym(library, procName));
    }
#endif
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
