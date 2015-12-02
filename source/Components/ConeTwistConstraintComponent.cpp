// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneNode.hpp>
#include <GTGE/Physics.hpp>

namespace GT
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(ConeTwistConstraintComponent, "ConeTwistConstraint")

    ConeTwistConstraintComponent::ConeTwistConstraintComponent(SceneNode &node)
        : Component(node),
          constraint(nullptr)
    {
    }

    ConeTwistConstraintComponent::~ConeTwistConstraintComponent()
    {
        delete this->constraint;
    }

    void ConeTwistConstraintComponent::SetAttachments(SceneNode &sceneNodeA, SceneNode &sceneNodeB, const glm::mat4 &frameA, const glm::mat4 &frameB)
    {
        RigidBody* bodyA = nullptr;
        RigidBody* bodyB = nullptr;

        auto dynamicsComponent = sceneNodeA.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();
            bodyA = &dynamicsComponent->GetRigidBody();
        }

        dynamicsComponent = sceneNodeB.GetComponent<DynamicsComponent>();
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
            this->constraint = new ConeTwistConstraint(*bodyA, *bodyB, frameA, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyA, *bodyB, frameA, frameB);
        }


        this->OnChanged();
    }


    void ConeTwistConstraintComponent::SetAttachments(SceneNode &sceneNodeB, const glm::mat4 &frameB)
    {
        RigidBody* bodyB = nullptr;

        auto dynamicsComponent = sceneNodeB.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            dynamicsComponent->ApplySceneNodeScaling();
            dynamicsComponent->ApplySceneNodeTransformation();
            bodyB = &dynamicsComponent->GetRigidBody();
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



        this->OnChanged();
    }


    void ConeTwistConstraintComponent::SetLimits(float swingSpan1, float swingSpan2, float twistSpan)
    {
        this->constraint->setLimit(swingSpan1, swingSpan2, twistSpan);

        this->OnChanged();
    }
}
