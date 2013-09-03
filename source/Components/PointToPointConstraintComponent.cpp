// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(PointToPointConstraintComponent, "PointToPointConstraint")

    PointToPointConstraintComponent::PointToPointConstraintComponent(SceneNode &node)
        : Component(node),
          constraint(nullptr)
    {
    }

    PointToPointConstraintComponent::~PointToPointConstraintComponent()
    {
        delete this->constraint;
    }

    void PointToPointConstraintComponent::SetAttachments(SceneNode &sceneNodeA, SceneNode &sceneNodeB, const glm::vec3 &pivotInA, const glm::vec3 &pivotInB)
    {
        RigidBody* bodyA = nullptr;
        RigidBody* bodyB = nullptr;

        auto dynamicsComponent = sceneNodeA.GetComponent<GTEngine::DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();
            bodyA = &dynamicsComponent->GetRigidBody();
        }

        dynamicsComponent = sceneNodeB.GetComponent<GTEngine::DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();
            bodyB = &dynamicsComponent->GetRigidBody();
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


    void PointToPointConstraintComponent::SetAttachments(SceneNode &sceneNodeB, const glm::vec3 &pivotInB)
    {
        RigidBody* bodyB = nullptr;

        auto dynamicsComponent = sceneNodeB.GetComponent<GTEngine::DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();
            bodyB = &dynamicsComponent->GetRigidBody();
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
