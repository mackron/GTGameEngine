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
        bool LoadExtendedMathLibrary(GTLib::Script &script);


        /// Converts the table at the given position to a glm::vec2.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec2 at the given position, or (0, 0) if an error occurs.
        glm::vec2 ToVector2(GTLib::Script &script, int position);

        /// Converts the table at the given position to a glm::vec3.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec3 at the given position, or (0, 0, 0) if an error occurs.
        glm::vec3 ToVector3(GTLib::Script &script, int position);

        /// Converts the table at the given position to a glm::vec4.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec4 at the given position, or (0, 0, 0, 0) if an error occurs.
        glm::vec4 ToVector4(GTLib::Script &script, int position);

        /// Converts the table at the given position to a glm::quat.
        ///
        /// @param script   [in] A reference to the script to read the table from.
        /// @param position [in] The position of the table to read.
        ///
        /// @return The glm::vec4 at the given position, or (0, 0, 0, 0) if an error occurs.
        glm::quat ToQuaternion(GTLib::Script &script, int position);


        /// Pushes a new math.vec2 table.
        ///
        /// @param value [in] The glm::vec2 containing the initial value of the math.vec2.
        /// @param x     [in] The initial x value.
        /// @param y     [in] The initial y value.
        void PushNewVector2(GTLib::Script &script, float x, float y);
        inline void PushNewVector2(GTLib::Script &script, const glm::vec2 &value) { PushNewVector2(script, value.x, value.y); }

        /// Pushes a new math.vec3 table.
        ///
        /// @param value [in] The glm::vec3 containing the initial value of the math.vec3.
        /// @param x     [in] The initial x value.
        /// @param y     [in] The initial y value.
        /// @param z     [in] The initial z value.
        void PushNewVector3(GTLib::Script &script, float x, float y, float z);
        inline void PushNewVector3(GTLib::Script &script, const glm::vec3 &value) { PushNewVector3(script, value.x, value.y, value.z); }

        /// Pushes a new math.vec4 table.
        ///
        /// @param value [in] The glm::vec4 containing the initial value of the math.vec4.
        /// @param x     [in] The initial x value.
        /// @param y     [in] The initial y value.
        /// @param z     [in] The initial z value.
        /// @param w     [in] The initial w value.
        void PushNewVector4(GTLib::Script &script, float x, float y, float z, float w);
        inline void PushNewVector4(GTLib::Script &script, const glm::vec4 &value) { PushNewVector4(script, value.x, value.y, value.z, value.w); }

        /// Pushes a new math.quat table.
        ///
        /// @param value [in] The glm::quat containing the initial value of the math.quat.
        /// @param x     [in] The initial x value.
        /// @param y     [in] The initial y value.
        /// @param z     [in] The initial z value.
        /// @param w     [in] The initial w value.
        void PushNewQuaternion(GTLib::Script &script, float x, float y, float z, float w);
        inline void PushNewQuaternion(GTLib::Script &script, const glm::quat &value) { PushNewQuaternion(script, value.x, value.y, value.z, value.w); }


        /// Determines whether or not the value at the given index is a math.vec2.
        ///
        /// @param script [in] The script object.
        /// @param index  [in] The index of the item to check.
        ///
        /// @return 'true' if the item is a math.vec2; false otherwise.
        bool IsVector2(GTLib::Script &script, int index);

        /// Determines whether or not the value at the given index is a math.vec3.
        ///
        /// @param script [in] The script object.
        /// @param index  [in] The index of the item to check.
        ///
        /// @return 'true' if the item is a math.vec3; false otherwise.
        bool IsVector3(GTLib::Script &script, int index);

        /// Determines whether or not the value at the given index is a math.vec4.
        ///
        /// @param script [in] The script object.
        /// @param index  [in] The index of the item to check.
        ///
        /// @return 'true' if the item is a math.vec4; false otherwise.
        bool IsVector4(GTLib::Script &script, int index);


        namespace FFI
        {
            namespace MathFFI
            {
            }
        }
    }
}

#endif