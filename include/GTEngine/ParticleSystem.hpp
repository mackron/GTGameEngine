// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ParticleSystem
#define GT_ParticleSystem

#include "ParticleSystemDefinition.hpp"
#include "ParticleEmitter.hpp"

namespace GT
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


        /// Resets the particle system to it's initial state.
        void Reset();

        /// Deletes all of the currently alive particles of every emitter, but does not reset timers.
        void ClearParticles();



        /// Steps the particle system.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds since the last update.
        void Update(double deltaTimeInSeconds);


        /// Retrieves the emitter count.
        size_t GetEmitterCount() const;

        /// Retrieves a pointer to the emitter at the given index.
        ///
        /// @param index [in] The index of the emitter to retrieve.
        ///
        /// @remarks
        ///     This asserts that the index is valid.
        const ParticleEmitter* GetEmitter(size_t index) const;


        /// Sets the world positition of the particle system.
        void SetPosition(const glm::vec3 &newPosition);

        /// Sets the world orientation of the particle system.
        void SetOrientation(const glm::quat &newOrientation);

        /// Sets the transformation of the particle system all at once.
        void SetTransform(const glm::vec3 &newPosition, const glm::quat &newOrientation);


        /// Retrieves the AABB of the particle system.
        ///
        /// @param aabbMin [out] A reference to the variable that will receive the minimum bounds of the AABB.
        /// @param aabbMax [out] A reference to the variable that will receive the maximum bounds of the AABB.
        void GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const;


        /// Sets the gravity velocity.
        ///
        /// @remarks
        ///     This will be applied to all existing and new emitters.
        void SetGravity(const glm::vec3 &gravity);

        /// Retrieves the gravity velocity.
        const glm::vec3 & GetGravity() const;


    private:

        /// Clears the particle system.
        void Clear();



    private:

        /// The definition of the particle system.
        const ParticleSystemDefinition &definition;


        /// The list of emitters making up this particle system.
        Vector<ParticleEmitter*> emitters;


        /// The position of the particle system.
        glm::vec3 position;

        /// The orientation of the particle system.
        glm::quat orientation;


        /// The gravity to apply to all particles of all emitters. Each emitter will multiply this with a factor to control how much
        /// influence the gravity has on the particles.
        glm::vec3 gravity;


    private:    // No copying, for now.
        ParticleSystem(const ParticleSystem &);
        ParticleSystem & operator=(const ParticleSystem &);
    };
}

#endif