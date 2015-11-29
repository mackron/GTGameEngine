// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/GTEngine.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    ModelDefinition::ModelDefinition()
        : absolutePath(), relativePath(),
          meshes(), m_bones(),
          m_animation(), animationChannelBones(), animationKeyCache(),
          animationAABBPadding(0.25f),
          m_convexHulls(), convexHullBuildSettings()
    {
    }

    ModelDefinition::~ModelDefinition()
    {
        this->ClearMeshes();
        this->ClearBones();
        this->ClearAnimations();
        this->ClearConvexHulls();
    }

    bool ModelDefinition::LoadFromFile(const char* relativePathIn)
    {
        bool devnull;
        return this->LoadFromFile(relativePathIn, nullptr, devnull);
    }

    bool ModelDefinition::LoadFromFile(const char* fileNameIn, const char* relativePathIn, bool &needsSerialize)
    {
        char newAbsolutePath[EASYVFS_MAX_PATH];
        char newRelativePath[EASYVFS_MAX_PATH];

        if (easypath_isabsolute(fileNameIn))
        {
            strcpy_s(newAbsolutePath, sizeof(newAbsolutePath), fileNameIn);

            if (relativePathIn != nullptr)
            {
                strcpy_s(newRelativePath, sizeof(newRelativePath), relativePathIn);
            }
            else
            {
                GT::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileNameIn);
                return false;
            }
        }
        else
        {
            strcpy_s(newRelativePath, sizeof(newRelativePath), fileNameIn);

            if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileNameIn, newAbsolutePath, sizeof(newAbsolutePath)))
            {
                return false;
            }
        }


        char nativeAbsolutePath[EASYVFS_MAX_PATH];
        if (easypath_extensionequal(newAbsolutePath, "gtmodel"))
        {
            strcpy_s(nativeAbsolutePath, sizeof(nativeAbsolutePath), newAbsolutePath);

            easypath_removeextension(newAbsolutePath);
            easypath_removeextension(newRelativePath);
        }
        else
        {
            easypath_copyandappendextension(nativeAbsolutePath, sizeof(nativeAbsolutePath), newAbsolutePath, "gtmodel");
        }


        bool loadFromNativeFile = false;

        // We need file info of both the foreign and native files. If the foreign file is different to the file that would used to generate
        // the existing native file, it will be reloaded.
        easyvfs_file_info foreignFileInfo;
        bool foreignFileExists = easyvfs_get_file_info(g_EngineContext->GetVFS(), newAbsolutePath, &foreignFileInfo);

        easyvfs_file_info nativeFileInfo;
        bool nativeFileExists = easyvfs_get_file_info(g_EngineContext->GetVFS(), nativeAbsolutePath, &nativeFileInfo);

        if (!foreignFileExists && !nativeFileExists)
        {
            return false;
        }
        else
        {
            if (!foreignFileExists || (nativeFileExists && nativeFileInfo.lastModifiedTime > foreignFileInfo.lastModifiedTime))
            {
                loadFromNativeFile = true;
            }
            else
            {
                loadFromNativeFile = false;
            }
        }


        bool successful = false;
        if (loadFromNativeFile)
        {
            successful     = this->LoadFromNativeFile(nativeFileInfo.absolutePath);
            needsSerialize = false;
        }
        else
        {
            successful     = this->LoadFromForeignFile(foreignFileInfo.absolutePath);
            needsSerialize = true;
        }


        if (successful)
        {
            this->absolutePath = newAbsolutePath;
            this->relativePath = newRelativePath;

            return true;
        }

        return false;
    }


    const char* ModelDefinition::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* ModelDefinition::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }


    void ModelDefinition::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            if (this->meshes[i].geometry != nullptr)
            {
                this->meshes[i].geometry->GenerateTangentsAndBitangents();
            }
        }
    }

    TransformAnimationKey* ModelDefinition::CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    {
        auto newKey = new TransformAnimationKey(position, rotation, scale);
        this->animationKeyCache.PushBack(newKey);

        return newKey;
    }

    void ModelDefinition::MapAnimationChannelToBone(Bone &bone, AnimationChannel &channel)
    {
        this->animationChannelBones.Add(&bone, &channel);
    }

    void ModelDefinition::BuildConvexDecomposition(ConvexHullBuildSettings &settings)
    {
        // We need to delete the old convex hulls.
        for (size_t i = 0; i < m_convexHulls.count; ++i)
        {
            delete m_convexHulls[i];
        }
        m_convexHulls.Clear();


        // After deleting the old convex hulls we can build the new ones.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            if (this->meshes[i].geometry != nullptr)
            {
                ConvexHull*  convexHulls;
                unsigned int count;
                ConvexHull::BuildConvexHulls(*this->meshes[i].geometry, convexHulls, count, settings);

                for (size_t iHull = 0; iHull < count; ++iHull)
                {
                    m_convexHulls.PushBack(new ConvexHull(convexHulls[iHull]));
                }

                ConvexHull::DeleteConvexHulls(convexHulls);
            }
        }

        // We're going to store the settings that were used to build the convex hulls. These will be stored as metadata in the .gtmodel file.
        this->convexHullBuildSettings = settings;
    }


    void ModelDefinition::ClearMeshes()
    {
        for (size_t iMesh = 0; iMesh < this->meshes.count; ++iMesh)
        {
            auto &mesh = this->meshes[iMesh];
            
            Renderer::DeleteVertexArray(mesh.geometry);
            MaterialLibrary::Delete(mesh.material);
            delete [] mesh.skinningVertexAttributes;
        }

        this->meshes.Clear();
    }

    void ModelDefinition::ClearBones()
    {
        for (size_t i = 0; i < m_bones.count; ++i)
        {
            delete m_bones.buffer[i];
        }
        m_bones.Clear();
    }

    void ModelDefinition::ClearAnimations(bool clearNamedSegments)
    {
        m_animation.Clear(clearNamedSegments);

        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
        this->animationKeyCache.Clear();

        this->animationChannelBones.Clear();
    }

    void ModelDefinition::ClearNamedAnimationSegments()
    {
        m_animation.ClearNamedSegments();
    }

    void ModelDefinition::ClearConvexHulls()
    {
        for (size_t i = 0; i < m_convexHulls.count; ++i)
        {
            delete m_convexHulls[i];
        }
        m_convexHulls.Clear();
    }



    void ModelDefinition::AddMesh(const ModelDefinition::Mesh &mesh)
    {
        this->meshes.PushBack(mesh);
    }

    ModelDefinition::Mesh* ModelDefinition::GetMeshByName(const char* meshName)
    {
        for (size_t iMesh = 0; iMesh < this->meshes.count; ++iMesh)
        {
            if (this->meshes[iMesh].name == meshName)
            {
                return &this->meshes[iMesh];
            }
        }

        return nullptr;
    }

    const ModelDefinition::Mesh* ModelDefinition::GetMeshByName(const char* meshName) const
    {
        for (size_t iMesh = 0; iMesh < this->meshes.count; ++iMesh)
        {
            if (this->meshes[iMesh].name == meshName)
            {
                return &this->meshes[iMesh];
            }
        }

        return nullptr;
    }

    size_t ModelDefinition::GetMeshCount() const
    {
        return this->meshes.count;
    }


    size_t ModelDefinition::AddBone(Bone* bone)
    {
        m_bones.PushBack(bone);
        return m_bones.count - 1;
    }

    Bone* ModelDefinition::GetBoneByName(const char* boneName)
    {
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            auto bone = m_bones[iBone];
            assert(bone != nullptr);
            {
                if (Strings::Equal(bone->GetName(), boneName))
                {
                    return bone;
                }
            }
        }

        return nullptr;
    }

    const Bone* ModelDefinition::GetBoneByName(const char* boneName) const
    {
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            auto bone = m_bones[iBone];
            assert(bone != nullptr);
            {
                if (Strings::Equal(bone->GetName(), boneName))
                {
                    return bone;
                }
            }
        }

        return nullptr;
    }


    Bone* ModelDefinition::GetBoneByIndex(size_t boneIndex)
    {
        return m_bones[boneIndex];
    }

    const Bone* ModelDefinition::GetBoneByIndex(size_t boneIndex) const
    {
        return m_bones[boneIndex];
    }


    bool ModelDefinition::FindBoneIndex(const Bone* bone, size_t &indexOut) const
    {
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            if (m_bones[iBone] == bone)
            {
                indexOut = iBone;
                return true;
            }
        }

        return false;
    }

    bool ModelDefinition::FindBoneIndex(const char* boneName, size_t &indexOut) const
    {
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            auto bone = m_bones[iBone];
            assert(bone != nullptr);
            {
                if (Strings::Equal(bone->GetName(), boneName))
                {
                    indexOut = iBone;
                    return true;
                }
            }
        }

        return false;
    }

    size_t ModelDefinition::GetBoneCount() const
    {
        return m_bones.count;
    }


    const glm::vec3 & ModelDefinition::GetAnimationAABBPadding() const
    {
        return this->animationAABBPadding;
    }

    void ModelDefinition::SetAnimationAABBPadding(const glm::vec3 &padding)
    {
        this->animationAABBPadding = padding;
    }


    ////////////////////////////////////////////////////////
    // Serialization/Deserialization

    void ModelDefinition::Serialize(Serializer &serializer) const
    {
        BasicSerializer intermediarySerializer;


        /////////////////////////////////////
        // Bones

        intermediarySerializer.Write(static_cast<uint32_t>(m_bones.count));
        
        // Local information.
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            auto bone = m_bones[iBone];
            assert(bone != nullptr);
            {
                // Name.
                intermediarySerializer.WriteString(bone->GetName());

                // Relative transformation.
                intermediarySerializer.Write(bone->GetPosition());
                intermediarySerializer.Write(bone->GetRotation());
                intermediarySerializer.Write(bone->GetScale());

                // 4x4 offset matrix.
                intermediarySerializer.Write(bone->GetOffsetMatrix());
            }
        }

        // Parents. -1 for bones without parents.
        for (size_t iBone = 0; iBone < m_bones.count; ++iBone)
        {
            auto bone = m_bones[iBone];
            assert(bone != nullptr);
            {
                auto parentBone = bone->GetParent();
                if (parentBone != nullptr)
                {
                    size_t parentBoneIndex;
                    if (!this->FindBoneIndex(parentBone, parentBoneIndex))
                    {
                        assert(false);
                    }
                    
                    intermediarySerializer.Write(static_cast<uint32_t>(parentBoneIndex));
                }
                else
                {
                    intermediarySerializer.Write(static_cast<uint32_t>(-1));
                }
            }
        }

        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_Model_Bones;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Meshes
        //
        // Vertex data is always saved in P3T2N3T3B3 format. Bones are referenced using an integer to index into the list of bones defined above.

        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(this->meshes.count));

        for (size_t iMesh = 0; iMesh < this->meshes.count; ++iMesh)
        {
            // Name.
            intermediarySerializer.WriteString(this->meshes[iMesh].name);


            // Material.
            intermediarySerializer.WriteString(this->meshes[iMesh].material->GetDefinition().relativePath);


            // Vertices.
            auto vertexArray = this->meshes[iMesh].geometry;
            assert(vertexArray != nullptr);
            {
                // Vertex Format.
                vertexArray->GetFormat().Serialize(intermediarySerializer);


                // Vertices.
                intermediarySerializer.Write(static_cast<uint32_t>(vertexArray->GetVertexCount()));

                if (vertexArray->GetVertexCount() > 0)
                {
                    intermediarySerializer.Write(vertexArray->GetVertexDataPtr(), vertexArray->GetFormat().GetSizeInBytes() * vertexArray->GetVertexCount());
                }


                // Indices.
                intermediarySerializer.Write(static_cast<uint32_t>(vertexArray->GetIndexCount()));

                if (vertexArray->GetIndexCount() > 0)
                {
                    intermediarySerializer.Write(vertexArray->GetIndexDataPtr(), sizeof(uint32_t) * vertexArray->GetIndexCount());
                }


                // Skinning Vertex Attributes.
                if (this->meshes[iMesh].skinningVertexAttributes != nullptr)
                {
                    intermediarySerializer.Write(static_cast<uint32_t>(vertexArray->GetVertexCount()));

                    if (vertexArray->GetVertexCount() > 0)
                    {
                        uint32_t totalBoneWeights = 0;


                        // Bone Counts.
                        for (size_t iVertex = 0; iVertex < vertexArray->GetVertexCount(); ++iVertex)
                        {
                            uint16_t count = static_cast<uint16_t>(this->meshes[iMesh].skinningVertexAttributes[iVertex].bones.count);
                            intermediarySerializer.Write(count);

                            totalBoneWeights += count;
                        }

                            
                        // Bone/Weight Pairs.
                        intermediarySerializer.Write(totalBoneWeights);

                        for (size_t iVertex = 0; iVertex < vertexArray->GetVertexCount(); ++iVertex)
                        {
                            auto &bones = this->meshes[iMesh].skinningVertexAttributes[iVertex].bones;
                            {
                                for (size_t iBone = 0; iBone < bones.count; ++iBone)
                                {
                                    auto &bone = bones[iBone];

                                    intermediarySerializer.Write(static_cast<uint32_t>(bone.boneIndex));
                                    intermediarySerializer.Write(static_cast<float   >(bone.weight));
                                }
                            }
                        }
                    }
                }
                else
                {
                    intermediarySerializer.Write(static_cast<uint32_t>(0));
                }


                // Default uniforms.
                this->meshes[iMesh].defaultUniforms.Serialize(intermediarySerializer);
            }
        }

        header.id          = Serialization::ChunkID_Model_Meshes;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Animation

        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(m_animation.GetKeyFrameCount()));

        for (uint32_t iKeyFrame = 0; iKeyFrame < m_animation.GetKeyFrameCount(); ++iKeyFrame)
        {
            // Time.
            intermediarySerializer.Write(static_cast<float>(m_animation.GetKeyFrameTimeByIndex(iKeyFrame)));
            
            
            // Channels.
            intermediarySerializer.Write(static_cast<uint32_t>(this->animationChannelBones.count));

            for (size_t iChannel = 0; iChannel < this->animationChannelBones.count; ++iChannel)
            {
                auto channel = this->animationChannelBones.buffer[iChannel]->value;
                auto bone    = this->animationChannelBones.buffer[iChannel]->key;

                assert(channel != nullptr);
                assert(bone    != nullptr);
                {
                    // Bone Index.
                    size_t boneIndex;
                    this->FindBoneIndex(bone, boneIndex);

                    intermediarySerializer.Write(static_cast<uint32_t>(boneIndex));


                    // Bone Transformation.
                    glm::vec3 position;
                    glm::quat rotation;
                    glm::vec3 scale;

                    auto key = static_cast<TransformAnimationKey*>(channel->GetKey(iKeyFrame));
                    if (key != nullptr)
                    {
                        position = key->position;
                        rotation = key->rotation;
                        scale    = key->scale;
                    }
                    else
                    {
                        // We should never actually get here, but for completeness we'll just write identities.
                        position = bone->GetPosition();
                        rotation = bone->GetRotation();
                        scale    = bone->GetScale();
                    }

                    intermediarySerializer.Write(position);
                    intermediarySerializer.Write(rotation);
                    intermediarySerializer.Write(scale);
                }
            }
        }

        intermediarySerializer.Write(this->animationAABBPadding);


        header.id          = Serialization::ChunkID_Model_Animation;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Animation Segments

        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(m_animation.GetNamedSegmentCount()));

        for (size_t iSegment = 0; iSegment < m_animation.GetNamedSegmentCount(); ++iSegment)
        {
            auto segment = m_animation.GetNamedSegmentByIndex(iSegment);
            assert(segment != nullptr);
            {
                intermediarySerializer.WriteString(segment->name);
                intermediarySerializer.Write(static_cast<uint32_t>(segment->startKeyFrame));
                intermediarySerializer.Write(static_cast<uint32_t>(segment->endKeyFrame));
            }
        }

        header.id          = Serialization::ChunkID_Model_AnimationSegments;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Animation Sequences

        /*
        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(this->animationSequences.count));

        for (size_t iSequence = 0; iSequence < this->animationSequences.count; ++iSequence)
        {
        }

        header.id          = Serialization::ChunkID_Model_AnimationSequences;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        */


        /////////////////////////////////////
        // Convex Hulls

        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(m_convexHulls.count));

        Vector<uint32_t> vertexCounts(m_convexHulls.count);
        Vector<uint32_t> indexCounts( m_convexHulls.count);
        Vector<float>    vertices;
        Vector<uint32_t> indices;

        for (size_t iConvexHull = 0; iConvexHull < m_convexHulls.count; ++iConvexHull)
        {
            auto convexHull = m_convexHulls[iConvexHull];
            assert(convexHull != nullptr);
            {
                uint32_t vertexCount = static_cast<uint32_t>(convexHull->GetVertexCount());
                uint32_t indexCount  = static_cast<uint32_t>(convexHull->GetIndexCount());

                vertexCounts.PushBack(vertexCount);
                indexCounts.PushBack( indexCount);

                for (uint32_t iVertex = 0; iVertex < vertexCount; ++iVertex)
                {
                    vertices.PushBack(convexHull->GetVertices()[iVertex * 3 + 0]);
                    vertices.PushBack(convexHull->GetVertices()[iVertex * 3 + 1]);
                    vertices.PushBack(convexHull->GetVertices()[iVertex * 3 + 2]);
                }

                for (uint32_t iIndex = 0; iIndex < indexCount; ++iIndex)
                {
                    indices.PushBack(convexHull->GetIndices()[iIndex]);
                }
            }
        }

        intermediarySerializer.Write(vertexCounts.buffer, vertexCounts.count * 4);
        intermediarySerializer.Write(indexCounts.buffer,  indexCounts.count  * 4);

        intermediarySerializer.Write(static_cast<uint32_t>(vertices.count));
        intermediarySerializer.Write(static_cast<uint32_t>(indices.count));

        intermediarySerializer.Write(vertices.buffer, vertices.count * 4);
        intermediarySerializer.Write(indices.buffer,  indices.count  * 4);

        intermediarySerializer.Write(this->convexHullBuildSettings);


        header.id          = Serialization::ChunkID_Model_ConvexHulls;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Null Chunk

        header.id          = Serialization::ChunkID_Null;
        header.version     = 1;
        header.sizeInBytes = 0;
        serializer.Write(header);
    }

    bool ModelDefinition::Deserialize(Deserializer &deserializer)
    {
        // Clear everything.
        this->ClearMeshes();
        this->ClearBones();
        this->ClearAnimations(true);        // <-- 'true' = clear animation segments, too.
        this->ClearConvexHulls();


        // We keep looping until we hit the null or unknown chunk.
        Serialization::ChunkHeader header;
        while (deserializer.Peek(&header, sizeof(header)) == sizeof(header))
        {
            bool finished = false;

            switch (header.id)
            {
            case Serialization::ChunkID_Model_Bones:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t boneCount;
                        deserializer.Read(boneCount);

                        for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
                        {
                            // Name.
                            String name;
                            deserializer.ReadString(name);

                            // Local transform.
                            glm::vec3 position;
                            glm::quat rotation;
                            glm::vec3 scale;

                            deserializer.Read(position);
                            deserializer.Read(rotation);
                            deserializer.Read(scale);

                            // 4x4 offset matrix.
                            glm::mat4 offsetMatrix;
                            deserializer.Read(offsetMatrix);


                            auto bone = new Bone;
                            bone->SetName(name.c_str());
                            bone->SetPosition(position);
                            bone->SetRotation(rotation);
                            bone->SetScale(scale);
                            bone->SetOffsetMatrix(offsetMatrix);
                            this->AddBone(bone);


                            // We need to create a channel for this bone. We then need to map that channel to a bone.
                            auto &channel = m_animation.CreateChannel();
                            this->animationChannelBones.Add(bone, &channel);
                        }


                        // Parents.
                        auto boneParentIndices = static_cast<uint32_t*>(malloc(boneCount * sizeof(uint32_t)));
                        deserializer.Read(boneParentIndices, boneCount * sizeof(uint32_t));

                        for (uint32_t iBone = 0; iBone < boneCount; ++iBone)
                        {
                            uint32_t parentIndex = boneParentIndices[iBone];

                            if (parentIndex != static_cast<uint32_t>(-1))
                            {
                                m_bones[parentIndex]->AttachChild(*m_bones[iBone]);
                            }
                        }

                        free(boneParentIndices);
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Model_Meshes:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t meshCount;
                        deserializer.Read(meshCount);

                        for (uint32_t iMesh = 0; iMesh < meshCount; ++iMesh)
                        {
                            ModelDefinition::Mesh newMesh;

                            // Name.
                            deserializer.ReadString(newMesh.name);


                            // Material
                            String materialName;
                            deserializer.ReadString(materialName);

                            newMesh.material = MaterialLibrary::Create(materialName.c_str());



                            // Geometry
                            VertexFormat vertexFormat;
                            vertexFormat.Deserialize(deserializer);
                            newMesh.geometry = Renderer::CreateVertexArray(VertexArrayUsage_Static, vertexFormat);
                            
                            // Vertices.
                            uint32_t vertexCount;
                            deserializer.Read(vertexCount);

                            if (vertexCount > 0)
                            {
                                newMesh.geometry->SetVertexData(nullptr, static_cast<size_t>(vertexCount));
                                auto vertexData = newMesh.geometry->MapVertexData();
                                {
                                    deserializer.Read(vertexData, vertexCount * vertexFormat.GetSizeInBytes());
                                }
                                newMesh.geometry->UnmapVertexData();
                            }

                            // Indices.
                            uint32_t indexCount;
                            deserializer.Read(indexCount);

                            if (indexCount > 0)
                            {
                                newMesh.geometry->SetIndexData(nullptr, static_cast<size_t>(indexCount));
                                auto indexData = newMesh.geometry->MapIndexData();
                                {
                                    deserializer.Read(indexData, indexCount * sizeof(uint32_t));
                                }
                                newMesh.geometry->UnmapIndexData();
                            }



                            // Skinning Vertex Attributes
                            uint32_t skinningVertexAttributeCount;
                            deserializer.Read(skinningVertexAttributeCount);

                            if (skinningVertexAttributeCount > 0)
                            {
                                newMesh.skinningVertexAttributes = new SkinningVertexAttribute[skinningVertexAttributeCount];

                                auto counts = static_cast<uint16_t*>(malloc(skinningVertexAttributeCount * sizeof(uint16_t)));
                                deserializer.Read(counts, skinningVertexAttributeCount * sizeof(uint16_t));

                                uint32_t totalBoneWeights;
                                deserializer.Read(totalBoneWeights);

                                auto boneWeights = static_cast<BoneWeightPair*>(malloc(totalBoneWeights * sizeof(BoneWeightPair)));
                                deserializer.Read(boneWeights, totalBoneWeights * sizeof(BoneWeightPair));


                                auto currentBoneWeight = boneWeights;
                                for (uint32_t iVertex = 0; iVertex < skinningVertexAttributeCount; ++iVertex)
                                {
                                    auto count = counts[iVertex];

                                    // Here we allocate the buffer for the bones. We trick the vector here by modifying attributes directly.
                                    newMesh.skinningVertexAttributes[iVertex].bones.Reserve(count);
                                    newMesh.skinningVertexAttributes[iVertex].bones.count = count;

                                    for (uint16_t iBone = 0; iBone < count; ++iBone)
                                    {
                                        newMesh.skinningVertexAttributes[iVertex].bones[iBone] = *currentBoneWeight++;
                                    }
                                }

                                free(counts);
                                free(boneWeights);
                            }


                            // Uniforms.
                            newMesh.defaultUniforms.Deserialize(deserializer);


                            // Finally, add the mesh.
                            this->AddMesh(newMesh);
                        }
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Model_Animation:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t keyFrameCount;
                        deserializer.Read(keyFrameCount);

                        for (size_t iKeyFrame = 0; iKeyFrame < keyFrameCount; ++iKeyFrame)
                        {
                            float time;
                            deserializer.Read(time);

                            size_t keyFrameIndex = m_animation.AppendKeyFrame(static_cast<double>(time));


                            // With the key frame added, we now need to iterate over each channel in the key frame.
                            uint32_t channelCount;
                            deserializer.Read(channelCount);

                            for (uint32_t iChannel = 0; iChannel < channelCount; ++iChannel)
                            {
                                uint32_t boneIndex;
                                deserializer.Read(boneIndex);

                                auto bone = m_bones[boneIndex];
                                assert(bone != nullptr);
                                {
                                    auto iChannelBone = this->animationChannelBones.Find(bone);
                                    assert(iChannelBone != nullptr);
                                    {
                                        auto channel = iChannelBone->value;

                                        glm::vec3 position;
                                        glm::quat rotation;
                                        glm::vec3 scale;
                                        
                                        deserializer.Read(position);
                                        deserializer.Read(rotation);
                                        deserializer.Read(scale);

                                        auto key = new TransformAnimationKey(position, rotation, scale);
                                        channel->SetKey(keyFrameIndex, key);


                                        // We need to cache the key.
                                        this->animationKeyCache.PushBack(key);
                                    }
                                }
                            }
                        }

                        deserializer.Read(this->animationAABBPadding);
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Model_AnimationSegments:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t animationSegmentCount;
                        deserializer.Read(animationSegmentCount);
                        
                        for (uint32_t iSegment = 0; iSegment < animationSegmentCount; ++iSegment)
                        {
                            String name;
                            uint32_t startKeyFrame;
                            uint32_t endKeyFrame;

                            deserializer.ReadString(name);
                            deserializer.Read(startKeyFrame);
                            deserializer.Read(endKeyFrame);

                            m_animation.AddNamedSegment(name.c_str(), static_cast<size_t>(startKeyFrame), static_cast<size_t>(endKeyFrame));
                        }
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Model_AnimationSequences:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Model_ConvexHulls:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t convexHullCount;
                        deserializer.Read(convexHullCount);

                        uint32_t* vertexCounts = static_cast<uint32_t*>(malloc(convexHullCount * sizeof(uint32_t)));
                        uint32_t* indexCounts  = static_cast<uint32_t*>(malloc(convexHullCount * sizeof(uint32_t)));

                        deserializer.Read(vertexCounts, convexHullCount * sizeof(uint32_t));
                        deserializer.Read(indexCounts,  convexHullCount * sizeof(uint32_t));


                        uint32_t totalVertexCount;
                        deserializer.Read(totalVertexCount);

                        uint32_t totalIndexCount;
                        deserializer.Read(totalIndexCount);


                        auto vertices = static_cast<float*   >(malloc(totalVertexCount * sizeof(float)));
                        auto indices  = static_cast<uint32_t*>(malloc(totalIndexCount  * sizeof(uint32_t)));

                        deserializer.Read(vertices, totalVertexCount * sizeof(float));
                        deserializer.Read(indices,  totalIndexCount  * sizeof(uint32_t));


                        auto currentVertices = vertices;
                        auto currentIndices  = indices;

                        for (uint32_t iConvexHull = 0; iConvexHull < convexHullCount; ++iConvexHull)
                        {
                            size_t vertexCount = static_cast<size_t>(vertexCounts[iConvexHull]);
                            size_t indexCount  = static_cast<size_t>(indexCounts[iConvexHull]);

                            m_convexHulls.PushBack(new ConvexHull(currentVertices, vertexCount, currentIndices, indexCount));

                            // Now we need to move our pointers forward.
                            currentVertices += vertexCount * 3;
                            currentIndices  += indexCount;
                        }


                        // Build Settings.
                        deserializer.Read(this->convexHullBuildSettings);


                        free(vertexCounts);
                        free(indexCounts);
                        free(vertices);
                        free(indices);
                    }
                    else
                    {
                        // Unsupported Version.
                        deserializer.Seek(header.sizeInBytes);
                    }

                    break;
                }

            case Serialization::ChunkID_Null:
                {
                    deserializer.Seek(sizeof(header));

                    finished = true;
                    break;
                }

            default:
                {
                    // Unknown chunk = Error.
                    finished = true;
                    return false;
                }
            }


            if (finished)
            {
                break;
            }
        }


        // If we get here, we were successful.
        return true;
    }


    ////////////////////////////////////////////////////////
    // Private

    bool ModelDefinition::LoadFromNativeFile(const String &absolutePathIn)
    {
        // When loading from a native file, all we need to do is deserialize.
        bool successful = false;

        easyvfs_file* pFile = easyvfs_open(g_EngineContext->GetVFS(), absolutePathIn.c_str(), EASYVFS_READ, 0);
        if (pFile != nullptr)
        {
            FileDeserializer deserializer(pFile);
            successful = this->Deserialize(deserializer);

            easyvfs_close(pFile);
        }

        return successful;
    }

    bool ModelDefinition::LoadFromForeignFile(const String &absolutePathIn)
    {
        // Currently, all foreign formats are loaded via Assimp.
        return this->LoadFromAssimpFile(absolutePathIn);
    }
}
