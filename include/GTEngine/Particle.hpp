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
            : position(0.0f, 0.0f, 0.0f, 1.0f), orientation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f, 1.0f),
              linearVelocity(0.0f, 0.0f, 0.0f, 1.0f), angularVelocity(1.0f, 0.0f, 0.0f, 0.0f),
              spawnLinearVelocity(0.0f, 0.0f, 0.0f, 1.0f), gravityLinearVelocity(0.0f, 0.0f, 0.0f, 1.0f), functionLinearVelocity(0.0f, 0.0f, 0.0f, 1.0f),
              lifetime(1.0), timeLeftToDeath(1.0),
              uTexCoordMin(0.0f), uTexCoordMax(1.0f),
              vTexCoordMin(0.0f), vTexCoordMax(1.0f)
        {
        }



        /// The position of the particle.
        glm::simdVec4 position;

        /// The orienation of the particle.
        glm::simdQuat orientation;

        /// The scale of the particle.e
        glm::simdVec4 scale;


        /// The current linear velocity of the particle.
        glm::simdVec4 linearVelocity;

        /// The current angular velocity of the particle.
        glm::simdQuat angularVelocity;


        /// The linear velocity of the particle when it was spawned.
        glm::simdVec4 spawnLinearVelocity;

        /// The current linear velocity applied to the particle due to gravity.
        glm::simdVec4 gravityLinearVelocity;

        /// The linear velocity of the particle as defined by the linear-velocity functions.
        glm::simdVec4 functionLinearVelocity;


        /// The lifetime of the particle.
        double lifetime;

        /// The amount of time left before the particle dies.
        double timeLeftToDeath;


        /// The low U texture coordinate to use with the particle.
        float uTexCoordMin;

        /// The high U texture coordinate to use with the particle.
        float uTexCoordMax;

        /// The low V texture coordinate to use with the particle.
        float vTexCoordMin;

        /// The high V texture coordinate to use with the particle.
        float vTexCoordMax;
    };
}

#endif