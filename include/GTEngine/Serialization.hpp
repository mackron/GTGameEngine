// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Serialization_hpp_
#define __GTEngine_Serialization_hpp_

#include <GTLib/Serializer.hpp>
#include <GTLib/Deserializer.hpp>
#include <cstdint>

namespace GTEngine
{
    namespace Serialization
    {
        /// Structure representing the header of a chunk for serialization/deserialization.
        __declspec(align(4)) struct ChunkHeader
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
        static const uint32_t ChunkID_Null                                    = CHUNK_ID(0x00000000U);
        

        /////////////////////////////////////////////////////
        // Scenes

        // Scene
        static const uint32_t ChunkID_Scene_Info                              = CHUNK_ID(0x00000001U);
        static const uint32_t ChunkID_Scene_Nodes                             = CHUNK_ID(0x00000002U);
        static const uint32_t ChunkID_Scene_NodesHierarchy                    = CHUNK_ID(0x00000003U);
        static const uint32_t ChunkID_Scene_Properties                        = CHUNK_ID(0x00000004U);
        static const uint32_t ChunkID_Scene_Navigation                        = CHUNK_ID(0x00000005U);
        static const uint32_t ChunkID_Scene_ScriptComponentData               = CHUNK_ID(0x00000006U);
        static const uint32_t ChunkID_Scene_EditorMetadata                    = CHUNK_ID(0x0000000FU);

        // SceneNode
        static const uint32_t ChunkID_SceneNode_General                       = CHUNK_ID(0x00000020U);
        static const uint32_t ChunkID_SceneNode_Components                    = CHUNK_ID(0x00000021U);


        /////////////////////////////////////////////////////
        // Components

        // ModelComponent
        static const uint32_t ChunkID_ModelComponent_Main                     = CHUNK_ID(0x00000100U);

        // CameraComponent
        static const uint32_t ChunkID_CameraComponent_Main                    = CHUNK_ID(0x00000110U);

        // PointLightComponent
        static const uint32_t ChunkID_PointLightComponent_Main                = CHUNK_ID(0x00000120U);

        // SpotLightComponent
        static const uint32_t ChunkID_SpotLightComponent_Main                 = CHUNK_ID(0x00000130U);

        // DirectionalLightComponent
        static const uint32_t ChunkID_DirectionalLightComponent_Main          = CHUNK_ID(0x00000140U);

        // AmbientLightComponent
        static const uint32_t ChunkID_AmbientLightComponent_Main              = CHUNK_ID(0x00000150U);

        // CollisionShapeComponent
        static const uint32_t ChunkID_CollisionShapeComponent_Main                 = CHUNK_ID(0x00000160U);
        static const uint32_t ChunkID_CollisionShapeComponent_BoxShape             = CHUNK_ID(0x00000161U);
        static const uint32_t ChunkID_CollisionShapeComponent_SphereShape          = CHUNK_ID(0x00000162U);
        static const uint32_t ChunkID_CollisionShapeComponent_EllipsoidShape       = CHUNK_ID(0x00000163U);
        static const uint32_t ChunkID_CollisionShapeComponent_CylinderShape        = CHUNK_ID(0x00000164U);
        static const uint32_t ChunkID_CollisionShapeComponent_CapsuleShape         = CHUNK_ID(0x00000165U);
        static const uint32_t ChunkID_CollisionShapeComponent_ConvexHullShape      = CHUNK_ID(0x00000166U);
        static const uint32_t ChunkID_CollisionShapeComponent_ModelConvexHullShape = CHUNK_ID(0x00000167U);

        // DynamicsComponent
        static const uint32_t ChunkID_DynamicsComponent_Main                  = CHUNK_ID(0x00000170U);

        // ProximityComponent
        static const uint32_t ChunkID_ProximityComponent_Main                 = CHUNK_ID(0x00000180U);

        // GenericConstraintComponent
        static const uint32_t ChunkID_GenericConstraintComponent_Main         = CHUNK_ID(0x00000190U);

        // PointToPointConstraintComponent
        static const uint32_t ChunkID_PointToPointConstraintComponent_Main    = CHUNK_ID(0x000001A0U);

        // ConeTwistConstraintComponent.
        static const uint32_t ChunkID_ConeTwistConstraintComponent_Main       = CHUNK_ID(0x000001B0U);

        // --- Leave a bit of space for future constraints types ---

        // OccluderComonent
        static const uint32_t ChunkID_OccluderComponent_Main                  = CHUNK_ID(0x00000200U);

        // ParticleSystemComponent
        static const uint32_t ChunkID_ParticleSystemComponent_Main            = CHUNK_ID(0x00000210U);

        // ScriptComponent
        static const uint32_t ChunkID_ScriptComponent_Main                    = CHUNK_ID(0x00000220U);
        static const uint32_t ChunkID_ScriptComponent_OnSerialize             = CHUNK_ID(0x00000221U);

        // PrefabComponent
        static const uint32_t ChunkID_PrefabComponent_Main                    = CHUNK_ID(0x00000230U);

        // --- Leave a bit of space for future non-editor component types ---

        // EditorMetadataComponent.
        static const uint32_t ChunkID_EditorMetadataComponent_Main            = CHUNK_ID(0x000004F0U);



        /////////////////////////////////////////////////////
        // Assets/Resources

        // Model
        static const uint32_t ChunkID_Model_Header                            = CHUNK_ID(0x00000500U);
        static const uint32_t ChunkID_Model_Meshes                            = CHUNK_ID(0x00000501U);
        static const uint32_t ChunkID_Model_Bones                             = CHUNK_ID(0x00000502U);
        static const uint32_t ChunkID_Model_Animation                         = CHUNK_ID(0x00000503U);
        static const uint32_t ChunkID_Model_AnimationSegments                 = CHUNK_ID(0x00000504U);
        static const uint32_t ChunkID_Model_AnimationSequences                = CHUNK_ID(0x00000505U);
        static const uint32_t ChunkID_Model_ConvexHulls                       = CHUNK_ID(0x00000506U);

        // Material
        static const uint32_t ChunkID_Material_Main                           = CHUNK_ID(0x00000510U);
        static const uint32_t ChunkID_Material_Parameters                     = CHUNK_ID(0x00000511U);

        // Particles.
        static const uint32_t ChunkID_ParticleSystemDefinition_Main           = CHUNK_ID(0x00000520U);
        static const uint32_t ChunkID_ParticleSystem_Main                     = CHUNK_ID(0x00000521U);
        static const uint32_t ChunkID_ParticleEmitter_Main                    = CHUNK_ID(0x00000522U);
        static const uint32_t ChunkID_ParticleEmitter_Particles               = CHUNK_ID(0x00000523U);
        


        /////////////////////////////////////////////////////
        // Scene State Stack

        static const uint32_t ChunkID_SceneStateStack                         = CHUNK_ID(0x00000800U);
        static const uint32_t ChunkID_SceneStateStackBranch                   = CHUNK_ID(0x00000801U);
        static const uint32_t ChunkID_SceneStateStackFrame                    = CHUNK_ID(0x00000802U);
        static const uint32_t ChunkID_SceneStateStackStagingArea              = CHUNK_ID(0x00000803U);
        static const uint32_t ChunkID_SceneStateStackRestoreCommands          = CHUNK_ID(0x00000804U);


        /////////////////////////////////////////////////////
        // Navigation Mesh

        static const uint32_t ChunkID_NavigationMesh_Main                     = CHUNK_ID(0x00000810U);
        static const uint32_t ChunkID_NavigationMesh_RecastPolyMesh           = CHUNK_ID(0x00000811U);
        static const uint32_t ChunkID_NavigationMesh_DetourNavMesh            = CHUNK_ID(0x00000812U);


        /////////////////////////////////////////////////////
        // Misc. Objects

        static const uint32_t ChunkID_Mesh_Material                           = CHUNK_ID(0x00001000U);
        static const uint32_t ChunkID_Mesh_Geometry                           = CHUNK_ID(0x00001001U);
        static const uint32_t ChunkID_VertexArray_Info                        = CHUNK_ID(0x00001010U);
        static const uint32_t ChunkID_VertexArray_Vertices                    = CHUNK_ID(0x00001011U);
        static const uint32_t ChunkID_VertexArray_Indices                     = CHUNK_ID(0x00001012U);
        static const uint32_t ChunkID_ShaderParameters                        = CHUNK_ID(0x00001013U);
        static const uint32_t ChunkID_Animation_Main                          = CHUNK_ID(0x00001014U);
        static const uint32_t ChunkID_Animation_PlaybackState                 = CHUNK_ID(0x00001015U);
        static const uint32_t ChunkID_AnimationKeyFrameQueue                  = CHUNK_ID(0x00001016U);
        static const uint32_t ChunkID_Prefab                                  = CHUNK_ID(0x00001017U);
        static const uint32_t ChunkID_GameStateHeader                         = CHUNK_ID(0x00001018U);
        static const uint32_t ChunkID_GameStateScenes                         = CHUNK_ID(0x00001019U);
        static const uint32_t ChunkID_GameStateGlobal                         = CHUNK_ID(0x0000101AU);

    }
}

#endif