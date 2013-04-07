// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleSystem.hpp>

namespace GTEngine
{
    ParticleSystem::ParticleSystem(const ParticleSystemDefinition &definitionIn)
        : definition(definitionIn)
    {
    }

    ParticleSystem::~ParticleSystem()
    {
    }
}