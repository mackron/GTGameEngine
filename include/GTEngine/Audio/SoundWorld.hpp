// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_Engine_SoundWorld_hpp_
#define __GT_Engine_SoundWorld_hpp_

#include <GTLib/Vector.hpp>
#include "Sound.hpp"


namespace GT
{
    namespace Engine
    {
        class EngineContext;

        /// Class representing the virtual world where sounds will be played.
        class SoundWorld
        {
        public:

            /// Constructor.
            SoundWorld(GT::Engine::EngineContext &engineContext);

            /// Destructor.
            ~SoundWorld();


            /// Adds a sound to the world.
            ///
            /// @param sound [in] A reference to the sound to add to the world.
            void AddSound(Sound &sound);

            /// Removes the given sound from the world.
            ///
            /// @param sound [in] A reference to the sound to remove from the world.
            void RemoveSound(Sound &sound);


            /// A helper function for plays a sound in place at the given position.
            ///
            /// @param fileName [in] The path of the sound file to play.
            /// @param position [in] The position to play the sound.
            /// @param relative [in] Whether or not the position is relative to the listener.
            ///
            /// @return True if the sound is played successfully.
            ///
            /// @remarks
            ///     The sound will not loop and there will be no feedback for when the sound is deleted. There is no ability to control
            ///     the playback of the sound. This is purely a helper method for easily playing back a sound.
            ///     @par
            ///     This function returns quickly - the sound will be played in the background and automatically cleaned up when it's
            ///     finished playing.
            bool PlaySound(const char* filePath, const glm::vec3 &position = glm::vec3(), bool relative = true);


            /// Stops playing all sounds.
            void StopAllSounds();


            ////////////////////////////////////////////////////////
            // Internal-Use-Only Methods

            /// Called by a sound playback event handler when an inline sound finishes playing.
            ///
            /// @param sound [in] A reference to the sound that has stopped playing.
            void _OnInlineSoundStop(Sound &sound);


        private:

            /// A reference to the engine context.
            GT::Engine::EngineContext &m_engineContext;

            /// The list of sounds that are currently in the world.
            GTLib::Vector<Sound*> m_sounds;

            /// The list of inline sounds. These sounds are created by PlaySound() and deleted when they are no longer playing.
            GTLib::Vector<Sound*> m_inlineSounds;


        private:    // No copying.
            SoundWorld(const SoundWorld &);
            SoundWorld & operator=(const SoundWorld &);
        };
    }
}

#endif // !__GT_Engine_SoundWorld_hpp_
