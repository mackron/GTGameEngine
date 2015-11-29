// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/ParticleSystemComponent.hpp>
#include <GTEngine/ParticleSystemLibrary.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Errors.hpp>

namespace GT
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ParticleSystemComponent, "ParticleSystem")

    ParticleSystemComponent::ParticleSystemComponent(SceneNode &sceneNode)
        : Component(sceneNode),
          particleSystem(nullptr), isOwner(false),
          isPlaying(false), playOnStartup(false),
          sceneNodeEventHandler()
    {
        sceneNode.AttachEventHandler(this->sceneNodeEventHandler);
    }

    ParticleSystemComponent::~ParticleSystemComponent()
    {
        if (this->isOwner)
        {
            ParticleSystemLibrary::Delete(this->particleSystem);
        }

        this->node.DetachEventHandler(this->sceneNodeEventHandler);
    }


    void ParticleSystemComponent::SetParticleSystem(ParticleSystem* newParticleSystem, bool takeOwnership)
    {
        if (this->isOwner)
        {
            ParticleSystemLibrary::Delete(this->particleSystem);
        }

        this->particleSystem = newParticleSystem;
        this->isOwner        = takeOwnership;

        if (this->particleSystem != nullptr)
        {
            this->particleSystem->SetTransform(this->node.GetWorldPosition(), this->node.GetWorldOrientation());

            if (this->IsPlayingOnStartup())
            {
                this->Play();
            }
            else
            {
                this->Pause();
            }
        }

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


    void ParticleSystemComponent::UpdateTransform()
    {
        if (this->particleSystem != nullptr)
        {
            this->particleSystem->SetTransform(this->node.GetWorldPosition(), this->node.GetWorldOrientation());
        }
    }


    void ParticleSystemComponent::Play()
    {
        if (!this->isPlaying)
        {
            if (this->particleSystem != nullptr && this->node.GetScene() != nullptr)
            {
                this->particleSystem->SetGravity(this->node.GetScene()->GetGravity());
            }

            this->isPlaying = true;
            this->OnChanged();
        }
    }

    void ParticleSystemComponent::Pause()
    {
        if (this->isPlaying)
        {
            this->isPlaying = false;
            this->OnChanged();
        }
    }

    bool ParticleSystemComponent::IsPlaying() const
    {
        return this->isPlaying;
    }


    void ParticleSystemComponent::PlayOnStartup(bool playOnStartupIn)
    {
        if (this->playOnStartup != playOnStartupIn)
        {
            this->playOnStartup = playOnStartupIn;
            this->OnChanged();
        }
    }

    bool ParticleSystemComponent::IsPlayingOnStartup() const
    {
        return this->playOnStartup;
    }


    void ParticleSystemComponent::Reset()
    {
        if (this->particleSystem != nullptr)
        {
            this->particleSystem->Reset();
        }

        if (this->playOnStartup)
        {
            this->Play();
        }
        else
        {
            this->Pause();
        }
    }


    void ParticleSystemComponent::GetEmitterLifetime(size_t emitterIndex, double &lifetimeMinOut, double &lifetimeMaxOut) const
    {
        if (this->particleSystem != nullptr)
        {
            this->particleSystem->GetEmitter(emitterIndex)->GetLifetime(lifetimeMinOut, lifetimeMaxOut);
        }
        else
        {
            lifetimeMinOut = 0.0;
            lifetimeMaxOut = 0.0;
        }
    }


    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ParticleSystemComponent::Serialize(Serializer &serializer) const
    {
        BasicSerializer intermediarySerializer;

        if (this->particleSystem != nullptr)
        {
            intermediarySerializer.WriteString(this->particleSystem->GetDefinition().GetRelativePath());
        }
        else
        {
            intermediarySerializer.WriteString("");
        }

        intermediarySerializer.Write(this->isPlaying);
        intermediarySerializer.Write(this->playOnStartup);



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ParticleSystemComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ParticleSystemComponent::Deserialize(Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        if (header.id == Serialization::ChunkID_ParticleSystemComponent_Main)
        {
            String relativeFilePath;
            deserializer.ReadString(relativeFilePath);

            this->SetParticleSystem(relativeFilePath.c_str());


            deserializer.Read(this->isPlaying);
            deserializer.Read(this->playOnStartup);
        }
        else
        {
            GT::PostError("Error deserializing particle system component. Unknown chunk ID (%d).", header.id);
        }
    }





    ///////////////////////////////////////////////////////
    // Private

    void ParticleSystemComponent::EventHandler::OnTransform(SceneNode &sceneNode)
    {
        sceneNode.GetComponent<ParticleSystemComponent>()->UpdateTransform();
    }
}
