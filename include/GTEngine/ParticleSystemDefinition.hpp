// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleSystemDefinition_hpp_
#define __GTEngine_ParticleSystemDefinition_hpp_

#include <GTCore/String.hpp>
#include "ParticleEmitter.hpp"

namespace GTEngine
{
    class ParticleSystemDefinition
    {
    public:

        /// Constructor.
        ParticleSystemDefinition();

        /// Destructor.
        ~ParticleSystemDefinition();


        /// Loads the particle system from a file.
        ///
        /// @remarks
        ///     If 'fileName' is absolute, a relative path must also be specified. It is an error for 'fileName' to be absolute and 'relativePath' to be null.
        bool LoadFromFile(const char* fileName, const char* relativePath = nullptr);


        /// Retrieves the absolute path.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path.
        const char* GetRelativePath() const;


        
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

        /// The absolute paht of the particle system.
        GTCore::String absolutePath;

        /// The relative path of the particle system.
        GTCore::String relativePath;


        /// The list of particle emitters making up the particle system. These emitters will always remain in their default state and
        /// should never be updated. Instead, they will be source of a copy operation which ParticleSystem instantiations will inherit.
        GTCore::Vector<ParticleEmitter*> emitters;
    };
}

#endif