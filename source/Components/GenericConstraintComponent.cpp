
#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(GenericConstraintComponent, "GenericConstraint");

    GenericConstraintComponent::GenericConstraintComponent(SceneNode &node)
        : Component(node),
          constraint(nullptr)
    {
    }

    GenericConstraintComponent::~GenericConstraintComponent()
    {
        delete this->constraint;
    }

    void GenericConstraintComponent::SetAttachments(SceneObject &objectA, SceneObject &objectB, const glm::mat4 &frameA, const glm::mat4 &frameB)
    {
        RigidBody* bodyA = nullptr;
        RigidBody* bodyB = nullptr;

        if (objectA.GetType() == GTEngine::SceneObjectType_SceneNode)
        {
            auto dynamicsComponent = static_cast<SceneNode &>(objectA).GetComponent<GTEngine::DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                bodyA = &dynamicsComponent->GetRigidBody();
            }
        }

        if (objectB.GetType() == GTEngine::SceneObjectType_SceneNode)
        {
            auto dynamicsComponent = static_cast<SceneNode &>(objectB).GetComponent<GTEngine::DynamicsComponent>();
            if (dynamicsComponent != nullptr)
            {
                bodyB = &dynamicsComponent->GetRigidBody();
            }
        }



        assert(bodyA != nullptr);
        assert(bodyB != nullptr);



        if (this->constraint == nullptr)
        {
            this->constraint = new GenericConstraint(*bodyA, *bodyB, frameA, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyA, *bodyB, frameA, frameB);
        }
    }


    void GenericConstraintComponent::SetAngularLowerLimit(const glm::vec3 &limit)
    {
        this->constraint->setAngularLowerLimit(btVector3(limit.x, limit.y, limit.z));
    }

    void GenericConstraintComponent::SetAngularUpperLimit(const glm::vec3 &limit)
    {
        this->constraint->setAngularUpperLimit(btVector3(limit.x, limit.y, limit.z));
    }
}
