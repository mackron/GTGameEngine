// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleFunction.hpp>

namespace GTEngine
{
    ///////////////////////////////////////
    // Size over Time

    void ParticleFunction_SizeOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        float scale = glm::mix(this->startSize, this->endSize, lifetimeRatio);

        particle.scale = glm::vec3(scale);
    }

    void ParticleFunction_SizeOverTime::SetStartAndEndSizes(float startSizeIn, float endSizeIn)
    {
        this->startSize = startSizeIn;
        this->endSize   = endSizeIn;
    }

    void ParticleFunction_SizeOverTime::GetStartAndEndSizes(float &startSizeOut, float &endSizeOut) const
    {
        startSizeOut = this->startSize;
        endSizeOut   = this->endSize;
    }



    ///////////////////////////////////////
    // Linear Velocity over Time

    void ParticleFunction_LinearVelocityOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        glm::vec3 velocity = glm::mix(this->startVelocity, this->endVelocity, lifetimeRatio);

        particle.functionLinearVelocity = velocity;
    }

    void ParticleFunction_LinearVelocityOverTime::SetStartAndEndVelocities(const glm::vec3 &startVelocityIn, const glm::vec3 &endVelocityIn)
    {
        this->startVelocity = startVelocityIn;
        this->endVelocity   = endVelocityIn;
    }

    void ParticleFunction_LinearVelocityOverTime::GetStartAndEndVelocities(glm::vec3 &startVelocityOut, glm::vec3 &endVelocityOut)
    {
        startVelocityOut = this->startVelocity;
        endVelocityOut   = this->endVelocity;
    }

    


    ///////////////////////////////////////
    // Angular Velocity over Time

    void ParticleFunction_AngularVelocityOverTime::Execute(Particle &particle, float lifetimeRatio)
    {
        glm::vec3 velocity = glm::mix(this->startVelocity, this->endVelocity, lifetimeRatio);

        particle.angularVelocity = velocity;
    }

    void ParticleFunction_AngularVelocityOverTime::SetStartAndEndVelocities(const glm::vec3 &startVelocityIn, const glm::vec3 &endVelocityIn)
    {
        this->startVelocity = startVelocityIn;
        this->endVelocity   = endVelocityIn;
    }

    void ParticleFunction_AngularVelocityOverTime::GetStartAndEndVelocities(glm::vec3 &startVelocityOut, glm::vec3 &endVelocityOut)
    {
        startVelocityOut = this->startVelocity;
        endVelocityOut   = this->endVelocity;
    }
}