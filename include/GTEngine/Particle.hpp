// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

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
              colour(1.0f, 1.0f, 1.0f, 1.0f),
              lifetime(1.0), timeLeftToDeath(1.0),
              uvTileInterpolationFactor(0.0f),
              uTexCoordMin0(0.0f), uTexCoordMax0(1.0f),
              vTexCoordMin0(0.0f), vTexCoordMax0(1.0f),
              uTexCoordMin1(0.0f), uTexCoordMax1(1.0f),
              vTexCoordMin1(0.0f), vTexCoordMax1(1.0f)
        {
        }



        /// The position of the particle.
        glm::vec4 position;

        /// The orienation of the particle.
        glm::quat orientation;

        /// The scale of the particle.e
        glm::vec4 scale;


        /// The current linear velocity of the particle.
        glm::vec4 linearVelocity;

        /// The current angular velocity of the particle.
        glm::quat angularVelocity;


        /// The linear velocity of the particle when it was spawned.
        glm::vec4 spawnLinearVelocity;

        /// The current linear velocity applied to the particle due to gravity.
        glm::vec4 gravityLinearVelocity;

        /// The linear velocity of the particle as defined by the linear-velocity functions.
        glm::vec4 functionLinearVelocity;


        /// The current colour of the particle.
        glm::vec4 colour;


        /// The lifetime of the particle.
        double lifetime;

        /// The amount of time left before the particle dies.
        double timeLeftToDeath;



        /// The interpolation factor to use when interpolating between texture tiles.
        float uvTileInterpolationFactor;


        /// The first low U texture coordinate to use with the particle.
        float uTexCoordMin0;

        /// The first high U texture coordinate to use with the particle.
        float uTexCoordMax0;

        /// The first low V texture coordinate to use with the particle.
        float vTexCoordMin0;

        /// The first high V texture coordinate to use with the particle.
        float vTexCoordMax0;


        /// The second low U texture coordinate to use with the particle.
        float uTexCoordMin1;

        /// The second high U texture coordinate to use with the particle.
        float uTexCoordMax1;

        /// The second low V texture coordinate to use with the particle.
        float vTexCoordMin1;

        /// The second high V texture coordinate to use with the particle.
        float vTexCoordMax1;
    };
}

#endif