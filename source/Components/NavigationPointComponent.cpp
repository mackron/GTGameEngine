
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(NavigationPointComponent, "NavigationPoint");

    NavigationPointComponent::NavigationPointComponent(SceneNode &node)
        : Component(node)
    {
    }
}