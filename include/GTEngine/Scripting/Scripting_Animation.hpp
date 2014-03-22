// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Animation_hpp_
#define __GTEngine_Scripting_Animation_hpp_

#include "../GameScript.hpp"
#include "../Animation/AnimationSequence.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the animation sequence library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        bool LoadAnimationLibrary(GTLib::Script &script);


        /// Converts the table at the given position to an AnimationSequence.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The AnimationSequence at the given position, or an empty sequence if an error occurs.
        AnimationSequence ToAnimationSequence(GTLib::Script &script, int position);
    }
}

#endif