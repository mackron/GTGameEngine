// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AudioComposer
#define GT_AudioComposer

#include "../Math.hpp"

namespace GT
{
    /// Static class for playing sounds.
    class AudioComposer
    {
    // Playback.
    public:

        /// Plays the sound given by the specified file name.
        ///
        /// @param fileName [in] The file name of the sound to play.
        static bool Play(const char* fileName, bool relativePositioning = true);
        static bool Play(const char* fileName, float x, float y, float z, bool relativePositioning = false);
        static bool Play(const char* fileName, const vec3 &position, bool relativePositioning = false) { return Play(fileName, position.x, position.y, position.z, relativePositioning); }



    // Listener.
    public:

        /// Sets the position of the listener.
        static void SetListenerPosition(float x, float y, float z);
        static void SetListenerPosition(const vec3 &position) { SetListenerPosition(position.x, position.y, position.z); }

        /// Sets the orientation of the listener.
        static void SetListenerOrientation(const quat &orientation);
    };
}


#endif
