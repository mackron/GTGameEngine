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
              m_hBuffer(NULL),
              m_streamer(nullptr)
        {
        }

        Sound::~Sound()
        {
            m_engineContext.GetAudioSystem().DeleteBuffer(m_hBuffer);
            m_engineContext.GetAssetLibrary().CloseSoundStreamer(m_streamer);
        }



        GTEngine::HAudioBuffer Sound::GetAudioBuffer()
        {
            return m_hBuffer;
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
                m_hBuffer = m_engineContext.GetAudioSystem().CreateStreamingBuffer(m_engineContext.GetAudioPlaybackDevice(), desc, callbacks, m_streamer);
                if (m_hBuffer == nullptr)
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
                m_engineContext.GetAudioSystem().PlayStreamingBuffer(m_hBuffer, false);
            }
        }

        void Sound::Stop()
        {
            m_engineContext.GetAudioSystem().StopBuffer(m_hBuffer);
        }

        void Sound::Pause()
        {
            m_engineContext.GetAudioSystem().PauseBuffer(m_hBuffer);
        }


        void Sound::SetPosition(const glm::vec3 &position)
        {
            m_engineContext.GetAudioSystem().SetBufferPosition(m_hBuffer, position);
        }

        glm::vec3 Sound::GetPosition() const
        {
            return m_engineContext.GetAudioSystem().GetBufferPosition(m_hBuffer);
        }

        void Sound::SetIsPositionRelative(bool isRelative)
        {
            m_engineContext.GetAudioSystem().SetIsBufferPositionRelative(m_hBuffer, isRelative);
        }

        bool Sound::IsPositionRelative() const
        {
            return m_engineContext.GetAudioSystem().IsBufferPositionRelative(m_hBuffer);
        }


        easyaudio_playback_state Sound::GetPlaybackState() const
        {
            return m_engineContext.GetAudioSystem().GetBufferPlaybackState(m_hBuffer);
        }
    }
}
