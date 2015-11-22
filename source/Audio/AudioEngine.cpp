// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Audio/AudioEngine.hpp>

namespace GTEngine
{
    AudioEngine::AudioEngine()
        : m_pContext(nullptr)
    {
    }

    AudioEngine::~AudioEngine()
    {
    }


    bool AudioEngine::Startup()
    {
        // Return immediately if we're already initialized.
        if (m_pContext != nullptr) {
            return true;
        }

        m_pContext = easyaudio_create_context();
        return m_pContext != nullptr;
    }

    void AudioEngine::Shutdown()
    {
        easyaudio_delete_context(m_pContext);
        m_pContext = nullptr;
    }


    unsigned int AudioEngine::GetPlaybackDeviceCount() const
    {
        return easyaudio_get_output_device_count(m_pContext);
    }

    bool AudioEngine::GetPlaybackDeviceInfo(unsigned int deviceIndex, PlaybackDeviceInfo &infoOut) const
    {
        easyaudio_device_info info;
        if (easyaudio_get_output_device_info(m_pContext, deviceIndex, &info))
        {
            infoOut.name = info.description;
            return true;
        }

        return true;
    }


    HPlaybackDevice AudioEngine::OpenPlaybackDevice(unsigned int deviceIndex)
    {
        return reinterpret_cast<HPlaybackDevice>(easyaudio_create_output_device(m_pContext, deviceIndex));
    }

    void AudioEngine::ClosePlaybackDevice(HPlaybackDevice device)
    {
        easyaudio_delete_output_device(reinterpret_cast<easyaudio_device*>(device));
    }



    void AudioEngine::SetListenerPosition(HPlaybackDevice device, glm::vec3 position)
    {
        easyaudio_set_listener_position(reinterpret_cast<easyaudio_device*>(device), position.x, position.y, position.z);
    }

    glm::vec3 AudioEngine::GetListenerPosition(HPlaybackDevice device) const
    {
        float pos[3];
        easyaudio_get_listener_position(reinterpret_cast<easyaudio_device*>(device), pos);

        return glm::vec3(pos[0], pos[1], pos[2]);
    }

    void AudioEngine::SetListenerOrientation(HPlaybackDevice device, glm::vec3 forward, glm::vec3 up)
    {
        easyaudio_set_listener_orientation(reinterpret_cast<easyaudio_device*>(device), forward.x, forward.y, forward.z, up.x, up.y, up.z);
    }

    void AudioEngine::GetListenerOrientation(HPlaybackDevice device, glm::vec3 &forwardOut, glm::vec3 &upOut) const
    {
        float forward[3];
        float up[3];
        easyaudio_get_listener_orientation(reinterpret_cast<easyaudio_device*>(device), forward, up);

        forwardOut = glm::vec3(forward[0], forward[1], forward[2]);
        upOut = glm::vec3(up[0], up[1], up[2]);
    }



    HAudioBuffer AudioEngine::CreateBuffer(HPlaybackDevice device, easyaudio_buffer_desc &desc)
    {
        return easyaudio_create_buffer(reinterpret_cast<easyaudio_device*>(device), &desc, 0);
    }

    void AudioEngine::DeleteBuffer(HAudioBuffer buffer)
    {
        easyaudio_delete_buffer(reinterpret_cast<easyaudio_buffer*>(buffer));
    }

    void AudioEngine::SetBufferPosition(HAudioBuffer buffer, glm::vec3 position)
    {
        easyaudio_set_position(reinterpret_cast<easyaudio_buffer*>(buffer), position.x, position.y, position.z);
    }

    glm::vec3 AudioEngine::GetBufferPosition(HAudioBuffer buffer) const
    {
        float pos[3];
        easyaudio_get_position(reinterpret_cast<easyaudio_buffer*>(buffer), pos);

        return glm::vec3(pos[0], pos[1], pos[2]);
    }

    void AudioEngine::SetIsBufferPositionRelative(HAudioBuffer buffer, bool isRelative)
    {
        if (isRelative) {
            easyaudio_set_3d_mode(reinterpret_cast<easyaudio_buffer*>(buffer), easyaudio_3d_mode_relative);
        } else {
            easyaudio_set_3d_mode(reinterpret_cast<easyaudio_buffer*>(buffer), easyaudio_3d_mode_absolute);
        }
    }

    bool AudioEngine::IsBufferPositionRelative(HAudioBuffer buffer) const
    {
        return easyaudio_get_3d_mode(reinterpret_cast<easyaudio_buffer*>(buffer)) == easyaudio_3d_mode_relative;
    }

    void AudioEngine::PlayBuffer(HAudioBuffer buffer, bool loop)
    {
        easyaudio_play(reinterpret_cast<easyaudio_buffer*>(buffer), loop);
    }

    void AudioEngine::StopBuffer(HAudioBuffer buffer)
    {
        easyaudio_stop(reinterpret_cast<easyaudio_buffer*>(buffer));
    }

    void AudioEngine::PauseBuffer(HAudioBuffer buffer)
    {
        easyaudio_pause(reinterpret_cast<easyaudio_buffer*>(buffer));
    }

    void AudioEngine::RewindBuffer(HAudioBuffer buffer)
    {
        easyaudio_set_playback_position(reinterpret_cast<easyaudio_buffer*>(buffer), 0);
    }

    easyaudio_playback_state AudioEngine::GetBufferPlaybackState(HAudioBuffer buffer)
    {
        return easyaudio_get_playback_state(reinterpret_cast<easyaudio_buffer*>(buffer));
    }

    bool AudioEngine::IsBufferPlaying(HAudioBuffer buffer)
    {
        return easyaudio_get_playback_state(reinterpret_cast<easyaudio_buffer*>(buffer)) == easyaudio_playing;
    }

    bool AudioEngine::IsBufferPaused(HAudioBuffer buffer)
    {
        return easyaudio_get_playback_state(reinterpret_cast<easyaudio_buffer*>(buffer)) == easyaudio_paused;
    }

    bool AudioEngine::IsBufferStopped(HAudioBuffer buffer)
    {
        return easyaudio_get_playback_state(reinterpret_cast<easyaudio_buffer*>(buffer)) == easyaudio_stopped;
    }


    void AudioEngine::OnBufferStop(HAudioBuffer buffer, easyaudio_event_callback_proc callback, void* pUserData)
    {
        easyaudio_register_stop_callback(reinterpret_cast<easyaudio_buffer*>(buffer), callback, pUserData);
    }




    //// High-Level Helpers ////

    HAudioBuffer AudioEngine::CreateStreamingBuffer(HPlaybackDevice device, easyaudio_buffer_desc &desc, easyaudio_streaming_callbacks callbacks, void* pUserData)
    {
        return easyaudio_create_streaming_buffer(reinterpret_cast<easyaudio_device*>(device), &desc, callbacks, pUserData);
    }

    void AudioEngine::PlayStreamingBuffer(HAudioBuffer buffer, bool loop)
    {
        easyaudio_play_streaming_buffer(reinterpret_cast<easyaudio_buffer*>(buffer), loop);
    }
}