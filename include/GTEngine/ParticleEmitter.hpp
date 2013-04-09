// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleEmitter_hpp_
#define __GTEngine_ParticleEmitter_hpp_

#include "Particle.hpp"
#include "MaterialLibrary.hpp"

namespace GTEngine
{
    /// Class used for emitting particles.
    ///
    /// The particles emitted by an emitter is basically just a big mesh.
    class ParticleEmitter
    {
    public:
        
        /// Constructor.
        ParticleEmitter();

        /// Copy constructor.
        ParticleEmitter(const ParticleEmitter &other);


        /// Destructor.
        ~ParticleEmitter();


        /// Starts the emitter.
        ///
        /// @remarks
        ///     If the emitter is in burst mode, the initial burst of particles will occur when Update() is next called.
        void Start();

        /// Stops the emitter.
        ///
        /// @remarks
        ///     Stopping the emitter does not destroy the particles. So long as Update() is called, particles will continue to animate
        ///     until they are themselves or the emitter is destroyed.
        void Stop();


        /// Updates the emitter.
        ///
        /// @param deltaTimeInSeconds [in] The delta time for doing time-based movement.
        ///
        /// @remarks
        ///     This will update the relevant properties of each particle and also spawn/delete particles where necessary.
        ///     @par
        ///     This does not orientate particles towards the camera. That needs to be done at render time.
        void Update(double deltaTimeInSeconds);


        /// Sets the duration of the emitter in seconds.
        ///
        /// @param durationInSeconds [in] The new duration of the emitter, in seconds.
        ///
        /// @remarks
        ///     If the duration is set to 0, the emitter will loop forever until it is explicitly stopped.
        void SetDurationInSeconds(double durationInSeconds);

        /// Retrieves the duration of the emitter.
        double GetDurationInSeconds() const;


        /// Sets the number of particles to emit per second, or in the case of burst mode the number of particles to emit during the burst.
        void SetEmissionRatePerSecond(unsigned int emissionRatePerSecond);

        /// Retrieves the number of particles to emit per second.
        unsigned int GetEmissionRatePerSecond() const;


        /// Enables burst mode.
        void EnableBurstMode();

        /// Disables burst mode.
        void DisableBurstMode();

        /// Determines whether or not burst mode is enabled.
        bool IsBurstModeEnabled() const;




        /// Retrieves the position of the emitter.
        const glm::vec3 & GetPosition() const;

        /// Sets the position of the emitter.
        void SetPosition(const glm::vec3 &position);

        
        /// Retrieves the orientation of the emitter.
        const glm::quat & GetOrientation() const;

        /// Sets the orientation of the emitter.
        void SetOrientation(const glm::quat &orientation);


        /// Retrieves the normalized forward vector of the emitter.
        glm::vec3 GetForwardVector() const;



        /// Sets the material to use with each particle.
        void SetMaterial(const char* relativePath);



    private:

        /// Keeps track of the amount of time to emit particles in seconds. When this is set to 0, the emitter will emit particles infinately. If particles
        /// are being bursted, this will control the amount of time between bursts.
        double durationInSeconds;

        /// The number of particles to spawn per second. If burst mode is enabled, this will control the number of particles to burst.
        unsigned int emissionRatePerSecond;

        /// Keeps track of whether or not the emitter is in burst mode. False by default.
        bool burst;



        /// The position of the emitter.
        glm::vec3 position;

        /// The orientation of the emitter.
        glm::quat orientation;



        /// The material that will be cloned for each particle.
        Material* material;
        
        
        
    private:    // No assignment operator.
        ParticleEmitter & operator=(const ParticleEmitter &);
    };
}

#endif
