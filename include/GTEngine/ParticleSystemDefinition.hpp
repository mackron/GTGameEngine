// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ParticleSystemDefinition_hpp_
#define __GTEngine_ParticleSystemDefinition_hpp_

#include <GTEngine/Core/String.hpp>
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
              ParticleEmitter* GetEmitter(size_t index);
        const ParticleEmitter* GetEmitter(size_t index) const;


        /// Appends a new emitter, returning a pointer to it.
        ///
        /// @return A pointer to the new emitter.
        ParticleEmitter* AppendNewEmitter();

        /// Deletes an emitter by it's index.
        ///
        /// @param index [in] The index of the emitter to delete.
        ///
        /// @remarks
        ///     The emitter will be completely deleted meaning any existing pointer to the emitter will be made invalid.
        void DeleteEmitterByIndex(size_t index);



        ///////////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the particle system definition.
        void Serialize(GTLib::Serializer &serializer);

        /// Deserializes the particle system definition.
        void Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// Clears the particle system, leaving it empty.
        void Clear();



    private:

        /// The absolute paht of the particle system.
        GTLib::String absolutePath;

        /// The relative path of the particle system.
        GTLib::String relativePath;


        /// The list of particle emitters making up the particle system. These emitters will always remain in their default state and
        /// should never be updated. Instead, they will be source of a copy operation which ParticleSystem instantiations will inherit.
        GTLib::Vector<ParticleEmitter*> emitters;
    };
}

#endif