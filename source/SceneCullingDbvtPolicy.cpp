
#include <GTEngine/SceneCullingDbvtPolicy.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    SceneCullingDbvtPolicy::SceneCullingDbvtPolicy()
    {
    }

    SceneCullingDbvtPolicy::~SceneCullingDbvtPolicy()
    {
    }


    void SceneCullingDbvtPolicy::Process(const btDbvtNode* leaf)
    {
        auto proxy           = static_cast<btBroadphaseProxy*>(leaf->data);
        auto collisionObject = static_cast<btCollisionObject*>(proxy->m_clientObject);

        assert(proxy           != nullptr);
        assert(collisionObject != nullptr);


        auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
        if (sceneNode != nullptr && sceneNode->IsVisible())
        {
            this->ProcessSceneNode(*sceneNode);
        }
    }
}
