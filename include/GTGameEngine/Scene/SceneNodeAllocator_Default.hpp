// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeAllocator_Default_hpp_
#define __GT_SceneNodeAllocator_Default_hpp_

#include <cstdint>
#include "SceneNodeAllocator.hpp"

namespace GT
{
    /// Default implementation of the scene node allocator.
    class SceneNodeAllocator_Default : public SceneNodeAllocator
    {
    public:

        /// @copydoc SceneNodeAllocator::Allocate()
        SceneNode* Allocate(uint64_t sceneNodeID);

        /// @copydoc SceneNodeAllocator::Delete()
        void Delete(SceneNode* pSceneNode);
    };
}

#endif
