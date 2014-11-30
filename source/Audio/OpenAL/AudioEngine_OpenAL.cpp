// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "AudioEngine_OpenAL.hpp"
#include "../g711.h"

#include <GTEngine/Errors.hpp>
#include <GTEngine/GTEngine.hpp>

#include <cstdio>
#include <cstring>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behavior: elements of array will be default initialized
#endif

#if defined(GT_PLATFORM_LINUX)
#include <dlfcn.h>
#endif // defined

#define AL_FORMAT_MONO_FLOAT32      0x10010
#define AL_FORMAT_STEREO_FLOAT32    0x10011

#define AL_FORMAT_MONO_DOUBLE_EXT   0x10012
#define AL_FORMAT_STEREO_DOUBLE_EXT 0x10013

#define AL_FORMAT_MONO_MULAW_EXT    0x10014
#define AL_FORMAT_STEREO_MULAW_EXT  0x10015

#define AL_FORMAT_MONO_ALAW_EXT     0x10016
#define AL_FORMAT_STEREO_ALAW_EXT   0x10017


namespace GTEngine
{
    ALenum ToOpenALDataFormat(AudioDataFormat format)
    {
        switch (format)
        {
        case AudioDataFormat_Mono8:       return AL_FORMAT_MONO8;
        case AudioDataFormat_Stereo8:     return AL_FORMAT_STEREO8;
        
        case AudioDataFormat_Mono16:      return AL_FORMAT_MONO16;
        case AudioDataFormat_Stereo16:    return AL_FORMAT_STEREO16;
        
        case AudioDataFormat_Mono32F:     return AL_FORMAT_MONO_FLOAT32;
        case AudioDataFormat_Stereo32F:   return AL_FORMAT_STEREO_FLOAT32;

        case AudioDataFormat_Mono64F:     return AL_FORMAT_MONO_DOUBLE_EXT;
        case AudioDataFormat_Stereo64F:   return AL_FORMAT_STEREO_DOUBLE_EXT;

        case AudioDataFormat_Mono_ALaw:   return AL_FORMAT_MONO_ALAW_EXT;
        case AudioDataFormat_Stereo_ALaw: return AL_FORMAT_STEREO_ALAW_EXT;

        case AudioDataFormat_Mono_ULaw:   return AL_FORMAT_MONO_MULAW_EXT;
        case AudioDataFormat_Stereo_ULaw: return AL_FORMAT_STEREO_MULAW_EXT;

        case AudioDataFormat_Mono24:
        case AudioDataFormat_Stereo24:
        default: break;
        }

        return AL_FORMAT_MONO8;
    }


    struct OpenALDevice
    {
        OpenALDevice()
            : device(nullptr), context(nullptr),
              currentListener(0)
        {
        }


        /// The OpenAL device.
        ALCdevice* device;

        /// The OpenAL context.
        ALCcontext* context;


        /// The current listener. We need to keep track of this so that when the listener is changed while current, it will be updated on the OpenAL side immediately.
        ListenerHandle currentListener;
        
        
    private:    // No copying.
        OpenALDevice(const OpenALDevice &);
        OpenALDevice & operator=(const OpenALDevice &);
    };

    struct OpenALListener
    {
        OpenALListener()
            : device(0), position(), orientation()
        {
        }


        /// The device that owns this listener.
        PlaybackDeviceHandle device;

        /// The position of the listener.
        glm::vec3 position;

        /// The orientation of the listener.
        glm::quat orientation;
    };

    struct OpenALSound
    {
        OpenALSound()
            : device(0), source(0)
        {
        }


        /// The device that owns this sound.
        PlaybackDeviceHandle device;

        /// The OpenAL source object.
        ALuint source;
    };

    struct OpenALBuffer
    {
        OpenALBuffer()
            : device(0), buffer(0)
        {
        }


        /// The device that owns this buffer.
        PlaybackDeviceHandle device;

        /// The OpenAL buffer object.
        ALuint buffer;
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
          m_alGetString(nullptr),
          m_alIsExtensionPresent(nullptr),
          m_alListener3f(nullptr),
          m_alListenerfv(nullptr),
          m_alGenSources(nullptr),
          m_alDeleteSources(nullptr),
          m_alSource3f(nullptr),
          m_alSourcei(nullptr),
          m_alGetSourcefv(nullptr),
          m_alSourceQueueBuffers(nullptr),
          m_alSourceUnqueueBuffers(nullptr),
          m_alSourcePlay(nullptr),
          m_alSourceStop(nullptr),
          m_alSourceRewind(nullptr),
          m_alSourcePause(nullptr),
          m_alGenBuffers(nullptr),
          m_alDeleteBuffers(nullptr),
          m_alBufferData(nullptr)
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

        m_alcMakeContextCurrent(contextAL);
        printf("%s\n", m_alGetString(AL_EXTENSIONS));

        return reinterpret_cast<size_t>(deviceHandle);
    }

    void AudioEngine_OpenAL::ClosePlaybackDevice(PlaybackDeviceHandle device)
    {
        auto deviceAL = reinterpret_cast<OpenALDevice*>(device);
        if (deviceAL != nullptr)
        {
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

    void AudioEngine_OpenAL::SetListenerOrientation(ListenerHandle listener, glm::quat orientation)
    {
        auto listenerAL = reinterpret_cast<OpenALListener*>(listener);
        if (listenerAL != nullptr)
        {
            listenerAL->orientation = orientation;

            auto deviceAL = reinterpret_cast<OpenALDevice*>(listenerAL->device);
            assert(deviceAL != nullptr);
            {
                if (deviceAL->currentListener == listener)
                {
                    m_alcMakeContextCurrent(deviceAL->context);


                    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
                    glm::vec3 up      = orientation * glm::vec3(0.0f, 1.0f,  0.0f);

                    ALfloat orientation[6];
                    orientation[0] = forward.x;
                    orientation[1] = forward.y;
                    orientation[2] = forward.z;
                    orientation[3] = up.x;
                    orientation[4] = up.y;
                    orientation[5] = up.z;

                    m_alListenerfv(AL_ORIENTATION, orientation);
                }
            }
        }
    }

    glm::quat AudioEngine_OpenAL::GetListenerOrientation(ListenerHandle listener) const
    {
        auto listenerAL = reinterpret_cast<OpenALListener*>(listener);
        if (listenerAL != nullptr)
        {
            return listenerAL->orientation;
        }

        return glm::quat();
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

    void AudioEngine_OpenAL::SetIsSoundPositionRelative(SoundHandle sound, bool isRelative)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourcei(soundAL->source, AL_SOURCE_RELATIVE, static_cast<ALint>(isRelative));
            }
        }
    }

    bool AudioEngine_OpenAL::IsSoundPositionRelative(SoundHandle sound) const
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            ALint isRelative;
            m_alGetSourcei(soundAL->source, AL_SOURCE_RELATIVE, &isRelative);

            return isRelative != 0;
        }

        return false;
    }

    void AudioEngine_OpenAL::QueueAudioBuffer(SoundHandle sound, AudioBufferHandle buffer)
    {
        auto soundAL  = reinterpret_cast<OpenALSound*>(sound);
        auto bufferAL = reinterpret_cast<OpenALBuffer*>(buffer);

        if (soundAL != nullptr && bufferAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourceQueueBuffers(soundAL->source, 1, &bufferAL->buffer);
            }
        }
    }

    void AudioEngine_OpenAL::UnqueueAudioBuffer(SoundHandle sound)
    {
        auto soundAL  = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);

                ALuint devnull;
                m_alSourceUnqueueBuffers(soundAL->source, 1, &devnull);
            }
        }
    }

    unsigned int AudioEngine_OpenAL::GetQueuedAudioBufferCount(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                
                ALint count = 0;
                m_alGetSourcei(soundAL->source, AL_BUFFERS_QUEUED, &count);

                return static_cast<unsigned int>(count);
            }
        }

        return 0;
    }

    unsigned int AudioEngine_OpenAL::GetProcessedQueuedAudioBufferCount(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                
                ALint count = 0;
                m_alGetSourcei(soundAL->source, AL_BUFFERS_PROCESSED, &count);

                return static_cast<unsigned int>(count);
            }
        }

        return 0;
    }

    void AudioEngine_OpenAL::PlaySound(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourcePlay(soundAL->source);
            }
        }
    }

    void AudioEngine_OpenAL::StopSound(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourceStop(soundAL->source);
            }
        }
    }

    void AudioEngine_OpenAL::PauseSound(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourcePause(soundAL->source);
            }
        }
    }

    void AudioEngine_OpenAL::RewindSound(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alSourceRewind(soundAL->source);
            }
        }
    }

    bool AudioEngine_OpenAL::IsSoundPlaying(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);

                ALint state = 0;
                m_alGetSourcei(soundAL->source, AL_SOURCE_STATE, &state);

                return state == AL_PLAYING;
            }
        }

        return false;
    }

    bool AudioEngine_OpenAL::IsSoundPaused(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);

                ALint state = 0;
                m_alGetSourcei(soundAL->source, AL_SOURCE_STATE, &state);

                return state == AL_PAUSED;
            }
        }

        return false;
    }

    bool AudioEngine_OpenAL::IsSoundStopped(SoundHandle sound)
    {
        auto soundAL = reinterpret_cast<OpenALSound*>(sound);
        if (soundAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(soundAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);

                ALint state = 0;
                m_alGetSourcei(soundAL->source, AL_SOURCE_STATE, &state);

                return state == AL_STOPPED;
            }
        }

        return false;
    }



    AudioBufferHandle AudioEngine_OpenAL::CreateAudioBuffer(PlaybackDeviceHandle device)
    {
        auto deviceAL = reinterpret_cast<OpenALDevice*>(device);
        if (deviceAL != nullptr)
        {
            auto bufferAL = new OpenALBuffer;
            bufferAL->device = device;

            m_alcMakeContextCurrent(deviceAL->context);
            m_alGenBuffers(1, &bufferAL->buffer);

            return reinterpret_cast<SoundHandle>(bufferAL);
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteAudioBuffer(AudioBufferHandle buffer)
    {
        auto bufferAL = reinterpret_cast<OpenALBuffer*>(buffer);
        if (bufferAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(bufferAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);
                m_alDeleteBuffers(1, &bufferAL->buffer);
            }

            delete bufferAL;
        }
    }

    void AudioEngine_OpenAL::SetAudioBufferData(AudioBufferHandle buffer, const void *data, size_t dataSizeInBytes, AudioDataFormat format, unsigned int frequency)
    {
        auto bufferAL = reinterpret_cast<OpenALBuffer*>(buffer);
        if (bufferAL != nullptr)
        {
            auto deviceAL = reinterpret_cast<OpenALDevice*>(bufferAL->device);
            assert(deviceAL != nullptr);
            {
                m_alcMakeContextCurrent(deviceAL->context);

                // OpenAL does not have reliable 24-bit support. We're going to downscale this to 16-bit if applicable.
                if (format == AudioDataFormat_Mono24 || format == AudioDataFormat_Stereo24)
                {
                    size_t   newDataSizeInBytes = (dataSizeInBytes / 3) * 2;     // <-- Going from 3-byte samples to 2-bytes.
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < (dataSizeInBytes / 3); ++iSample)
                    {
                        const uint8_t* samplePtr = reinterpret_cast<const uint8_t*>(data) + (iSample * 3);

                        uint32_t s0 = samplePtr[0];
                        uint32_t s1 = samplePtr[1];
                        uint32_t s2 = samplePtr[2];

                        int sample32 = ((s0 << 0) | (s1 << 8) | (s2 << 16));
                        if (sample32 & 0x800000)
                        {
                            sample32 |= ~0xffffff;
                        }

                        int16_t sample16 = static_cast<int16_t>(GTLib::Round((sample32 / 16777216.0f) * 65536.0f));

                        newData[iSample] = sample16;
                    }

                    m_alBufferData(bufferAL->buffer, (format == AudioDataFormat_Mono24) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if (format == AudioDataFormat_Mono32F || format == AudioDataFormat_Stereo32F && !m_alIsExtensionPresent("AL_EXT_FLOAT32"))
                {
                    // We don't natively support 32-bit float formats, so we'll need to convert to 16-bit.
                    size_t   newDataSizeInBytes = (dataSizeInBytes / 4) * 2;
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));
                    
                    for (size_t iSample = 0; iSample < (dataSizeInBytes / 4); ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(reinterpret_cast<const float*>(data)[iSample] * 65536.0f);
                    }

                    m_alBufferData(bufferAL->buffer, (format == AudioDataFormat_Mono32F) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if (format == AudioDataFormat_Mono64F || format == AudioDataFormat_Stereo64F && !m_alIsExtensionPresent("AL_EXT_DOUBLE"))
                {
                    // We don't natively support 64-bit float formats, so we'll need to convert to 16-bit.
                    size_t   newDataSizeInBytes = (dataSizeInBytes / 8) * 2;
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));
                    
                    for (size_t iSample = 0; iSample < (dataSizeInBytes / 8); ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(reinterpret_cast<const double*>(data)[iSample] * 65536.0f);
                    }

                    m_alBufferData(bufferAL->buffer, (format == AudioDataFormat_Mono64F) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if (format == AudioDataFormat_Mono_ALaw || format == AudioDataFormat_Stereo_ALaw && !m_alIsExtensionPresent("AL_EXT_ALAW"))
                {
                    // In this case we don't natively support A-Law.
                    size_t   newDataSizeInBytes = dataSizeInBytes * 2;    // <-- expanding from 8-bit compressed to 16-bit decompressed.
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < dataSizeInBytes; ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(alaw2linear(reinterpret_cast<const unsigned char*>(data)[iSample]));
                    }

                    m_alBufferData(bufferAL->buffer, (format == AudioDataFormat_Mono_ALaw) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if (format == AudioDataFormat_Mono_ULaw || format == AudioDataFormat_Stereo_ULaw && !m_alIsExtensionPresent("AL_EXT_MULAW"))
                {
                    // In this case we don't natively support A-Law.
                    size_t   newDataSizeInBytes = dataSizeInBytes * 2;    // <-- expanding from 8-bit compressed to 16-bit decompressed.
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < dataSizeInBytes; ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(ulaw2linear(reinterpret_cast<const unsigned char*>(data)[iSample]));
                    }

                    m_alBufferData(bufferAL->buffer, (format == AudioDataFormat_Mono_ULaw) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else
                {
                    m_alBufferData(bufferAL->buffer, ToOpenALDataFormat(format), data, static_cast<ALsizei>(dataSizeInBytes), static_cast<ALsizei>(frequency));
                }
            }
        }
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
        m_alGetString            = reinterpret_cast<LPALGETSTRING           >(GetOpenALProc(m_library, "alGetString"));
        m_alIsExtensionPresent   = reinterpret_cast<LPALISEXTENSIONPRESENT  >(GetOpenALProc(m_library, "alIsExtensionPresent"));
        m_alListener3f           = reinterpret_cast<LPALLISTENER3F          >(GetOpenALProc(m_library, "alListener3f"));
        m_alListenerfv           = reinterpret_cast<LPALLISTENERFV          >(GetOpenALProc(m_library, "alListenerfv"));
        m_alGenSources           = reinterpret_cast<LPALGENSOURCES          >(GetOpenALProc(m_library, "alGenSources"));
        m_alDeleteSources        = reinterpret_cast<LPALDELETESOURCES       >(GetOpenALProc(m_library, "alDeleteSources"));
        m_alSource3f             = reinterpret_cast<LPALSOURCE3F            >(GetOpenALProc(m_library, "alSource3f"));
        m_alSourcei              = reinterpret_cast<LPALSOURCEI             >(GetOpenALProc(m_library, "alSourcei"));
        m_alGetSourcefv          = reinterpret_cast<LPALGETSOURCEFV         >(GetOpenALProc(m_library, "alGetSourcefv"));
        m_alGetSourcei           = reinterpret_cast<LPALGETSOURCEI          >(GetOpenALProc(m_library, "alGetSourcei"));
        m_alSourceQueueBuffers   = reinterpret_cast<LPALSOURCEQUEUEBUFFERS  >(GetOpenALProc(m_library, "alSourceQueueBuffers"));
        m_alSourceUnqueueBuffers = reinterpret_cast<LPALSOURCEUNQUEUEBUFFERS>(GetOpenALProc(m_library, "alSourceUnqueueBuffers"));
        m_alSourcePlay           = reinterpret_cast<LPALSOURCEPLAY          >(GetOpenALProc(m_library, "alSourcePlay"));
        m_alSourceStop           = reinterpret_cast<LPALSOURCESTOP          >(GetOpenALProc(m_library, "alSourceStop"));
        m_alSourceRewind         = reinterpret_cast<LPALSOURCEREWIND        >(GetOpenALProc(m_library, "alSourceRewind"));
        m_alSourcePause          = reinterpret_cast<LPALSOURCEPAUSE         >(GetOpenALProc(m_library, "alSourcePause"));
        m_alGenBuffers           = reinterpret_cast<LPALGENBUFFERS          >(GetOpenALProc(m_library, "alGenBuffers"));
        m_alDeleteBuffers        = reinterpret_cast<LPALDELETEBUFFERS       >(GetOpenALProc(m_library, "alDeleteBuffers"));
        m_alBufferData           = reinterpret_cast<LPALBUFFERDATA          >(GetOpenALProc(m_library, "alBufferData"));

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
