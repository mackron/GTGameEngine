// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleSystem.hpp>

namespace GTEngine
{
    ParticleSystem::ParticleSystem(const ParticleSystemDefinition &definitionIn)
        : definition(definitionIn), emitters()
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
                this->emitters.PushBack(new ParticleEmitter(*definitionEmitter));
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