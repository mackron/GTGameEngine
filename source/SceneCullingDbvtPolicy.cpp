
#include <GTEngine/SceneCullingDbvtPolicy.hpp>
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/CollisionGroups.hpp>

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
            // We check the objects collision group to determine the type of the component we're retrieving.
            if (proxy->m_collisionFilterGroup & CollisionGroups::Model)
            {
                auto modelComponent = sceneNode->GetComponent<ModelComponent>();
                assert(modelComponent != nullptr);

                this->ProcessModel(*modelComponent);
            }
            else if (proxy->m_collisionFilterGroup & CollisionGroups::PointLight)
            {
                auto pointLightComponent = sceneNode->GetComponent<PointLightComponent>();
                assert(pointLightComponent != nullptr);

                this->ProcessPointLight(*pointLightComponent);
            }

            //this->ProcessSceneNode(*sceneNode);
        }
    }
}
