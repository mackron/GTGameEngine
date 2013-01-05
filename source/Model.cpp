
#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>
#include <GTEngine/Math.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Timing.hpp>
#include <cfloat>

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


    void Model::CopyAndAttachBones(const GTCore::Dictionary<Bone*> &inputBones)
    {
        // We do this in two passes. The first pass makes copies but does not link with parents. The second pass will link the bones together.
        for (size_t i = 0; i < inputBones.count; ++i)
        {
            auto bone = inputBones.buffer[i]->value;
            assert(bone != nullptr);

            this->bones.PushBack(new Bone(*bone));
        }

        // This is the second pass. We need to link the bones together to form their hierarchy.
        for (size_t i = 0; i < inputBones.count; ++i)
        {
            auto inputBone = inputBones.buffer[i]->value;
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

    void Model::CopyAnimation(const Animation &sourceAnimation, const GTCore::Map<Bone*, AnimationChannel*> &sourceAnimationChannelBones)
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
    }

    void Model::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            this->meshes[i]->GenerateTangentsAndBitangents();
        }
    }

    void Model::GetBaseAABB(glm::vec3 &aabbMin, glm::vec3 &aabbMax) const
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



    void Model::OnDefinitionChanged()
    {
        this->Clear();


        // We need to create copies of the bones. It is important that this is done before adding the meshes.
        this->CopyAndAttachBones(this->definition.bones);

        // Now the animation.
        this->CopyAnimation(this->definition.animation, this->definition.animationChannelBones);
        
        // Now we need to create the meshes. This must be done after adding the bones.
        for (size_t i = 0; i < this->definition.meshGeometries.count; ++i)
        {
            if (this->definition.meshSkinningVertexAttributes[i] != nullptr)
            {
                this->AttachMesh(this->definition.meshGeometries[i], this->definition.meshMaterials[i]->GetDefinition().fileName.c_str(), this->definition.meshSkinningVertexAttributes[i]);
            }
            else
            {
                this->AttachMesh(this->definition.meshGeometries[i], this->definition.meshMaterials[i]->GetDefinition().fileName.c_str());
            }
        }
    }



    void Model::Serialize(GTCore::Serializer &serializer) const
    {
        // A version just in case we need to change a few things, which is probable.
        serializer.Write(static_cast<uint32_t>(1));


        // A model will be serialized differently depending on whether or not it was loaded from a file. When loaded from a file, we
        // will only save basic state information. Otherwise, if the model is procedural, we'll want to save everything.
        if (&this->definition == &NullModelDefinition)
        {
            serializer.Write(true);

            // Meshes.
            serializer.Write(static_cast<uint32_t>(this->meshes.count));
            for (size_t i = 0; i < this->meshes.count; ++i)
            {
                this->meshes[i]->Serialize(serializer, true);       // 'true' means to write the geometry data.
            }
        }
        else
        {
            serializer.Write(false);

            // Meshes.
            serializer.Write(static_cast<uint32_t>(this->meshes.count));
            for (size_t i = 0; i < this->meshes.count; ++i)
            {
                this->meshes[i]->Serialize(serializer, false);       // 'false' means to not save the geometry data.
            }

            // Bones.
            serializer.Write(static_cast<uint32_t>(this->bones.count));
            for (size_t i = 0; i < this->bones.count; ++i)
            {
                this->bones[i]->Serialize(serializer);
            }

            // Animation.
            this->animation.Serialize(serializer);
            serializer.Write(this->animationPlaybackSpeed);
        }
    }

    void Model::Deserialize(GTCore::Deserializer &deserializer)
    {
        // A version just in case we need to change a few things, which is probable.
        uint32_t version;
        deserializer.Read(version);


        bool isProcedural;
        deserializer.Read(isProcedural);


        uint32_t meshCount;
        deserializer.Read(meshCount);

        // We now have to pass a few assertions. First of all, if 'isProcedural' is true, it must also be try that this->definition == NullModelDefinition. If it is
        // not procedural, we must assert that the mesh counts are the same.
        if (isProcedural)
        {
            assert(&this->definition == &NullModelDefinition);
            {
                // If the mesh counts are different, it might just mean that the model is empty. This is a valid state. In this case, we'll just re-create the meshes.
                if (meshCount != this->meshes.count)
                {
                    this->Clear();

                    for (size_t i = 0; i < meshCount; ++i)
                    {
                        auto mesh = this->AttachMesh(nullptr, nullptr, nullptr);
                        mesh->Deserialize(deserializer);
                    }
                }
                else
                {
                    for (size_t i = 0; i < meshCount; ++i)
                    {
                        this->meshes[i]->Deserialize(deserializer);
                    }
                }
            }
        }
        else
        {
            assert(meshCount == this->meshes.count);
            assert(!this->definition.fileName.IsEmpty());
            {
                for (uint32_t i = 0; i < meshCount; ++i)
                {
                    this->meshes[i]->Deserialize(deserializer);
                }
            }


            // Bones.
            uint32_t boneCount;
            deserializer.Read(boneCount);

            assert(boneCount == this->bones.count);
            {
                for (uint32_t i = 0; i < boneCount; ++i)
                {
                    this->bones[i]->Deserialize(deserializer);
                }
            }


            // Animation.
            this->animation.Deserialize(deserializer);
            deserializer.Read(this->animationPlaybackSpeed);
        }
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
            this->animationChannelBones.buffer[i]->key->UpdateSkinningTransform();
        }
    }

    bool Model::IsAnimating() const
    {
        return this->animation.IsPlaying();
    }

    void Model::SetAnimationPlaybackSpeed(double speed)
    {
        this->animationPlaybackSpeed = speed;
    }
}


// Private
namespace GTEngine
{
    Bone* Model::GetBoneByName(const char* name, size_t* indexOut)
    {
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            if (GTCore::Strings::Equal(name, this->bones[i]->GetName()))
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
    }
}


