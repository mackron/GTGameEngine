// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptVariable.hpp>

namespace GTEngine
{
    ScriptVariable::ScriptVariable(ScriptVariableType typeIn, const char* nameIn)
        : type(typeIn), name(nameIn)
    {
    }

    ScriptVariable::~ScriptVariable()
    {
    }


    ScriptVariableType ScriptVariable::GetType() const
    {
        return this->type;
    }

    const char* ScriptVariable::GetName() const
    {
        return this->name.c_str();
    }



    /////////////////////////////////////
    // Number

    ScriptVariable_Number::ScriptVariable_Number(const char* name)
        : ScriptVariable(ScriptVariableType_Number, name)
    {
    }

    ScriptVariable_Number::~ScriptVariable_Number()
    {
    }
}