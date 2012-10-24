
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(ConeTwistConstraintComponent, "ConeTwistConstraint");

    ConeTwistConstraintComponent::ConeTwistConstraintComponent(SceneNode &node)
        : Component(node),
          constraint(nullptr)
    {
    }

    ConeTwistConstraintComponent::~ConeTwistConstraintComponent()
    {
        delete this->constraint;
    }

    void ConeTwistConstraintComponent::SetAttachments(SceneObject &objectA, SceneObject &objectB, const glm::mat4 &frameA, const glm::mat4 &frameB)
    {
        RigidBody* bodyA = nullptr;
        RigidBody* bodyB = nullptr;

        if (objectA.GetType() == GTEngine::SceneObjectType_SceneNode)
        {
            auto dynamicsComponent = static_cast<SceneNode &>(objectA).GetComponent<GTEngine::DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                dynamicsComponent->ApplySceneNodeScaling();
                dynamicsComponent->ApplySceneNodeTransformation();
                bodyA = &dynamicsComponent->GetRigidBody();
            }
        }

        if (objectB.GetType() == GTEngine::SceneObjectType_SceneNode)
        {
            auto dynamicsComponent = static_cast<SceneNode &>(objectB).GetComponent<GTEngine::DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                dynamicsComponent->ApplySceneNodeScaling();
                dynamicsComponent->ApplySceneNodeTransformation();
                bodyB = &dynamicsComponent->GetRigidBody();
            }
        }



        assert(bodyA != nullptr);
        assert(bodyB != nullptr);



        if (this->constraint == nullptr)
        {
            this->constraint = new ConeTwistConstraint(*bodyA, *bodyB, frameA, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyA, *bodyB, frameA, frameB);
        }
    }


    void ConeTwistConstraintComponent::SetAttachments(SceneObject &objectB, const glm::mat4 &frameB)
    {
        RigidBody* bodyB = nullptr;

        if (objectB.GetType() == GTEngine::SceneObjectType_SceneNode)
        {
            auto dynamicsComponent = static_cast<SceneNode &>(objectB).GetComponent<GTEngine::DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                dynamicsComponent->ApplySceneNodeScaling();
                dynamicsComponent->ApplySceneNodeTransformation();
                bodyB = &dynamicsComponent->GetRigidBody();
            }
        }



        assert(bodyB != nullptr);



        if (this->constraint == nullptr)
        {
            this->constraint = new ConeTwistConstraint(*bodyB, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyB, frameB);
        }
    }
}
