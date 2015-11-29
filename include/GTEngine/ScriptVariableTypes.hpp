// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ScriptVariableTypes
#define GT_ScriptVariableTypes

namespace GT
{
    enum ScriptVariableType
    {
        ScriptVariableType_Unknown = 0,
        ScriptVariableType_None    = 0,

        ScriptVariableType_Number,
        ScriptVariableType_Vec2,
        ScriptVariableType_Vec3,
        ScriptVariableType_Vec4,
        ScriptVariableType_Boolean,
        ScriptVariableType_String,
        ScriptVariableType_Prefab,
    };
}

#endif