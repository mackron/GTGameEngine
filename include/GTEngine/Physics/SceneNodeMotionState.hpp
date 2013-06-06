// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeMotionState_hpp_
#define __GTEngine_SceneNodeMotionState_hpp_

#include "Bullet.hpp"

namespace GTEngine
{
    class SceneNode;

    /// Motion state for scene nodes.
    class SceneNodeMotionState : public btMotionState
    {
    public:

        /// Constructor.
        SceneNodeMotionState(SceneNode &node)
            : node(node)
        {
        };

        void setWorldTransform(const btTransform &worldTrans);
        void getWorldTransform(btTransform &worldTrans) const;

    private:

        /// The scene node.
        SceneNode &node;


    private:    // No copying.
        SceneNodeMotionState(const SceneNodeMotionState &);
        SceneNodeMotionState & operator=(const SceneNodeMotionState &);
    };
}

#endif