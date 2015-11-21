// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/Logging.hpp>


// PointLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(PointLightComponent, "PointLight")

    PointLightComponent::PointLightComponent(SceneNode &node)
        : Component(node), colour(1.0f, 1.0f, 1.0f),
          radius(16.0f), falloff(1.0f),
          castShadows(false)
    {
    }


    void PointLightComponent::SetColour(const glm::vec3 &colourIn)
    {
        this->colour = colourIn;
        this->OnChanged();
    }


    void PointLightComponent::SetRadius(float radiusIn)
    {
        this->radius = radiusIn;
        this->OnChanged();
    }

    float PointLightComponent::GetRadius() const
    {
        return this->radius;
    }


    void PointLightComponent::SetFalloff(float falloffIn)
    {
        this->falloff = falloffIn;
        this->OnChanged();
    }

    float PointLightComponent::GetFalloff() const
    {
        return this->falloff;
    }


    void PointLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
        this->OnChanged();
    }

    void PointLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
        this->OnChanged();
    }

    void PointLightComponent::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(this->colour);
        intermediarySerializer.Write(this->radius);
        intermediarySerializer.Write(this->falloff);
        intermediarySerializer.Write(this->castShadows);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_PointLightComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void PointLightComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_PointLightComponent_Main)
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(this->colour);
                    deserializer.Read(this->radius);
                    deserializer.Read(this->falloff);
                    deserializer.Read(this->castShadows);

                    this->OnChanged();
                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing PointLightComponent. Main chunk has an unsupported version (%d). Skipping.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
        else
        {
            GTEngine::Log("Error deserializing PointLightComponent. Unknown Chunk ID (%d). Skipping.", header.id);

            deserializer.Seek(header.sizeInBytes);
        }
    }
}

// SpotLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(SpotLightComponent, "SpotLight")

    SpotLightComponent::SpotLightComponent(SceneNode &node)
        : Component(node), innerAngle(40.0f), outerAngle(45.0f), colour(1.0f, 1.0f, 1.0f), length(16.0f), falloff(1.0f),
          castShadows(false)
    {
    }


    void SpotLightComponent::SetColour(const glm::vec3 &colourIn)
    {
        this->colour = colourIn;
        this->OnChanged();
    }



    void SpotLightComponent::SetLength(float lengthIn)
    {
        this->length = lengthIn;
        this->OnChanged();
    }

    float SpotLightComponent::GetLength() const
    {
        return this->length;
    }


    void SpotLightComponent::SetFalloff(float falloffIn)
    {
        this->falloff = falloffIn;
        this->OnChanged();
    }

    float SpotLightComponent::GetFalloff() const
    {
        return this->falloff;
    }



    void SpotLightComponent::SetAngles(float newInnerAngle, float newOuterAngle)
    {
        if (newInnerAngle > newOuterAngle)
        {
            newInnerAngle = newOuterAngle;
        }

        this->innerAngle = newInnerAngle;
        this->outerAngle = newOuterAngle;

        this->OnChanged();
    }


    void SpotLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
        this->OnChanged();
    }

    void SpotLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
        this->OnChanged();
    }


    void SpotLightComponent::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(this->innerAngle);
        intermediarySerializer.Write(this->outerAngle);
        intermediarySerializer.Write(this->colour);
        intermediarySerializer.Write(this->length);
        intermediarySerializer.Write(this->falloff);
        intermediarySerializer.Write(this->castShadows);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SpotLightComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void SpotLightComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_SpotLightComponent_Main)
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(this->innerAngle);
                    deserializer.Read(this->outerAngle);
                    deserializer.Read(this->colour);
                    deserializer.Read(this->length);
                    deserializer.Read(this->falloff);
                    deserializer.Read(this->castShadows);

                    this->OnChanged();
                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing SpotLightComponent. Main chunk has an unsupported version (%d). Skipping.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
        else
        {
            GTEngine::Log("Error deserializing SpotLightComponent. Unknown Chunk ID (%d). Skipping.", header.id);

            deserializer.Seek(header.sizeInBytes);
        }
    }
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(DirectionalLightComponent, "DirectionalLight")

    DirectionalLightComponent::DirectionalLightComponent(SceneNode &node)
        : Component(node),
          colour(1.0f, 1.0f, 1.0f),
          castShadows(false)
    {
    }


    void DirectionalLightComponent::SetColour(const glm::vec3 &colourIn)
    {
        this->colour = colourIn;
        this->OnChanged();
    }


    void DirectionalLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
        this->OnChanged();
    }

    void DirectionalLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
        this->OnChanged();
    }


    void DirectionalLightComponent::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(this->colour);
        intermediarySerializer.Write(this->castShadows);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_DirectionalLightComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void DirectionalLightComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_DirectionalLightComponent_Main)
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(this->colour);
                    deserializer.Read(this->castShadows);

                    this->OnChanged();
                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing DirectionalLightComponent. Main chunk has an unsupported version (%d). Skipping.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
        else
        {
            GTEngine::Log("Error deserializing DirectionalLightComponent. Unknown Chunk ID (%d). Skipping.", header.id);

            deserializer.Seek(header.sizeInBytes);
        }
    }
}

// AmbientLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(AmbientLightComponent, "AmbientLight")

    AmbientLightComponent::AmbientLightComponent(SceneNode &node)
        : Component(node),
          colour(1.0f, 1.0f, 1.0f)
    {
    }


    void AmbientLightComponent::SetColour(const glm::vec3 &colourIn)
    {
        this->colour = colourIn;
        this->OnChanged();
    }


    void AmbientLightComponent::Serialize(GTLib::Serializer &serializer) const
    {
        GTLib::BasicSerializer intermediarySerializer;
        intermediarySerializer.Write(this->colour);


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_AmbientLightComponent_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void AmbientLightComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_AmbientLightComponent_Main)
        {
            switch (header.version)
            {
            case 1:
                {
                    deserializer.Read(this->colour);

                    this->OnChanged();
                    break;
                }

            default:
                {
                    GTEngine::Log("Error deserializing AmbientLightComponent. Main chunk has an unsupported version (%d). Skipping.", header.version);

                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }
        }
        else
        {
            GTEngine::Log("Error deserializing AmbientLightComponent. Unknown Chunk ID (%d). Skipping.", header.id);

            deserializer.Seek(header.sizeInBytes);
        }
    }
}
