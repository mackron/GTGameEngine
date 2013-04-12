// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Particle_hpp_
#define __GTEngine_Particle_hpp_

#include "Math.hpp"

namespace GTEngine
{
    /// Class representing an individual particle emitted by an emitter.
    class Particle
    {
    public:

        /// Constructor.
        Particle()
            : position(), orientation(), scale(1.0f, 1.0f, 1.0f), linearVelocity(), angularVelocity(glm::vec3(0.0f, 0.0f, 0.0f)), timeLeftToDeath(1.0)
        {
        }



        /// The position of the particle.
        glm::vec3 position;

        /// The orienation of the particle.
        glm::quat orientation;

        /// The scale of the particle.e
        glm::vec3 scale;


        /// The current linear velocity of the particle.
        glm::vec3 linearVelocity;

        /// The current angular velocity of the particle.
        glm::vec3 angularVelocity;


        /// The amount of time left before the particle dies.
        double timeLeftToDeath;
    };
}

#endif