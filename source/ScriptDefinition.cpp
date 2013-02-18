// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptDefinition.hpp>

namespace GTEngine
{
    ScriptDefinition::ScriptDefinition(const char* absolutePathIn, const char* relativePathIn, const char* scriptStringIn)
        : absolutePath(absolutePathIn), relativePath(relativePathIn), scriptString(scriptStringIn)
    {
    }

    ScriptDefinition::~ScriptDefinition()
    {
    }


    const char* ScriptDefinition::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* ScriptDefinition::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }

    const char* ScriptDefinition::GetScriptString() const
    {
        return this->scriptString.c_str();
    }
}