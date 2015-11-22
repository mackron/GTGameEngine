// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_Sound_hpp_
#define __GT_Engine_Sound_hpp_

#include <GTLib/Vector.hpp>
#include "SoundStreamer.hpp"
#include "AudioEngine.hpp"

namespace GT
{
    namespace Engine
    {
        class EngineContext;
        class SoundWorld;

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


            /// Sets the world that owns this sound.
            void SetWorld(SoundWorld* pWorld) { m_pWorld = pWorld; }

            /// Retrieves a pointer to the world that owns this sound.
            SoundWorld* GetWorld() const { return m_pWorld; }


            /// Retrieves a handle to the audio buffer.
            GTEngine::HAudioBuffer GetAudioBuffer();


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
            easyaudio_playback_state GetPlaybackState() const;

            /// Helper method for determining whether or not the sound is playing.
            bool IsPlaying() const { return this->GetPlaybackState() == easyaudio_playing; }

            /// Helper method for determining whether or not the sound is paused.
            bool IsPaused() const { return this->GetPlaybackState() == easyaudio_paused; }

            /// Helper method for determining whether or not the sound is stopped.
            bool IsStopped() const { return this->GetPlaybackState() == easyaudio_stopped; }



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


        private:

            /// A reference to the engine context that owns the sound.
            EngineContext &m_engineContext;

            /// A pointer to the sound world, if any, that owns this sound.
            SoundWorld* m_pWorld;

            /// A handle to the audio buffer for this sound.
            GTEngine::HAudioBuffer m_hBuffer;

            /// A pointer to the streamer to load the audio data from.
            GTEngine::SoundStreamer* m_streamer;



        private:    // No copying.
            Sound(const Sound &);
            Sound & operator=(const Sound &);
        };
    }
}

#endif // !__GT_Engine_Sound_hpp_
