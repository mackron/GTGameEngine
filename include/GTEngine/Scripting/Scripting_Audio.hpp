// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Scripting_Audio
#define GT_Scripting_Audio

#include "../GameScript.hpp"

namespace GT
{
    /// Loads the audio library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    bool LoadAudioLibrary(GT::Script &script);


    namespace AudioFFI
    {
        /// Plays a sound by it's file name.
        ///
        /// @remarks
        ///     Argument 1: The file name of the sound to play.
        ///     Argument 2: A math.vec3 representing the position of the sound. Can be nil, in which case it will be played in stereo mode (no positioning).
        ///     Argument 3: Whether or not the sound should be positioned relative to the listener.
        int Play(GT::Script &script);

        /// Sets the position of the listener.
        ///
        /// @remarks
        ///     Argument 1: A math.vec3 representing the new position of the listener.
        int SetListenerPosition(GT::Script &script);

        /// Sets the orientation of the listener.
        ///
        /// @remarks
        ///     Argument 1: A math.quat representing the new orientation of the listener.
        int SetListenerOrientation(GT::Script &script);
    }
}

#endif