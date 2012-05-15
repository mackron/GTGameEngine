
#include <GTEngine/Armature.hpp>

namespace GTEngine
{
    Armature::Armature()
        : rootBones()
    {
    }

    Armature::~Armature()
    {
    }

    void Armature::AddRootBone(BoneWithWeights &bone)
    {
        // Don't add it if it already exists...
        if (!this->rootBones.Exists(&bone))
        {
            this->rootBones.PushBack(&bone);
        }
    }

    void Armature::RemoveRootBone(BoneWithWeights &bone)
    {
        this->rootBones.RemoveFirst(&bone);
    }
}
