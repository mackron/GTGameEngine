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
        OpenALDevice(HPlaybackDevice handleIn, ALCdevice* pDeviceALIn, ALCcontext* pContextALIn)
            : handle(handleIn),
              pDeviceAL(pDeviceALIn),
              pContextAL(pContextALIn),
              hCurrentListener(0)
        {
        }


        /// The object's handle. This is used for handle validation.
        HPlaybackDevice handle;


        /// The OpenAL hDevice.
        ALCdevice* pDeviceAL;

        /// The OpenAL context.
        ALCcontext* pContextAL;


        /// The current listener. We need to keep track of this so that when the listener is changed while current, it will be updated on the OpenAL side immediately.
        HListener hCurrentListener;


    private:    // No copying.
        OpenALDevice(const OpenALDevice &);
        OpenALDevice & operator=(const OpenALDevice &);
    };

    struct OpenALListener
    {
        OpenALListener(HListener handleIn, HPlaybackDevice hDeviceIn)
            : handle(handleIn), hDevice(hDeviceIn), position(), orientation()
        {
        }


        /// The object's handle. This is used for handle validation.
        HListener handle;

        /// The hDevice that owns this listener.
        HPlaybackDevice hDevice;

        /// The position of the listener.
        glm::vec3 position;

        /// The orientation of the listener.
        glm::quat orientation;
    };

    struct OpenALSound
    {
        OpenALSound(HSound handleIn, HPlaybackDevice hDeviceIn)
            : handle(handleIn), hDevice(hDeviceIn), source(0)
        {
        }


        /// The object's handle. This is used for handle validation.
        HSound handle;

        /// The hDevice that owns this hSound.
        HPlaybackDevice hDevice;

        /// The OpenAL source object.
        ALuint source;
    };

    struct OpenALBuffer
    {
        OpenALBuffer(HAudioBuffer handleIn, HPlaybackDevice hDeviceIn)
            : handle(handleIn), hDevice(hDeviceIn), hBuffer(0)
        {
        }


        /// The object's handle. This is used for handle validation.
        HAudioBuffer handle;

        /// The hDevice that owns this hBuffer.
        HPlaybackDevice hDevice;

        /// The OpenAL hBuffer object.
        ALuint hBuffer;
    };



    AudioEngine_OpenAL::AudioEngine_OpenAL()
        : m_library(nullptr),
          m_playbackDeviceInfos(), m_captureDeviceInfos(),
          //m_instantiatedPlaybackDevices(), m_instantiatedCaptureDevices(),
          m_playbackDevices(),
          m_listeners(),
          m_sounds(),
          m_audioBuffers(),
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
        // TODO: Every playback hDevice needs to be closed. This will in turn delete every listener and hSound.


#if defined(GT_PLATFORM_WINDOWS)
        FreeLibrary(m_library);
        m_library = nullptr;
#endif

#if defined(GT_PLATFORM_LINUX)
        dlclose(m_library);
        m_library = nullptr;
#endif

        m_playbackDeviceInfos.Clear();
        m_captureDeviceInfos.Clear();

        m_alcMakeContextCurrent(nullptr);
    }



    size_t AudioEngine_OpenAL::GetPlaybackDeviceCount() const
    {
        return m_playbackDeviceInfos.GetCount();
    }

    AudioEngine::PlaybackDeviceInfo AudioEngine_OpenAL::GetPlaybackDeviceInfo(size_t hDeviceIndex) const
    {
        return m_playbackDeviceInfos[hDeviceIndex];
    }

    HPlaybackDevice AudioEngine_OpenAL::OpenPlaybackDevice(size_t hDeviceIndex)
    {
        ALCdevice* pDeviceAL = m_alcOpenDevice(m_playbackDeviceInfos[hDeviceIndex].name.c_str());
        if (pDeviceAL == nullptr)
        {
            GTEngine::PostError("AudioEngine_OpenAL - Failed to open hDevice.");

            return 0;
        }

        ALCcontext* pContextAL = m_alcCreateContext(pDeviceAL, nullptr);
        if (pContextAL == nullptr)
        {
            GTEngine::PostError("AudioEngine_OpenAL - Failed to create context.");

            m_alcCloseDevice(pDeviceAL);
            return 0;
        }


        HPlaybackDevice handle = m_playbackDevices.CreateHandle();
        if (handle == 0)
        {
            GTEngine::PostError("AudioEngine_OpenAL - Failed to create hDevice handle.");

            m_alcDestroyContext(pContextAL);
            m_alcCloseDevice(pDeviceAL);
            return 0;
        }


        OpenALDevice* pDevice = new OpenALDevice(handle, pDeviceAL, pContextAL);
        m_playbackDevices.AssociateObjectWithHandle(handle, pDevice);

        //m_instantiatedPlaybackDevices.PushBack(reinterpret_cast<size_t>(hDeviceHandle));

        m_alcMakeContextCurrent(pContextAL);
        printf("%s\n", m_alGetString(AL_EXTENSIONS));

        return handle;
    }

    void AudioEngine_OpenAL::ClosePlaybackDevice(HPlaybackDevice hDevice)
    {
        auto pDevice = this->GetPlaybackDevicePtr(hDevice);
        if (pDevice != nullptr)
        {
            m_alcDestroyContext(pDevice->pContextAL);
            m_alcCloseDevice(pDevice->pDeviceAL);

            m_playbackDevices.DeleteHandle(pDevice->handle);
            delete pDevice;
        }
    }



    HListener AudioEngine_OpenAL::CreateListener(HPlaybackDevice hDevice)
    {
        auto pDevice = this->GetPlaybackDevicePtr(hDevice);
        if (pDevice != nullptr)
        {
            HListener handle = m_listeners.CreateHandle();
            if (handle != 0)
            {
                auto pListener = new OpenALListener(handle, hDevice);

                m_listeners.AssociateObjectWithHandle(handle, pListener);
                return handle;
            }
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteListener(HListener hListener)
    {
        auto pListener = this->GetListenerPtr(hListener);
        if (pListener != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(hListener);
            assert(pDevice != nullptr);
            {
            }

            m_listeners.DeleteHandle(hListener);
            delete pListener;
        }
    }

    void AudioEngine_OpenAL::SetListenerPosition(HListener hListener, glm::vec3 position)
    {
        auto pListener = this->GetListenerPtr(hListener);
        if (pListener != nullptr)
        {
            pListener->position = position;

            auto pDevice = this->GetPlaybackDevicePtr(pListener->hDevice);
            assert(pDevice != nullptr);
            {
                if (pDevice->hCurrentListener == hListener)
                {
                    m_alcMakeContextCurrent(pDevice->pContextAL);
                    m_alListener3f(AL_POSITION, position.x, position.y, position.z);
                }
            }
        }
    }

    glm::vec3 AudioEngine_OpenAL::GetListenerPosition(HListener hListener) const
    {
        auto pListener = this->GetListenerPtr(hListener);
        if (pListener != nullptr)
        {
            return pListener->position;
        }

        return glm::vec3(0, 0, 0);
    }

    void AudioEngine_OpenAL::SetListenerOrientation(HListener hListener, glm::quat orientation)
    {
        auto pListener = this->GetListenerPtr(hListener);
        if (pListener != nullptr)
        {
            pListener->orientation = orientation;

            auto pDevice = this->GetPlaybackDevicePtr(pListener->hDevice);
            assert(pDevice != nullptr);
            {
                if (pDevice->hCurrentListener == hListener)
                {
                    m_alcMakeContextCurrent(pDevice->pContextAL);


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

    glm::quat AudioEngine_OpenAL::GetListenerOrientation(HListener hListener) const
    {
        auto pListener = this->GetListenerPtr(hListener);
        if (pListener != nullptr)
        {
            return pListener->orientation;
        }

        return glm::quat();
    }





    HSound AudioEngine_OpenAL::CreateSound(HPlaybackDevice hDevice)
    {
        auto pDevice = this->GetPlaybackDevicePtr(hDevice);
        if (pDevice != nullptr)
        {
            HSound handle = m_sounds.CreateHandle();
            if (handle != 0)
            {
                auto pSound = new OpenALSound(handle, hDevice);

                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alGenSources(1, &pSound->source);

                m_sounds.AssociateObjectWithHandle(handle, pSound);
                return handle;
            }
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteSound(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alDeleteSources(1, &pSound->source);
            }

            m_sounds.DeleteHandle(hSound);
            delete pSound;
        }
    }

    bool AudioEngine_OpenAL::IsValidSound(HSound hSound) const
    {
        return this->GetSoundPtr(hSound) != nullptr;
    }

    void AudioEngine_OpenAL::SetSoundPosition(HSound hSound, glm::vec3 position)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSource3f(pSound->source, AL_POSITION, position.x, position.y, position.z);
            }
        }
    }

    glm::vec3 AudioEngine_OpenAL::GetSoundPosition(HSound hSound) const
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALfloat position[3];
                m_alGetSourcefv(pSound->source, AL_POSITION, position);

                return glm::vec3(position[0], position[1], position[2]);
            }
        }

        return glm::vec3(0, 0, 0);
    }

    void AudioEngine_OpenAL::SetIsSoundPositionRelative(HSound hSound, bool isRelative)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourcei(pSound->source, AL_SOURCE_RELATIVE, static_cast<ALint>(isRelative));
            }
        }
    }

    bool AudioEngine_OpenAL::IsSoundPositionRelative(HSound hSound) const
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            ALint isRelative;
            m_alGetSourcei(pSound->source, AL_SOURCE_RELATIVE, &isRelative);

            return isRelative != 0;
        }

        return false;
    }

    void AudioEngine_OpenAL::QueueAudioBuffer(HSound hSound, HAudioBuffer hBuffer)
    {
        auto pSound  = this->GetSoundPtr(hSound);
        auto pBuffer = this->GetAudioBufferPtr(hBuffer);

        if (pSound != nullptr && pBuffer != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourceQueueBuffers(pSound->source, 1, &pBuffer->hBuffer);
            }
        }
    }

    void AudioEngine_OpenAL::UnqueueAudioBuffer(HSound hSound)
    {
        auto pSound  = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALuint devnull;
                m_alSourceUnqueueBuffers(pSound->source, 1, &devnull);
            }
        }
    }

    unsigned int AudioEngine_OpenAL::GetQueuedAudioBufferCount(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALint count = 0;
                m_alGetSourcei(pSound->source, AL_BUFFERS_QUEUED, &count);

                return static_cast<unsigned int>(count);
            }
        }

        return 0;
    }

    unsigned int AudioEngine_OpenAL::GetProcessedQueuedAudioBufferCount(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALint count = 0;
                m_alGetSourcei(pSound->source, AL_BUFFERS_PROCESSED, &count);

                return static_cast<unsigned int>(count);
            }
        }

        return 0;
    }

    void AudioEngine_OpenAL::PlaySound(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourcePlay(pSound->source);
            }
        }
    }

    void AudioEngine_OpenAL::StopSound(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourceStop(pSound->source);
            }
        }
    }

    void AudioEngine_OpenAL::PauseSound(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourcePause(pSound->source);
            }
        }
    }

    void AudioEngine_OpenAL::RewindSound(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alSourceRewind(pSound->source);
            }
        }
    }

    bool AudioEngine_OpenAL::IsSoundPlaying(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALint state = 0;
                m_alGetSourcei(pSound->source, AL_SOURCE_STATE, &state);

                return state == AL_PLAYING;
            }
        }

        return false;
    }

    bool AudioEngine_OpenAL::IsSoundPaused(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALint state = 0;
                m_alGetSourcei(pSound->source, AL_SOURCE_STATE, &state);

                return state == AL_PAUSED;
            }
        }

        return false;
    }

    bool AudioEngine_OpenAL::IsSoundStopped(HSound hSound)
    {
        auto pSound = this->GetSoundPtr(hSound);
        if (pSound != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pSound->hDevice);
            if (pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

                ALint state = 0;
                m_alGetSourcei(pSound->source, AL_SOURCE_STATE, &state);

                return state == AL_STOPPED;
            }
        }

        return true;
    }



    HAudioBuffer AudioEngine_OpenAL::CreateAudioBuffer(HPlaybackDevice hDevice)
    {
        auto pDevice = this->GetPlaybackDevicePtr(hDevice);
        if (pDevice != nullptr)
        {
            HAudioBuffer handle = m_audioBuffers.CreateHandle();
            if (handle != 0)
            {
                auto pBuffer = new OpenALBuffer(handle, hDevice);

                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alGenBuffers(1, &pBuffer->hBuffer);

                m_audioBuffers.AssociateObjectWithHandle(handle, pBuffer);
                return handle;
            }
        }

        return 0;
    }

    void AudioEngine_OpenAL::DeleteAudioBuffer(HAudioBuffer hBuffer)
    {
        auto pBuffer = this->GetAudioBufferPtr(hBuffer);
        if (pBuffer != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pBuffer->hDevice);
            if(pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);
                m_alDeleteBuffers(1, &pBuffer->hBuffer);
            }

            m_audioBuffers.DeleteHandle(hBuffer);
            delete pBuffer;
        }
    }

    void AudioEngine_OpenAL::SetAudioBufferData(HAudioBuffer hBuffer, const void *data, size_t dataSizeInBytes, AudioDataFormat format, unsigned int frequency)
    {
        auto pBuffer = this->GetAudioBufferPtr(hBuffer);
        if (pBuffer != nullptr)
        {
            auto pDevice = this->GetPlaybackDevicePtr(pBuffer->hDevice);
            if(pDevice != nullptr)
            {
                m_alcMakeContextCurrent(pDevice->pContextAL);

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

                    m_alBufferData(pBuffer->hBuffer, (format == AudioDataFormat_Mono24) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if ((format == AudioDataFormat_Mono32F || format == AudioDataFormat_Stereo32F) && !m_alIsExtensionPresent("AL_EXT_FLOAT32"))
                {
                    // We don't natively support 32-bit float formats, so we'll need to convert to 16-bit.
                    size_t   newDataSizeInBytes = (dataSizeInBytes / 4) * 2;
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < (dataSizeInBytes / 4); ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(reinterpret_cast<const float*>(data)[iSample] * 65536.0f);
                    }

                    m_alBufferData(pBuffer->hBuffer, (format == AudioDataFormat_Mono32F) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if ((format == AudioDataFormat_Mono64F || format == AudioDataFormat_Stereo64F) && !m_alIsExtensionPresent("AL_EXT_DOUBLE"))
                {
                    // We don't natively support 64-bit float formats, so we'll need to convert to 16-bit.
                    size_t   newDataSizeInBytes = (dataSizeInBytes / 8) * 2;
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < (dataSizeInBytes / 8); ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(reinterpret_cast<const double*>(data)[iSample] * 65536.0f);
                    }

                    m_alBufferData(pBuffer->hBuffer, (format == AudioDataFormat_Mono64F) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if ((format == AudioDataFormat_Mono_ALaw || format == AudioDataFormat_Stereo_ALaw) && !m_alIsExtensionPresent("AL_EXT_ALAW"))
                {
                    // In this case we don't natively support A-Law.
                    size_t   newDataSizeInBytes = dataSizeInBytes * 2;    // <-- expanding from 8-bit compressed to 16-bit decompressed.
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < dataSizeInBytes; ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(alaw2linear(reinterpret_cast<const unsigned char*>(data)[iSample]));
                    }

                    m_alBufferData(pBuffer->hBuffer, (format == AudioDataFormat_Mono_ALaw) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else if ((format == AudioDataFormat_Mono_ULaw || format == AudioDataFormat_Stereo_ULaw) && !m_alIsExtensionPresent("AL_EXT_MULAW"))
                {
                    // In this case we don't natively support A-Law.
                    size_t   newDataSizeInBytes = dataSizeInBytes * 2;    // <-- expanding from 8-bit compressed to 16-bit decompressed.
                    int16_t* newData = static_cast<int16_t*>(malloc(newDataSizeInBytes));

                    for (size_t iSample = 0; iSample < dataSizeInBytes; ++iSample)
                    {
                        newData[iSample] = static_cast<int16_t>(ulaw2linear(reinterpret_cast<const unsigned char*>(data)[iSample]));
                    }

                    m_alBufferData(pBuffer->hBuffer, (format == AudioDataFormat_Mono_ULaw) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, newData, static_cast<ALsizei>(newDataSizeInBytes), static_cast<ALsizei>(frequency));

                    free(newData);
                }
                else
                {
                    m_alBufferData(pBuffer->hBuffer, ToOpenALDataFormat(format), data, static_cast<ALsizei>(dataSizeInBytes), static_cast<ALsizei>(frequency));
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


        size_t hDeviceNameLength;
        while ((hDeviceNameLength = strlen(playbackDeviceList)) > 0)
        {
            PlaybackDeviceInfo hDeviceInfo;
            hDeviceInfo.name = playbackDeviceList;

            m_playbackDeviceInfos.PushBack(hDeviceInfo);

            playbackDeviceList += hDeviceNameLength + 1;
        }
    }

    void AudioEngine_OpenAL::ExtractCaptureDevices()
    {
        const char* captureDeviceList = m_alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
        if (captureDeviceList != nullptr)
        {
            size_t hDeviceNameLength;
            while ((hDeviceNameLength = strlen(captureDeviceList)) > 0)
            {
                CaptureDeviceInfo hDeviceInfo;
                hDeviceInfo.name = captureDeviceList;

                m_captureDeviceInfos.PushBack(hDeviceInfo);

                captureDeviceList += hDeviceNameLength + 1;
            }
        }
    }

    OpenALDevice* AudioEngine_OpenAL::GetPlaybackDevicePtr(HPlaybackDevice hPlaybackDevice) const
    {
        auto pObject = m_playbackDevices.GetAssociatedObject(hPlaybackDevice);
        if (pObject != nullptr)
        {
            if (hPlaybackDevice == pObject->handle)
            {
                return pObject;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }

    OpenALListener* AudioEngine_OpenAL::GetListenerPtr(HListener hListener) const
    {
        auto pObject = m_listeners.GetAssociatedObject(hListener);
        if (pObject != nullptr)
        {
            if (hListener == pObject->handle)
            {
                return pObject;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }

    OpenALSound* AudioEngine_OpenAL::GetSoundPtr(HSound hSound) const
    {
        auto pObject = m_sounds.GetAssociatedObject(hSound);
        if (pObject != nullptr)
        {
            if (hSound == pObject->handle)
            {
                return pObject;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
        }
    }

    OpenALBuffer* AudioEngine_OpenAL::GetAudioBufferPtr(HAudioBuffer hAudioBuffer) const
    {
        auto pObject = m_audioBuffers.GetAssociatedObject(hAudioBuffer);
        if (pObject != nullptr)
        {
            if (hAudioBuffer == pObject->handle)
            {
                return pObject;
            }
            else
            {
                // Invalid handle (uniqueness IDs do not match).
                return nullptr;
            }
        }
        else
        {
            // Invalid handle (deleted or null).
            return nullptr;
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
