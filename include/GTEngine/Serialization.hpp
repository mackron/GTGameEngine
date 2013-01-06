
#ifndef __GTEngine_Serialization_hpp_
#define __GTEngine_Serialization_hpp_

#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>
#include <cstdint>

namespace GTEngine
{
    namespace Serialization
    {
        /// Structure representing the header of a chunk for serialization/deserialization.
        struct ChunkHeader
        {
            uint32_t id;
            uint32_t version;
            uint32_t sizeInBytes;
        };

        /// Structure for pairing scene nodes. Useful for parenting.
        struct SceneNodeIndexPair
        {
            uint32_t index0;
            uint32_t index1;
        };



        // Chunk IDs
        static const uint32_t ChunkID_Null = 0;
        
        // Scene
        static const uint32_t ChunkID_SceneNodes               = 1;
        static const uint32_t ChunkID_SceneNodesHierarchy      = 2;
        static const uint32_t ChunkID_SceneEditorMetadata      = 100;

        // DynamicsComponent
        static const uint32_t ChunkID_Dynamics_General         = 200;
        static const uint32_t ChunkID_Dynamics_RigidBody       = 201;
        static const uint32_t ChunkID_Dynamics_BoxShape        = 210;
        static const uint32_t ChunkID_Dynamics_SphereShape     = 211;
        static const uint32_t ChunkID_Dynamics_EllipsoidShape  = 212;
        static const uint32_t ChunkID_Dynamics_CylinderShape   = 213;
        static const uint32_t ChunkID_Dynamics_CapsuleShape    = 214;
        static const uint32_t ChunkID_Dynamics_ConvexHullShape = 215;


        // SceneObject
        static const uint32_t ChunkID_SceneObject     = 10000;

        // SceneNode
        static const uint32_t ChunkID_SceneNode_General        = 10100;
        static const uint32_t ChunkID_SceneNode_Components     = 10101;
    }
}

#endif