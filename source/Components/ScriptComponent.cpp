
#include <GTEngine/Components/ScriptComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ScriptComponent, "Script");

    ScriptComponent::ScriptComponent(SceneNode &node)
        : Component(node)
    {
    }

    ScriptComponent::~ScriptComponent()
    {
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void ScriptComponent::Serialize(GTCore::Serializer &serializer) const
    {
        (void)serializer;
    }

    void ScriptComponent::Deserialize(GTCore::Deserializer &deserializer)
    {
        (void)deserializer;
    }
}