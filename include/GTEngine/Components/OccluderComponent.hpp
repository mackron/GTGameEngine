
#ifndef __GTEngine_OccluderComponent_hpp_
#define __GTEngine_OccluderComponent_hpp_

#include "../Component.hpp"
#include "../Physics.hpp"

namespace GTEngine
{
    /// Class representing a component for objects that act as occluders.
    ///
    /// The engine uses the physics engine internally for doing occlusion culling. Thus, this component uses a collision object as it's main attribute. Unlike
    /// DynamicsComponent, OccluderComponent can only have a single box shape attached. This is due to occluders never really being made up of anything other
    /// than very simple primitives, for efficiency.
    GTENGINE_DECL_COMPONENT_START(OccluderComponent)
    public:

        /// Constructor.
        OccluderComponent(SceneNode &node);

        /// Destructor.
        ~OccluderComponent();


        /// Sets the collision shape to a box.
        void SetBoxShape(float halfX, float halfY, float halfZ);


        /// Retrieves a reference to the internal collision object.
              CollisionObject & GetCollisionObject()       { return this->collisionObject; }
        const CollisionObject & GetCollisionObject() const { return this->collisionObject; }


        /// Applies a scale to the rigid body.
        void ApplyScaling(float x, float y, float z);
        void ApplyScaling(const glm::vec3 &scale) { this->ApplyScaling(scale.x, scale.y, scale.z); }


    private:

        /// The ghost object we'll be using for proximity detection.
        CollisionObject collisionObject;

        /// The collision shape we're using. This starts life as an empty shape. When the first collision shape is added it is turned into
        /// a compound shape. We need to use a compound shape in order to get offsets working properly.
        btBoxShape* collisionShape;


    GTENGINE_DECL_COMPONENT_END()
}


#endif
