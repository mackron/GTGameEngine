
#include <GTEngine/Physics/SceneNodeMotionState.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    void SceneNodeMotionState::setWorldTransform(const btTransform &worldTrans)
    {
        this->node.SetWorldTransform(worldTrans);
    }

    void SceneNodeMotionState::getWorldTransform(btTransform &worldTrans) const
    {
        this->node.GetWorldTransform(worldTrans);
    }
}
