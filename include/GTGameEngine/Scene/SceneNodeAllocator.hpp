// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeAllocator_hpp_
#define __GT_SceneNodeAllocator_hpp_

#include <cstdint>

/// The extra bytes to allocate for each scene node. These extra bytes are used internally by the scene and must be a multiple
/// of alignof(SceneNode).
///
/// The first 8 bytes of this is reserved for Scene and the second 8 bytes is reserved for SceneSteppingPipeline. This is needed
/// so that both of these classes can set some custom data for each scene node with fast access, while at the same time keeping
/// it hidden from public view.
#define GT_SCENE_NODE_EXTRA_BYTES      16

namespace GT
{
    class SceneNode;

    /// Base class for scene node allocators.
    class SceneNodeAllocator
    {
    public:

        /// Allocates a scene node.
        ///
        /// The allocator must create at least an additional GT_SCENE_NODE_EXTRA_BYTES bytes of data in front of the returned pointer. Thus, the layout
        /// should look like [GT_SCENE_NODE_EXTRA_BYTES][sizeof(SceneNode)]. These bytes will be used internally by the scene and stepping pipeline.
        virtual SceneNode* Allocate(uint64_t sceneNodeID) = 0;

        /// Deallocates a scenenode.
        virtual void Delete(SceneNode* pSceneNode) = 0;
    };
}

#endif