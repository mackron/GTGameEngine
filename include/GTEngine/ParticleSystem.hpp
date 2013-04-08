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


        /// Refreshes the particle system to bring it up-to-date with it's attached definition.
        ///
        /// @remarks
        ///     You will typically not need to call this. It is mainly intended for use by editting tools.
        void Refresh();



        /// Retrieves the emitter count.
        size_t GetEmitterCount() const;

        /// Retrieves a pointer to the emitter at the given index.
        ///
        /// @param index [in] The index of the emitter to retrieve.
        ///
        /// @remarks
        ///     This asserts that the index is valid.
        const ParticleEmitter* GetEmitter(size_t index) const;


    private:

        /// Clears the particle system.
        void Clear();



    private:

        /// The definition of the particle system.
        const ParticleSystemDefinition &definition;


        /// The list of emitters making up this particle system.
        GTCore::Vector<ParticleEmitter*> emitters;



    private:    // No copying, for now.
        ParticleSystem(const ParticleSystem &);
        ParticleSystem & operator=(const ParticleSystem &);
    };
}

#endif