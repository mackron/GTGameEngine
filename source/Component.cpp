// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Component.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GTEngine
{
    void Component::Serialize(GTLib::Serializer &) const
    {
    }

    void Component::Deserialize(GTLib::Deserializer &)
    {
    }

    void Component::OnPostSceneNodeDeserialized()
    {
    }

    void Component::OnChanged()
    {
        auto scene = this->node.GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->node, *this);
        }
    }


    Component* CreateComponentByName(const char* componentName, SceneNode &hostSceneNode)
    {
        if (GTLib::Strings::Equal(componentName, ModelComponent::Name))
        {
            return new ModelComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, CameraComponent::Name))
        {
            return new CameraComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, PointLightComponent::Name))
        {
            return new PointLightComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, SpotLightComponent::Name))
        {
            return new SpotLightComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, DirectionalLightComponent::Name))
        {
            return new DirectionalLightComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, AmbientLightComponent::Name))
        {
            return new AmbientLightComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, DynamicsComponent::Name))
        {
            return new DynamicsComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, ProximityComponent::Name))
        {
            return new ProximityComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, GenericConstraintComponent::Name))
        {
            return new GenericConstraintComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, PointToPointConstraintComponent::Name))
        {
            return new PointToPointConstraintComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, ConeTwistConstraintComponent::Name))
        {
            return new ConeTwistConstraintComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, OccluderComponent::Name))
        {
            return new OccluderComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, ParticleSystemComponent::Name))
        {
            return new ParticleSystemComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, ScriptComponent::Name))
        {
            return new ScriptComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, PrefabComponent::Name))
        {
            return new PrefabComponent(hostSceneNode);
        }
        else if (GTLib::Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            return new EditorMetadataComponent(hostSceneNode);
        }
        else
        {
            return GlobalGame->CreateCustomComponent(componentName, hostSceneNode);
        }
    }
}