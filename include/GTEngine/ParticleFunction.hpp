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


    /// Base class for basic scalar functions.
    class ParticleFunction_Scalar : public ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction_Scalar(ParticleFunctionType type, float rangeMin, float rangeMax);

        /// Copy constructor.
        ParticleFunction_Scalar(const ParticleFunction_Scalar &other);
        
        /// Destructor.
        virtual ~ParticleFunction_Scalar();



        /// Sets the range.
        ///
        /// @param rangeMin [in] The minimum value in the range.
        /// @param rangeMax [in] The maximum value in the range.
        void SetRange(float rangeMin, float rangeMax);

        /// Retrieves the range.
        ///
        /// @param rangeMinOut [out] A reference to the variable that will receive the min value.
        /// @param rangeMaxOut [out] A reference to the variable that will receive the max value.
        void GetRange(float &rangeMinOut, float &rangeMaxOut) const;


        /// Evaluates the function.
        ///
        /// @param lifetimeRatio [in] A value between 0 and 1 that specifies where in it's life the particle is currently at.
        float Evaluate(float lifetimeRatio) const;


    private:

        /// The min value in the range.
        float rangeMin;

        /// The max value in the range.
        float rangeMax;
    };


    /// Base class for basic vec3 functions.
    class ParticleFunction_Vector3 : public ParticleFunction
    {
    public:

        /// Constructor.
        ParticleFunction_Vector3(ParticleFunctionType type, const glm::vec3 &rangeMin, const glm::vec3 &rangeMax);

        /// Copy constructor.
        ParticleFunction_Vector3(const ParticleFunction_Vector3 &other);

        /// Destructor.
        virtual ~ParticleFunction_Vector3();


        /// Sets the range.
        ///
        /// @param rangeMin [in] The minimum value in the range.
        /// @param rangeMax [in] The maximum value in the range.
        virtual void SetRange(const glm::vec3 &rangeMin, const glm::vec3 &rangeMax);

        /// Retrieves the range.
        ///
        /// @param rangeMinOut [out] A reference to the variable that will receive the min value.
        /// @param rangeMaxOut [out] A reference to the variable that will receive the max value.
        virtual void GetRange(glm::vec3 &rangeMinOut, glm::vec3 &rangeMaxOut) const;


        /// Evaluates the function.
        ///
        /// @param lifetimeRatio [in] A value between 0 and 1 that specifies where in it's life the particle is currently at.
        glm::vec3 Evaluate(float lifetimeRatio) const;




    private:

        /// The min value in the range.
        glm::vec3 rangeMin;

        /// The max value in the range.
        glm::vec3 rangeMax;
    };




    /// The Size over Time particle function.
    class ParticleFunction_SizeOverTime : public ParticleFunction_Scalar
    {
    public:

        /// Constructor.
        ParticleFunction_SizeOverTime()
            : ParticleFunction_Scalar(ParticleFunctionType_SizeOverTime, 1.0f, 1.0f)
        {
        }

        /// Copy constructor.
        ParticleFunction_SizeOverTime(const ParticleFunction_SizeOverTime &other)
            : ParticleFunction_Scalar(other)
        {
        }



        /// ParticleFunction::Execute().
        void Execute(Particle &particle, float lifetimeRatio);
    };



    /// The Linear Velocity over Time particle function.
    class ParticleFunction_LinearVelocityOverTime : public ParticleFunction_Vector3
    {
    public:

        /// Constructor.
        ParticleFunction_LinearVelocityOverTime()
            : ParticleFunction_Vector3(ParticleFunctionType_LinearVelocityOverTime, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        {
        }

        /// Copy constructor.
        ParticleFunction_LinearVelocityOverTime(const ParticleFunction_LinearVelocityOverTime &other)
            : ParticleFunction_Vector3(other)
        {
        }


        /// ParticleFunction::Execute()
        void Execute(Particle &particle, float lifetimeRatio);
    };




    /// The Angular Velocity over Time particle function.
    class ParticleFunction_AngularVelocityOverTime : public ParticleFunction_Vector3
    {
    public:

        /// Constructor.
        ParticleFunction_AngularVelocityOverTime()
            : ParticleFunction_Vector3(ParticleFunctionType_AngularVelocityOverTime, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
              rangeMinQuat(), rangeMaxQuat()
        {
        }

        /// Copy constructor.
        ParticleFunction_AngularVelocityOverTime(const ParticleFunction_AngularVelocityOverTime &other)
            : ParticleFunction_Vector3(other),
              rangeMinQuat(other.rangeMinQuat), rangeMaxQuat(other.rangeMaxQuat)
        {
        }


        /// ParticleFunction::Execute()
        void Execute(Particle &particle, float lifetimeRatio);

        /// ParticleFunction_Vector3::SetRange().
        void SetRange(const glm::vec3 &rangeMin, const glm::vec3 &rangeMax);



    private:

        /// The min value in the range, as a quaternion.
        glm::quat rangeMinQuat;

        /// The max value in the range, as a quaternion.
        glm::quat rangeMaxQuat;
    };
}

#endif