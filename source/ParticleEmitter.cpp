// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleEmitter.hpp>

namespace GTEngine
{
    ParticleEmitter::ParticleEmitter()
        : durationInSeconds(5.0f), emissionRatePerSecond(10U), burst(false),
          position(), orientation(),
          material(nullptr)
    {
    }

    ParticleEmitter::~ParticleEmitter()
    {
    }


    void ParticleEmitter::Start()
    {
    }

    void ParticleEmitter::Stop()
    {
    }


    void ParticleEmitter::Update(double deltaTimeInSeconds, const glm::vec3 &cameraPosition, const glm::quat &cameraOrientation)
    {
        (void)deltaTimeInSeconds;
        (void)cameraPosition;
        (void)cameraOrientation;
    }


    void ParticleEmitter::SetDurationInSeconds(double newDurationInSeconds)
    {
        this->durationInSeconds = newDurationInSeconds;
    }

    double ParticleEmitter::GetDurationInSeconds() const
    {
        return this->durationInSeconds;
    }


    void ParticleEmitter::SetEmissionRatePerSecond(unsigned int newEmissionRatePerSecond)
    {
        this->emissionRatePerSecond = newEmissionRatePerSecond;
    }

    unsigned int ParticleEmitter::GetEmissionRatePerSecond() const
    {
        return this->emissionRatePerSecond;
    }


    void ParticleEmitter::EnableBurstMode()
    {
        this->burst = true;
    }

    void ParticleEmitter::DisableBurstMode()
    {
        this->burst = false;
    }

    bool ParticleEmitter::IsBurstModeEnabled() const
    {
        return this->burst;
    }



    const glm::vec3 & ParticleEmitter::GetPosition() const
    {
        return this->position;
    }

    void ParticleEmitter::SetPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;
    }


    const glm::quat & ParticleEmitter::GetOrientation() const
    {
        return this->orientation;
    }

    void ParticleEmitter::SetOrientation(const glm::quat &newOrientation)
    {
        this->orientation = newOrientation;
    }

    
    glm::vec3 ParticleEmitter::GetForwardVector() const
    {
        return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }


    void ParticleEmitter::SetMaterial(const char* relativePath)
    {
        MaterialLibrary::Delete(this->material);
        this->material = MaterialLibrary::Create(relativePath);

        // TODO: Apply the material to all existing particles.
    }
}
