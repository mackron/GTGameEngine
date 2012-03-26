
#include <GTEngine/SceneNodeFactory.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <cassert>

namespace GTEngine
{
    SceneNode * SceneNodeFactory::CreateEmptyNode()
    {
        return new SceneNode;
    }

    SceneNode * SceneNodeFactory::Create2DCameraNode(float left, float right, float bottom, float top, float znear, float zfar)
    {
        auto node = SceneNodeFactory::CreateEmptyNode();
        assert(node != nullptr);
        
        auto camera = node->AddComponent<GTEngine::CameraComponent>();
        camera->Set2DProjection(left, right, bottom, top, znear, zfar);

        return node;
    }

    SceneNode * SceneNodeFactory::Create3DCameraNode(float fov, float aspect, float znear, float zfar)
    {
        auto node = SceneNodeFactory::CreateEmptyNode();
        assert(node != nullptr);
        
        auto camera = node->AddComponent<GTEngine::CameraComponent>();
        camera->Set3DProjection(fov, aspect, znear, zfar);

        return node;
    }

    SceneNode * SceneNodeFactory::CreateModelNode(Model * model, bool giveOwnership)
    {
        auto node = SceneNodeFactory::CreateEmptyNode();
        assert(node != nullptr);

        if (model != nullptr)
        {
            auto modelComponent = node->AddComponent<GTEngine::ModelComponent>();
            assert(modelComponent != nullptr);

            modelComponent->SetModel(model, giveOwnership);  // <-- second argument specifies that the component should take ownership of the model.
        }

        return node;
    }


    void SceneNodeFactory::DeleteNode(SceneNode *nodeToDelete)
    {
        delete nodeToDelete;
    }
}