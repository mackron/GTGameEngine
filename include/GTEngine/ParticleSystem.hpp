// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleSystem_hpp_
#define __GTEngine_ParticleSystem_hpp_

#include "ParticleSystemDefinition.hpp"
#include "ParticleEmitter.hpp"

namespace GTEngine
{
    class ParticleSystem
    {
    public:

        /// Constructor.
        ParticleSystem(const ParticleSystemDefinition &definition);

        /// Destructor.
        ~ParticleSystem();


        /// Retrieves a reference to the definition.
        const ParticleSystemDefinition & GetDefinition() const { return this->definition; }



    private:

        /// The definition of the particle system.
        const ParticleSystemDefinition &definition;



    private:    // No copying, for now.
        ParticleSystem(const ParticleSystem &);
        ParticleSystem & operator=(const ParticleSystem &);
    };
}

#endif