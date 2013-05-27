// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

// This file is where anything requiring Assimp is located. It is forbidden to place any Assimp code anywhere other
// than inside this file.

#include <GTEngine/ModelDefinition.hpp>

namespace GTEngine
{
    bool ModelDefinition::LoadFromAssimpFile(const GTCore::String &absolutePathIn)
    {
        (void)absolutePathIn;
        return false;
    }
}