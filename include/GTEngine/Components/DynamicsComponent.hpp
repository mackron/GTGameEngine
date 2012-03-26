
#ifndef __GTEngine_DynamicsComponent_hpp_
#define __GTEngine_DynamicsComponent_hpp_

#include "../Component.hpp"
#include "../Physics.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing a dynamics component.
    ///
    /// The dynamics component is used to do collision detection and physics. 
    GTENGINE_DECL_COMPONENT_START(DynamicsComponent)
    public:

        /// Constructor.
        DynamicsComponent(SceneNode &node);

        /// Destructor.
        ~DynamicsComponent();


        /// Adds a cube collision shape.
        void AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        
        /// Adds a sphere collision shape.
        void AddSphereCollisionShape(float radius, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds an elipsoid collision shape.
        void AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Removes every collision shape.
        void RemoveAllCollisionShapes();


        /// Sets the mass of the rigid body. If this is 0.0, it will be static. Defaults to 0.0.
        void SetMass(float mass);

        /// Retrieves the mass of the rigid body.
        float GetMass() const;

        /// Sets whether or not the component is kinematic.
        void IsKinematic(bool isKinematic);

        /// Returns whether or not the rigid body is kinematic.
        bool IsKinematic() const;


        /// Sets the friction of the object.
        /// @param friction [in] The new friction value to use with the object. Larger values means more friction.
        void SetFriction(float friction);

        /// Retrieves the friction of the object.
        float GetFriction() const;

        /// Sets the restitution of the object (bounciness).
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


        /// Sets the collision group and masks to use with the scene node.
        /// @param group [in] The collision group(s) this scene node is part of.
        /// @param mask  [in] A mask of collision groups that this scene node will collide with.
        ///
        /// @remarks
        ///     Note that changing the collision filter requires that the internal dynamics objects are removed from the internal dynamics world
        ///     and then re-added. Thus, don't call this method too frequently.
        void SetCollisionFilter(short group, short mask);

        /// Retrieves the collision group as specified in SetCollisionFilter().
        short GetCollisionGroup() const { return this->collisionGroup; }
        
        /// Retrieves the collision mask as specified in SetCollisionFilter().
        short GetCollisionMask() const { return this->collisionMask; }


        /// Retrieves a reference to the rigid body.
        RigidBody & GetRigidBody() { return *this->rigidBody; }


        /// Applies a scale to the rigid body.
        void ApplyScaling(float x, float y, float z);


        

    private:

        /// Helper method for generically adding a collision shape.
        void AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ);

        /// Updates the mass of the rigid body. This will remove the the rigid body from the world, if it has one, and then re-add it.
        void UpdateMass();


    private:

        /// The collision shape we're using. This starts life as an empty shape. When the first collision shape is added it is turned into
        /// a compound shape. We need to use a compound shape in order to get offsets working properly.
        btCompoundShape collisionShape;

        /// The motion state of the body. This will be different depending on whether or not the rigid body is kinematic.
        SceneNodeMotionState motionState;

        /// The rigid body associated with this component.
        RigidBody* rigidBody;

        
        /// The mass of the rigid body. Defaults to 0 (static).
        float mass;

        /// Keeps track of whether or not the object is kinematic.
        bool isKinematic;


        /// The collision group the scene node is part of. This is set with SetCollisionFilter(). Defaults to 1.
        short collisionGroup;

        /// The collision mask for controlling the collision groups the scene node can collide with. This is set with SetCollisionFilter(). Defaults to -1 (collides with everything).
        short collisionMask;



    GTENGINE_DECL_COMPONENT_END()
}

#endif