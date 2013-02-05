// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/Scene.hpp>

// Lighting utils.
namespace GTEngine
{
    float GetApproximateAttenuationRadius(double c, double l, double q)
    {
        // If 'q' is 0.0, we'll end up with a division by 0 bug. In this case, we'll replace it with a tiny value for approximation.
        if (q == 0.0)
        {
            q = 0.000001;
        }

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 1000.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
    }
}

// PointLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(PointLightComponent, "PointLight");

    PointLightComponent::PointLightComponent(SceneNode &node)
        : Component(node), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f),
          castShadows(false)
    {
    }


    void PointLightComponent::SetColour(const glm::vec3 &colour)
    {
        this->colour = colour;
        this->OnChanged();
    }


    void PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;

        this->OnChanged();
    }

    float PointLightComponent::GetApproximateRadius() const
    {
        return GetApproximateAttenuationRadius(this->constantAttenuation, this->linearAttenuation, this->quadraticAttenuation);
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


    void PointLightComponent::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(this->colour);
        serializer.Write(this->constantAttenuation);
        serializer.Write(this->linearAttenuation);
        serializer.Write(this->quadraticAttenuation);
        serializer.Write(this->castShadows);
    }

    void PointLightComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(this->colour);
        deserializer.Read(this->constantAttenuation);
        deserializer.Read(this->linearAttenuation);
        deserializer.Read(this->quadraticAttenuation);
        deserializer.Read(this->castShadows);

        this->OnChanged();
    }
}

// SpotLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(SpotLightComponent, "SpotLight");

    SpotLightComponent::SpotLightComponent(SceneNode &node)
        : Component(node), innerAngle(40.0f), outerAngle(45.0f), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f),
          castShadows(false)
    {
    }


    void SpotLightComponent::SetColour(const glm::vec3 &colour)
    {
        this->colour = colour;
        this->OnChanged();
    }


    void SpotLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;

        this->OnChanged();
    }

    float SpotLightComponent::GetApproximateLength() const
    {
        double c = this->constantAttenuation;
        double l = this->linearAttenuation;
        double q = this->quadraticAttenuation;

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 1000.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
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


    void SpotLightComponent::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(this->innerAngle);
        serializer.Write(this->outerAngle);
        serializer.Write(this->colour);
        serializer.Write(this->constantAttenuation);
        serializer.Write(this->linearAttenuation);
        serializer.Write(this->quadraticAttenuation);
        serializer.Write(this->castShadows);
    }

    void SpotLightComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(this->innerAngle);
        deserializer.Read(this->outerAngle);
        deserializer.Read(this->colour);
        deserializer.Read(this->constantAttenuation);
        deserializer.Read(this->linearAttenuation);
        deserializer.Read(this->quadraticAttenuation);
        deserializer.Read(this->castShadows);

        this->OnChanged();
    }
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(DirectionalLightComponent, "DirectionalLight");

    DirectionalLightComponent::DirectionalLightComponent(SceneNode &node)
        : Component(node),
          colour(1.0f, 1.0f, 1.0f),
          castShadows(false)
    {
    }


    void DirectionalLightComponent::SetColour(const glm::vec3 &colour)
    {
        this->colour = colour;
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


    void DirectionalLightComponent::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(this->colour);
        serializer.Write(this->castShadows);
    }

    void DirectionalLightComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(this->colour);
        deserializer.Read(this->castShadows);

        this->OnChanged();
    }
}

// AmbientLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(AmbientLightComponent, "AmbientLight");

    AmbientLightComponent::AmbientLightComponent(SceneNode &node)
        : Component(node),
          colour(1.0f, 1.0f, 1.0f)
    {
    }


    void AmbientLightComponent::SetColour(const glm::vec3 &colour)
    {
        this->colour = colour;
        this->OnChanged();
    }


    void AmbientLightComponent::Serialize(GTCore::Serializer &serializer) const
    {
        serializer.Write(this->colour);
    }

    void AmbientLightComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        deserializer.Read(this->colour);

        this->OnChanged();
    }
}
