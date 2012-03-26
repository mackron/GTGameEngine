
#include <GTEngine/Components/DynamicsComponent.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(DynamicsComponent, "Dynamics");

    DynamicsComponent::DynamicsComponent(SceneNode &node)
        : Component(node),
          collisionShape(),
          motionState(node),
          rigidBody(new RigidBody(0.0f, collisionShape, &motionState)),
          mass(0.0f),
          isKinematic(false),
          collisionGroup(1), collisionMask(-1)
    {
        this->rigidBody->setUserPointer(&node);
    }

    DynamicsComponent::~DynamicsComponent()
    {
        // We're first going to remove the rigid body from the dynamics world, if we have one.
        auto world = this->rigidBody->getWorld();
        if (world != nullptr)
        {
            world->removeRigidBody(this->rigidBody);
        }

        // Now we need to delete the collision shapes.
        while (this->collisionShape.getNumChildShapes() > 0)
        {
            auto child = this->collisionShape.getChildShape(0);
            this->collisionShape.removeChildShapeByIndex(0);

            delete child;
        }

        // We will delete the rigid body last.
        delete this->rigidBody;
    }

    void DynamicsComponent::AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btBoxShape(btVector3(halfX, halfY, halfZ)), offsetX, offsetY, offsetZ);
    }

    void DynamicsComponent::AddSphereCollisionShape(float radius, float offsetX, float offsetY, float offsetZ)
    {
        // Bullet's sphere shape does not support scaling, so we will create an ellipsoid instead.
        this->AddEllipsoidCollisionShape(radius, radius, radius, offsetX, offsetY, offsetZ);

        //btVector3 position(0.0f, 0.0f, 0.0f);
        //this->AddCollisionShape(new btMultiSphereShape(&position, &radius, 1), offsetX, offsetY, offsetZ);
    }

    void DynamicsComponent::AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX, float offsetY, float offsetZ)
    {
        this->AddCollisionShape(new btEllipsoidShape(btVector3(radiusX, radiusY, radiusZ)), offsetX, offsetY, offsetZ);
    }

    void DynamicsComponent::RemoveAllCollisionShapes()
    {
        // Since the shapes are being changed we need to remove the rigid body from the world first.
        auto world = this->rigidBody->getWorld();
        if (world != nullptr)
        {
            world->removeRigidBody(this->rigidBody);
        }


        // All children need to be removed from the shape.
        while (this->collisionShape.getNumChildShapes() > 0)
        {
            auto child = this->collisionShape.getChildShape(0);
            this->collisionShape.removeChildShapeByIndex(0);

            delete child;
        }


        // We should also update the mass, not that it would matter. We will do it for correctness.
        this->UpdateMass();

        // Now the rigid body needs to be re-added.
        if (world != nullptr)
        {
            world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }


    void DynamicsComponent::SetMass(float newMass)
    {
        if (this->mass != newMass)
        {
            this->mass = newMass;
            this->UpdateMass();
        }
    }

    float DynamicsComponent::GetMass() const
    {
        return this->mass;
    }


    void DynamicsComponent::IsKinematic(bool kinematic)
    {
        if (this->isKinematic != kinematic)
        {
            this->isKinematic = kinematic;

            // When changing the kinematic state, the object must first be removed from the world. It will be readded at the end.
            auto world = this->rigidBody->getWorld();
            if (world != nullptr)
            {
                world->removeRigidBody(this->rigidBody);
            }

            if (kinematic)
            {
                this->SetMass(0.0f);

                this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
            }
            else
            {
                this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
                this->rigidBody->setActivationState(ACTIVE_TAG);
                this->rigidBody->setDeactivationTime(0.0f);
            }

            // The rigid body needs to be readded to the world where applicable.
            if (world != nullptr)
            {
                world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
            }
        }
    }


    void DynamicsComponent::SetFriction(float friction)
    {
        this->rigidBody->setFriction(friction);
    }

    float DynamicsComponent::GetFriction() const
    {
        return this->rigidBody->getFriction();
    }

    void DynamicsComponent::SetRestitution(float restitution)
    {
        this->rigidBody->setRestitution(restitution);
    }

    float DynamicsComponent::GetRestitution() const
    {
        return this->rigidBody->getRestitution();
    }


    void DynamicsComponent::SetDamping(float linear, float angular)
    {
        this->rigidBody->setDamping(linear, angular);
    }

    float DynamicsComponent::GetLinearDamping() const
    {
        return this->rigidBody->getLinearDamping();
    }

    float DynamicsComponent::GetAngularDamping() const
    {
        return this->rigidBody->getAngularDamping();
    }


    void DynamicsComponent::SetCollisionFilter(short group, short mask)
    {
        if (this->collisionGroup != group || this->collisionMask != mask)
        {
            this->collisionGroup = group;
            this->collisionMask  = mask;

            // The body needs to be removed and re-added to it's world for changes to take effect.
            auto world = this->rigidBody->getWorld();
            if (world != nullptr)
            {
                world->removeRigidBody(this->rigidBody);
                world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
            }
        }
    }


    void DynamicsComponent::ApplyScaling(float x, float y, float z)
    {
        // The geometry is changing, thus we need to remove the body from the world and re-add it after the changes.
        auto world = this->rigidBody->getWorld();
        if (world != nullptr)
        {
            world->removeRigidBody(this->rigidBody);
        }

        // Now we simply apply the scaling to the shape.
        this->collisionShape.setLocalScaling(btVector3(x, y, z));

        // With a change in the geometry, we need to update the mass.
        this->UpdateMass();

        // Now we need to re-add the body.
        if (world != nullptr)
        {
            world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }


    void DynamicsComponent::AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ)
    {
        // When changing a collision shape, we always want to first remove the rigid body from it's world. We then re-add the body
        // when we're finished with the modifications.
        auto world = this->rigidBody->getWorld();
        if (world != nullptr)
        {
            world->removeRigidBody(this->rigidBody);
        }


        // All we need to do is add the new shape to the compound shape...
        this->collisionShape.addChildShape(btTransform(btMatrix3x3::getIdentity(), btVector3(offsetX, offsetY, offsetZ)), shape);

        // We need to make sure the shape is scaled correctly.
        glm::vec3 nodeScale = this->node.GetWorldScale();
        this->collisionShape.setLocalScaling(btVector3(nodeScale.x, nodeScale.y, nodeScale.z));


        // With a change in the shape, we also need to update the mass.
        this->UpdateMass();

        // Now we need to re-add the rigid body to the world, if it has one.
        if (world != nullptr)
        {
            world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }

    void DynamicsComponent::UpdateMass()
    {
        auto world = this->rigidBody->getWorld();
        if (world != nullptr)
        {
            world->removeRigidBody(this->rigidBody);
        }

        // If we only have a single shape attached to the compound shape, we're going to calculate the local inertia based on that shape.
        btVector3 inertia;

        if (this->collisionShape.getNumChildShapes() == 1)
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, *this->collisionShape.getChildShape(0));
        }
        else
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, this->collisionShape);
        }

        this->rigidBody->setMassProps(this->mass, inertia);
        this->rigidBody->updateInertiaTensor();

        if (world != nullptr)
        {
            world->addRigidBody(this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }
}
