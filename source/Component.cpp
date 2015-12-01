// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Component.hpp>
#include <GTEngine/GTEngine.hpp>

namespace GT
{
    Component::Component(SceneNode &node)
        : node(node), onChangedLockCounter(0)
    {
    }

    Component::~Component()
    {
    }

    void Component::Serialize(Serializer &) const
    {
    }

    void Component::Deserialize(Deserializer &)
    {
    }

    void Component::OnPostSceneNodeDeserialized()
    {
    }

    void Component::OnChanged(unsigned int whatChangedFlags)
    {
        auto scene = this->node.GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->node, *this, whatChangedFlags);
        }
    }

    void Component::LockOnChanged()
    {
        this->onChangedLockCounter += 1;
    }

    void Component::UnlockOnChanged()
    {
        assert(this->onChangedLockCounter > 0);
        {
            this->onChangedLockCounter -= 1;
        }
    }

    Component* CreateComponentByName(const char* componentName, SceneNode &hostSceneNode)
    {
        if (Strings::Equal(componentName, ModelComponent::Name))
        {
            return new ModelComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, CameraComponent::Name))
        {
            return new CameraComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, PointLightComponent::Name))
        {
            return new PointLightComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, SpotLightComponent::Name))
        {
            return new SpotLightComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, DirectionalLightComponent::Name))
        {
            return new DirectionalLightComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, AmbientLightComponent::Name))
        {
            return new AmbientLightComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, DynamicsComponent::Name))
        {
            return new DynamicsComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, ProximityComponent::Name))
        {
            return new ProximityComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, GenericConstraintComponent::Name))
        {
            return new GenericConstraintComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, PointToPointConstraintComponent::Name))
        {
            return new PointToPointConstraintComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, ConeTwistConstraintComponent::Name))
        {
            return new ConeTwistConstraintComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, OccluderComponent::Name))
        {
            return new OccluderComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, ParticleSystemComponent::Name))
        {
            return new ParticleSystemComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, ScriptComponent::Name))
        {
            return new ScriptComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, PrefabComponent::Name))
        {
            return new PrefabComponent(hostSceneNode);
        }
        else if (Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            return new EditorMetadataComponent(hostSceneNode);
        }
        else
        {
            return GlobalGame->CreateCustomComponent(componentName, hostSceneNode);
        }
    }
}