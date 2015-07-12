// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Audio/Sound.hpp>
#include <GTEngine/EngineContext.hpp>
#include <GTLib/String.hpp>
#include <GTLib/Path.hpp>

#include "Streamers/SoundStreamer_WAV.hpp"
#include "Streamers/SoundStreamer_Vorbis.hpp"

#pragma warning(push)
#pragma warning(disable: 4351)      // Buffers will be default initialized.

namespace GT
{
    namespace Engine
    {
        Sound::Sound(EngineContext &engineContext)
            : m_engineContext(engineContext),
              m_hSound(0),
              m_hBuffers(),
              m_streamer(nullptr),
              m_playbackThread(nullptr),
              m_playbackThreadEventHandler(*this),
              m_playbackThreadProc(engineContext, m_playbackThreadEventHandler),
              m_playbackState(SoundPlaybackState_Stopped)
        {
            m_hSound      = engineContext.GetAudioSystem().CreateSound(engineContext.GetAudioPlaybackDevice());
            m_hBuffers[0] = engineContext.GetAudioSystem().CreateAudioBuffer(engineContext.GetAudioPlaybackDevice());
            m_hBuffers[1] = engineContext.GetAudioSystem().CreateAudioBuffer(engineContext.GetAudioPlaybackDevice());
        }

        Sound::~Sound()
        {
            this->Stop();
            m_engineContext.GetAudioSystem().DeleteSound(m_hSound);
            m_engineContext.GetAssetLibrary().CloseSoundStreamer(m_streamer);

            m_engineContext.UnacquireThread(m_playbackThread);
        }


        GTEngine::HSound Sound::GetSoundHandle()
        {
            return m_hSound;
        }

        GTEngine::HAudioBuffer* Sound::GetAudioBuffers()
        {
            return m_hBuffers;
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
            if (m_streamer != nullptr)
            {
                switch (m_playbackState)
                {
                case SoundPlaybackState_Stopped:
                    {
                        if (m_playbackThread == nullptr)
                        {
                            m_playbackThread = m_engineContext.AcquireThread();
                        }

                        m_playbackThreadProc.Initialize(m_hSound, m_hBuffers, *m_streamer);
                        m_playbackThread->Start(m_playbackThreadProc);

                        this->OnStart();

                        break;
                    }

                case SoundPlaybackState_Paused:
                    {
                        m_playbackThreadProc.Resume();

                        this->OnResume();

                        break;
                    }

                case SoundPlaybackState_Playing:
                default:
                    {
                        // Don't do anything in the default case.
                        break;
                    }
                }


                // Make sure the playback state is set correctly.
                m_playbackState = SoundPlaybackState_Playing;
            }
        }

        void Sound::Stop()
        {
            if (m_playbackState != SoundPlaybackState_Stopped)
            {
                m_playbackState = SoundPlaybackState_Stopped;
                m_playbackThreadProc.Stop();

                this->OnStop();
            }
        }

        void Sound::Pause()
        {
            if (m_playbackState == SoundPlaybackState_Playing)
            {
                m_playbackState = SoundPlaybackState_Paused;
                m_playbackThreadProc.Pause();

                this->OnPause();
            }
        }


        void Sound::SetPosition(const glm::vec3 &position)
        {
            m_engineContext.GetAudioSystem().SetSoundPosition(m_hSound, position);
        }

        glm::vec3 Sound::GetPosition() const
        {
            return m_engineContext.GetAudioSystem().GetSoundPosition(m_hSound);
        }

        void Sound::SetIsPositionRelative(bool isRelative)
        {
            m_engineContext.GetAudioSystem().SetIsSoundPositionRelative(m_hSound, isRelative);
        }

        bool Sound::IsPositionRelative() const
        {
            return m_engineContext.GetAudioSystem().IsSoundPositionRelative(m_hSound);
        }


        SoundPlaybackState Sound::GetPlaybackState() const
        {
            return m_playbackState;
        }


        void Sound::AddPlaybackEventHandler(SoundPlaybackEventHandler &eventHandler)
        {
            assert(!m_playbackEventHandlers.Exists(&eventHandler));     // <-- If this fails it means the event handler is already attached.
            {
                m_playbackEventHandlers.PushBack(&eventHandler);
            }
        }

        void Sound::RemovePlaybackEventHandler(SoundPlaybackEventHandler &eventHandler)
        {
            m_playbackEventHandlers.RemoveFirstOccuranceOf(&eventHandler);

            assert(!m_playbackEventHandlers.Exists(&eventHandler));     // <-- If this fails it means the event handler was attached more than once.
        }

        void Sound::RemovePlaybackEventHandlerAtIndex(size_t eventHandlerIndex)
        {
            m_playbackEventHandlers.Remove(eventHandlerIndex);
        }

        size_t Sound::GetPlaybackEventHandlerCount() const
        {
            return m_playbackEventHandlers.GetCount();
        }

        SoundPlaybackEventHandler & Sound::GetPlaybackEventHandlerAtIndex(size_t index)
        {
            assert(m_playbackEventHandlers[index] != nullptr);

            return *m_playbackEventHandlers[index];
        }



        /////////////////////////////////////////////////
        // Private

        void Sound::OnStart()
        {
            auto eventHandlers = m_playbackEventHandlers;
            for (size_t iEventHandler = 0; iEventHandler < eventHandlers.GetCount(); ++iEventHandler)
            {
                auto eventHandler = eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    eventHandler->OnStart();
                }
            }
        }

        void Sound::OnStop()
        {
            auto eventHandlers = m_playbackEventHandlers;
            for (size_t iEventHandler = 0; iEventHandler < eventHandlers.GetCount(); ++iEventHandler)
            {
                auto eventHandler = eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    eventHandler->OnStop();
                }
            }
        }

        void Sound::OnResume()
        {
            auto eventHandlers = m_playbackEventHandlers;
            for (size_t iEventHandler = 0; iEventHandler < eventHandlers.GetCount(); ++iEventHandler)
            {
                auto eventHandler = eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    eventHandler->OnResume();
                }
            }
        }

        void Sound::OnPause()
        {
            auto eventHandlers = m_playbackEventHandlers;
            for (size_t iEventHandler = 0; iEventHandler < eventHandlers.GetCount(); ++iEventHandler)
            {
                auto eventHandler = eventHandlers[iEventHandler];
                assert(eventHandler != nullptr);
                {
                    eventHandler->OnPause();
                }
            }
        }
    }
}

#pragma warning(pop)
