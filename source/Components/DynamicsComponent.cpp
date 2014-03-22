// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

// HACD headers need to be first.
#include <GTEngine/HACD/hacdCircularList.h>
#include <GTEngine/HACD/hacdVector.h>
#include <GTEngine/HACD/hacdICHull.h>
#include <GTEngine/HACD/hacdGraph.h>
#include <GTEngine/HACD/hacdHACD.h>

#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Physics.hpp>
#include <GTEngine/Logging.hpp>


namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT_ATTRIBS(DynamicsComponent, "Dynamics")

    DynamicsComponent::DynamicsComponent(SceneNode &node)
        : CollisionShapeComponent(node),
          motionState(node),
          rigidBody(new RigidBody(0.0f, collisionShape, &motionState)),
          world(nullptr),
          mass(0.0f),
          isKinematic(false),
          useWithNavigationMesh(true)
    {
        this->rigidBody->setUserPointer(&node);
    }

    DynamicsComponent::~DynamicsComponent()
    {
        // We're first going to remove the rigid body from the dynamics world, if we have one.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        // We will delete the physics objects last.
        delete this->rigidBody;
    }


    void DynamicsComponent::SetMass(float newMass)
    {
        if (this->mass != newMass)
        {
            this->mass = newMass;
            this->UpdateMass();

            this->OnChanged();
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
            auto world = this->rigidBody->GetWorld();
            if (world != nullptr)
            {
                world->RemoveRigidBody(*this->rigidBody);
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
                world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
            }


            this->OnChanged();
        }
    }

    bool DynamicsComponent::IsKinematic() const
    {
        return this->isKinematic;
    }

    bool DynamicsComponent::IsStatic() const
    {
        return this->mass == 0.0f && !this->isKinematic;
    }


    void DynamicsComponent::SetFriction(float friction)
    {
        this->rigidBody->setFriction(friction);
        this->OnChanged();
    }

    float DynamicsComponent::GetFriction() const
    {
        return this->rigidBody->getFriction();
    }

    void DynamicsComponent::SetRestitution(float restitution)
    {
        this->rigidBody->setRestitution(restitution);
        this->OnChanged();
    }

    float DynamicsComponent::GetRestitution() const
    {
        return this->rigidBody->getRestitution();
    }


    void DynamicsComponent::SetDamping(float linear, float angular)
    {
        this->rigidBody->setDamping(linear, angular);
        this->OnChanged();
    }

    float DynamicsComponent::GetLinearDamping() const
    {
        return this->rigidBody->getLinearDamping();
    }

    float DynamicsComponent::GetAngularDamping() const
    {
        return this->rigidBody->getAngularDamping();
    }




    void DynamicsComponent::ApplySceneNodeTransformation()
    {
        btTransform transform;
        if (this->rigidBody->getMotionState() != nullptr)
        {
            this->rigidBody->getMotionState()->getWorldTransform(transform);

            this->rigidBody->setWorldTransform(transform);
            this->rigidBody->setInterpolationWorldTransform(transform);
        }
    }


    void DynamicsComponent::SetLinearVelocity(float x, float y, float z)
    {
        this->rigidBody->setLinearVelocity(btVector3(x, y, z) * this->rigidBody->getLinearFactor());
    }

    glm::vec3 DynamicsComponent::GetLinearVelocity() const
    {
        return GTEngine::ToGLMVector3(this->rigidBody->getLinearVelocity());
    }


    void DynamicsComponent::SetAngularVelocity(float x, float y, float z)
    {
        this->rigidBody->setAngularVelocity(btVector3(x, y, z) * this->rigidBody->getAngularFactor());
    }

    glm::vec3 DynamicsComponent::GetAngularVelocity() const
    {
        return GTEngine::ToGLMVector3(this->rigidBody->getAngularVelocity());
    }


    void DynamicsComponent::SetLinearFactor(float x, float y, float z)
    {
        this->rigidBody->setLinearFactor(btVector3(x, y, z));
        this->OnChanged();
    }

    glm::vec3 DynamicsComponent::GetLinearFactor() const
    {
        return ToGLMVector3(this->rigidBody->getLinearFactor());
    }


    void DynamicsComponent::SetAngularFactor(float factor)
    {
        this->rigidBody->setAngularFactor(factor);
        this->OnChanged();
    }

    void DynamicsComponent::SetAngularFactor(float x, float y, float z)
    {
        this->rigidBody->setAngularFactor(btVector3(x, y, z));
        this->OnChanged();
    }

    glm::vec3 DynamicsComponent::GetAngularFactor() const
    {
        return ToGLMVector3(this->rigidBody->getAngularFactor());
    }



    void DynamicsComponent::SetGravity(float x, float y, float z)
    {
        this->rigidBody->setGravity(btVector3(x, y, z));
        this->OnChanged();
    }

    glm::vec3 DynamicsComponent::GetGravity() const
    {
        return ToGLMVector3(this->rigidBody->getGravity());
    }

    void DynamicsComponent::ApplyGravity()
    {
        this->rigidBody->applyGravity();
    }


    void DynamicsComponent::ApplyCentralForce(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyCentralForce(btVector3(x, y, z));
    }

    void DynamicsComponent::ApplyTorque(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyTorque(btVector3(x, y, z));
    }


    void DynamicsComponent::ApplyImpulse(float x, float y, float z, float relPosX, float relPosY, float relPosZ)
    {
        this->rigidBody->activate();
        this->rigidBody->applyImpulse(btVector3(x, y, z), btVector3(relPosX, relPosY, relPosZ));
    }

    void DynamicsComponent::ApplyCentralImpulse(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyCentralImpulse(btVector3(x, y, z));
    }

    void DynamicsComponent::ApplyTorqueImpulse(float x, float y, float z)
    {
        this->rigidBody->activate();
        this->rigidBody->applyTorqueImpulse(btVector3(x, y, z));
    }


    void DynamicsComponent::DisableDeactivation()
    {
        this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
        this->rigidBody->activate();

        this->OnChanged();
    }

    void DynamicsComponent::EnableDeactivation()
    {
        this->rigidBody->setActivationState(ACTIVE_TAG);
        this->rigidBody->setDeactivationTime(0.0f);

        this->OnChanged();
    }

    bool DynamicsComponent::IsDeactivationEnabled() const
    {
        return this->rigidBody->getActivationState() == ACTIVE_TAG;
    }

    void DynamicsComponent::Activate()
    {
        this->rigidBody->activate();
    }


    void DynamicsComponent::DisableNavigationMeshGeneration()
    {
        this->useWithNavigationMesh = false;
        this->OnChanged();
    }

    void DynamicsComponent::EnableNavigationMeshGeneration()
    {
        this->useWithNavigationMesh = true;
        this->OnChanged();
    }

    bool DynamicsComponent::IsNavigationMeshGenerationEnabled() const
    {
        return this->useWithNavigationMesh && this->IsStatic();
    }



    ///////////////////////////////////////////////////////
    // Serialization/Deserialization.

    void DynamicsComponent::Serialize(GTLib::Serializer &serializer) const
    {
        // The collision shapes need to be serialized first.
        CollisionShapeComponent::Serialize(serializer);

        // Now we write our own details, including rigid body properties.
        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_DynamicsComponent_Main;
        header.version     = 1;
        header.sizeInBytes =
            sizeof(float)                 +     // <-- Mass
            sizeof(float)                 +     // <-- Friction
            sizeof(float)                 +     // <-- Restitution
            sizeof(float) + sizeof(float) +     // <-- Linear + Angular damping
            sizeof(glm::vec3)             +     // <-- Linear velocity
            sizeof(glm::vec3)             +     // <-- Angular velocity
            sizeof(glm::vec3)             +     // <-- Linear factor
            sizeof(glm::vec3)             +     // <-- Angular factor
            sizeof(glm::vec3)             +     // <-- Gravity
            sizeof(bool)                  +     // <-- Is Kinematic?
            sizeof(bool)                  +     // <-- Is deactivation enabled?
            sizeof(bool);                       // <-- Is navigation mesh generation enabled?

        serializer.Write(header);
        {
            serializer.Write(this->mass);
            serializer.Write(this->GetFriction());
            serializer.Write(this->GetRestitution());
            serializer.Write(this->GetLinearDamping());
            serializer.Write(this->GetAngularDamping());
            serializer.Write(this->GetLinearVelocity());
            serializer.Write(this->GetAngularVelocity());
            serializer.Write(this->GetLinearFactor());
            serializer.Write(this->GetAngularFactor());
            serializer.Write(this->GetGravity());
            serializer.Write(this->IsKinematic());
            serializer.Write(this->IsDeactivationEnabled());
            serializer.Write(this->IsNavigationMeshGenerationEnabled());
        }
    }

    void DynamicsComponent::Deserialize(GTLib::Deserializer &deserializer)
    {
        // When deserializing, it's much, much more efficient to first remove the object from the existing scene, change the settings, and then
        // re-add it than it is to call each individual method. Thus, that's what we're doing here.
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }


        // The first thing to deserialize is the collision shape component
        CollisionShapeComponent::Deserialize(deserializer);


        float     newMass                    = this->mass;
        float     newFriction                = this->GetFriction();
        float     newRestitution             = this->GetRestitution();
        float     newLinearDamping           = this->GetLinearDamping();
        float     newAngularDamping          = this->GetAngularDamping();
        glm::vec3 newLinearVelocity          = this->GetLinearVelocity();
        glm::vec3 newAngularVelocity         = this->GetAngularVelocity();
        glm::vec3 newLinearFactor            = this->GetLinearFactor();
        glm::vec3 newAngularFactor           = this->GetAngularFactor();
        glm::vec3 newGravity                 = this->GetGravity();
        bool      newIsKinematic             = this->IsKinematic();
        bool      isDeactivationEnabled      = this->IsDeactivationEnabled();
        bool      isNavMeshGenerationEnabled = this->IsNavigationMeshGenerationEnabled();



        // First chunk is general info.
        Serialization::ChunkHeader header;
        deserializer.Read(header);
        if (header.id == Serialization::ChunkID_DynamicsComponent_Main)
        {
            switch (header.version)
            {
                case 1:
                {
                    deserializer.Read(newMass);
                    deserializer.Read(newFriction);
                    deserializer.Read(newRestitution);
                    deserializer.Read(newLinearDamping);
                    deserializer.Read(newAngularDamping);
                    deserializer.Read(newLinearVelocity);
                    deserializer.Read(newAngularVelocity);
                    deserializer.Read(newLinearFactor);
                    deserializer.Read(newAngularFactor);
                    deserializer.Read(newGravity);
                    deserializer.Read(newIsKinematic);
                    deserializer.Read(isDeactivationEnabled);
                    deserializer.Read(isNavMeshGenerationEnabled);

                    break;
                }

            default:
                {
                    GTEngine::Log("DynamicsComponent deserialization error. The main chunk version (%d) is unsupported. Deserialization will continue, but do not expect stability!", header.version);
                    break;
                }
            }
        }



        this->mass = newMass;
        this->rigidBody->setFriction(newFriction);
        this->rigidBody->setRestitution(newRestitution);
        this->rigidBody->setDamping(newLinearDamping, newAngularDamping);
        this->rigidBody->setLinearVelocity(ToBulletVector3(newLinearVelocity));
        this->rigidBody->setAngularVelocity(ToBulletVector3(newAngularVelocity));
        this->rigidBody->setLinearFactor(ToBulletVector3(newLinearFactor));
        this->rigidBody->setAngularFactor(ToBulletVector3(newAngularFactor));
        this->rigidBody->setGravity(ToBulletVector3(newGravity));

        this->isKinematic = newIsKinematic;

        if (newIsKinematic)
        {
            this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else
        {
            this->rigidBody->setCollisionFlags(this->rigidBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
            this->rigidBody->setActivationState(ACTIVE_TAG);
            this->rigidBody->setDeactivationTime(0.0f);
        }



        if (isDeactivationEnabled)
        {
            this->EnableDeactivation();
        }
        else
        {
            this->DisableDeactivation();
        }

        if (isNavMeshGenerationEnabled)
        {
            this->EnableNavigationMeshGeneration();
        }
        else
        {
            this->DisableNavigationMeshGeneration();
        }



        // The scale needs to be applied.
        this->ApplySceneNodeScaling();
        this->ApplySceneNodeTransformation();

        // The mass needs to be updated.
        this->UpdateMass();



        // At this point the rigid body should be in it's new state and we can re-add it to the world.
        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }


        this->OnChanged();
    }


    //////////////////////////////////////////////////////////
    // Private

    void DynamicsComponent::UpdateMass()
    {
        auto world = this->rigidBody->GetWorld();
        if (world != nullptr)
        {
            world->RemoveRigidBody(*this->rigidBody);
        }

        // If we only have a single shape attached to the compound shape, we're going to calculate the local inertia based on that shape.
        btVector3 inertia;

        if (this->GetCollisionShapeCount() == 1)
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, this->GetCollisionShapeAtIndex(0));
        }
        else
        {
            inertia = GTEngine::BulletUtils::CalculateLocalInertia(this->mass, this->collisionShape);
        }

        this->rigidBody->setMassProps(this->mass, inertia);
        this->rigidBody->updateInertiaTensor();


        // If we've turned into a static object there's a few things we'll want to do. First, we want the linear and angular velocities to
        // be reset to zero. After that, we want to ensure the rigid body is positioned at the same position as the scene node.
        if (this->mass == 0.0f)
        {
            this->rigidBody->setLinearVelocity( btVector3(0.0f, 0.0f, 0.0f));
            this->rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

            this->ApplySceneNodeTransformation();
        }

        if (world != nullptr)
        {
            world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }
    }

    void DynamicsComponent::OnPreCollisionShapeChanged()
    {
        this->world = this->rigidBody->GetWorld();
        if (this->world != nullptr)
        {
            this->world->RemoveRigidBody(*this->rigidBody);
        }
    }

    void DynamicsComponent::OnPostCollisionShapeChanged()
    {
        // With a change in the shape, we also need to update the mass.
        this->UpdateMass();

        // Now we need to re-add the rigid body to the world, if it has one.
        if (this->world != nullptr)
        {
            this->world->AddRigidBody(*this->rigidBody, this->collisionGroup, this->collisionMask);
        }

        this->world = nullptr;
    }
}
