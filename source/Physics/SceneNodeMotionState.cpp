// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Physics/SceneNodeMotionState.hpp>
#include <GTGE/SceneNode.hpp>

namespace GT
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
