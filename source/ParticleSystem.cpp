// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ParticleSystem.hpp>

namespace GT
{
    ParticleSystem::ParticleSystem(const ParticleSystemDefinition &definitionIn)
        : definition(definitionIn),
          emitters(),
          position(), orientation(),
          gravity()
    {
        this->Refresh();
    }

    ParticleSystem::~ParticleSystem()
    {
        this->Clear();
    }

    void ParticleSystem::Refresh()
    {
        // 1) Clear.
        this->Clear();

        // 2) Re-create.
        for (size_t i = 0; i < this->definition.GetEmitterCount(); ++i)
        {
            auto definitionEmitter = this->definition.GetEmitter(i);
            assert(definitionEmitter != nullptr);
            {
                auto newEmitter = new ParticleEmitter(*definitionEmitter);
                newEmitter->SetPosition(this->position);
                newEmitter->SetOrientation(this->orientation);
                newEmitter->Seed();

                this->emitters.PushBack(newEmitter);
            }
        }
    }


    void ParticleSystem::Reset()
    {
        for (size_t iEmitter = 0; iEmitter < this->emitters.count; ++iEmitter)
        {
            auto emitter = this->emitters[iEmitter];
            assert(emitter != nullptr);
            {
                emitter->Reset();
            }
        }
    }

    void ParticleSystem::ClearParticles()
    {
        for (size_t iEmitter = 0; iEmitter < this->emitters.count; ++iEmitter)
        {
            auto emitter = this->emitters[iEmitter];
            assert(emitter != nullptr);
            {
                emitter->ClearParticles();
            }
        }
    }


    void ParticleSystem::Update(double deltaTimeInSeconds)
    {
        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            auto emitter = this->emitters[i];
            assert(emitter != nullptr);
            {
                emitter->Update(deltaTimeInSeconds, this->gravity);
            }
        }
    }


    size_t ParticleSystem::GetEmitterCount() const
    {
        return this->emitters.count;
    }

    const ParticleEmitter* ParticleSystem::GetEmitter(size_t index) const
    {
        return this->emitters[index];
    }


    void ParticleSystem::SetPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;

        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            auto emitter = this->emitters[i];
            assert(emitter != nullptr);
            {
                emitter->SetPosition(newPosition);
            }
        }
    }

    void ParticleSystem::SetOrientation(const glm::quat &newOrientation)
    {
        this->orientation = newOrientation;

        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            auto emitter = this->emitters[i];
            assert(emitter != nullptr);
            {
                emitter->SetOrientation(newOrientation);
            }
        }
    }

    void ParticleSystem::SetTransform(const glm::vec3 &newPosition, const glm::quat &newOrientation)
    {
        this->position    = newPosition;
        this->orientation = newOrientation;

        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            auto emitter = this->emitters[i];
            assert(emitter != nullptr);
            {
                emitter->SetPosition(newPosition);
                emitter->SetOrientation(newOrientation);
            }
        }
    }


    void ParticleSystem::GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        if (this->emitters.count > 0)
        {
            aabbMin = glm::vec3( FLT_MAX);
            aabbMax = glm::vec3(-FLT_MAX);

            for (size_t iEmitter = 0; iEmitter < this->emitters.count; ++iEmitter)
            {
                auto emitter = this->emitters[iEmitter];
                assert(emitter != nullptr);
                {
                    glm::vec3 emitterAABBMin;
                    glm::vec3 emitterAABBMax;
                    emitter->GetAABB(emitterAABBMin, emitterAABBMax);

                    aabbMin = glm::min(aabbMin, emitterAABBMin);
                    aabbMax = glm::max(aabbMax, emitterAABBMax);
                }
            }
        }
        else
        {
            aabbMin = glm::vec3(0.0f, 0.0f, 0.0f);
            aabbMax = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }


    void ParticleSystem::SetGravity(const glm::vec3 &newGravity)
    {
        this->gravity = newGravity;
    }

    const glm::vec3 & ParticleSystem::GetGravity() const
    {
        return this->gravity;
    }




    //////////////////////////////////
    // Private

    void ParticleSystem::Clear()
    {
        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            delete this->emitters[i];
        }
        this->emitters.Clear();
    }
}