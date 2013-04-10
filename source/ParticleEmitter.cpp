// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleEmitter.hpp>
#include <GTEngine/Rendering.hpp>

namespace GTEngine
{
    ParticleEmitter::ParticleEmitter()
        : burst(false), durationInSeconds(0.0), emissionRatePerSecond(10.0),
          startSpeed(5.0), lifetime(5.0),
          gravityFactor(0.0),
          emissionShapeType(EmissionShapeType_Cone), emissionShape(),
          position(), orientation(),
          material(nullptr),
          timeSinceLastEmission(0.0),
          random(),
          particles(),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3T2N3C4))
    {
        this->SetMaterial("engine/materials/simple-diffuse.material");

        this->emissionShape.cone.angle  = 25.0f;
        this->emissionShape.cone.radius = 1.0f;
    }

    ParticleEmitter::ParticleEmitter(const ParticleEmitter &other)
        : burst(other.burst), durationInSeconds(other.durationInSeconds), emissionRatePerSecond(other.emissionRatePerSecond),
          startSpeed(other.startSpeed), lifetime(other.lifetime),
          gravityFactor(other.gravityFactor),
          emissionShapeType(other.emissionShapeType), emissionShape(other.emissionShape),
          position(other.position), orientation(other.orientation),
          material((other.material != nullptr) ? MaterialLibrary::CreateCopy(*other.material) : nullptr),
          timeSinceLastEmission(other.timeSinceLastEmission),
          random(other.random),
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


    void ParticleEmitter::Update(double deltaTimeInSeconds, const glm::vec3 &gravity)
    {
        float deltaTimeInSecondsF = static_cast<float>(deltaTimeInSeconds);

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
                    particle.velocity += gravity * static_cast<float>(this->gravityFactor * deltaTimeInSeconds);
                    particle.position += particle.velocity * deltaTimeInSecondsF;

                    ++iParticle;
                }
            }
        }


        // Now we need to spawn some particles if applicable.
        this->timeSinceLastEmission += deltaTimeInSeconds;

        double emissionRate = 1.0 / static_cast<double>(this->emissionRatePerSecond);
        if (this->timeSinceLastEmission >= emissionRate)
        {
            int spawnCount = static_cast<int>(this->timeSinceLastEmission / emissionRate);
            for (int i = 0; i < spawnCount; ++i)
            {
                Particle particle;

                // The spawning position and direction is different depending on the spawn shape. The position and direction is calculated
                // in local space, which is then later transformed by the emitters transform.
                glm::vec3 spawnPosition  = glm::vec3(0.0f, 0.0f,  0.0f);
                glm::vec3 spawnDirection = glm::vec3(0.0f, 0.0f, -1.0f);

                switch (this->emissionShapeType)
                {
                case EmissionShapeType_Cone:
                    {
                        // We first get an untransformed random position on the surface of a circle. The z axis will be 0.0.
                        glm::vec3 normalizedPosition;
                        this->random.NextCircle(1.0f, normalizedPosition.x, normalizedPosition.y);

                        float distanceFactor = glm::length(normalizedPosition);

                        spawnPosition.x = normalizedPosition.x * this->emissionShape.cone.radius * std::sqrt(distanceFactor);
                        spawnPosition.y = normalizedPosition.y * this->emissionShape.cone.radius * std::sqrt(distanceFactor);
                        spawnPosition.z = 0.0f;

                        // Now we need an untransformed direction.
                        float rotationAngle    = this->emissionShape.cone.angle * distanceFactor;
                        glm::vec3 rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), glm::normalize(normalizedPosition));
                    
                        spawnDirection = glm::normalize(glm::angleAxis(rotationAngle, rotationAxis) * glm::vec3(0.0f, 0.0f, -1.0f));


                        break;
                    }

                case EmissionShapeType_Sphere:
                    {
                        this->random.NextSphere(this->emissionShape.sphere.radius, spawnPosition.x, spawnPosition.y, spawnPosition.z);
                        spawnDirection = glm::normalize(spawnPosition);

                        break;
                    }

                case EmissionShapeType_Box:
                    {
                        this->random.NextBox(this->emissionShape.box.x, this->emissionShape.box.y, this->emissionShape.box.z, spawnPosition.x, spawnPosition.y, spawnPosition.z);

                        break;
                    }

                default: break;
                }


                // Now we need to transform the position and calculate the velocity.
                glm::mat4 transform = glm::mat4_cast(this->orientation);
                transform[3]        = glm::vec4(this->position, 1.0f);

                particle.position = glm::vec3(transform * glm::vec4(spawnPosition, 1.0f));
                particle.velocity = this->orientation * spawnDirection * static_cast<float>(this->startSpeed);



                //particle.scale = glm::vec3(0.2f);  // <-- TEMP!

                particle.timeLeftToDeath = this->lifetime;
                this->particles.PushBack(particle);
            }

            this->timeSinceLastEmission = 0.0;
        }
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



    void ParticleEmitter::SetDurationInSeconds(double newDurationInSeconds)
    {
        this->durationInSeconds = newDurationInSeconds;
    }

    double ParticleEmitter::GetDurationInSeconds() const
    {
        return this->durationInSeconds;
    }


    void ParticleEmitter::SetEmissionRatePerSecond(double newEmissionRatePerSecond)
    {
        this->emissionRatePerSecond = newEmissionRatePerSecond;
    }

    double ParticleEmitter::GetEmissionRatePerSecond() const
    {
        return this->emissionRatePerSecond;
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


    double ParticleEmitter::GetGravityFactor() const
    {
        return this->gravityFactor;
    }

    void ParticleEmitter::SetGravityFactor(double newGravityFactor)
    {
        this->gravityFactor = newGravityFactor;
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
