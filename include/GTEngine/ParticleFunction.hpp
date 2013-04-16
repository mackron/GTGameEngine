// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleFunction_hpp_
#define __GTEngine_ParticleFunction_hpp_

#include "ParticleFunctionTypes.hpp"
#include "Particle.hpp"

namespace GTEngine
{
    /// Base class representing a function that is run on a particle.
    class ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction(ParticleFunctionType typeIn)
            : type(typeIn)
        {
        }

        /// Copy constructor.
        ParticleFunction(const ParticleFunction &other)
            : type(other.type)
        {
        }


        /// Retrieves the type of this function.
        ParticleFunctionType GetType() const
        {
            return this->type;
        }


        /// Executes the function.
        ///
        /// @param particle      [in] A reference to the particle that this function is being applied to.
        /// @param lifetimeRatio [in] A value between 0 and 1 that specifies where in it's life the particle is currently at.
        ///
        /// @remarks
        ///     When 'lifetimeRatio' is at 0 it means the particle is at the beginning of it's life. When it's at 1 it means the particle
        ///     is at the end of it's life.
        virtual void Execute(Particle &particle, float lifetimeRatio) = 0;


    private:

        /// The type of this function.
        ParticleFunctionType type;
    };


    /// The Size over Time particle function.
    class ParticleFunction_SizeOverTime : public ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction_SizeOverTime()
            : ParticleFunction(ParticleFunctionType_SizeOverTime),
              startSize(1.0f), endSize(1.0f)
        {
        }

        /// Copy constructor.
        ParticleFunction_SizeOverTime(const ParticleFunction_SizeOverTime &other)
            : ParticleFunction(other),
              startSize(other.startSize), endSize(other.endSize)
        {
        }



        /// ParticleFunction::Execute().
        void Execute(Particle &particle, float lifetimeRatio);


        /// Sets the start and end size this function should use in determining the size of the particle at a particlur point in time.
        ///
        /// @param startSizeIn [in] The scale of the particle at the beginning of it's life.
        /// @param endSizeIn   [in] The scale of the particle at the end of it's life.
        void SetStartAndEndSizes(float startSizeIn, float endSizeIn);

        /// Retrieves the start and end size this function is using in determining the size of the particle at a particular point in time.
        ///
        /// @param startSizeOut [out] A reference to the variable that will receive the start size.
        /// @param endSizeOut   [out] A reference to the variable that will receive the end size.
        void GetStartAndEndSizes(float &startSizeOut, float &endSizeOut) const;



    private:

        /// The size of the particle at the beginning of it's life.
        float startSize;

        /// The size of the particle at the end of it's life.
        float endSize;
    };



    /// The Linear Velocity over Time particle function.
    class ParticleFunction_LinearVelocityOverTime : public ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction_LinearVelocityOverTime()
            : ParticleFunction(ParticleFunctionType_LinearVelocityOverTime),
              startVelocity(0.0f, 0.0f, 0.0f), endVelocity(0.0f, 0.0f, 0.0f)
        {
        }

        /// Copy constructor.
        ParticleFunction_LinearVelocityOverTime(const ParticleFunction_LinearVelocityOverTime &other)
            : ParticleFunction(other),
              startVelocity(other.startVelocity), endVelocity(other.endVelocity)
        {
        }


        /// ParticleFunction::Execute()
        void Execute(Particle &particle, float lifetimeRatio);


        /// Sets the start and end linear velocities this function should use in determining the size of the particle at a particular point in time.
        ///
        /// @param startVelocityIn [in] The linear velocity of the particle at the beginning of it's life.
        /// @param endVelocityIn   [in] The linear velocity of the particle at the end of it's life.
        void SetStartAndEndVelocities(const glm::vec3 &startVelocityIn, const glm::vec3 &endVelocityIn);

        /// Retrieves the start and end linear velocities this function should use in determining the size of the particle at a particular point in time.
        ///
        /// @param startVelocityOut [out] A reference to the variable that will receive the start velocity.
        /// @param endVelocityOut   [out] A reference to the variable that will receive the end velocity.
        void GetStartAndEndVelocities(glm::vec3 &startVelocityOut, glm::vec3 &endVelocityOut);



    private:

        /// The velocity to apply to the particle at the beginning of it's life.
        glm::vec3 startVelocity;

        /// The velocity to apply to the particle at the end of it's life.
        glm::vec3 endVelocity;
    };




    /// The Angular Velocity over Time particle function.
    class ParticleFunction_AngularVelocityOverTime : public ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction_AngularVelocityOverTime()
            : ParticleFunction(ParticleFunctionType_AngularVelocityOverTime),
              startVelocity(0.0f, 0.0f, 0.0f), endVelocity(0.0f, 0.0f, 0.0f)
        {
        }

        /// Copy constructor.
        ParticleFunction_AngularVelocityOverTime(const ParticleFunction_AngularVelocityOverTime &other)
            : ParticleFunction(other),
              startVelocity(other.startVelocity), endVelocity(other.endVelocity)
        {
        }


        /// ParticleFunction::Execute()
        void Execute(Particle &particle, float lifetimeRatio);


        /// Sets the start and end linear velocities this function should use in determining the size of the particle at a particular point in time.
        ///
        /// @param startVelocityIn [in] The linear velocity of the particle at the beginning of it's life.
        /// @param endVelocityIn   [in] The linear velocity of the particle at the end of it's life.
        void SetStartAndEndVelocities(const glm::vec3 &startVelocityIn, const glm::vec3 &endVelocityIn);

        /// Retrieves the start and end linear velocities this function should use in determining the size of the particle at a particular point in time.
        ///
        /// @param startVelocityOut [out] A reference to the variable that will receive the start velocity.
        /// @param endVelocityOut   [out] A reference to the variable that will receive the end velocity.
        void GetStartAndEndVelocities(glm::vec3 &startVelocityOut, glm::vec3 &endVelocityOut);



    private:

        /// The velocity to apply to the particle at the beginning of it's life.
        glm::vec3 startVelocity;

        /// The velocity to apply to the particle at the end of it's life.
        glm::vec3 endVelocity;
    };
}

#endif