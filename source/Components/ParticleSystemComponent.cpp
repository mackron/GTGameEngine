// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ParticleSystemComponent.hpp>
#include <GTEngine/ParticleSystemLibrary.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ParticleSystemComponent, "ParticleSystem");

    ParticleSystemComponent::ParticleSystemComponent(SceneNode &sceneNode)
        : Component(sceneNode),
          particleSystem(nullptr), isOwner(false)
    {
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
        if (this->isOwner)
        {
            ParticleSystemLibrary::Delete(this->particleSystem);
        }
    }


    void ParticleSystemComponent::SetParticleSystem(ParticleSystem* newParticleSystem, bool takeOwnership)
    {
        if (this->isOwner)
        {
            ParticleSystemLibrary::Delete(this->particleSystem);
        }
        
        this->particleSystem = newParticleSystem;
        this->isOwner        = takeOwnership;

        this->OnChanged();
    }

    ParticleSystem* ParticleSystemComponent::SetParticleSystem(const char* fileName, const char* relativeTo)
    {
        this->SetParticleSystem(ParticleSystemLibrary::Create(fileName, relativeTo), true);
        return this->particleSystem;
    }

    void ParticleSystemComponent::UnsetParticleSystem()
    {
        this->SetParticleSystem(static_cast<ParticleSystem*>(nullptr));
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ParticleSystemComponent::Serialize(GTCore::Serializer &serializer) const
    {
        (void)serializer;
    }

    void ParticleSystemComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        (void)deserializer;
    }
}