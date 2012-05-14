
#include <GTEngine/Model.hpp>
#include <GTEngine/VertexArrayFactory.hpp>
#include <GTEngine/CPUVertexShader_SimpleTransform.hpp>

namespace GTEngine
{
    Model::Model()
        : meshes(), bones(), animations(),
          currentAnimation(nullptr)
    {
    }

    Model::~Model()
    {
        // Meshes and Armatures.
        for (size_t i = 0; i < this->meshes.count; ++i)
        {
            delete this->meshes[i]->GetArmature();
            delete this->meshes[i];
        }

        // Animations.
        for (size_t i = 0; i < this->animations.count; ++i)
        {
            delete this->animations.buffer[i]->value;
        }

        // Bones
        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i]->value;
        }
    }

    void Model::AttachMesh(VertexArray* geometry, Material* material, const Armature* armature)
    {
        Armature* newArmature = nullptr;

        // We need to create a copy of the armature.
        if (armature != nullptr)
        {
            newArmature = new Armature;

            auto &armatureRootBones = armature->GetRootBones();
            for (size_t i = 0; i < armatureRootBones.count; ++i)
            {
                auto iBone = this->bones.Find(armatureRootBones[i]->GetName());
                if (iBone != nullptr)
                {
                    newArmature->AddRootBone(*iBone->value);
                }
            }
        }

        // Now we create the mesh object...
        auto newMesh = new Mesh(geometry, material, newArmature);

        // ... and add it to the list.
        this->meshes.PushBack(newMesh);
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

    void Model::CopyAndAddAnimations(const GTCore::Dictionary<ArmatureAnimation*> &inputAnimations)
    {
        for (size_t i = 0; i < inputAnimations.count; ++i)
        {
            auto animation = inputAnimations.buffer[i]->value;
            assert(animation != nullptr);

            auto newAnimation = new ArmatureAnimation(animation->GetName());

            // Duration.
            newAnimation->SetDurationInSeconds(animation->GetDurationInSeconds());

            // Channels.
            for (size_t i = 0; i < animation->GetChannelCount(); ++i)
            {
                auto &channel = animation->GetChannel(i);

                auto bone = this->bones.Find(channel.GetBone().GetName())->value;
                assert(bone != nullptr);

                
                auto newChannel = newAnimation->AddChannel(*bone);
                assert(newChannel != nullptr);


                // Keys.
                for (size_t iKey = 0; iKey < channel.GetKeyCount(); ++iKey)
                {
                    auto &key = channel.GetKey(iKey);
                    newChannel->AddKey(key.time, key.position, key.rotation, key.scale);
                }
            }

            // Now we add the new animation to our local animation map.
            this->animations.Add(newAnimation->GetName(), newAnimation);
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
        if (this->currentAnimation != nullptr)
        {
            return true;
        }

        return false;
    }


    // !!! Animation !!!
    void Model::PlayAnimation(const char* animationName, bool loop)
    {
        auto iAnimation = this->animations.Find(animationName);
        if (iAnimation != nullptr)
        {
            this->currentAnimation = iAnimation->value;

            this->currentAnimation->Play(loop);
        }
    }

    void Model::StopAnimation()
    {
        if (this->currentAnimation != nullptr)
        {
            this->currentAnimation->Stop();

            this->currentAnimation = nullptr;
        }
    }

    void Model::PauseAnimation()
    {
        if (this->currentAnimation != nullptr)
        {
            this->currentAnimation->Pause();
        }
    }

    void Model::ResumeAnimation()
    {
        if (this->currentAnimation != nullptr)
        {
            this->currentAnimation->Play();
        }
    }

    ArmatureAnimation* Model::GetCurrentAnimation()
    {
        return this->currentAnimation;
    }

    const char* Model::GetCurrentAnimationName() const
    {
        if (this->currentAnimation != nullptr)
        {
            return this->currentAnimation->GetName();
        }

        return nullptr;
    }

    void Model::StepAnimation(double deltaSeconds)
    {
        if (this->currentAnimation != nullptr)
        {
            this->currentAnimation->Step(deltaSeconds);
        }
    }
}


