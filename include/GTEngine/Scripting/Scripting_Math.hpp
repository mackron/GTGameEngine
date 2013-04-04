// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Math_hpp_
#define __GTEngine_Scripting_Math_hpp_

#include "../GameScript.hpp"
#include "../Math.hpp"

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


        /// Converts the table at the given position to a glm::vec2.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec2 at the given position, or (0, 0) if an error occurs.
        glm::vec2 ToVector2(GTCore::Script &script, int position);

        /// Converts the table at the given position to a glm::vec3.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec3 at the given position, or (0, 0, 0) if an error occurs.
        glm::vec3 ToVector3(GTCore::Script &script, int position);

        /// Converts the table at the given position to a glm::vec4.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec4 at the given position, or (0, 0, 0, 0) if an error occurs.
        glm::vec4 ToVector4(GTCore::Script &script, int position);

        /// Converts the table at the given position to a glm::quat.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec4 at the given position, or (0, 0, 0, 0) if an error occurs.
        glm::quat ToQuaternion(GTCore::Script &script, int position);


        namespace FFI
        {
            namespace MathFFI
            {
            }
        }
    }
}

#endif