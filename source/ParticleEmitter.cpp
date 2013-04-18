// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleEmitter.hpp>
#include <GTEngine/Rendering.hpp>

namespace GTEngine
{
    ParticleEmitter::ParticleEmitter()
        : position(), orientation(),
          flags(0), durationInSeconds(0.0), emissionRatePerSecond(10.0), gravityFactor(0.0),
          emissionShapeType(EmissionShapeType_Cone), emissionShapeCone(), emissionShapeSphere(), emissionShapeBox(),
          startSpeedMin(5.0), startSpeedMax(5.0),
          startRotationMin(), startRotationMax(),
          lifetimeMin(5.0), lifetimeMax(5.0),
          material(nullptr),
          timeSinceLastEmission(1.0 / emissionRatePerSecond),
          random(),
          particles(),
          functions(),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3T2N3C4))
    {
        this->SetMaterial("engine/materials/simple-diffuse.material");
    }

    ParticleEmitter::ParticleEmitter(const ParticleEmitter &other)
        : position(other.position), orientation(other.orientation),
          flags(other.flags), durationInSeconds(other.durationInSeconds), emissionRatePerSecond(other.emissionRatePerSecond), gravityFactor(other.gravityFactor),
          emissionShapeType(other.emissionShapeType), emissionShapeCone(other.emissionShapeCone), emissionShapeSphere(other.emissionShapeSphere), emissionShapeBox(other.emissionShapeBox),
          startSpeedMin(other.startSpeedMin), startSpeedMax(other.startSpeedMax),
          startRotationMin(other.startRotationMin), startRotationMax(other.startRotationMax),
          lifetimeMin(other.lifetimeMin), lifetimeMax(other.lifetimeMax),
          material((other.material != nullptr) ? MaterialLibrary::CreateCopy(*other.material) : nullptr),
          timeSinceLastEmission(other.timeSinceLastEmission),
          random(other.random),
          particles(other.particles),
          functions(),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P3T2N3C4))
    {
        // The functions need to be copied over.
        for (size_t iFunction = 0; iFunction < other.GetFunctionCount(); ++iFunction)
        {
            auto &functionToCopy = other.GetFunction(iFunction);
            {
                switch (functionToCopy.GetType())
                {
                case ParticleFunctionType_SizeOverTime:
                    {
                        this->functions.PushBack(new ParticleFunction_SizeOverTime(reinterpret_cast<const ParticleFunction_SizeOverTime &>(functionToCopy)));
                        break;
                    }

                case ParticleFunctionType_LinearVelocityOverTime:
                    {
                        this->functions.PushBack(new ParticleFunction_LinearVelocityOverTime(reinterpret_cast<const ParticleFunction_LinearVelocityOverTime &>(functionToCopy)));
                        break;
                    }

                case ParticleFunctionType_AngularVelocityOverTime:
                    {
                        this->functions.PushBack(new ParticleFunction_AngularVelocityOverTime(reinterpret_cast<const ParticleFunction_AngularVelocityOverTime &>(functionToCopy)));
                        break;
                    }


                default: break;
                }
            }
        }


        this->vertexArray->SetData(other.vertexArray->GetVertexDataPtr(), other.vertexArray->GetVertexCount(), other.vertexArray->GetIndexDataPtr(), other.vertexArray->GetIndexCount());
    }


    ParticleEmitter::~ParticleEmitter()
    {
        for (size_t iFunction = 0; iFunction < this->functions.count; ++iFunction)
        {
            delete this->functions[iFunction];
        }

        Renderer::DeleteVertexArray(this->vertexArray);
    }


    void ParticleEmitter::Start()
    {
        this->HasDoneFirstEmission(false);
    }

    void ParticleEmitter::Stop()
    {
        this->HasDoneFirstEmission(false);
    }


    void ParticleEmitter::Update(double deltaTimeInSeconds, const glm::vec3 &gravity)
    {
        float deltaTimeInSecondsF = static_cast<float>(deltaTimeInSeconds);


        // Now we need to spawn some particles if applicable. We need to do this before stepping particles because we want
        // to do an initial iteration of each attached function.
        this->timeSinceLastEmission += deltaTimeInSeconds;
        int spawnCount = 0;

        if (!this->IsBurstModeEnabled())
        {
            double emissionRate = 1.0 / this->emissionRatePerSecond;
            if (this->timeSinceLastEmission >= emissionRate || !this->HasDoneFirstEmission())
            {
                spawnCount = static_cast<int>(this->timeSinceLastEmission / emissionRate);

                if (this->HasDoneFirstEmission())
                {
                    this->timeSinceLastEmission = 0.0;
                }
                else
                {
                    if (this->emissionRatePerSecond > 0.0)
                    {
                        spawnCount = 1;
                    }
                }
            }
        }
        else
        {
            if (this->durationInSeconds > 0.0 || !this->HasDoneFirstEmission())                         // <-- Ensures that if the duration is 0.0, only a single burst is done.
            {
                if (this->timeSinceLastEmission >= this->durationInSeconds)
                {
                    spawnCount = static_cast<int>(this->emissionRatePerSecond);
                    this->timeSinceLastEmission = 0.0;
                }
            }
        }


        for (int i = 0; i < spawnCount; ++i)
        {
            auto &particle = this->particles.PushNewParticle();



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

                    spawnPosition.x = normalizedPosition.x * this->emissionShapeCone.radius * std::sqrt(distanceFactor);
                    spawnPosition.y = normalizedPosition.y * this->emissionShapeCone.radius * std::sqrt(distanceFactor);
                    spawnPosition.z = 0.0f;

                    // Now we need an untransformed direction.
                    float rotationAngle    = this->emissionShapeCone.angle * distanceFactor;
                    glm::vec3 rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), glm::normalize(normalizedPosition));
                    
                    spawnDirection = glm::normalize(glm::angleAxis(rotationAngle, rotationAxis) * glm::vec3(0.0f, 0.0f, -1.0f));


                    break;
                }

            case EmissionShapeType_Sphere:
                {
                    this->random.NextSphere(this->emissionShapeSphere.radius, spawnPosition.x, spawnPosition.y, spawnPosition.z);
                    spawnDirection = glm::normalize(spawnPosition);

                    break;
                }

            case EmissionShapeType_Box:
                {
                    this->random.NextBox(this->emissionShapeBox.x, this->emissionShapeBox.y, this->emissionShapeBox.z, spawnPosition.x, spawnPosition.y, spawnPosition.z);

                    break;
                }

            default: break;
            }




            // Now we need to transform the position and calculate the velocity.
            glm::mat4 transform = glm::mat4_cast(this->orientation);
            transform[3]        = glm::vec4(this->position, 1.0f);

            particle.orientation = Math::quatFromEulerFast(glm::radians(glm::vec3(
                this->random.Next(this->startRotationMin.x, this->startRotationMax.x),
                this->random.Next(this->startRotationMin.y, this->startRotationMax.y),
                this->random.Next(this->startRotationMin.z, this->startRotationMax.z))));

            particle.position            = glm::vec3(transform * glm::vec4(spawnPosition, 1.0f));
            particle.spawnLinearVelocity = this->orientation * spawnDirection * static_cast<float>(this->random.Next(this->startSpeedMin, this->startSpeedMax));


            particle.timeLeftToDeath = particle.lifetime = this->random.Next(this->lifetimeMin, this->lifetimeMax);
        }




        // Here we will update any still-alive particles.
        size_t iParticle = 0;
        while (iParticle < this->particles.GetCount())
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
                    particle.gravityLinearVelocity += gravity * static_cast<float>(this->gravityFactor * deltaTimeInSeconds);

                    particle.linearVelocity  = particle.gravityLinearVelocity + particle.spawnLinearVelocity + particle.functionLinearVelocity;
                    particle.position       += particle.linearVelocity * deltaTimeInSecondsF;
                    particle.orientation     = particle.orientation * Math::fastMix(glm::quat(), particle.angularVelocity, deltaTimeInSecondsF);


                    // At this point we need to run all of the functions that are currently being used by the emitter.
                    float lifetimeRatio = static_cast<float>(1.0 - (particle.timeLeftToDeath / particle.lifetime));
                    for (size_t iFunction = 0; iFunction < this->functions.count; ++iFunction)
                    {
                        auto function = this->functions[iFunction];
                        assert(function != nullptr);
                        {
                            function->Execute(particle, lifetimeRatio);
                        }
                    }


                    ++iParticle;
                }
            }
        }



        // We need to have the emitter know that the first emission has been performed.
        if (spawnCount > 0)
        {
            this->HasDoneFirstEmission(true);
        }
    }


    void ParticleEmitter::EnableBurstMode()
    {
        this->flags = this->flags | BurstEnabled;
    }

    void ParticleEmitter::DisableBurstMode()
    {
        this->flags = this->flags & ~BurstEnabled;
    }

    bool ParticleEmitter::IsBurstModeEnabled() const
    {
        return (this->flags & BurstEnabled) != 0;
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


    double ParticleEmitter::GetGravityFactor() const
    {
        return this->gravityFactor;
    }

    void ParticleEmitter::SetGravityFactor(double newGravityFactor)
    {
        this->gravityFactor = newGravityFactor;
    }




    void ParticleEmitter::GetStartSpeed(double &startSpeedMinOut, double &startSpeedMaxOut) const
    {
        startSpeedMinOut = this->startSpeedMin;
        startSpeedMaxOut = this->startSpeedMax;
    }


    void ParticleEmitter::SetStartSpeed(double newStartSpeedMin, double newStartSpeedMax)
    {
        this->startSpeedMin = newStartSpeedMin;
        this->startSpeedMax = newStartSpeedMax;
    }


    void ParticleEmitter::GetLifetime(double &lifetimeMinOut, double &lifetimeMaxOut) const
    {
        lifetimeMinOut = this->lifetimeMin;
        lifetimeMaxOut = this->lifetimeMax;
    }

    void ParticleEmitter::SetLifetime(double newLifetimeMin, double newLifetimeMax)
    {
        this->lifetimeMin = newLifetimeMin;
        this->lifetimeMax = newLifetimeMax;
    }




    ParticleEmitter::EmissionShapeType ParticleEmitter::GetEmissionShapeType() const
    {
        return this->emissionShapeType;
    }

    void ParticleEmitter::SetEmissionShapeType(ParticleEmitter::EmissionShapeType newEmissionShapeType)
    {
        this->emissionShapeType = newEmissionShapeType;
    }


    void ParticleEmitter::SetConeEmissionShape(float radius, float angle)
    {
        this->emissionShapeType = EmissionShapeType_Cone;
        
        this->emissionShapeCone.radius = radius;
        this->emissionShapeCone.angle  = angle;
    }

    void ParticleEmitter::SetSphereEmissionShape(float radius)
    {
        this->emissionShapeType = EmissionShapeType_Sphere;

        this->emissionShapeSphere.radius = radius;
    }

    void ParticleEmitter::SetBoxEmissionShape(float x, float y, float z)
    {
        this->emissionShapeType = EmissionShapeType_Box;

        this->emissionShapeBox.x = x;
        this->emissionShapeBox.y = y;
        this->emissionShapeBox.z = z;
    }


    void ParticleEmitter::GetConeEmissionShape(float &radiusOut, float &angleOut) const
    {
        radiusOut = this->emissionShapeCone.radius;
        angleOut  = this->emissionShapeCone.angle;
    }

    void ParticleEmitter::GetSphereEmissionShape(float &radiusOut) const
    {
        radiusOut = this->emissionShapeSphere.radius;
    }

    void ParticleEmitter::GetBoxEmissionShape(float &xOut, float &yOut, float &zOut) const
    {
        xOut = this->emissionShapeBox.x;
        yOut = this->emissionShapeBox.y;
        zOut = this->emissionShapeBox.z;
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


    bool ParticleEmitter::SetMaterial(const char* relativePath)
    {
        auto newMaterial = MaterialLibrary::Create(relativePath);
        if (newMaterial != nullptr)
        {
            MaterialLibrary::Delete(this->material);
            this->material = newMaterial;

            return true;
        }

        return false;
    }


    size_t ParticleEmitter::GetFunctionCount() const
    {
        return this->functions.count;
    }

    ParticleFunction & ParticleEmitter::GetFunction(size_t index)
    {
        return *this->functions[index];
    }

    const ParticleFunction & ParticleEmitter::GetFunction(size_t index) const
    {
        return *this->functions[index];
    }

    void ParticleEmitter::RemoveFunctionByIndex(size_t index)
    {
        delete this->functions[index];
        this->functions.Remove(index);
    }

    ParticleFunction & ParticleEmitter::AddFunction(ParticleFunctionType type)
    {
        ParticleFunction* newFunction = nullptr;

        switch (type)
        {
        case ParticleFunctionType_SizeOverTime:
            {
                newFunction = new ParticleFunction_SizeOverTime;
                break;
            }

        case ParticleFunctionType_LinearVelocityOverTime:
            {
                newFunction = new ParticleFunction_LinearVelocityOverTime;
                break;
            }

        case ParticleFunctionType_AngularVelocityOverTime:
            {
                newFunction = new ParticleFunction_AngularVelocityOverTime;
                break;
            }

        default: break;
        }

        assert(newFunction != nullptr);


        this->functions.PushBack(newFunction);
        return* newFunction;
    }




    ////////////////////////////////////////////////
    // Private

    bool ParticleEmitter::HasDoneFirstEmission() const
    {
        return (this->flags & DoneFirstEmission) != 0;
    }

    void ParticleEmitter::HasDoneFirstEmission(bool doneFirstEmission)
    {
        if (doneFirstEmission)
        {
            this->flags = this->flags | DoneFirstEmission;
        }
        else
        {
            this->flags = this->flags & ~DoneFirstEmission;
        }
    }
}
