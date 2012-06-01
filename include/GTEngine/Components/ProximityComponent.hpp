
#ifndef __GTEngine_ProximityComponent_hpp_
#define __GTEngine_ProximityComponent_hpp_

#include "../Component.hpp"
#include "../Physics.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing a component for checking for proximity between objects.
    ///
    /// This component does not do any dynamics operations such as collision response and physics simulation. It is used only for checking
    /// whether or not objects are in proximity to other objects.
    GTENGINE_DECL_COMPONENT_START(ProximityComponent)
    public:

        /// Constructor.
        ProximityComponent(SceneNode &node);

        /// Destructor.
        ~ProximityComponent();


        /// Sets the collision shape to a box.
        void SetBoxShape(float halfX, float halfY, float halfZ);
        
        /// Adds a sphere collision shape.
        void SetSphereShape(float radius);

        /// Adds an elipsoid collision shape.
        void SetEllipsoidShape(float radiusX, float radiusY, float radiusZ);


        /// Retrieves a reference to the internal ghost object.
              GhostObject & GetGhostObject()       { return this->ghostObject; }
        const GhostObject & GetGhostObject() const { return this->ghostObject; }


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


        /// Applies a scale to the rigid body.
        void ApplyScaling(float x, float y, float z);
        void ApplyScaling(const glm::vec3 &scale) { this->ApplyScaling(scale.x, scale.y, scale.z); }



    public:

        /// Class for iterating over the scene nodes currently in proximity to this component.
        class Iterator
        {
        public:

            /// Constructor.
            Iterator(ProximityComponent &component)
                : component(component), otherNode(nullptr), manifoldArray(), i(0)
            {
                ++(*this);
            }


            // Pre-incement.
            Iterator & operator++();

            // operator bool.
            operator bool() const;


        public:

            /// The component we're iterating over.
            ProximityComponent &component;

            /// A pointer to the othe scene node that is in proximity to this one.
            SceneNode* otherNode;

            /// Array for manifolds. We store this here so that we don't have to re-instantiated every iteration.
            btManifoldArray manifoldArray;

            /// The counter for retrieving pairs. We use this in determining whether or not we're at the end of the iterator.
            int i;


        private:    // No copying.
            Iterator(const Iterator &);
            Iterator & operator=(const Iterator &);
        };


    private:

        /// Helper method for setting the shape of the ghost object.
        void SetShape(btCollisionShape* newShape);
        

    private:

        /// The ghost object we'll be using for proximity detection.
        GhostObject ghostObject;

        /// The collision shape we're using. This starts life as an empty shape. When the first collision shape is added it is turned into
        /// a compound shape. We need to use a compound shape in order to get offsets working properly.
        btCollisionShape* collisionShape;


        /// The collision group the scene node is part of. This is set with SetCollisionFilter(). Defaults to 1.
        short collisionGroup;

        /// The collision mask for controlling the collision groups the scene node can collide with. This is set with SetCollisionFilter(). Defaults to -1 (collides with everything).
        short collisionMask;


    friend class Iterator;
    GTENGINE_DECL_COMPONENT_END()
}

#endif
