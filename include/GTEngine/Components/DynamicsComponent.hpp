// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DynamicsComponent_hpp_
#define __GTEngine_DynamicsComponent_hpp_

#include "CollisionShapeComponent.hpp"
#include "../VertexArrayLibrary.hpp"        // TODO: Check if we can remove this.
#include <GTLib/Vector.hpp>

namespace GTEngine
{
    /// Class representing a dynamics component.
    ///
    /// The dynamics component is used to do collision detection and physics. 
    class DynamicsComponent : public GTEngine::CollisionShapeComponent
    {
    public:

        /// Constructor.
        DynamicsComponent(SceneNode &node);

        /// Destructor.
        ~DynamicsComponent();



        /// Sets the mass of the rigid body. If this is 0.0, it will be static. Defaults to 0.0.
        void SetMass(float mass);

        /// Retrieves the mass of the rigid body.
        float GetMass() const;

        /// Sets whether or not the component is kinematic.
        void IsKinematic(bool isKinematic);

        /// Returns whether or not the rigid body is kinematic.
        bool IsKinematic() const;

        /// Determines whether or not the rigid body is static.
        bool IsStatic() const;


        /// Sets the friction of the object.
        ///
        /// @param friction [in] The new friction value to use with the object. Larger values means more friction.
        void SetFriction(float friction);

        /// Retrieves the friction of the object.
        float GetFriction() const;

        /// Sets the restitution of the object (bounciness).
        ///
        /// @param restitution [in] The new restitution. Larger values means more bounciness.
        void SetRestitution(float restitution);

        /// Retrieves the restitution of the object.
        float GetRestitution() const;


        /// Sets the linear and angular damping.
        void SetDamping(float linear, float angular);

        /// Retrieves the linear damping.
        float GetLinearDamping() const;

        /// Retrieves the angular damping.
        float GetAngularDamping() const;



        /// Retrieves a reference to the rigid body.
              RigidBody & GetRigidBody()       { return *this->rigidBody; }
        const RigidBody & GetRigidBody() const { return *this->rigidBody; }


        /// Applies the transformation of the scene node to the rigid body.
        void ApplySceneNodeTransformation();



        /// Sets the linear velocity of the physics object.
        void SetLinearVelocity(float x, float y, float z);
        void SetLinearVelocity(const glm::vec3 &velocity) { this->SetLinearVelocity(velocity.x, velocity.y, velocity.z); }

        /// Retrieves the linear velocity of the object.
        glm::vec3 GetLinearVelocity() const;


        /// Sets the angular velocity of the physics object.
        void SetAngularVelocity(float x, float y, float z);
        void SetAngularVelocity(const glm::vec3 &velocity) { this->SetAngularVelocity(velocity.x, velocity.y, velocity.z); }

        /// Retrieves the angular velocity of the object.
        glm::vec3 GetAngularVelocity() const;
        

        /// Sets the linear factor.
        void SetLinearFactor(float x, float y, float z);
        void SetLinearFactor(const glm::vec3 &factor) { this->SetLinearFactor(factor.x, factor.y, factor.z); }

        /// Retrieves the linear factor.
        glm::vec3 GetLinearFactor() const;


        /// Sets the angular factor.
        void SetAngularFactor(float factor);
        void SetAngularFactor(float x, float y, float z);
        void SetAngularFactor(const glm::vec3 &factor) { this->SetAngularFactor(factor.x, factor.y, factor.z); }

        /// Retrieves the angular factor.
        glm::vec3 GetAngularFactor() const;


        /// Sets the gravity of the object.
        void SetGravity(float x, float y, float z);
        void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the gravity of the object.
        glm::vec3 GetGravity() const;

        /// Applies the objects gravity.
        void ApplyGravity();


        /// Applies a central force to the body.
        void ApplyCentralForce(float x, float y, float z);
        void ApplyCentralForce(const glm::vec3 &force) { this->ApplyCentralForce(force.x, force.y, force.z); }

        /// Applies torque to the body.
        void ApplyTorque(float x, float y, float z);
        void ApplyTorque(const glm::vec3 &torque) { this->ApplyTorque(torque.x, torque.y, torque.z); }


        /// Applies an impulse to the body.
        void ApplyImpulse(float x, float y, float z, float relPosX, float relPosY, float relPosZ);
        void ApplyImpulse(const glm::vec3 &impulse, const glm::vec3 &relativePosition) { this->ApplyImpulse(impulse.x, impulse.y, impulse.z, relativePosition.x, relativePosition.y, relativePosition.z); }

        /// Applies a central impulse to the body.
        void ApplyCentralImpulse(float x, float y, float z);
        void ApplyCentralImpulse(const glm::vec3 &impulse) { this->ApplyCentralImpulse(impulse.x, impulse.y, impulse.z); }

        /// Applies a torque impulse.
        void ApplyTorqueImpulse(float x, float y, float z);
        void ApplyTorqueImpulse(const glm::vec3 &impulse) { this->ApplyCentralImpulse(impulse.x, impulse.y, impulse.z); }


        /// Disables deactivation of the rigid body.
        void DisableDeactivation();

        /// Enables deactivation of the rigid body.
        void EnableDeactivation();

        /// Determines whether or not deactivation is enabled.
        bool IsDeactivationEnabled() const;

        /// Activates a deactivated rigid body.
        void Activate();


        /// Disables navigation mesh generation.
        void DisableNavigationMeshGeneration();

        /// Enables navigation mesh generation.
        void EnableNavigationMeshGeneration();

        /// Determines whether or not the component should be used with navigation mesh generation.
        bool IsNavigationMeshGenerationEnabled() const;



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTLib::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// Updates the mass of the rigid body. This will remove the the rigid body from the world, if it has one, and then re-add it.
        void UpdateMass();


        /////////////////////////////////////////////////
        // CollisionShapeComponent Events.

        /// CollisionShapeComponent::OnPreCollisionShapeChanged().
        void OnPreCollisionShapeChanged();

        /// CollisionShapeComponent::OnPostColliisonShapeChanged().
        void OnPostCollisionShapeChanged();



        


    private:

        /// The motion state of the body. This will be different depending on whether or not the rigid body is kinematic.
        SceneNodeMotionState motionState;

        /// The rigid body associated with this component.
        RigidBody* rigidBody;

        /// A very temporary pointer that keeps track of the world the rigid body is contained in. This is used for when the rigid body
        /// needs to be removed from and then re-added to the world.
        GTEngine::DynamicsWorld* world;

        
        /// The mass of the rigid body. Defaults to 0 (static).
        float mass;

        /// Keeps track of whether or not the object is kinematic.
        bool isKinematic;

        /// Keeps track of whether or not this object should be used for navigation mesh generation. Defaults to true.
        bool useWithNavigationMesh;



        GTENGINE_DECL_COMPONENT_ATTRIBS(DynamicsComponent)
    };
}

#endif