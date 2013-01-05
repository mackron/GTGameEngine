
#include <GTEngine/Component.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GTEngine
{
    void Component::Serialize(GTCore::Serializer &) const
    {
    }

    void Component::Deserialize(GTCore::Deserializer &)
    {
    }




    Component* CreateComponentByName(const char* componentName, SceneNode &hostSceneNode)
    {
        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
        {
            return new ModelComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, CameraComponent::Name))
        {
            return new CameraComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
        {
            return new PointLightComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
        {
            return new SpotLightComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, DirectionalLightComponent::Name))
        {
            return new DirectionalLightComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, AmbientLightComponent::Name))
        {
            return new AmbientLightComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, DynamicsComponent::Name))
        {
            return new DynamicsComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, ProximityComponent::Name))
        {
            return new ProximityComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, GenericConstraintComponent::Name))
        {
            return new GenericConstraintComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, PointToPointConstraintComponent::Name))
        {
            return new PointToPointConstraintComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, ConeTwistConstraintComponent::Name))
        {
            return new ConeTwistConstraintComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, OccluderComponent::Name))
        {
            return new OccluderComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, SpriteComponent::Name))
        {
            return new SpriteComponent(hostSceneNode);
        }
        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            return new EditorMetadataComponent(hostSceneNode);
        }
        else
        {
            return GlobalGame->CreateCustomComponent(componentName, hostSceneNode);
        }
    }
}