
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
        //
        // We have IDs for every class that needs them. Each class is allocated a range of 16 IDs, which should be enough. We supply ranges in
        // case deserializers more easily check if the chunk their reading is one of their own. For example, if a chunk is read that the deserializer
        // does not know about, but that chunk ID is in the range of IDs for that particular object being read, it can assume that it might be an
        // older or newer chunk. The deserializer won't really want to produce an error in this case because the data it's looking at is still
        // related to the object.
        //
        // Another example is where a deserializer might want to keep looping over chunks until it finds one that is not related, in which case that
        // will indicate to the deserializer that it is finished.
        //
        // The last 8 bits should be the pattern 10101010. We use this as a simple validity check. This restricts our max chunk ID to 0xFFFFFF.
        #define CHUNK_ID(id)                ((static_cast<uint32_t>(0xAAU) << 23) |  static_cast<uint32_t>(id))


        /// Checks whether or not the given chunk ID is valid.
        ///
        /// @param id [in] The ID to check.
        ///
        /// @return True if the ID is valid, false otherwise.
        inline bool IsChunkIDValid(uint32_t id)
        {
            return id >> 23 == static_cast<uint32_t>(0xAAU);
        }


        // Nothing/Null/Unknown/Terminator.
        //
        // A chunk of this type is really good for marking the end of a group of chunks. It can operate in a similar way to a null terminator in a string.
        static const uint32_t ChunkID_Null                                  = CHUNK_ID(0x00000000U);
        

        /////////////////////////////////////////////////////
        // Scenes

        // Scene
        static const uint32_t ChunkID_SceneNodes                            = CHUNK_ID(0x00000001U);
        static const uint32_t ChunkID_SceneNodesHierarchy                   = CHUNK_ID(0x00000002U);
        static const uint32_t ChunkID_SceneEditorMetadata                   = CHUNK_ID(0x0000000FU);

        // SceneObject
        static const uint32_t ChunkID_SceneObject                           = CHUNK_ID(0x00000010U);

        // SceneNode
        static const uint32_t ChunkID_SceneNode_General                     = CHUNK_ID(0x00000020U);
        static const uint32_t ChunkID_SceneNode_Components                  = CHUNK_ID(0x00000021U);


        /////////////////////////////////////////////////////
        // Components

        // ModelComponent
        static const uint32_t ChunkID_ModelComponent_Main                   = CHUNK_ID(0x00000100U);

        // CameraComponent
        static const uint32_t ChunkID_CameraComponent_Main                  = CHUNK_ID(0x00000110U);

        // PointLightComponent
        static const uint32_t ChunkID_PointLightComponent_Main              = CHUNK_ID(0x00000120U);

        // SpotLightComponent
        static const uint32_t ChunkID_SpotLightComponent_Main               = CHUNK_ID(0x00000130U);

        // DirectionalLightComponent
        static const uint32_t ChunkID_DirectionalLightComponent_Main        = CHUNK_ID(0x00000140U);

        // AmbientLightComponent
        static const uint32_t ChunkID_AmbientLightComponent_Main            = CHUNK_ID(0x00000150U);

        // DynamicsComponent
        static const uint32_t ChunkID_DynamicsComponent_Main                = CHUNK_ID(0x00000160U);
        static const uint32_t ChunkID_DynamicsComponent_RigidBody           = CHUNK_ID(0x00000161U);
        static const uint32_t ChunkID_DynamicsComponent_BoxShape            = CHUNK_ID(0x00000162U);
        static const uint32_t ChunkID_DynamicsComponent_SphereShape         = CHUNK_ID(0x00000163U);
        static const uint32_t ChunkID_DynamicsComponent_EllipsoidShape      = CHUNK_ID(0x00000164U);
        static const uint32_t ChunkID_DynamicsComponent_CylinderShape       = CHUNK_ID(0x00000165U);
        static const uint32_t ChunkID_DynamicsComponent_CapsuleShape        = CHUNK_ID(0x00000166U);
        static const uint32_t ChunkID_DynamicsComponent_ConvexHullShape     = CHUNK_ID(0x00000167U);

        // GenericConstraintComponent
        static const uint32_t ChunkID_GenericConstraintComponent_Main       = CHUNK_ID(0x00000170U);

        // PointToPointConstraintComponent
        static const uint32_t ChunkID_PointToPointConstraintComponent_Main  = CHUNK_ID(0x00000180U);

        // ConeTwistConstraintComponent.
        static const uint32_t ChunkID_ConeTwistConstraintComponent_Main     = CHUNK_ID(0x00000190U);

        // --- Leave a bit of space for future constraints types ---

        // ProximityComponent
        static const uint32_t ChunkID_ProximityComponent_Main               = CHUNK_ID(0x00000200U);

        // OccluderComonent
        static const uint32_t ChunkID_OccluderComponent_Main                = CHUNK_ID(0x00000210U);

        // SpriteComponent
        static const uint32_t ChunkID_SpriteComponent_Main                  = CHUNK_ID(0x00000220U);

        // --- Leave a bit of space for future non-editor comonent types ---

        // EditorMetadataComponent.
        static const uint32_t ChunkID_EditorMetadataComponent_Main          = CHUNK_ID(0x000004F0U);



        /////////////////////////////////////////////////////
        // Assets/Resources

        // Model
        static const uint32_t ChunkID_Model_Main                            = CHUNK_ID(0x00000500U);
    }
}

#endif