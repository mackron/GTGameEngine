// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_SoundWorld
#define GT_SoundWorld

#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/Threading/Mutex.hpp>
#include "../Math.hpp"
#include <easy_audio/easy_audio.h>

namespace GT
{
    class EngineContext;

    /// Class representing the virtual world where sounds will be played.
    class SoundWorld
    {
    public:

        /// Constructor.
        SoundWorld(GT::EngineContext &engineContext);

        /// Destructor.
        ~SoundWorld();


        /// Initializes the sound world.
        bool Startup();

        /// Shuts down the sound world.
        void Shutdown();


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



    private:

        /// A reference to the engine context.
        GT::EngineContext &m_engineContext;

        /// A pointer to the easy_audio world.
        easyaudio_world* m_pWorld;



    private:    // No copying.
        SoundWorld(const SoundWorld &);
        SoundWorld & operator=(const SoundWorld &);
    };
}

#endif // !GT_SoundWorld
