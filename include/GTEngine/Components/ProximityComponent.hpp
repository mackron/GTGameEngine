// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ProximityComponent_hpp_
#define __GTEngine_ProximityComponent_hpp_

#include "CollisionShapeComponent.hpp"
#include "../AlignedType.hpp"
#include <GTEngine/Core/SortedVector.hpp>

namespace GTEngine
{
    /// Class representing a component for checking for proximity between objects.
    ///
    /// This component does not do any dynamics operations such as collision response and physics simulation. It is used only for checking
    /// whether or not objects are in proximity to other objects.
    class ProximityComponent : public CollisionShapeComponent, public SIMDAlignedType
    {
    public:

        /// Constructor.
        ProximityComponent(SceneNode &node);

        /// Destructor.
        ~ProximityComponent();


        /// Retrieves a reference to the internal ghost object.
              GhostObject & GetGhostObject()       { return this->ghostObject; }
        const GhostObject & GetGhostObject() const { return this->ghostObject; }


        /// Applies the scene node's transform.
        void ApplySceneNodeTransformation();


        /// Retrieves the list of IDs of scene nodes that are currently sitting inside the volume of this component.
        const GTLib::SortedVector<uint64_t> & GetSceneNodesInsideVolume() const { return this->sceneNodesInsideVolume; }

        
        /// Performs a complete check of scene nodes that are contained inside the volume.
        ///
        /// @param sceneNodesEntered [out] A reference to the vector that will receive the ID's of the scene nodes that have just entered the volume.
        /// @param sceneNodesLeft    [out] A reference to the vector that will receive the ID's of the scene nodes that have just left the volume.
        void UpdateContainment(GTLib::Vector<uint64_t> &sceneNodesEntered, GTLib::Vector<uint64_t> &sceneNodesLeft);



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTLib::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTLib::Deserializer &deserializer);



    public:

        /// Class for iterating over the scene nodes currently in proximity to this component.
        class Iterator
        {
        public:

            /// Constructor.
            Iterator(ProximityComponent &component);

            /// Constructor.
            Iterator(SceneNode &sceneNode);


            // Pre-incement.
            Iterator & operator++();

            // operator bool.
            operator bool() const;


        public:

            /// The component we're iterating over.
            ProximityComponent* component;

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

        /////////////////////////////////////////////////
        // CollisionShapeComponent Events.

        /// CollisionShapeComponent::OnPreCollisionShapeChanged().
        void OnPreCollisionShapeChanged();

        /// CollisionShapeComponent::OnPostColliisonShapeChanged().
        void OnPostCollisionShapeChanged();
        

    private:

        /// The ghost object we'll be using for proximity detection.
        GhostObject ghostObject;

        /// A very temporary pointer that keeps track of the world the rigid body is contained in. This is used for when the rigid body
        /// needs to be removed from and then re-added to the world.
        GTEngine::DynamicsWorld* m_world;


        /// The IDs of the scene nodes that are currently inside the volume of the component.
        GTLib::SortedVector<uint64_t> sceneNodesInsideVolume;


        friend class Iterator;
        GTENGINE_DECL_COMPONENT_ATTRIBS(ProximityComponent)
    };
}

#endif
