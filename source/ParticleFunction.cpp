// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleFunction.hpp>

namespace GTEngine
{
    void* ParticleFunction::operator new(size_t nbytes)
    {
        return _mm_malloc(nbytes, 16);
    }
    
    void ParticleFunction::operator delete(void* buffer)
    {
        _mm_free(buffer);
    }
    
    
    
    
    
    
    ///////////////////////////////////////
    // Generic Scalar

    ParticleFunction_Scalar::ParticleFunction_Scalar(ParticleFunctionType type, float rangeMinIn, float rangeMaxIn)
        : ParticleFunction(type), rangeMin(rangeMinIn), rangeMax(rangeMaxIn)
    {
    }

    ParticleFunction_Scalar::ParticleFunction_Scalar(const ParticleFunction_Scalar &other)
        : ParticleFunction(other), rangeMin(other.rangeMin), rangeMax(other.rangeMax)
    {
    }

    ParticleFunction_Scalar::~ParticleFunction_Scalar()
    {
    }

    void ParticleFunction_Scalar::SetRange(float rangeMinIn, float rangeMaxIn)
    {
        this->rangeMin = rangeMinIn;
        this->rangeMax = rangeMaxIn;
    }

    void ParticleFunction_Scalar::GetRange(float &rangeMinOut, float &rangeMaxOut) const
    {
        rangeMinOut = this->rangeMin;
        rangeMaxOut = this->rangeMax;
    }

    float ParticleFunction_Scalar::Evaluate(float lifetimeRatio) const
    {
        return glm::mix(this->rangeMin, this->rangeMax, lifetimeRatio);
    }



    ///////////////////////////////////////
    // Generic Vector3

    ParticleFunction_Vector3::ParticleFunction_Vector3(ParticleFunctionType type, const glm::vec3 &rangeMinIn, const glm::vec3 &rangeMaxIn)
        : ParticleFunction(type), rangeMin(rangeMinIn), rangeMax(rangeMaxIn)
    {
    }

    ParticleFunction_Vector3::ParticleFunction_Vector3(const ParticleFunction_Vector3 &other)
        : ParticleFunction(other), rangeMin(other.rangeMin), rangeMax(other.rangeMax)
    {
    }

    ParticleFunction_Vector3::~ParticleFunction_Vector3()
    {
    }

    void ParticleFunction_Vector3::SetRange(const glm::vec3 &rangeMinIn, const glm::vec3 &rangeMaxIn)
    {
        this->rangeMin = rangeMinIn;
        this->rangeMax = rangeMaxIn;
    }

    void ParticleFunction_Vector3::GetRange(glm::vec3 &rangeMinOut, glm::vec3 &rangeMaxOut) const
    {
        rangeMinOut = this->rangeMin;
        rangeMaxOut = this->rangeMax;
    }

    glm::vec3 ParticleFunction_Vector3::Evaluate(float lifetimeRatio) const
    {
        return glm::mix(this->rangeMin, this->rangeMax, lifetimeRatio);
    }




    ///////////////////////////////////////
    // Size over Time

    void ParticleFunction_SizeOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        particle.scale = glm::simdVec4(this->Evaluate(lifetimeRatio));
    }



    ///////////////////////////////////////
    // Linear Velocity over Time

    void ParticleFunction_LinearVelocityOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        glm::vec3 temp = this->Evaluate(lifetimeRatio);
        particle.functionLinearVelocity = glm::simdVec4(temp.x, temp.y, temp.z, 1.0f);
    }
    


    ///////////////////////////////////////
    // Angular Velocity over Time

    void ParticleFunction_AngularVelocityOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        particle.angularVelocity = glm::fastMix(this->rangeMinQuat, this->rangeMaxQuat, lifetimeRatio);
    }

    void ParticleFunction_AngularVelocityOverTime::SetRange(const glm::vec3 &rangeMin, const glm::vec3 &rangeMax)
    {
        ParticleFunction_Vector3::SetRange(rangeMin, rangeMax);

        this->rangeMinQuat = glm::simdQuat(glm::radians(rangeMin));
        this->rangeMaxQuat = glm::simdQuat(glm::radians(rangeMax));
    }
}
