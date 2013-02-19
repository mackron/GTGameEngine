// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptVariableTypes_hpp_
#define __GTEngine_ScriptVariableTypes_hpp_

namespace GTEngine
{
    enum ScriptVariableType
    {
        ScriptVariableType_Unknown = 0,
        ScriptVariableType_None    = 0,

        ScriptVariableType_Number,
        ScriptVariableType_Vec2,
        ScriptVariableType_Vec3,
        ScriptVariableType_Vec4,
        ScriptVariableType_String,
        ScriptVariableType_Prefab,
    };
}

#endif