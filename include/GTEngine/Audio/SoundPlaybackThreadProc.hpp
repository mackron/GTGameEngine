// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_SoundPlaybackThreadProc_hpp_
#define __GT_Engine_SoundPlaybackThreadProc_hpp_

#include <GTLib/Threading.hpp>
#include <GTEngine/Audio/AudioEngine.hpp>
#include <GTEngine/Audio/SoundStreamer.hpp>
#include <GTEngine/Audio/SoundPlaybackState.hpp>
#include <GTEngine/Audio/SoundPlaybackEventHandler.hpp>

namespace GT
{
    namespace Engine
    {
        class EngineContext;

        /// Thread procedure for the playback thread.
        ///
        /// When Run() is called, the sound will automatically start playing. Call Pause() immediately after starting the thread to have
        /// the sound start in a paused state.
        class SoundPlaybackThreadProc : public GTLib::Threading::Job
        {
        public:

            /// Constructor.
            SoundPlaybackThreadProc(EngineContext &engineContext);
            SoundPlaybackThreadProc(EngineContext &engineContext, SoundPlaybackEventHandler &playbackEventHandler);

            /// Destructor.
            ~SoundPlaybackThreadProc();


            /// Initializes the playback thread procedure.
            ///
            /// @param sound   [in] The sound source handle.
            /// @param buffers [in] A pointer to a 2-element buffer containing the front and back buffers.
            void Initialize(GTEngine::SoundHandle source, GTEngine::AudioBufferHandle buffers[2], GTEngine::SoundStreamer &streamer);


            /////////////////////////////////////////////
            // Playback

            /// Stops playback of the sound.
            ///
            /// @remarks
            ///     This will cause the playback thread to exit. The thread will need to be restarted.
            void Stop();

            /// Pauses playback of the sound.
            void Pause();

            /// Resumes playback of the sound.
            void Resume();


            /////////////////////////////////////////////
            // Virtual Implementations

            /// GTLib::Threading::Job::Run()
            void Run();



        private:

            /// Reads the next chunk of data from the streamer and copies it into the given buffer.
            ///
            /// @param buffer [in] The buffer to copy the next chunk of data into.
            ///
            /// @return True if anything was copied into the buffer; false otherwise.
            bool ReadAndQueueNextChunk(GTEngine::AudioBufferHandle buffer);

            /// Waits for the read buffer to process.
            ///
            /// @return The number of buffers that were processed, or 0 if some kind of error occurs.
            ///
            /// @remarks
            ///     There is a slight chance that both the read and write buffer is processed.
            unsigned int WaitForReadBufferToProcess();


        private:

            /// A handle to the engine context that owns the audio engine object.
            EngineContext &m_engineContext;

            /// A handle to the sound source.
            GTEngine::SoundHandle m_source;

            /// The read and write buffers to use when streaming audio data.
            GTEngine::AudioBufferHandle m_buffers[2];

            /// A pointer to the sound streamer to read the audio data from.
            GTEngine::SoundStreamer* m_streamer;

            /// A reference to the playback event handler.
            SoundPlaybackEventHandler* m_playbackEventHandler;

            /// The current playback state. This starts of playing.
            SoundPlaybackState m_playbackState;

            /// When the playback thread is waiting for a buffer to process, it'll keep looping until it sees that it's done. This
            /// semaphore will dictate when the loop should iterate. We use a semaphore so that we can quickly terminate the loop
            /// if needed.
            GTLib::Semaphore m_bufferProcessingLoopLock;

            /// Mutex controlling reading from the sound streamer.
            GTLib::Mutex m_streamingLock;
        };
    }
}

#endif
