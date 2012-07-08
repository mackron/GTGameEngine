
#include <GTEngine/DefaultSceneUpdateManager.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    DefaultSceneUpdateManager::DefaultSceneUpdateManager()
        : objects()
    {
    }

    DefaultSceneUpdateManager::~DefaultSceneUpdateManager()
    {
    }


    bool DefaultSceneUpdateManager::NeedsUpdate(SceneObject &object) const
    {
        // We care about everything except:
        //   - Physics objects (handled by the physics manager).

        if (object.GetType() != SceneObjectType_PhysicsObject)
        {
            // If we are a scene node, we want to ignore those with the SceneNode::NoUpdate flag.
            if (object.GetType() == SceneObjectType_SceneNode)
            {
                return (static_cast<SceneNode&>(object).GetFlags() & SceneNode::NoUpdate) == 0;
            }

            return true;
        }

        return false;
    }
    
    void DefaultSceneUpdateManager::AddObject(SceneObject &object)
    {
        this->objects.PushBack(&object);
    }

    void DefaultSceneUpdateManager::RemoveObject(SceneObject &object)
    {
        this->objects.RemoveFirst(&object);
    }

    void DefaultSceneUpdateManager::Step(double deltaTimeInSeconds)
    {
        for (size_t i = 0; i < this->objects.count; ++i)
        {
            auto object = this->objects[i];
            assert(object != nullptr);

            switch (object->GetType())
            {
            case SceneObjectType_SceneNode:
                {
                    this->StepSceneNode(static_cast<SceneNode&>(*object), deltaTimeInSeconds);
                    break;
                }


            case SceneObjectType_Unknown:
            case SceneObjectType_None:
            case SceneObjectType_PhysicsObject:
            case SceneObjectType_Particle:
            default: break;
            }
        }
    }



    /////////////////////////////////////////////////////////
    // Protected Methods.

    void DefaultSceneUpdateManager::StepSceneNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // If we have an animated model component, that needs to have it's animation stepped.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr && model->IsAnimating())
            {
                model->StepAnimation(deltaTimeInSeconds);
            }
        }

        // Now we just let the scene node itself know about it.
        node.OnUpdate(deltaTimeInSeconds);
    }
}
