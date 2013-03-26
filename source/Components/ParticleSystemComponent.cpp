// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/ParticleSystemComponent.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ParticleSystemComponent, "ParticleSystem");

    ParticleSystemComponent::ParticleSystemComponent(SceneNode &sceneNode)
        : Component(sceneNode)
    {
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
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