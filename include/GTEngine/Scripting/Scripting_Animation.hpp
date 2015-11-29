// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Scripting_Animation
#define GT_Scripting_Animation

#include "../GameScript.hpp"
#include "../Animation/AnimationSequence.hpp"

namespace GT
{
    /// Loads the animation sequence library.
    ///
    /// @param script [in] A reference to the script to load the library into.
    bool LoadAnimationLibrary(GT::Script &script);


    /// Converts the table at the given position to an AnimationSequence.
    ///
    /// @param script   [in] A reference to the script to read the table from.
    /// @param position [in] The position of the table to read.
    ///
    /// @return The AnimationSequence at the given position, or an empty sequence if an error occurs.
    AnimationSequence ToAnimationSequence(GT::Script &script, int position);
}

#endif