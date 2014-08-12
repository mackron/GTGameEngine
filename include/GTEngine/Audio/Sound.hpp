// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_Sound_hpp_
#define __GT_Engine_Sound_hpp_

#include <GTLib/Vector.hpp>
#include "SoundStreamer.hpp"
#include "SoundPlaybackState.hpp"
#include "SoundPlaybackThreadProc.hpp"

namespace GT
{
    namespace Engine
    {
        class EngineContext;

        /// Class representing a sound.
        class Sound
        {
        public:

            /// Constructor.
            ///
            /// @param engineContext [in] A reference to the engine context that owns the sound.
            Sound(EngineContext &engineContext);

            /// Destructor.
            ~Sound();


            /// Retrieve the internal sound source handle.
            ///
            /// @return The internal sound source handle.
            GTEngine::SoundHandle GetSoundHandle();

            /// Retrieves the internal audio buffers for the the sound.
            ///
            /// @return The two audio data buffers used by the sound.
            ///
            /// @remarks
            ///     The returned pointer is of a 2-element buffer.
            GTEngine::AudioBufferHandle* GetAudioBuffers();


            /// Loads a sound from the given sound file.
            ///
            /// @param relativeFilePath [in] The relative path of the sound file to use as the source of this sound.
            ///
            /// @remarks
            ///     The entire file is not loaded all in one go. Instead the data will be streamed from the file.
            ///     @par
            ///     This does not automatically start playback. Start playing the sound with Play().
            ///     @par
            ///     This can be called when the sound is already playing a different sound, in which case the sound will be
            ///     stopped and will need to be resumed with a call to Play().
            bool LoadFromFile(const char* relativeFilePath);


            /// Starts, resumes or replays the sound.
            void Play();

            /// Stops the sound playback.
            ///
            /// @remarks
            ///     This will bring the sound back to the start again. Use Pause() if you would rather keep the currently playback location.
            void Stop();

            /// Pauses playback of the sound.
            ///
            /// @remarks
            ///     Use Stop() to stop playback and bring the sound back to the start.
            void Pause();

            /// Retrieves the playback state of the sound (whether or not it is playing, stopped or paused).
            SoundPlaybackState GetPlaybackState() const;

            /// Helper method for determining whether or not the sound is playing.
            bool IsPlaying() const { return this->GetPlaybackState() == SoundPlaybackState_Playing; }

            /// Helper method for determining whether or not the sound is paused.
            bool IsPaused() const { return this->GetPlaybackState() == SoundPlaybackState_Paused; }

            /// Helper method for determining whether or not the sound is stopped.
            bool IsStopped() const { return this->GetPlaybackState() == SoundPlaybackState_Stopped; }



            /// Sets the position of the sound.
            ///
            /// @param position [in] The position of the sound.
            ///
            /// @remarks
            ///     Whether or not the position of the sound is relative or absolute depends on the relative positioning as defined by
            ///     SetIsPositionRelative(), IsPositionRelative(), etc.
            void SetPosition(const glm::vec3 &position);

            /// Retrieves the position of the sound.
            ///
            /// @return The position of the sound.
            glm::vec3 GetPosition() const;


            /// Sets whether or not the position of the sound is relative to the listener.
            ///
            /// @param isRelative [in] Whether or not the position should be relative to the listener.
            void SetIsPositionRelative(bool isRelative);

            /// Determines whether or not the position of the sound is relative to the listener.
            ///
            /// @return True if the position of the sound is relative to the listener; false otherwise.
            bool IsPositionRelative() const;



            /// Attaches a playback event handler to the sound.
            ///
            /// @param eventHandler [in] A reference to the event handler to attach to the sound.
            void AddPlaybackEventHandler(SoundPlaybackEventHandler &eventHandler);

            /// Removes the given playback event handler from the sound.
            ///
            /// @param eventHandler [in] A reference to the event handler to remove from the sound.
            void RemovePlaybackEventHandler(SoundPlaybackEventHandler &eventHandler);

            /// Removes the playback event handler at the given index.
            ///
            /// @param eventHandlerIndex [in] The index of the event handler to remove.
            ///
            /// @remarks
            ///     The index of all event other event handlers that are located at indices after eventHandlerIndex will be moved down a position.
            void RemovePlaybackEventHandlerAtIndex(size_t eventHandlerIndex);

            /// Retrieves the number of event handlers that are attached to the sound.
            ///
            /// @return The number of event handlers that are attached to the sound.
            size_t GetPlaybackEventHandlerCount() const;

            /// Retrieves a reference to the event handler at the given index.
            ///
            /// @param index [in] The index of the event handler to retrieve.
            SoundPlaybackEventHandler & GetPlaybackEventHandlerAtIndex(size_t index);



        private:

            /// Helper for posting the OnStart() event to every attached event handler.
            void OnStart();

            /// Helper for posting the OnStop() event to every attached event handler.
            void OnStop();

            /// Helper for posting the OnResume() event to every attached event handler.
            void OnResume();

            /// Helper for posting the OnPause() event to every attached event handler.
            void OnPause();


        private:

            /// Custom implementation of the SoundPlaybackEventHandler so we can catch when the playback thread has stopped playing.
            class PlaybackThreadPlaybackEventHandler : public SoundPlaybackEventHandler
            {
            public:

                /// Constructor.
                PlaybackThreadPlaybackEventHandler(Sound &sound)
                    : SoundPlaybackEventHandler(), m_sound(sound)
                {
                }

                
                /// SoundPlaybackEventHandler::OnStop().
                void OnStop()
                {
                    m_sound.Stop();
                }


            private:

                /// A reference to the sound.
                Sound &m_sound;


            private:    // No copying.
                PlaybackThreadPlaybackEventHandler(const PlaybackThreadPlaybackEventHandler &);
                PlaybackThreadPlaybackEventHandler & operator=(const PlaybackThreadPlaybackEventHandler &);
            };



        private:

            /// A reference to the engine context that owns the sound.
            EngineContext &m_engineContext;

            /// The list of event handlers that are attached to the sound.
            GTLib::Vector<SoundPlaybackEventHandler*> m_playbackEventHandlers;


            /// The handle to the sound source.
            GTEngine::SoundHandle m_hSound;

            /// The handle to the two audio data buffers. Index 0 is the read buffer and 1 is the write buffer.
            GTEngine::AudioBufferHandle m_hBuffers[2];


            /// A pointer to the streamer to load the audio data from.
            GTEngine::SoundStreamer* m_streamer;

            /// A pointer to the thread that is playing the sound. This is acquired through the engine context.
            GTLib::Thread* m_playbackThread;

            /// The playback event handler that will be attached to the playback thread.
            PlaybackThreadPlaybackEventHandler m_playbackThreadEventHandler;

            /// The playback thread procedure that will do the actual streaming on a separate thread.
            SoundPlaybackThreadProc m_playbackThreadProc;


            /// The current playback state.
            SoundPlaybackState m_playbackState;



        private:    // No copying.
            Sound(const Sound &);
            Sound & operator=(const Sound &);
        };
    }
}

#endif // !__GT_Engine_Sound_hpp_
