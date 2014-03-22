// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Audio_hpp_
#define __GTEngine_Scripting_Audio_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the audio library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadAudioLibrary(GTLib::Script &script);


        namespace AudioFFI
        {
            /// Plays a sound by it's file name.
            ///
            /// @remarks
            ///     Argument 1: The file name of the sound to play.
            ///     Argument 2: A math.vec3 representing the position of the sound. Can be nil, in which case it will be played in stereo mode (no positioning).
            ///     Argument 3: Whether or not the sound should be positioned relative to the listener.
            int Play(GTLib::Script &script);

            /// Sets the position of the listener.
            ///
            /// @remarks
            ///     Argument 1: A math.vec3 representing the new position of the listener.
            int SetListenerPosition(GTLib::Script &script);

            /// Sets the orientation of the listener.
            ///
            /// @remarks
            ///     Argument 1: A math.quat representing the new orientation of the listener.
            int SetListenerOrientation(GTLib::Script &script);
        }
    }
}

#endif