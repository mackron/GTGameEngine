
#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    Model::Model()
        : meshes(), bones(),
          animation(), animationChannelBones(), animationKeyCache()
    {
    }

    Model::~Model()
    {
        // Meshes.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            delete this->meshes[i];
        }

        // Bones
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i]->value;
        }

        // Animation keys.
        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
    }


    Mesh* Model::AttachMesh(VertexArray* geometry, Material* material)
    {
        auto newMesh = new Mesh(geometry, material);

        this->meshes.PushBack(newMesh);

        return newMesh;
    }

    Mesh* Model::AttachMesh(VertexArray* geometryIn, Material* materialIn, const GTCore::Vector<BoneWeights*> &bonesIn)
    {
        auto newMesh = this->AttachMesh(geometryIn, materialIn);
        if (newMesh != nullptr)
        {
            for (size_t i = 0; i < bonesIn.count; ++i)
            {
                auto bone = bonesIn.buffer[i];
                assert(bone != nullptr);

                this->AddBoneWeightsToMesh(*newMesh, *bone);
            }
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

            auto newBone = new Bone(*bone);
            this->bones.Add(newBone->GetName(), newBone);
        }

        // This is the second pass. We need to link the bones together to form their hierarchy.
        for (size_t i = 0; i < inputBones.count; ++i)
        {
            auto inputBone = inputBones.buffer[i]->value;
            assert(inputBone != nullptr);

            if (inputBone->GetParent() != nullptr)
            {
                auto bone = this->bones.Find(inputBone->GetName())->value;
                assert(bone != nullptr);

                auto parentBone = this->bones.Find(inputBone->GetParent()->GetName())->value;
                assert(parentBone != nullptr);

                parentBone->AttachChild(*bone);
            }
        }
    }

    void Model::CopyAnimation(Animation &sourceAnimation, GTCore::Map<AnimationChannel*, Bone*> &sourceAnimationChannelBones)
    {
        // We first need to create all of the key frames.
        for (size_t iKeyFrame = 0; iKeyFrame < sourceAnimation.GetKeyFrameCount(); ++iKeyFrame)
        {
            this->animation.AppendKeyFrame(sourceAnimation.GetKeyFrameTimeByIndex(iKeyFrame));
        }


        // Here we add all of our channels to the animation. They will be empty to begin with, but filled below.
        for (size_t iChannel = 0; iChannel < sourceAnimationChannelBones.count; ++iChannel)
        {
            auto sourceChannel = sourceAnimationChannelBones.buffer[iChannel]->key;
            auto sourceBone    = sourceAnimationChannelBones.buffer[iChannel]->value;

            auto bone = this->bones.Find(sourceBone->GetName())->value;
            assert(bone != nullptr);

            auto &newChannel = this->animation.CreateChannel();
            this->animationChannelBones.Add(&newChannel, bone);

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


    bool Model::IsAnimating() const
    {
        return this->animation.IsPlaying();
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
        this->animation.Step(step);

        // Now that we've stepped the animation, we need to update the bone positions.
        size_t startKeyFrame;
        size_t endKeyFrame;
        auto interpolationFactor = this->animation.GetKeyFramesAtCurrentPlayback(startKeyFrame, endKeyFrame);

        for (size_t i = 0; i < this->animationChannelBones.count; ++i)
        {
            auto iChannelBone = this->animationChannelBones.buffer[i];

            auto firstKey = static_cast<TransformAnimationKey*>(iChannelBone->key->GetKey(startKeyFrame));
            auto endKey   = static_cast<TransformAnimationKey*>(iChannelBone->key->GetKey(endKeyFrame));
            auto bone     = iChannelBone->value;

            if (firstKey != nullptr && endKey != nullptr)
            {
                glm::vec3 position = glm::mix(firstKey->position, endKey->position, interpolationFactor);
                glm::quat rotation =      mix(firstKey->rotation, endKey->rotation, interpolationFactor);
                glm::vec3 scale    = glm::mix(firstKey->scale,    endKey->scale,    interpolationFactor);

                bone->SetPosition(position);
                bone->SetRotation(rotation);
                bone->SetScale(scale);
            }
        }

        // Now we need to loop over each channel again and update the skinning transformations. It's important that this is done separately from the
        // loop above to ensure all dependants have been updated beforehand.
        for (size_t i = 0; i < this->animationChannelBones.count; ++i)
        {
            this->animationChannelBones.buffer[i]->value->UpdateSkinningTransform();
        }
    }
}


// Private
namespace GTEngine
{
    void Model::AddBoneWeightsToMesh(Mesh &mesh, const BoneWeights &bone)
    {
        auto iLocalBone = this->bones.Find(bone.name.c_str());
        assert(iLocalBone != nullptr);

        auto localBone = iLocalBone->value;
        assert(localBone != nullptr);

        mesh.AttachBoneWeights(*localBone, bone.weights.count, bone.weights.buffer);
    }
}


