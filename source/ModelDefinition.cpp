
#include <GTEngine/ModelDefinition.hpp>
#include <GTEngine/MaterialLibrary.hpp>

namespace GTEngine
{
    ModelDefinition::ModelDefinition()
        : meshGeometries(), meshMaterials(), meshBones(),
          bones(),
          animation(), animationChannelBones(), animationKeyCache()
    {
    }

    ModelDefinition::~ModelDefinition()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            delete this->meshGeometries[i];
        }

        for (size_t i = 0; i < this->meshMaterials.count; ++i)
        {
            MaterialLibrary::Delete(this->meshMaterials[i]);
        }

        for (size_t i = 0; i < this->meshBones.count; ++i)
        {
            auto meshBone = this->meshBones[i];
            if (meshBone != nullptr)
            {
                for (size_t j = 0; j < meshBone->count; ++j)
                {
                    delete meshBone->buffer[j];
                }
            }

            delete this->meshBones[i];
        }

        for (size_t i = 0; i < this->bones.count; ++i)
        {
            delete this->bones.buffer[i]->value;
        }

        for (size_t i = 0; i < this->animationKeyCache.count; ++i)
        {
            delete this->animationKeyCache[i];
        }
    }

    void ModelDefinition::GenerateTangentsAndBitangents()
    {
        for (size_t i = 0; i < this->meshGeometries.count; ++i)
        {
            this->meshGeometries[i]->GenerateTangentsAndBitangents();
        }
    }

    /// Creates an animation key for the given bone and returns it.
    TransformAnimationKey* ModelDefinition::CreateAnimationKey(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    {
        auto newKey = new TransformAnimationKey(position, rotation, scale);
        this->animationKeyCache.PushBack(newKey);

        return newKey;
    }

    /// Maps a bone to an animation channel.
    void ModelDefinition::MapBoneToAnimationChannel(AnimationChannel &channel, Bone &bone)
    {
        this->animationChannelBones.Add(&channel, &bone);
    }
}