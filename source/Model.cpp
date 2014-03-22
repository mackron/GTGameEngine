// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>
#include <GTEngine/Math.hpp>
#include <GTEngine/Logging.hpp>
#include <GTLib/Timing.hpp>
#include <cfloat>

#undef min
#undef max

namespace GTEngine
{
    static ModelDefinition NullModelDefinition;

    Model::Model()
        : definition(NullModelDefinition),
          meshes(), bones(),
          aabbMin(), aabbMax(), isAABBValid(false),
          animation(), animationChannelBones(), animationKeyCache(),
          animationPlaybackSpeed(1.0)
    {
    }

    Model::Model(const ModelDefinition &definitionIn)
        : definition(definitionIn),
          meshes(), bones(),
          aabbMin(), aabbMax(), isAABBValid(false),
          animation(), animationChannelBones(), animationKeyCache(),
          animationPlaybackSpeed(1.0)
    {
        // This will get the model into the correct state.
        this->OnDefinitionChanged();
    }

    Model::~Model()
    {
        this->Clear();
    }


    Mesh* Model::AttachMesh(VertexArray* geometry, const char* materialFileName, DrawMode drawMode)
    {
        auto newMesh = new Mesh(drawMode);
        newMesh->SetGeometry(geometry);
        newMesh->SetMaterial(materialFileName);

        this->meshes.PushBack(newMesh);

        // The AABB is no longer valid.
        this->isAABBValid = false;

        return newMesh;
    }

    Mesh* Model::AttachMesh(VertexArray* geometryIn, const char* materialFileName, const SkinningVertexAttribute* skinningVertexAttributes)
    {
        auto newMesh = this->AttachMesh(geometryIn, materialFileName);
        if (newMesh != nullptr)
        {
            newMesh->SetSkinningData(this->bones.buffer, skinningVertexAttributes);
        }

        return newMesh;
    }


    void Model::CopyAndAttachBones(const GTLib::Vector<Bone*> &inputBones)
    {
        // We do this in two passes. The first pass makes copies but does not link with parents. The second pass will link the bones together.
        for (size_t i = 0; i < inputBones.count; ++i)
        {
            auto bone = inputBones[i];
            assert(bone != nullptr);

            this->bones.PushBack(new Bone(*bone));
        }

        // This is the second pass. We need to link the bones together to form their hierarchy.
        for (size_t i = 0; i < inputBones.count; ++i)
        {
            auto inputBone = inputBones[i];
            assert(inputBone != nullptr);

            if (inputBone->GetParent() != nullptr)
            {
                auto bone = this->GetBoneByName(inputBone->GetName());
                assert(bone != nullptr);

                auto parentBone = this->GetBoneByName(inputBone->GetParent()->GetName());
                assert(parentBone != nullptr);

                parentBone->AttachChild(*bone);
            }
        }
    }

    void Model::CopyAnimation(const Animation &sourceAnimation, const GTLib::Map<Bone*, AnimationChannel*> &sourceAnimationChannelBones)
    {
        // We first need to create all of the key frames.
        for (size_t iKeyFrame = 0; iKeyFrame < sourceAnimation.GetKeyFrameCount(); ++iKeyFrame)
        {
            this->animation.AppendKeyFrame(sourceAnimation.GetKeyFrameTimeByIndex(iKeyFrame));
        }


        // Here we add all of our channels to the animation. They will be empty to begin with, but filled below.
        for (size_t iChannel = 0; iChannel < sourceAnimationChannelBones.count; ++iChannel)
        {
            auto sourceChannel = sourceAnimationChannelBones.buffer[iChannel]->value;
            auto sourceBone    = sourceAnimationChannelBones.buffer[iChannel]->key;

            auto bone = this->GetBoneByName(sourceBone->GetName());
            assert(bone != nullptr);

            auto &newChannel = this->animation.CreateChannel();
            this->animationChannelBones.Add(bone, &newChannel);

            // Now we loop through all the keys and add them.
            auto &sourceKeys = sourceChannel->GetKeys();
            for (size_t iKey = 0; iKey < sourceKeys.count; ++iKey)
            {
                auto sourceKey = static_cast<TransformAnimationKey*>(sourceKeys.buffer[iKey]->value);

                auto newKey = new TransformAnimationKey(sourceKey->position, sourceKey->rotation, sourceKey->scale);
                this->animationKeyCache.PushBack(newKey);

                newChannel.SetKey(sourceKeys.buffer[iKey]->key, newKey);
            }
        }


        // Here we add the named segments.
        for (size_t iSegment = 0; iSegment < sourceAnimation.GetNamedSegmentCount(); ++iSegment)
        {
            auto name    = sourceAnimation.GetNamedSegmentNameByIndex(iSegment);
            auto segment = sourceAnimation.GetNamedSegmentByIndex(iSegment);

            assert(name    != nullptr);
            assert(segment != nullptr);

            this->animation.AddNamedSegment(name, segment->startKeyFrame, segment->endKeyFrame);
        }
    }


    void Model::ApplyTransformation(const glm::mat4 &transform)
    {
        // We're going to use a CPU vertex shader here.
        CPUVertexShader_SimpleTransform shader(transform);
        
        // We need to execute the shader on all meshes.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            auto mesh = this->meshes[i];
            assert(mesh != nullptr);

            auto geometry = mesh->GetGeometry();

            auto &format      = geometry->GetFormat();
            auto  vertexCount = geometry->GetVertexCount();
            auto  vertexData  = geometry->MapVertexData();

            shader.Execute(vertexData, vertexCount, format, vertexData);

            geometry->UnmapVertexData();
        }


        this->GenerateTangentsAndBitangents();


        // The AABB is no longer valid.
        this->isAABBValid = false;
    }

    void Model::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            this->meshes[i]->GenerateTangentsAndBitangents();
        }
    }

    void Model::GetAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
    {
        // We may need to update the AABB.
        if (!this->isAABBValid)
        {
            if (this->meshes.count > 0)
            {
                // We need to iterate over the positions of every vertex of every mesh and find the bounds.
                this->aabbMin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
                this->aabbMax = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

                for (size_t i = 0; i < this->meshes.count; ++i)
                {
                    glm::vec3 vaMin;
                    glm::vec3 vaMax;
                    this->meshes[i]->GetGeometry()->CalculateAABB(vaMin, vaMax);

                    if (vaMin.x < this->aabbMin.x) this->aabbMin.x = vaMin.x;
                    if (vaMin.y < this->aabbMin.y) this->aabbMin.y = vaMin.y;
                    if (vaMin.z < this->aabbMin.z) this->aabbMin.z = vaMin.z;

                    if (vaMax.x > this->aabbMax.x) this->aabbMax.x = vaMax.x;
                    if (vaMax.y > this->aabbMax.y) this->aabbMax.y = vaMax.y;
                    if (vaMax.z > this->aabbMax.z) this->aabbMax.z = vaMax.z;
                }

                this->isAABBValid = true;
            }

            if (this->aabbMax.x - this->aabbMin.x < glm::epsilon<float>()) this->aabbMax.x = this->aabbMin.x + glm::epsilon<float>();
            if (this->aabbMax.y - this->aabbMin.y < glm::epsilon<float>()) this->aabbMax.y = this->aabbMin.y + glm::epsilon<float>();
            if (this->aabbMax.z - this->aabbMin.z < glm::epsilon<float>()) this->aabbMax.z = this->aabbMin.z + glm::epsilon<float>();
        }

        aabbMin = this->aabbMin;
        aabbMax = this->aabbMax;
    }

    void Model::SetAABB(const glm::vec3 &aabbMinIn, const glm::vec3 &aabbMaxIn)
    {
        this->aabbMin = aabbMinIn;
        this->aabbMax = aabbMaxIn;

        this->isAABBValid = true;
    }



    void Model::OnDefinitionChanged()
    {
        this->Clear();


        // We need to create copies of the bones. It is important that this is done before adding the meshes.
        this->CopyAndAttachBones(this->definition.bones);

        // Now the animation.
        this->CopyAnimation(this->definition.animation, this->definition.animationChannelBones);
        
        // Now we need to create the meshes. This must be done after adding the bones.
        for (size_t i = 0; i < this->definition.meshes.count; ++i)
        {
            auto &definitionMesh = this->definition.meshes[i];
            Mesh* newMesh = nullptr;

            if (definitionMesh.skinningVertexAttributes != nullptr)
            {
                newMesh = this->AttachMesh(definitionMesh.geometry, definitionMesh.material->GetDefinition().relativePath.c_str(), definitionMesh.skinningVertexAttributes);
            }
            else
            {
                newMesh = this->AttachMesh(definitionMesh.geometry, definitionMesh.material->GetDefinition().relativePath.c_str());
            }

            // Material parameters.
            if (newMesh != nullptr)
            {
                auto newMeshMaterial = newMesh->GetMaterial();
                if (newMeshMaterial != nullptr)
                {
                    if (definitionMesh.material != nullptr)
                    {
                        newMeshMaterial->SetParameters(definitionMesh.material->GetParameters());
                    }

                    newMeshMaterial->SetParameters(definitionMesh.defaultUniforms);
                }
            }
        }
    }



    void Model::Serialize(GTLib::Serializer &serializer) const
    {
        // A model has a fairly complex set of properties. Geometry, materials, bones, animation state, etc. We're going to have a null chunk
        // at the end so we can do an iteration-based deserializer.

        // The first chunk contains the mesh data. We save the mesh data differently depending on whether or not the model is procedural. If
        // it is, we need to save the geometry data.
        GTLib::BasicSerializer meshesSerializer;

        bool serializeMeshGeometry = &this->definition == &NullModelDefinition;
        
        meshesSerializer.Write(static_cast<uint32_t>(meshes.count));
        for (size_t i = 0; i < meshes.count; ++i)
        {
            auto mesh = this->meshes[i];
            assert(mesh != nullptr);
            {
                mesh->Serialize(meshesSerializer, serializeMeshGeometry);

                // We need to write a boolean that specifies whether or not the material is the same as that specified by the definition.
                bool usingSameMaterial = false;
                if (&this->definition != &NullModelDefinition && this->definition.meshes[i].material != nullptr && mesh->GetMaterial() != nullptr)
                {
                    auto &definitionA = this->definition.meshes[i].material->GetDefinition();
                    auto &definitionB = mesh->GetMaterial()->GetDefinition();

                    usingSameMaterial = definitionA.relativePath == definitionB.relativePath;
                }

                meshesSerializer.Write(usingSameMaterial);
            }
        }

        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_Model_Meshes;
        header.version     = 1;
        header.sizeInBytes = meshesSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(meshesSerializer.GetBuffer(), header.sizeInBytes);



        // Now bones. We'll only write this chunk if we actually have bones.
        if (this->bones.count > 0)
        {
            GTLib::BasicSerializer bonesSerializer;
            
            bonesSerializer.Write(static_cast<uint32_t>(bones.count));
            for (size_t i = 0; i < this->bones.count; ++i)
            {
                auto bone = this->bones[i];
                assert(bone != nullptr);
                {
                    bone->Serialize(bonesSerializer);
                }
            }


            header.id          = Serialization::ChunkID_Model_Bones;
            header.version     = 1;
            header.sizeInBytes = bonesSerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(bonesSerializer.GetBuffer(), header.sizeInBytes);
        }

        
        // Finally, the animation. We only write this chunk if we actually have animation key frames.
        if (this->animation.GetKeyFrameCount() > 0)
        {
            GTLib::BasicSerializer animationSerializer;

            this->animation.Serialize(animationSerializer);
            animationSerializer.Write(this->animationPlaybackSpeed);


            header.id      = Serialization::ChunkID_Model_Animation;
            header.version = 1;
            header.sizeInBytes = animationSerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(animationSerializer.GetBuffer(), header.sizeInBytes);
        }



        // Finally, the null terminator.
        header.id          = Serialization::ChunkID_Null;
        header.version     = 1;
        header.sizeInBytes = 0;
        serializer.Write(header);
    }

    void Model::Deserialize(GTLib::Deserializer &deserializer)
    {
        // A model is tied to a definition. When a model is saved, it will be saved based on that definition. If the definition has changed
        // it will mean the serialized data isn't really valid. Thus, if the definition is any different, we going to skip everything and
        // just load up the model based on the definition.
        bool isAbandoned = false;


        // We're going to us a iteration based deserialization system here. Basically, we keep reading chunks until we hit the null terminator.
        Serialization::ChunkHeader header;

        do
        {
            deserializer.Read(header);

            switch (header.id)
            {
            case Serialization::ChunkID_Model_Meshes:
                {
                    if (isAbandoned)
                    {
                        deserializer.Seek(header.sizeInBytes);
                        break;
                    }

                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t meshCount;
                            deserializer.Read(meshCount);

                            // We need to check if it's still possible to deserialize. If the mesh count is different, we can't reliably load anything, so
                            // we'll need to abandon everything. Note that we don't immediately return, because we want to maintain the integrity of the
                            // deserializer.
                            if (this->meshes.count != meshCount && this->meshes.count != 0)
                            {
                                isAbandoned = true;
                                this->OnDefinitionChanged();

                                deserializer.Seek(header.sizeInBytes - sizeof(meshCount));
                                break;
                            }


                            // Keeps track of whether or not we are allocating new meshes.
                            bool allocateNewMeshes = false;

                            // If we don't have any meshes, it probably means we're deserializing from an empty model. We'll create the meshes here.
                            if (this->meshes.count == 0)
                            {
                                allocateNewMeshes = true;
                            }
                            else
                            {
                                assert(this->meshes.count == meshCount);

                                // We don't want to allocate new meshes.
                                allocateNewMeshes = false;
                            }


                            for (uint32_t iMesh = 0; iMesh < meshCount; ++iMesh)
                            {
                                Mesh* mesh = nullptr;

                                if (allocateNewMeshes)
                                {
                                    mesh = this->AttachMesh(nullptr, nullptr, nullptr);
                                }
                                else
                                {
                                    mesh = this->meshes[iMesh];
                                }


                                assert(mesh != nullptr);
                                {
                                    mesh->Deserialize(deserializer);

                                    // The next byte should be a boolean specifying whether or not we are using the same material as that defined by the definition.
                                    bool usingSameMaterial;
                                    deserializer.Read(usingSameMaterial);

                                    if (usingSameMaterial)
                                    {
                                        auto newMaterial = this->definition.meshes[iMesh].material;
                                        if (newMaterial != nullptr)
                                        {
                                            mesh->SetMaterial(newMaterial->GetDefinition().relativePath.c_str());
                                            mesh->GetMaterial()->SetParameters(this->definition.meshes[iMesh].defaultUniforms);
                                        }
                                    }
                                }
                            }

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing model. Meshes chunk has an unsupported version (%d).", header.version);
                            break;
                        }
                    }

                    break;
                }


            case Serialization::ChunkID_Model_Bones:
                {
                    if (isAbandoned)
                    {
                        deserializer.Seek(header.sizeInBytes);
                        break;
                    }

                    switch (header.version)
                    {
                    case 1:
                        {
                            uint32_t boneCount;
                            deserializer.Read(boneCount);

                            if (this->bones.count == boneCount)
                            {
                                for (size_t iBone = 0; iBone < this->bones.count; ++iBone)
                                {
                                    this->bones[iBone]->Deserialize(deserializer);
                                }
                            }
                            else
                            {
                                isAbandoned = true;
                                this->OnDefinitionChanged();

                                deserializer.Seek(header.sizeInBytes - sizeof(boneCount));
                                break;
                            }


                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing model. Bones chunk has an unsupported version (%d).", header.version);
                            break;
                        }
                    }

                    break;
                }

            case Serialization::ChunkID_Model_Animation:
                {
                    if (isAbandoned)
                    {
                        deserializer.Seek(header.sizeInBytes);
                        break;
                    }

                    switch (header.version)
                    {
                    case 1:
                        {
                            this->animation.Deserialize(deserializer);
                            deserializer.Read(this->animationPlaybackSpeed);

                            break;
                        }

                    default:
                        {
                            GTEngine::Log("Error deserializing model. Animation chunk has an unsupported version (%d).", header.version);
                            break;
                        }
                    }

                    break;
                }

            default:
                {
                    // We don't know about the chunk so we'll just skip it.
                    deserializer.Seek(header.sizeInBytes);
                    break;
                }
            }

        } while (header.id != Serialization::ChunkID_Null);


        // The AABB is no longer valid after deserialization.
        this->isAABBValid = false;
    }


   
    ///////////////////////////////////////////////////////////////////
    // Animation.

    void Model::PlayAnimation(const AnimationSequence &sequence)
    {
        this->animation.Play(sequence);
    }

    void Model::StopAnimation()
    {
        this->animation.Stop();
    }

    void Model::PauseAnimation()
    {
        this->animation.Pause();
    }

    void Model::ResumeAnimation()
    {
        this->animation.Resume();
    }

    void Model::StepAnimation(double step)
    {
        this->animation.Step(step * this->animationPlaybackSpeed);

        glm::vec3 boneAABBMin = glm::vec3( FLT_MAX);
        glm::vec3 boneAABBMax = glm::vec3(-FLT_MAX);

        // Now that we've stepped the animation, we need to update the bone positions.
        size_t startKeyFrame;
        size_t endKeyFrame;
        auto interpolationFactor = this->animation.GetKeyFramesAtCurrentPlayback(startKeyFrame, endKeyFrame);

        for (size_t i = 0; i < this->animationChannelBones.count; ++i)
        {
            auto iChannelBone = this->animationChannelBones.buffer[i];

            auto firstKey = static_cast<TransformAnimationKey*>(iChannelBone->value->GetKey(startKeyFrame));
            auto endKey   = static_cast<TransformAnimationKey*>(iChannelBone->value->GetKey(endKeyFrame));
            auto bone     = iChannelBone->key;

            if (firstKey != nullptr && endKey != nullptr)
            {
                glm::vec3 position = glm::mix(  firstKey->position, endKey->position, interpolationFactor);
                glm::quat rotation = glm::slerp(firstKey->rotation, endKey->rotation, interpolationFactor);
                glm::vec3 scale    = glm::mix(  firstKey->scale,    endKey->scale,    interpolationFactor);

                bone->SetPosition(position);
                bone->SetRotation(rotation);
                bone->SetScale(scale);
            }
        }

        // Now we need to loop over each channel again and update the skinning transformations. It's important that this is done separately from the
        // loop above to ensure all dependants have been updated beforehand.
        for (size_t i = 0; i < this->animationChannelBones.count; ++i)
        {
            glm::vec3 position;
            glm::quat devnull0;
            glm::vec3 devnull1;
            this->animationChannelBones.buffer[i]->key->UpdateSkinningTransform(position, devnull0, devnull1);

            boneAABBMin = glm::min(boneAABBMin, position);
            boneAABBMax = glm::max(boneAABBMax, position);
        }


        // The AABB needs to be set, but with bone AABB padding applied.
        boneAABBMin -= this->definition.GetAnimationAABBPadding();
        boneAABBMax += this->definition.GetAnimationAABBPadding();
        this->SetAABB(boneAABBMin, boneAABBMax);
    }

    bool Model::IsAnimating() const
    {
        return this->animation.IsPlaying();
    }

    bool Model::IsAnimationPaused() const
    {
        return this->animation.IsPaused();
    }

    void Model::SetAnimationPlaybackSpeed(double speed)
    {
        this->animationPlaybackSpeed = speed;
    }


    Bone* Model::GetBoneByName(const char* name, size_t* indexOut)
    {
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            if (GTLib::Strings::Equal(name, this->bones[i]->GetName()))
            {
                if (indexOut != nullptr)
                {
                    *indexOut = i;
                }

                return this->bones[i];
            }
        }

        return nullptr;
    }

    Bone* Model::GetBoneByIndex(size_t boneIndex)
    {
        return this->bones[boneIndex];
    }
}


// Private
namespace GTEngine
{
    void Model::Clear()
    {
        // Meshes.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            delete this->meshes[i];
        }
        this->meshes.Clear();

        // Bones
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i];
        }
        this->bones.Clear();

        // Animation keys.
        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
        this->animationKeyCache.Clear();
        this->animationChannelBones.Clear();


        // The AABB is no longer valid.
        this->isAABBValid = false;
    }
}


