
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(PointToPointConstraintComponent, "PointToPointConstraint");

    PointToPointConstraintComponent::PointToPointConstraintComponent(SceneNode &node)
        : Component(node),
          constraint(nullptr)
    {
    }

    PointToPointConstraintComponent::~PointToPointConstraintComponent()
    {
        delete this->constraint;
    }

    void PointToPointConstraintComponent::SetAttachments(SceneObject &objectA, SceneObject &objectB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB)
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
            this->constraint = new PointToPointConstraint(*bodyA, *bodyB, pivotInA, pivotInB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyA, *bodyB, pivotInA, pivotInB);
        }


        this->OnChanged();
    }


    void PointToPointConstraintComponent::SetAttachments(SceneObject &objectB, const glm::vec3 &pivotInB)
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
            this->constraint = new PointToPointConstraint(*bodyB, pivotInB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyB, pivotInB);
        }


        this->OnChanged();
    }
}
