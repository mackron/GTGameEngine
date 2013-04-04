// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Math_hpp_
#define __GTEngine_Scripting_Math_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the extended math library into the given script.
        ///
        /// @param script [in] The script to load the math library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     This will load the library into the global 'math' table. This will not override existing functions.
        bool LoadExtendedMathLibrary(GTCore::Script &script);


        namespace FFI
        {
            namespace MathFFI
            {
            }
        }
    }
}

#endif