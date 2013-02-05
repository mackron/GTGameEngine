// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Physics/SceneNodeMotionState.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    void SceneNodeMotionState::setWorldTransform(const btTransform &worldTrans)
    {
        this->node.SetWorldTransform(worldTrans, false);        // <-- 'false' indicates that the rigid body should not have it's position updated. This is super important.
    }

    void SceneNodeMotionState::getWorldTransform(btTransform &worldTrans) const
    {
        this->node.GetWorldTransform(worldTrans);
    }
}
