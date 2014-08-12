// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Audio/SoundPlaybackThreadProc.hpp>
#include <GTEngine/EngineContext.hpp>

#pragma warning(push)
#pragma warning(disable: 4351)      // Buffers will be default initialized.

namespace GT
{
    namespace Engine
    {
        static const int ReadBufferIndex  = 0;
        static const int WriteBufferIndex = 1;


        SoundPlaybackThreadProc::SoundPlaybackThreadProc(EngineContext &engineContext)
            : m_engineContext(engineContext),
              m_source(0),
              m_buffers(),
              m_streamer(nullptr),
              m_playbackEventHandler(nullptr),
              m_playbackState(SoundPlaybackState_Stopped),
              m_bufferProcessingLoopLock(0),
              m_streamingLock()
        {
            m_buffers[0] = 0;
            m_buffers[1] = 0;
        }

        SoundPlaybackThreadProc::SoundPlaybackThreadProc(EngineContext &engineContext, SoundPlaybackEventHandler &playbackEventHandler)
            : SoundPlaybackThreadProc(engineContext)
        {
            m_playbackEventHandler = &playbackEventHandler;
        }

        SoundPlaybackThreadProc::~SoundPlaybackThreadProc()
        {
            this->Stop();
        }


        void SoundPlaybackThreadProc::Initialize(GTEngine::SoundHandle source, GTEngine::AudioBufferHandle buffers[2], GTEngine::SoundStreamer &streamer)
        {
            assert(m_source == 0);
            assert(m_buffers[0] == 0 && m_buffers[1] == 0);
            assert(m_streamer == nullptr);
            assert(m_playbackState == SoundPlaybackState_Stopped);

            m_source     = source;
            m_buffers[0] = buffers[0];
            m_buffers[1] = buffers[1];
            m_streamer   = &streamer;
        }


        /////////////////////////////////////////////
        // Playback

        void SoundPlaybackThreadProc::Stop()
        {
            if (m_playbackState != SoundPlaybackState_Stopped)
            {
                m_streamingLock.Lock();
                {
                    if (m_playbackState != SoundPlaybackState_Stopped)
                    {
                        m_engineContext.GetAudioSystem().StopSound(m_source);
                        m_playbackState = SoundPlaybackState_Stopped;

                        m_bufferProcessingLoopLock.Release();

                        if (m_playbackEventHandler != nullptr)
                        {
                            m_playbackEventHandler->OnStop();
                        }
                    }
                }
                m_streamingLock.Unlock();
            }
        }

        void SoundPlaybackThreadProc::Pause()
        {
            if (m_playbackState == SoundPlaybackState_Playing)
            {
                m_engineContext.GetAudioSystem().PauseSound(m_source);
                m_playbackState = SoundPlaybackState_Paused;

                if (m_playbackEventHandler != nullptr)
                {
                    m_playbackEventHandler->OnPause();
                }
            }
        }

        void SoundPlaybackThreadProc::Resume()
        {
            if (m_playbackState == SoundPlaybackState_Paused)
            {
                m_playbackState = SoundPlaybackState_Playing;
                m_engineContext.GetAudioSystem().PlaySound(m_source);

                if (m_playbackEventHandler != nullptr)
                {
                    m_playbackEventHandler->OnResume();
                }
            }
        }


        /////////////////////////////////////////////
        // Virtual Implementations

        void SoundPlaybackThreadProc::Run()
        {
            assert(m_source != 0);      // <-- If this fails, it means Initialize() hasn't be called, or it has invalid arguments.
            assert(m_buffers[0] != 0 && m_buffers[1] != 0);
            assert(m_streamer != nullptr);
            assert(m_playbackState == SoundPlaybackState_Stopped);

            auto &audioSystem = m_engineContext.GetAudioSystem();


            // We want the sound to start playing by default.
            m_playbackState = SoundPlaybackState_Playing;


            // We want to load both the read and write buffers, however we want to start playing audio as soon as possible, so we're going
            // to read the first chunk, start playing, and then start reading the next one.
            this->ReadAndQueueNextChunk(m_buffers[WriteBufferIndex]);
            audioSystem.PlaySound(m_source);
            this->ReadAndQueueNextChunk(m_buffers[ReadBufferIndex]);


            // Keep playing so long as we haven't stopped.
            while (m_playbackState != SoundPlaybackState_Stopped)
            {
                // We need to wait for the read buffer to process. The return value is the number of buffers that were processed. If it returns 1, it means just the
                // read buffer was processed. It if returns 2, it means both the read and write buffers were processed, in which case the sound needs to be restarted.
                // If it returns 0, it was interrupted.
                unsigned int processedBufferCount = this->WaitForReadBufferToProcess();
                if (processedBufferCount > 0)
                {
                    if (m_playbackState != SoundPlaybackState_Stopped)      // <-- The sound playback may have stopped while waiting for data.
                    {
                        if (processedBufferCount == 1)
                        {
                            // The read buffer needs to be unqueued.
                            audioSystem.UnqueueAudioBuffer(m_source);

                            // The buffers need to be swapped.
                            GTEngine::AudioBufferHandle hTempBuffer = m_buffers[0];
                            m_buffers[0] = m_buffers[1];
                            m_buffers[1] = hTempBuffer;

                            // Now we want to load up the next chunk.
                            this->ReadAndQueueNextChunk(m_buffers[ReadBufferIndex]);
                        }
                        else
                        {
                            assert(processedBufferCount == 2);
                            assert(audioSystem.IsSoundStopped(m_source));

                            audioSystem.UnqueueAudioBuffer(m_source);
                            audioSystem.UnqueueAudioBuffer(m_source);

                            this->ReadAndQueueNextChunk(m_buffers[ReadBufferIndex]);
                            audioSystem.PlaySound(m_source);
                            this->ReadAndQueueNextChunk(m_buffers[WriteBufferIndex]);
                        }
                    }
                }
                else
                {
                    assert(processedBufferCount == 0);
                    m_playbackState = SoundPlaybackState_Stopped;       // <-- This will terminate the loop.
                }
            }


            // At this point the sound has stopped so we want to let everything know. There is a strong chance the event handler for OnStop() will delete
            // this object and the applicable thread, so this needs to be the very last thing we do here.
            if (m_playbackEventHandler != nullptr)
            {
                m_playbackEventHandler->OnStop();
            }
        }



        /////////////////////////////////////////////
        // Private Methods

        bool SoundPlaybackThreadProc::ReadAndQueueNextChunk(GTEngine::AudioBufferHandle buffer)
        {
            assert(buffer     != 0);
            assert(m_source   != 0);
            assert(m_streamer != nullptr);

            bool result = false;

            m_streamingLock.Lock();
            {
                if (m_playbackState != SoundPlaybackState_Stopped)
                {
                    auto &audioSystem = m_engineContext.GetAudioSystem();

                    size_t chunkBufferSizeInBytes = 0;
                    const void* chunkBuffer = m_streamer->ReadNextChunk(chunkBufferSizeInBytes);
                    if (chunkBuffer != nullptr)
                    {
                        audioSystem.SetAudioBufferData(buffer, chunkBuffer, chunkBufferSizeInBytes, m_streamer->GetFormat(), m_streamer->GetSampleRate());
                        audioSystem.QueueAudioBuffer(m_source, buffer);

                        result = true;
                    }
                }
            }
            m_streamingLock.Unlock();

            return result;
        }

        unsigned int SoundPlaybackThreadProc::WaitForReadBufferToProcess()
        {
            auto &audioSystem = m_engineContext.GetAudioSystem();

            if (audioSystem.GetQueuedAudioBufferCount(m_source) > 0)
            {
                while (!audioSystem.IsSoundStopped(m_source))
                {
                    unsigned int processedBufferCount = audioSystem.GetProcessedQueuedAudioBufferCount(m_source);
                    if (processedBufferCount > 0)
                    {
                        return processedBufferCount;
                    }


                    m_bufferProcessingLoopLock.Wait(10);
                }
            }

            return 0;
        }
    }
}

#pragma warning(pop)