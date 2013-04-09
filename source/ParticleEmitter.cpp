// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleEmitter.hpp>
#include <GTEngine/Rendering.hpp>

namespace GTEngine
{
    ParticleEmitter::ParticleEmitter()
        : durationInSeconds(0.0), emissionRatePerSecond(10U), burst(false),
          startSpeed(5.0), lifetime(5.0),
          position(), orientation(),
          material(nullptr),
          timeSinceLastEmission(0.0),
          particles(),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3T2N3C4))
    {
        this->SetMaterial("engine/materials/simple-diffuse.material");
    }

    ParticleEmitter::ParticleEmitter(const ParticleEmitter &other)
        : durationInSeconds(other.durationInSeconds), emissionRatePerSecond(other.emissionRatePerSecond), burst(other.burst),
          startSpeed(other.startSpeed), lifetime(other.lifetime),
          position(other.position), orientation(other.orientation),
          material((other.material != nullptr) ? MaterialLibrary::CreateCopy(*other.material) : nullptr),
          timeSinceLastEmission(other.timeSinceLastEmission),
          particles(other.particles),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3T2N3C4))
    {
        this->vertexArray->SetData(other.vertexArray->GetVertexDataPtr(), other.vertexArray->GetVertexCount(), other.vertexArray->GetIndexDataPtr(), other.vertexArray->GetIndexCount());
    }


    ParticleEmitter::~ParticleEmitter()
    {
        Renderer::DeleteVertexArray(this->vertexArray);
    }


    void ParticleEmitter::Start()
    {
    }

    void ParticleEmitter::Stop()
    {
    }


    void ParticleEmitter::Update(double deltaTimeInSeconds)
    {
        // We will first update any still-alive particles.
        size_t iParticle = 0;
        while (iParticle < this->particles.count)
        {
            auto &particle = this->GetParticle(iParticle);
            {
                particle.timeLeftToDeath -= deltaTimeInSeconds;
                if (particle.timeLeftToDeath <= 0.0)
                {
                    // The particle is dead. Needs to be removed.
                    this->particles.Remove(iParticle);
                }
                else
                {
                    // The particle is still alive. We need to update it.
                    //
                    // For now, we will just move it in the direction of the emitter for the sake of testing.
                    particle.position += this->GetForwardVector() * static_cast<float>(this->startSpeed * deltaTimeInSeconds);

                    ++iParticle;
                }
            }
        }


        // Now we need to spawn some particles if applicable.
        this->timeSinceLastEmission += deltaTimeInSeconds;
        if (this->timeSinceLastEmission >= 1.0 / static_cast<double>(this->emissionRatePerSecond))
        {
            Particle particle;
            particle.position        = this->position;
            particle.timeLeftToDeath = this->lifetime;
            this->particles.PushBack(particle);

            this->timeSinceLastEmission = 0.0;
        }
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



    double ParticleEmitter::GetStartSpeed() const
    {
        return this->startSpeed;
    }

    void ParticleEmitter::SetStartSpeed(double newStartSpeed)
    {
        this->startSpeed = newStartSpeed;
    }


    double ParticleEmitter::GetLifetime() const
    {
        return this->lifetime;
    }

    void ParticleEmitter::SetLifetime(double newLifetime)
    {
        this->lifetime = newLifetime;
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
