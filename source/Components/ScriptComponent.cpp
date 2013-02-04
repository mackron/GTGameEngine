
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
}