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


        /// Steps the particle system.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds since the last update.
        void Update(double deltaTimeInSeconds);


        /// Sets the world positition of the particle system.
        void SetPosition(const glm::vec3 &newPosition);

        /// Sets the world orientation of the particle system.
        void SetOrientation(const glm::quat &newOrientation);

        /// Sets the transformation of the particle system all at once.
        void SetTransform(const glm::vec3 &newPosition, const glm::quat &newOrientation);



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


        /// The position of the particle system.
        glm::vec3 position;

        /// The orientation of the particle system.
        glm::quat orientation;


        /// The list of emitters making up this particle system.
        GTCore::Vector<ParticleEmitter*> emitters;



    private:    // No copying, for now.
        ParticleSystem(const ParticleSystem &);
        ParticleSystem & operator=(const ParticleSystem &);
    };
}

#endif