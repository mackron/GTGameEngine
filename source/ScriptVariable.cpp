// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptVariable.hpp>

namespace GTEngine
{
    ScriptVariable::ScriptVariable(ScriptVariableType typeIn)
        : type(typeIn)
    {
    }

    ScriptVariable::~ScriptVariable()
    {
    }


    ScriptVariableType ScriptVariable::GetType() const
    {
        return this->type;
    }
}