// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeAllocator_Default
#define GT_SceneNodeAllocator_Default

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
