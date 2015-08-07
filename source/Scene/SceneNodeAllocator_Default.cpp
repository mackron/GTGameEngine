// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeAllocator_Default.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>

namespace GT
{
    SceneNode* SceneNodeAllocator_Default::Allocate(uint64_t sceneNodeID)
    {
        assert((GT_SCENE_NODE_EXTRA_BYTES % alignof(SceneNode)) == 0);

        uint8_t* ptr8 = reinterpret_cast<uint8_t*>(_aligned_malloc(GT_SCENE_NODE_EXTRA_BYTES + sizeof(SceneNode), alignof(SceneNode)));
        if (ptr8 != nullptr)
        {
            return new (ptr8 + GT_SCENE_NODE_EXTRA_BYTES) SceneNode(sceneNodeID);
        }

        return nullptr;
    }

    void SceneNodeAllocator_Default::Delete(SceneNode* pSceneNode)
    {
        assert(pSceneNode != nullptr);

        pSceneNode->~SceneNode();
        _aligned_free(reinterpret_cast<uint8_t*>(pSceneNode - GT_SCENE_NODE_EXTRA_BYTES));
    }
}
