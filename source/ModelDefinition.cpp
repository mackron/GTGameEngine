// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/IO.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    ModelDefinition::ModelDefinition(const char* fileNameIn)
        : fileName(fileNameIn), absolutePath(),
          meshGeometries(), meshMaterials(), meshSkinningVertexAttributes(),
          bones(),
          animation(), animationChannelBones(), animationKeyCache(),
          convexHulls(), convexHullBuildSettings()
    {
        if (this->fileName != "")
        {
            GTCore::IO::FindAbsolutePath(fileNameIn, this->absolutePath);
        }
    }

    ModelDefinition::~ModelDefinition()
    {
        this->ClearMeshGeometries();
        this->ClearMeshSkinningVertexAttributes();
        this->ClearBones();
        this->ClearAnimations();
        this->ClearMaterials();
        this->ClearConvexHulls();
    }


    bool ModelDefinition::LoadFromFile(const char* fileNameIn, const char* relativePathIn)
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


        if (IO::IsSupportedModelExtension(newAbsolutePath.c_str()))
        {
            if (!GTCore::IO::FileExists(newAbsolutePath.c_str()))
            {
                if (GTCore::Path::ExtensionEqual(newAbsolutePath.c_str(), "gtmodel"))
                {
                    newAbsolutePath = GTCore::IO::RemoveExtension(newAbsolutePath.c_str());
                    newRelativePath = GTCore::IO::RemoveExtension(newRelativePath.c_str());

                    if (!GTCore::IO::FileExists(newAbsolutePath.c_str()))
                    {
                        return false;
                    }
                }
                else
                {
                    newAbsolutePath += ".gtmodel";
                    newRelativePath += ".gtmodel";

                    if (!GTCore::IO::FileExists(newAbsolutePath.c_str()))
                    {
                        return false;
                    }
                }
            }


            // If we've made it here, it means the file exists and is a supported model file. All we need to do is check the extension to determine
            // whether or not we are loading from a native or foreign format.
            bool successful = false;
            if (GTCore::Path::ExtensionEqual(newAbsolutePath.c_str(), "gtmodel"))
            {
                successful = this->LoadFromNativeFile(newAbsolutePath);
            }
            else
            {
                successful = this->LoadFromForeignFile(newAbsolutePath);
            }


            if (successful)
            {
                this->absolutePath = newAbsolutePath;
                this->relativePath = newRelativePath;

                return true;
            }
        }

        return false;
    }


    void ModelDefinition::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            this->meshGeometries[i]->GenerateTangentsAndBitangents();
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
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            auto va = this->meshGeometries[i];
            if (va != nullptr)
            {
                ConvexHull* convexHulls;
                size_t      count;
                ConvexHull::BuildConvexHulls(*va, convexHulls, count, settings);

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


    void ModelDefinition::ClearMeshGeometries()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            // It's important that we garbage collect here.
            Renderer::DeleteVertexArray(this->meshGeometries[i]);
        }
        this->meshGeometries.Clear();
    }

    void ModelDefinition::ClearMeshSkinningVertexAttributes()
    {
        for (size_t i = 0; i < this->meshSkinningVertexAttributes.count; ++i)
        {
            delete [] this->meshSkinningVertexAttributes[i];
        }
        this->meshSkinningVertexAttributes.Clear();
    }

    void ModelDefinition::ClearBones()
    {
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i]->value;
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

    void ModelDefinition::ClearMaterials()
    {
        for (size_t i = 0; i < this->meshMaterials.count; ++i)
        {
            MaterialLibrary::Delete(this->meshMaterials[i]);
        }
        this->meshMaterials.Clear();
    }

    void ModelDefinition::ClearConvexHulls()
    {
        for (size_t i = 0; i < this->convexHulls.count; ++i)
        {
            delete this->convexHulls[i];
        }
        this->convexHulls.Clear();
    }


    ////////////////////////////////////////////////////////
    // Serialization/Deserialization

    void ModelDefinition::Serialize(GTCore::Serializer &serializer)
    {
        GTCore::BasicSerializer intermediarySerializer;


        /////////////////////////////////////
        // Bones

        intermediarySerializer.Write(static_cast<uint32_t>(this->bones.count));
        
        // Local information.
        for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
        {
            auto bone = this->bones.buffer[iBone]->value;
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
            auto bone = this->bones.buffer[iBone]->value;
            assert(bone != nullptr);
            {
                auto parentBone = bone->GetParent();
                if (parentBone != nullptr)
                {
                    auto iParentBone = this->bones.Find(parentBone->GetName());
                    assert(iParentBone != nullptr);

                    intermediarySerializer.Write(static_cast<uint32_t>(iParentBone->index));
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
        intermediarySerializer.Write(static_cast<uint32_t>(this->meshGeometries.count));

        for (size_t iMesh = 0; iMesh < this->meshGeometries.count; ++iMesh)
        {
            // Material.
            intermediarySerializer.WriteString(this->meshMaterials[iMesh]->GetDefinition().relativePath);


            // Vertices.
            auto vertexArray = this->meshGeometries[iMesh];
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
                    if (this->meshSkinningVertexAttributes[iMesh] != nullptr)
                    {
                        intermediarySerializer.Write(static_cast<uint32_t>(vertexArray->GetVertexCount()));

                        if (vertexArray->GetVertexCount() > 0)
                        {
                            uint32_t totalBoneWeights = 0;


                            // Bone Counts.
                            for (size_t iVertex = 0; iVertex < vertexArray->GetVertexCount(); ++iVertex)
                            {
                                uint16_t count = static_cast<uint16_t>(this->meshSkinningVertexAttributes[iMesh][iVertex].bones.count);
                                intermediarySerializer.Write(count);

                                totalBoneWeights += count;
                            }

                            
                            // Bone/Weight Pairs.
                            intermediarySerializer.Write(totalBoneWeights);

                            for (size_t iVertex = 0; iVertex < vertexArray->GetVertexCount(); ++iVertex)
                            {
                                auto &bones = this->meshSkinningVertexAttributes[iMesh][iVertex].bones;
                                {
                                    for (size_t iBone = 0; iBone < bones.count; ++iBone)
                                    {
                                        auto &bone = bones[iBone];

                                        serializer.Write(static_cast<uint32_t>(bone.boneIndex));
                                        serializer.Write(static_cast<float   >(bone.weight));
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
                    auto iBone = this->bones.Find(bone->GetName());
                    assert(iBone != nullptr);
                    {
                        intermediarySerializer.Write(static_cast<uint32_t>(iBone->index));
                    }


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
        this->ClearMeshGeometries();
        this->ClearMeshSkinningVertexAttributes();
        this->ClearMaterials();
        this->ClearBones();
        this->ClearAnimations();
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
                            this->bones.Add(name.c_str(), bone);


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
                                this->bones.buffer[parentIndex]->value->AttachChild(*this->bones.buffer[iBone]->value);
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
                            // Material
                            GTCore::String materialName;
                            deserializer.ReadString(materialName);

                            this->meshMaterials.PushBack(MaterialLibrary::Create(materialName.c_str()));



                            // Geometry
                            auto vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3T3B3);
                            
                            // Vertices.
                            uint32_t vertexCount;
                            deserializer.Read(vertexCount);

                            if (vertexCount > 0)
                            {
                                vertexArray->SetVertexData(nullptr, static_cast<size_t>(vertexCount));
                                auto vertexData = vertexArray->MapVertexData();
                                {
                                    deserializer.Read(vertexData, vertexCount * VertexFormat::P3T2N3T3B3.GetSizeInBytes());
                                }
                                vertexArray->UnmapVertexData();
                            }

                            // Indices.
                            uint32_t indexCount;
                            deserializer.Read(indexCount);

                            if (indexCount > 0)
                            {
                                vertexArray->SetIndexData(nullptr, static_cast<size_t>(indexCount));
                                auto indexData = vertexArray->MapIndexData();
                                {
                                    deserializer.Read(indexData, indexCount * sizeof(uint32_t));
                                }
                                vertexArray->UnmapIndexData();
                            }



                            // Skinning Vertex Attributes
                            uint32_t skinningVertexAttributeCount;
                            deserializer.Read(skinningVertexAttributeCount);

                            if (skinningVertexAttributeCount > 0)
                            {
                                auto skinningVertexAttributes = new SkinningVertexAttribute[skinningVertexAttributeCount];

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
                                    skinningVertexAttributes[iVertex].bones.Reserve(count);
                                    skinningVertexAttributes[iVertex].bones.count = count;

                                    for (uint16_t iBone = 0; iBone < count; ++iBone)
                                    {
                                        skinningVertexAttributes[iVertex].bones[iBone] = *currentBoneWeight++;
                                    }
                                }

                                this->meshSkinningVertexAttributes.PushBack(skinningVertexAttributes);


                                free(counts);
                                free(boneWeights);
                            }
                            else
                            {
                                this->meshSkinningVertexAttributes.PushBack(nullptr);
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

                                auto bone = this->bones.buffer[boneIndex]->value;
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

            case Serialization::ChunkID_Model_ConvexHulls:
                {
                    deserializer.Seek(sizeof(header));

                    if (header.version == 1)
                    {
                        uint32_t convexHullCount;
                        deserializer.Read(convexHullCount);

                        uint32_t* vertexCounts = static_cast<uint32_t*>(malloc(convexHullCount * sizeof(uint32_t)));
                        uint32_t* indexCounts  = static_cast<uint32_t*>(malloc(convexHullCount * sizeof(uint32_t)));

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
        auto file = GTCore::IO::Open(absolutePathIn.c_str(), GTCore::IO::OpenMode::Binary | GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            GTCore::FileDeserializer deserializer(file);
            if (this->Deserialize(deserializer))
            {
                return true;
            }
        }

        return false;
    }

    bool ModelDefinition::LoadFromForeignFile(const GTCore::String &absolutePathIn)
    {
        // Currently, all foreign formats are loaded via Assimp.
        return this->LoadFromAssimpFile(absolutePathIn);
    }
}
