// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Particle.hpp>

namespace GTEngine
{
    Particle::Particle()
        : position(), orientation(), scale(1.0f, 1.0f, 1.0f), timeLeftToDeath(1.0)
    {
    }

    Particle::~Particle()
    {
    }
}