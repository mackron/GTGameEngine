// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/IO.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    ModelDefinition::ModelDefinition()
        : absolutePath(), relativePath(),
          meshes(), bones(),
          animation(), animationChannelBones(), animationKeyCache(),
          convexHulls(), convexHullBuildSettings()
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
        GTCore::String newAbsolutePath;
        GTCore::String newRelativePath;

        if (GTCore::Path::IsAbsolute(fileNameIn))
        {
            newAbsolutePath = fileNameIn;

            if (relativePathIn != nullptr)
            {
                newRelativePath = relativePathIn;
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileNameIn);
                return false;
            }
        }
        else
        {
            newRelativePath = fileNameIn;

            if (!GTCore::IO::FindAbsolutePath(fileNameIn, newAbsolutePath))
            {
                return false;
            }
        }


        GTCore::String nativeAbsolutePath;
        if (GTCore::Path::ExtensionEqual(newAbsolutePath.c_str(), "gtmodel"))
        {
            nativeAbsolutePath = newAbsolutePath;

            newAbsolutePath = GTCore::IO::RemoveExtension(newAbsolutePath.c_str());
            newRelativePath = GTCore::IO::RemoveExtension(newRelativePath.c_str());
        }
        else
        {
            nativeAbsolutePath = newAbsolutePath + ".gtmodel";
        }


        bool loadFromNativeFile = false;

        // We need file info of both the foreign and native files. If the foreign file is different to the file that would used to generate
        // the existing native file, it will be reloaded. 
        GTCore::IO::FileInfo foreignFileInfo;
        GTCore::IO::GetFileInfo(newAbsolutePath.c_str(), foreignFileInfo);

        GTCore::IO::FileInfo nativeFileInfo;
        GTCore::IO::GetFileInfo(nativeAbsolutePath.c_str(), nativeFileInfo);

        if (!foreignFileInfo.exists && !nativeFileInfo.exists)
        {
            return false;
        }
        else
        {
            if (!foreignFileInfo.exists || (nativeFileInfo.exists && nativeFileInfo.lastModifiedTime > foreignFileInfo.lastModifiedTime))
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
        for (size_t i = 0; i < this->convexHulls.count; ++i)
        {
            delete this->convexHulls[i];
        }
        this->convexHulls.Clear();


        // After deleting the old convex hulls we can build the new ones.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            if (this->meshes[i].geometry != nullptr)
            {
                ConvexHull* convexHulls;
                size_t      count;
                ConvexHull::BuildConvexHulls(*this->meshes[i].geometry, convexHulls, count, settings);

                for (size_t iHull = 0; iHull < count; ++iHull)
                {
                    this->convexHulls.PushBack(new ConvexHull(convexHulls[iHull]));
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
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i];
        }
        this->bones.Clear();
    }

    void ModelDefinition::ClearAnimations(bool clearNamedSegments)
    {
        this->animation.Clear(clearNamedSegments);

        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
        this->animationKeyCache.Clear();

        this->animationChannelBones.Clear();
    }

    void ModelDefinition::ClearNamedAnimationSegments()
    {
        this->animation.ClearNamedSegments();
    }

    void ModelDefinition::ClearConvexHulls()
    {
        for (size_t i = 0; i < this->convexHulls.count; ++i)
        {
            delete this->convexHulls[i];
        }
        this->convexHulls.Clear();
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



    size_t ModelDefinition::AddBone(Bone* bone)
    {
        this->bones.PushBack(bone);
        return this->bones.count - 1;
    }

    Bone* ModelDefinition::GetBoneByName(const char* boneName)
    {
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones[iBone];
            assert(bone != nullptr);
            {
                if (GTCore::Strings::Equal(bone->GetName(), boneName))
                {
                    return bone;
                }
            }
        }

        return nullptr;
    }

    const Bone* ModelDefinition::GetBoneByName(const char* boneName) const
    {
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones[iBone];
            assert(bone != nullptr);
            {
                if (GTCore::Strings::Equal(bone->GetName(), boneName))
                {
                    return bone;
                }
            }
        }

        return nullptr;
    }

    bool ModelDefinition::FindBoneIndex(const Bone* bone, size_t &indexOut) const
    {
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            if (this->bones[iBone] == bone)
            {
                indexOut = iBone;
                return true;
            }
        }

        return false;
    }

    bool ModelDefinition::FindBoneIndex(const char* boneName, size_t &indexOut) const
    {
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones[iBone];
            assert(bone != nullptr);
            {
                if (GTCore::Strings::Equal(bone->GetName(), boneName))
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
        return this->bones.count;
    }



    ////////////////////////////////////////////////////////
    // Serialization/Deserialization

    void ModelDefinition::Serialize(GTCore::Serializer &serializer) const
    {
        GTCore::BasicSerializer intermediarySerializer;


        /////////////////////////////////////
        // Bones

        intermediarySerializer.Write(static_cast<uint32_t>(this->bones.count));
        
        // Local information.
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones[iBone];
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
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones[iBone];
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
                assert(vertexArray->GetFormat().GetSize() == VertexFormat::P3T2N3T3B3.GetSize());
                {
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
                }
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
        intermediarySerializer.Write(static_cast<uint32_t>(this->animation.GetKeyFrameCount()));

        for (uint32_t iKeyFrame = 0; iKeyFrame < this->animation.GetKeyFrameCount(); ++iKeyFrame)
        {
            // Time.
            intermediarySerializer.Write(static_cast<float>(this->animation.GetKeyFrameTimeByIndex(iKeyFrame)));
            
            
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

        header.id          = Serialization::ChunkID_Model_Animation;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);



        /////////////////////////////////////
        // Animation Segments

        intermediarySerializer.Clear();
        intermediarySerializer.Write(static_cast<uint32_t>(this->animation.GetNamedSegmentCount()));

        for (size_t iSegment = 0; iSegment < this->animation.GetNamedSegmentCount(); ++iSegment)
        {
            auto segment = this->animation.GetNamedSegmentByIndex(iSegment);
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
        intermediarySerializer.Write(static_cast<uint32_t>(this->convexHulls.count));

        GTCore::Vector<uint32_t> vertexCounts(this->convexHulls.count);
        GTCore::Vector<uint32_t> indexCounts( this->convexHulls.count);
        GTCore::Vector<float>    vertices;
        GTCore::Vector<uint32_t> indices;

        for (size_t iConvexHull = 0; iConvexHull < this->convexHulls.count; ++iConvexHull)
        {
            auto convexHull = this->convexHulls[iConvexHull];
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

    bool ModelDefinition::Deserialize(GTCore::Deserializer &deserializer)
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
                            GTCore::String name;
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
                            auto &channel = this->animation.CreateChannel();
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
                                this->bones[parentIndex]->AttachChild(*this->bones[iBone]);
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
                            GTCore::String materialName;
                            deserializer.ReadString(materialName);

                            newMesh.material = MaterialLibrary::Create(materialName.c_str());



                            // Geometry
                            newMesh.geometry = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3T3B3);
                            
                            // Vertices.
                            uint32_t vertexCount;
                            deserializer.Read(vertexCount);

                            if (vertexCount > 0)
                            {
                                newMesh.geometry->SetVertexData(nullptr, static_cast<size_t>(vertexCount));
                                auto vertexData = newMesh.geometry->MapVertexData();
                                {
                                    deserializer.Read(vertexData, vertexCount * VertexFormat::P3T2N3T3B3.GetSizeInBytes());
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

                            size_t keyFrameIndex = this->animation.AppendKeyFrame(static_cast<double>(time));


                            // With the key frame added, we now need to iterate over each channel in the key frame.
                            uint32_t channelCount;
                            deserializer.Read(channelCount);

                            for (uint32_t iChannel = 0; iChannel < channelCount; ++iChannel)
                            {
                                uint32_t boneIndex;
                                deserializer.Read(boneIndex);

                                auto bone = this->bones[boneIndex];
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
                            GTCore::String name;
                            uint32_t startKeyFrame;
                            uint32_t endKeyFrame;

                            deserializer.ReadString(name);
                            deserializer.Read(startKeyFrame);
                            deserializer.Read(endKeyFrame);

                            this->animation.AddNamedSegment(name.c_str(), static_cast<size_t>(startKeyFrame), static_cast<size_t>(endKeyFrame));
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

                            this->convexHulls.PushBack(new ConvexHull(currentVertices, vertexCount, currentIndices, indexCount));

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

    bool ModelDefinition::LoadFromNativeFile(const GTCore::String &absolutePathIn)
    {
        // When loading from a native file, all we need to do is deserialize.
        bool successful = false;

        auto file = GTCore::IO::Open(absolutePathIn.c_str(), GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            GTCore::FileDeserializer deserializer(file);
            successful = this->Deserialize(deserializer);

            GTCore::IO::Close(file);
        }

        return successful;
    }

    bool ModelDefinition::LoadFromForeignFile(const GTCore::String &absolutePathIn)
    {
        // Currently, all foreign formats are loaded via Assimp.
        return this->LoadFromAssimpFile(absolutePathIn);
    }
}
