// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneNodeMotionState
#define GT_SceneNodeMotionState

#include "Bullet.hpp"

namespace GT
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