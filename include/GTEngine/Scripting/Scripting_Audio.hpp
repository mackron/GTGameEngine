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
        bool LoadAudioLibrary(GTCore::Script &script);


        namespace AudioFFI
        {
            /// Plays a sound by it's file name.
            ///
            /// @remarks
            ///     Argument 1: The file name of the sound to play.
            int Play(GTCore::Script &script);
        }
    }
}

#endif