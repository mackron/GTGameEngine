// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Audio/Sound.hpp>
#include <GTEngine/EngineContext.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Path.hpp>

#include "Streamers/SoundStreamer_WAV.hpp"
#include "Streamers/SoundStreamer_Vorbis.hpp"

namespace GT
{
    namespace Engine
    {
        ea_bool EA_Read(void* pUserData, void* pDataOut, unsigned int bytesToRead, unsigned int* bytesReadOut)
        {
            GTEngine::SoundStreamer* pStreamer = reinterpret_cast<GTEngine::SoundStreamer*>(pUserData);
            assert(pStreamer != NULL);

            return pStreamer->Read(pDataOut, bytesToRead, bytesReadOut);
        }

        ea_bool EA_Seek(void* pUserData, unsigned int offsetInBytesFromStart)
        {
            GTEngine::SoundStreamer* pStreamer = reinterpret_cast<GTEngine::SoundStreamer*>(pUserData);
            assert(pStreamer != NULL);

            return pStreamer->Seek(offsetInBytesFromStart);
        }


        Sound::Sound(EngineContext &engineContext)
            : m_engineContext(engineContext),
              m_pBuffer(nullptr),
              m_streamer(nullptr)
        {
        }

        Sound::~Sound()
        {
            easyaudio_delete_buffer(m_pBuffer);
            m_engineContext.GetAssetLibrary().CloseSoundStreamer(m_streamer);
        }



        easyaudio_buffer* Sound::GetAudioBuffer()
        {
            return m_pBuffer;
        }


        bool Sound::LoadFromFile(const char* relativeFilePath)
        {
            // If the sound is already playing, it needs to be stopped immediately.
            this->Stop();

            // The old streamer needs to be closed.
            if (m_streamer != nullptr)
            {
                m_engineContext.GetAssetLibrary().CloseSoundStreamer(m_streamer);
                m_streamer = nullptr;
            }


            m_streamer = m_engineContext.GetAssetLibrary().OpenSoundStreamer(relativeFilePath);
            if (m_streamer != nullptr)
            {
                easyaudio_buffer_desc desc;
                desc.flags         = 0;
                desc.format        = m_streamer->GetFormat();
                desc.channels      = m_streamer->GetNumChannels();
                desc.sampleRate    = m_streamer->GetSampleRate();
                desc.bitsPerSample = m_streamer->GetBitsPerSample();
                desc.sizeInBytes   = 0;
                desc.pInitialData  = nullptr;

                if (desc.channels == 1) {
                    desc.flags = EASYAUDIO_ENABLE_3D;
                }


                easyaudio_streaming_callbacks callbacks;
                callbacks.read = EA_Read;
                callbacks.seek = EA_Seek;
                m_pBuffer = easyaudio_create_streaming_buffer(m_engineContext.GetAudioPlaybackDevice(), &desc, callbacks, m_streamer);
                if (m_pBuffer == nullptr)
                {
                    m_engineContext.GetAssetLibrary().CloseSoundStreamer(m_streamer);
                    m_streamer = nullptr;

                    return false;
                }

                return true;
            }
            else
            {
                // Could not load the file.
                return false;
            }
        }

        void Sound::Play()
        {
            if (m_streamer != nullptr) {
                easyaudio_play_streaming_buffer(m_pBuffer, false);
            }
        }

        void Sound::Stop()
        {
            easyaudio_stop(m_pBuffer);
        }

        void Sound::Pause()
        {
            easyaudio_pause(m_pBuffer);
        }


        void Sound::SetPosition(const glm::vec3 &position)
        {
            easyaudio_set_position(m_pBuffer, position.x, position.y, position.z);
        }

        glm::vec3 Sound::GetPosition() const
        {
            float pos[3];
            easyaudio_get_position(m_pBuffer, pos);

            return glm::vec3(pos[0], pos[1], pos[2]);
        }

        void Sound::SetIsPositionRelative(bool isRelative)
        {
            if (isRelative) {
                easyaudio_set_3d_mode(m_pBuffer, easyaudio_3d_mode_relative);
            } else {
                easyaudio_set_3d_mode(m_pBuffer, easyaudio_3d_mode_absolute);
            }
        }

        bool Sound::IsPositionRelative() const
        {
            return easyaudio_get_3d_mode(m_pBuffer) == easyaudio_3d_mode_relative;
        }


        easyaudio_playback_state Sound::GetPlaybackState() const
        {
            return easyaudio_get_playback_state(m_pBuffer);
        }
    }
}
