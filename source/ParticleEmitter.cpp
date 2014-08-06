// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ParticleEmitter.hpp>
#include <GTEngine/Rendering.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wcast-align"
    #endif
#endif

namespace GTEngine
{
    inline void MixFromOriginSin(float angle, float a, float &s0, float &s1, float &d)
    {
        static const __m128 c0 = _mm_set1_ps(0.16666666666666666666666666666667f);
        static const __m128 c1 = _mm_set1_ps(0.00833333333333333333333333333333f);
        static const __m128 c2 = _mm_set1_ps(0.00019841269841269841269841269841f);

        __m128 x  = _mm_set_ps((1.0f - a) * angle, a * angle, angle, 0.0f);

        
        __m128 x3 = _mm_mul_ps(x,  _mm_mul_ps(x, x));
        __m128 x5 = _mm_mul_ps(x3, _mm_mul_ps(x, x));
        __m128 x7 = _mm_mul_ps(x5, _mm_mul_ps(x, x));

        __m128 y0 = _mm_mul_ps(x3, c0);
        __m128 y1 = _mm_mul_ps(x5, c1);
        __m128 y2 = _mm_mul_ps(x7, c2);
        
        __m128 y  = _mm_sub_ps(_mm_add_ps(_mm_sub_ps(x, y0), y1), y2);


        GLM_ALIGN(16) float results[4];
        _mm_store_ps(results, y);

        s0 =        results[3];
        s1 =        results[2];
        d  = 1.0f / results[1];
    }


    inline glm::simdQuat MixFromOrigin(const glm::simdQuat &q, float a)
    {
        if (q.w > 1.0f - glm::epsilon<float>())
        {
            return q;
        }
        else
        {
            float angle = glm::fastAcos(q.w);

            float s0;
            float s1;
            float d;
            MixFromOriginSin(angle, a, s0, s1, d);


            glm::simdQuat result    = s1 * q;
                          result.w += s0;
                          result    = result * d;

            return result;
        }
    }



    ParticleEmitter::ParticleEmitter()
        : name(""), position(), orientation(),
          flags(0), durationInSeconds(0.0), delayInSeconds(0.0),
          gravityFactor(0.0), emissionRatePerSecond(10.0),
          emissionShapeType(EmissionShapeType_Cone), emissionShapeCone(), emissionShapeSphere(), emissionShapeBox(),
          startSpeedMin(5.0), startSpeedMax(5.0),
          startRotationMin(), startRotationMax(),
          startScaleMin(1.0f, 1.0f, 1.0f), startScaleMax(1.0f, 1.0f, 1.0f),
          lifetimeMin(5.0), lifetimeMax(5.0),
          material(nullptr),
          textureTilesX(1), textureTilesY(1),
          functions(),
          random(),
          timeSinceLastEmission(1.0 / emissionRatePerSecond),
          particles(),
          aabbMin(), aabbMax(),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P4T4N4C4))
    {
        this->SetMaterial("engine/materials/simple-diffuse.material");
    }

    ParticleEmitter::ParticleEmitter(const ParticleEmitter &other)
        : name(other.name), position(other.position), orientation(other.orientation),
          flags(other.flags), durationInSeconds(other.durationInSeconds),  delayInSeconds(other.delayInSeconds),
          gravityFactor(other.gravityFactor), emissionRatePerSecond(other.emissionRatePerSecond),
          emissionShapeType(other.emissionShapeType), emissionShapeCone(other.emissionShapeCone), emissionShapeSphere(other.emissionShapeSphere), emissionShapeBox(other.emissionShapeBox),
          startSpeedMin(other.startSpeedMin), startSpeedMax(other.startSpeedMax),
          startRotationMin(other.startRotationMin), startRotationMax(other.startRotationMax),
          startScaleMin(other.startScaleMin), startScaleMax(other.startScaleMax),
          lifetimeMin(other.lifetimeMin), lifetimeMax(other.lifetimeMax),
          material((other.material != nullptr) ? MaterialLibrary::CreateCopy(*other.material) : nullptr),
          textureTilesX(other.textureTilesX), textureTilesY(other.textureTilesY),
          functions(),
          random(other.random),
          timeSinceLastEmission(other.timeSinceLastEmission),
          particles(other.particles),
          aabbMin(other.aabbMin), aabbMax(other.aabbMax),
          vertexArray(Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P4T4N4C4))
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

                case ParticleFunctionType_ColourOverTime:
                    {
                        this->functions.PushBack(new ParticleFunction_ColourOverTime(reinterpret_cast<const ParticleFunction_ColourOverTime &>(functionToCopy)));
                        break;
                    }

                case ParticleFunctionType_AlphaOverTime:
                    {
                        this->functions.PushBack(new ParticleFunction_AlphaOverTime(reinterpret_cast<const ParticleFunction_AlphaOverTime &>(functionToCopy)));
                        break;
                    }


                default:
                    {
                        assert(false);
                        
                        GTEngine::Log("You've forgotten to handle the particle function in the ParticleEmitter copy constructor!");
                        break;
                    }
                }
            }
        }


        this->vertexArray->SetData(other.vertexArray->GetVertexDataPtr(), other.vertexArray->GetVertexCount(), other.vertexArray->GetIndexDataPtr(), other.vertexArray->GetIndexCount());
    }


    ParticleEmitter::~ParticleEmitter()
    {
        MaterialLibrary::Delete(this->material);

        for (size_t iFunction = 0; iFunction < this->functions.count; ++iFunction)
        {
            delete this->functions[iFunction];
        }

        Renderer::DeleteVertexArray(this->vertexArray);
    }


    void ParticleEmitter::Reset()
    {
        this->ClearParticles();

        // TODO: Reset the random seed.

        this->timeSinceLastEmission = 0.0;
        this->HasDoneFirstEmission(false);
    }

    void ParticleEmitter::ClearParticles()
    {
        this->particles.Clear();
    }


    void ParticleEmitter::Update(double deltaTimeInSeconds, const glm::vec3 &gravityIn)
    {
        // To begin with, we'll preallocate a buffer that should be about the right size.
        if (this->particles.GetBufferSize() == 0)
        {
            this->particles.Resize(static_cast<size_t>(this->lifetimeMax * this->emissionRatePerSecond));
        }



        float         deltaTimeInSecondsF = static_cast<float>(deltaTimeInSeconds);
        glm::simdVec4 gravity             = glm::simdVec4(gravityIn.x, gravityIn.y, gravityIn.z, 1.0f) * static_cast<float>(this->gravityFactor * deltaTimeInSeconds);
        glm::simdQuat orientationSIMD     = glm::simdQuat(this->orientation);


        // Now we need to spawn some particles if applicable. We need to do this before stepping particles because we want
        // to do an initial iteration of each attached function.
        this->timeSinceLastEmission += deltaTimeInSeconds;
        int spawnCount = 0;

        if (this->HasDoneFirstEmission() || this->timeSinceLastEmission >= this->delayInSeconds)   // <-- Delay
        {
            // We need to subtract the delay time from the time since the last emission so that the correct number of particles are spawned
            // for the first emission.
            if (!this->HasDoneFirstEmission())
            {
                this->timeSinceLastEmission -= this->delayInSeconds;
            }


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
                    if (this->timeSinceLastEmission >= this->durationInSeconds || !this->HasDoneFirstEmission())
                    {
                        spawnCount = static_cast<int>(this->emissionRatePerSecond);
                        this->timeSinceLastEmission = 0.0;
                    }
                }
            }
        }


        for (int i = 0; i < spawnCount; ++i)
        {
            auto &particle = this->particles.PushNewParticle();



            // The spawning position and direction is different depending on the spawn shape. The position and direction is calculated
            // in local space, which is then later transformed by the emitters transform.
            glm::simdVec4 spawnPosition  = glm::simdVec4(0.0f, 0.0f,  0.0f, 1.0f);
            glm::simdVec4 spawnDirection = glm::simdVec4(0.0f, 0.0f, -1.0f, 1.0f);

            switch (this->emissionShapeType)
            {
            case EmissionShapeType_Cone:
                {
                    // We first get an untransformed random position on the surface of a circle. The z axis will be 0.0.
                    glm::simdVec4 normalizedPosition;
                    this->random.NextCircle(1.0f, normalizedPosition.x, normalizedPosition.y);

                    float distanceFactor = glm::length(normalizedPosition);

                    spawnPosition.x = normalizedPosition.x * this->emissionShapeCone.radius * std::sqrt(distanceFactor);
                    spawnPosition.y = normalizedPosition.y * this->emissionShapeCone.radius * std::sqrt(distanceFactor);
                    spawnPosition.z = 0.0f;

                    // Now we need an untransformed direction.
                    float rotationAngle    = this->emissionShapeCone.angle * distanceFactor;
                    glm::simdVec4 rotationAxis = glm::cross(glm::simdVec4(0.0f, 0.0f, -1.0f, 1.0f), glm::normalize(normalizedPosition));
                    
                    spawnDirection = glm::normalize(glm::angleAxisSIMD(glm::radians(rotationAngle), glm::vec3(rotationAxis.x, rotationAxis.y, rotationAxis.z)) * glm::simdVec4(0.0f, 0.0f, -1.0f, 0.0f));


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

            default:
                {
                    assert(false);
                    break;
                }
            }




            // Now we need to transform the position and calculate the velocity.
            glm::simdMat4 transform = glm::mat4SIMD_cast(orientationSIMD);
            transform[3] = glm::simdVec4(this->position, 1.0f);

            particle.scale = glm::simdVec4(
                this->random.Next(this->startScaleMin.x, this->startScaleMax.x),
                this->random.Next(this->startScaleMin.y, this->startScaleMax.y),
                this->random.Next(this->startScaleMin.z, this->startScaleMax.z),
                1.0f);

            particle.orientation = glm::simdQuat(glm::radians(glm::vec3(
                this->random.Next(this->startRotationMin.x, this->startRotationMax.x),
                this->random.Next(this->startRotationMin.y, this->startRotationMax.y),
                this->random.Next(this->startRotationMin.z, this->startRotationMax.z))));

            particle.position            = transform * spawnPosition;
            particle.spawnLinearVelocity = (orientationSIMD * spawnDirection) * static_cast<float>(this->random.Next(this->startSpeedMin, this->startSpeedMax));


            particle.timeLeftToDeath = particle.lifetime = this->random.Next(this->lifetimeMin, this->lifetimeMax);
        }




        // Here we will update any still-alive particles.
        __m128 m128_aabbMin = _mm_set1_ps( FLT_MAX);
        __m128 m128_aabbMax = _mm_set1_ps(-FLT_MAX);


        const float    uTexCoordSize  = 1.0f / this->textureTilesX;
        const float    vTexCoordSize  = 1.0f / this->textureTilesY;
        const uint32_t tileCount      = this->textureTilesX * this->textureTilesY;


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
                    particle.gravityLinearVelocity += gravity;
                    particle.linearVelocity         = particle.gravityLinearVelocity + particle.spawnLinearVelocity + particle.functionLinearVelocity;

                    particle.position              += particle.linearVelocity * deltaTimeInSecondsF;
                    particle.orientation            = particle.orientation * MixFromOrigin(particle.angularVelocity, deltaTimeInSecondsF);

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


                    // The tile we pick will depend on the lifetime ratio of the particle.
                    float currentTile = glm::floor(lifetimeRatio * (tileCount - 1));

                    float uvTile0 = currentTile;
                    float uvTile1 = (currentTile < tileCount - 1) ? currentTile + 1 : currentTile;
                    

                    float uStartTile0      = glm::mod(uvTile0, static_cast<float>(this->textureTilesX));
                    float vStartTile0      = glm::floor(uvTile0 / this->textureTilesX);

                    particle.uTexCoordMin0 =        uStartTile0 / this->textureTilesX;
                    particle.uTexCoordMax0 = particle.uTexCoordMin0 + uTexCoordSize;
                    particle.vTexCoordMin0 = 1.0f - vStartTile0 / this->textureTilesY - vTexCoordSize;
                    particle.vTexCoordMax0 = particle.vTexCoordMin0 + vTexCoordSize;


                    float uStartTile1      = glm::mod(uvTile1, static_cast<float>(this->textureTilesX));
                    float vStartTile1      = glm::floor(uvTile1 / this->textureTilesX);

                    particle.uTexCoordMin1 =        uStartTile1 / this->textureTilesX;
                    particle.uTexCoordMax1 = particle.uTexCoordMin1 + uTexCoordSize;
                    particle.vTexCoordMin1 = 1.0f - vStartTile1 / this->textureTilesY - vTexCoordSize;
                    particle.vTexCoordMax1 = particle.vTexCoordMin1 + vTexCoordSize;


                    particle.uvTileInterpolationFactor = (uvTile1 - (lifetimeRatio * (tileCount - 1)));


                    // We need to check the position against the AABB.
                    m128_aabbMin = _mm_min_ps(m128_aabbMin, particle.position.Data);
                    m128_aabbMax = _mm_max_ps(m128_aabbMax, particle.position.Data);


                    ++iParticle;
                }
            }
        }


        
        GLM_ALIGN(16) float aabbMinFloats[4];
        GLM_ALIGN(16) float aabbMaxFloats[4];

        _mm_store_ps(aabbMinFloats, m128_aabbMin);
        _mm_store_ps(aabbMaxFloats, m128_aabbMax);

        this->aabbMin = glm::vec3(aabbMinFloats[0], aabbMinFloats[1], aabbMinFloats[2]);
        this->aabbMax = glm::vec3(aabbMaxFloats[0], aabbMaxFloats[1], aabbMaxFloats[2]);
        

        // AABB padding. Should probably find a way to do this more accurately.
        this->aabbMin -= glm::vec3(4.0f, 4.0f, 4.0f);
        this->aabbMax += glm::vec3(4.0f, 4.0f, 4.0f);


        // We need to have the emitter know that the first emission has been performed.
        if (spawnCount > 0)
        {
            this->HasDoneFirstEmission(true);
        }
    }


    const char* ParticleEmitter::GetName() const
    {
        return this->name.c_str();
    }

    void ParticleEmitter::SetName(const char* newName)
    {
        this->name = newName;
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


    void ParticleEmitter::SetDelayInSeconds(double newDelayInSeconds)
    {
        this->delayInSeconds = newDelayInSeconds;
    }

    double ParticleEmitter::GetDelayInSeconds() const
    {
        return this->delayInSeconds;
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


    void ParticleEmitter::GetStartRotation(glm::vec3 &startRotationMinOut, glm::vec3 &startRotationMaxOut) const
    {
        startRotationMinOut = this->startRotationMin;
        startRotationMaxOut = this->startRotationMax;
    }

    void ParticleEmitter::SetStartRotation(const glm::vec3 &newStartRotationMin, const glm::vec3 &newStartRotationMax)
    {
        this->startRotationMin = newStartRotationMin;
        this->startRotationMax = newStartRotationMax;
    }


    void ParticleEmitter::GetStartScale(glm::vec3 &startScaleMinOut, glm::vec3 &startScaleMaxOut) const
    {
        startScaleMinOut = this->startScaleMin;
        startScaleMaxOut = this->startScaleMax;
    }

    void ParticleEmitter::SetStartScale(const glm::vec3 &newStartScaleMin, const glm::vec3 &newStartScaleMax)
    {
        this->startScaleMin = newStartScaleMin;
        this->startScaleMax = newStartScaleMax;
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


    void ParticleEmitter::SetTextureTiling(unsigned int xTileCount, unsigned int yTileCount)
    {
        if (xTileCount == 0) xTileCount = 1;
        if (yTileCount == 0) yTileCount = 1;

        this->textureTilesX = xTileCount;
        this->textureTilesY = yTileCount;
    }

    void ParticleEmitter::GetTextureTiling(unsigned int &xTileCount, unsigned int &yTileCount) const
    {
        xTileCount = this->textureTilesX;
        yTileCount = this->textureTilesY;
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
        auto newFunction = this->InstantiateFunctionByType(type);
        assert(newFunction != nullptr);
        {
            this->functions.PushBack(newFunction);
        }

        return* newFunction;
    }


    void ParticleEmitter::GetAABB(glm::vec3 &aabbMinOut, glm::vec3 &aabbMaxOut) const
    {
        aabbMinOut = this->aabbMin;
        aabbMaxOut = this->aabbMax;
    }


    void ParticleEmitter::Seed(int32_t seed)
    {
        if (seed == 0)
        {
            this->random.Seed();
        }
        else
        {
            this->random.Seed(seed);
        }
    }



    ////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ParticleEmitter::Serialize(GTLib::Serializer &serializer, bool serializeParticles)
    {
        GTLib::BasicSerializer intermediarySerializer;

        // First chunk is the main information.
        intermediarySerializer.WriteString(this->name.c_str());
        intermediarySerializer.Write(this->position);
        intermediarySerializer.Write(this->orientation);
        intermediarySerializer.Write(this->flags);
        intermediarySerializer.Write(this->durationInSeconds);
        intermediarySerializer.Write(this->delayInSeconds);
        intermediarySerializer.Write(this->gravityFactor);
        intermediarySerializer.Write(this->emissionRatePerSecond);
        intermediarySerializer.Write(static_cast<uint32_t>(this->emissionShapeType));
        intermediarySerializer.Write(this->emissionShapeCone);
        intermediarySerializer.Write(this->emissionShapeSphere);
        intermediarySerializer.Write(this->emissionShapeBox);
        intermediarySerializer.Write(this->startSpeedMin);
        intermediarySerializer.Write(this->startSpeedMax);
        intermediarySerializer.Write(this->startRotationMin);
        intermediarySerializer.Write(this->startRotationMax);
        intermediarySerializer.Write(this->startScaleMin);
        intermediarySerializer.Write(this->startScaleMax);
        intermediarySerializer.Write(this->lifetimeMin);
        intermediarySerializer.Write(this->lifetimeMax);
        intermediarySerializer.WriteString(this->material->GetDefinition().relativePath.c_str());
        intermediarySerializer.Write(this->textureTilesX);
        intermediarySerializer.Write(this->textureTilesY);

        // Functions.
        uint32_t functionCount = static_cast<uint32_t>(this->functions.count);
        intermediarySerializer.Write(functionCount);

        for (uint32_t iFunction = 0; iFunction < functionCount; ++iFunction)
        {
            auto function = this->functions[iFunction];
            assert(function != nullptr);
            {
                this->SerializeFunction(intermediarySerializer, *function);
            }
        }


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ParticleEmitter_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);




        // Now we'll want to serialize each active particle if applicable.
        if (serializeParticles)
        {
            intermediarySerializer.Clear();

            intermediarySerializer.Write(this->random);
            intermediarySerializer.Write(this->timeSinceLastEmission);
            
            uint32_t particleCount = static_cast<uint32_t>(this->particles.GetCount());
            intermediarySerializer.Write(particleCount);

            for (uint32_t iParticle = 0; iParticle < particleCount; ++iParticle)
            {
                this->SerializeParticle(intermediarySerializer, this->particles.GetParticle(iParticle));
            }



            header.id          = Serialization::ChunkID_ParticleEmitter_Particles;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }
    }


    void ParticleEmitter::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        
        if (header.id == Serialization::ChunkID_ParticleEmitter_Main)
        {
            if (header.version == 1)
            {
                // Before deserializing we are going to want to clear a few things.
                for (size_t iFunction = 0; iFunction < this->functions.count; ++iFunction)
                {
                    delete this->functions[iFunction];
                }
                this->functions.Clear();



                deserializer.ReadString(this->name);
                deserializer.Read(this->position);
                deserializer.Read(this->orientation);
                deserializer.Read(this->flags);
                deserializer.Read(this->durationInSeconds);
                deserializer.Read(this->delayInSeconds);
                deserializer.Read(this->gravityFactor);
                deserializer.Read(this->emissionRatePerSecond);

                uint32_t serializedEmissionShapeType;
                deserializer.Read(serializedEmissionShapeType);
                this->emissionShapeType = static_cast<EmissionShapeType>(serializedEmissionShapeType);

                deserializer.Read(this->emissionShapeCone);
                deserializer.Read(this->emissionShapeSphere);
                deserializer.Read(this->emissionShapeBox);
        
                deserializer.Read(this->startSpeedMin);
                deserializer.Read(this->startSpeedMax);
                deserializer.Read(this->startRotationMin);
                deserializer.Read(this->startRotationMax);
                deserializer.Read(this->startScaleMin);
                deserializer.Read(this->startScaleMax);
                deserializer.Read(this->lifetimeMin);
                deserializer.Read(this->lifetimeMax);

                GTLib::String materialRelativePath;
                deserializer.ReadString(materialRelativePath);
                this->SetMaterial(materialRelativePath.c_str());

                deserializer.Read(this->textureTilesX);
                deserializer.Read(this->textureTilesY);


                // Functions.
                uint32_t functionCount;
                deserializer.Read(functionCount);

                for (uint32_t iFunction = 0; iFunction < functionCount; ++iFunction)
                {
                    auto function = this->DeserializeFunction(deserializer);
                    assert(function != nullptr);
                    {
                        this->functions.PushBack(function);
                    }
                }



                // Particles. This is only done if we have the chunk.
                if (deserializer.Peek(&header, sizeof(header)) == sizeof(header))
                {
                    if (header.id == Serialization::ChunkID_ParticleEmitter_Particles)
                    {
                        deserializer.Seek(sizeof(header));  // <-- We read the header with Peek(), so we just skip.


                        // Existing particles need to be cleared.
                        this->particles.Clear();



                        if (header.version == 1)
                        {
                            deserializer.Read(this->random);
                            deserializer.Read(this->timeSinceLastEmission);

                            uint32_t particleCount;
                            deserializer.Read(particleCount);

                            for (uint32_t iParticle = 0; iParticle < particleCount; ++iParticle)
                            {
                                auto &particle = this->particles.PushNewParticle();
                                {
                                    this->DeserializeParticle(deserializer, particle);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            GTEngine::PostError("Error deserializing particle emitter. Unknown chunk ID (%d).", header.id);
            return;
        }
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



    void ParticleEmitter::SerializeFunction(GTLib::Serializer &serializer, const ParticleFunction &function)
    {
        serializer.Write(static_cast<uint32_t>(function.GetType()));

        switch (function.GetType())
        {
        case ParticleFunctionType_SizeOverTime:
        case ParticleFunctionType_AlphaOverTime:
            {
                float rangeMin;
                float rangeMax;
                static_cast<const ParticleFunction_Scalar &>(function).GetRange(rangeMin, rangeMax);

                serializer.Write(rangeMin);
                serializer.Write(rangeMax);

                break;
            }

        case ParticleFunctionType_LinearVelocityOverTime:
        case ParticleFunctionType_AngularVelocityOverTime:
        case ParticleFunctionType_ColourOverTime:
            {
                glm::vec3 rangeMin;
                glm::vec3 rangeMax;
                static_cast<const ParticleFunction_Vector3 &>(function).GetRange(rangeMin, rangeMax);

                serializer.Write(rangeMin);
                serializer.Write(rangeMax);

                break;
            }


        default:
            {
                assert(false);
                break;
            }
        }
    }

    ParticleFunction* ParticleEmitter::DeserializeFunction(GTLib::Deserializer &deserializer)
    {
        uint32_t serializedType;
        deserializer.Read(serializedType);

        auto function = this->InstantiateFunctionByType(static_cast<ParticleFunctionType>(serializedType));
        assert(function != nullptr);
        {
            switch (function->GetType())
            {
            case ParticleFunctionType_SizeOverTime:
            case ParticleFunctionType_AlphaOverTime:
                {
                    float rangeMin;
                    float rangeMax;
                    deserializer.Read(rangeMin);
                    deserializer.Read(rangeMax);

                    static_cast<ParticleFunction_Scalar*>(function)->SetRange(rangeMin, rangeMax);

                    break;
                }

            case ParticleFunctionType_LinearVelocityOverTime:
            case ParticleFunctionType_AngularVelocityOverTime:
            case ParticleFunctionType_ColourOverTime:
                {
                    glm::vec3 rangeMin;
                    glm::vec3 rangeMax;
                    deserializer.Read(rangeMin);
                    deserializer.Read(rangeMax);

                    static_cast<ParticleFunction_Vector3*>(function)->SetRange(rangeMin, rangeMax);

                    break;
                }


            default:
                {
                    assert(false);
                    break;
                }
            }
        }


        return function;
    }


    void ParticleEmitter::SerializeParticle(GTLib::Serializer &serializer, const Particle &particle)
    {
        serializer.Write(particle);
    }

    void ParticleEmitter::DeserializeParticle(GTLib::Deserializer &deserializer, Particle &particle)
    {
        deserializer.Read(particle);
    }


    ParticleFunction* ParticleEmitter::InstantiateFunctionByType(ParticleFunctionType type)
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

        case ParticleFunctionType_ColourOverTime:
            {
                newFunction = new ParticleFunction_ColourOverTime;
                break;
            }

        case ParticleFunctionType_AlphaOverTime:
            {
                newFunction = new ParticleFunction_AlphaOverTime;
                break;
            }

        default:
            {
                assert(false);
                break;
            }
        }


        assert(newFunction != nullptr);

        return newFunction;
    }
}

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
