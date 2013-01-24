
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
            this->constraint = new GenericConstraint(*bodyA, *bodyB, frameA, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyA, *bodyB, frameA, frameB);
        }


        this->OnChanged();
    }

    void GenericConstraintComponent::SetAttachments(SceneObject &objectB, const glm::mat4 &frameB)
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
            this->constraint = new GenericConstraint(*bodyB, frameB);
        }
        else
        {
            this->constraint->SetAttachments(*bodyB, frameB);
        }


        this->OnChanged();
    }



    void GenericConstraintComponent::SetLinearLowerLimit(const glm::vec3 &limit)
    {
        this->constraint->setLinearLowerLimit(btVector3(limit.x, limit.y, limit.z));
        this->OnChanged();
    }

    void GenericConstraintComponent::SetLinearUpperLimit(const glm::vec3 &limit)
    {
        this->constraint->setLinearUpperLimit(btVector3(limit.x, limit.y, limit.z));
        this->OnChanged();
    }

    void GenericConstraintComponent::SetAngularLowerLimit(const glm::vec3 &limit)
    {
        this->constraint->setAngularLowerLimit(btVector3(limit.x, limit.y, limit.z));
        this->OnChanged();
    }

    void GenericConstraintComponent::SetAngularUpperLimit(const glm::vec3 &limit)
    {
        this->constraint->setAngularUpperLimit(btVector3(limit.x, limit.y, limit.z));
        this->OnChanged();
    }
}
