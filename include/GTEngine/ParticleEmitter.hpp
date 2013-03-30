// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleEmitter_hpp_
#define __GTEngine_ParticleEmitter_hpp_

#include "Particle.hpp"

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

        /// Destructor.
        ~ParticleEmitter();


        /// Updates the emitter.
        ///
        /// @param deltaTimeInSeconds [in] The delta time for doing time-based movement.
        ///
        /// @remarks
        ///     This will update the relevant properties of each particle and also spawn/delete particles where necessary.
        void Update(double deltaTimeInSeconds);



    private:



    };
}

#endif